//
// Created by Martin Miralles-Cordal on 5/8/2018.
//

#include <proc/elf/Executable.hpp>

#include <proc/elf/DataStructures.hpp>
#include <Kernel.hpp>

#include <utility>
#include <cstring>
#include <util/LinkedList.hpp>

namespace elf {

namespace {

class SegmentRAII
{
  public:
    SegmentRAII() {}
    SegmentRAII(void *addr, size_t pages) : _addr(addr), _pages(pages) {}

    ~SegmentRAII()
    {
        if (_armed && _addr && _pages) {
            kernel->mmu()->pfree(_addr, _pages);
        }
    }

    SegmentRAII(SegmentRAII &&other)
    {
        *this = std::move(other);
    }

    SegmentRAII &operator=(SegmentRAII &&other)
    {
        _addr = other._addr;
        _pages = other._pages;
        _armed = other._armed;
        other.disarm();
        return *this;
    }

    bool operator==(SegmentRAII const &rhs)
    {
        return _addr == rhs._addr && _pages == rhs._pages && _armed == rhs._armed;
    }

    void disarm() { _armed = false; }

  private:
    void *_addr = nullptr;
    size_t _pages = 0;
    bool _armed = true;
};

template <typename T>
T readObject(sys::FileReader &file, size_t offset, size_t *bytesRead = nullptr)
{
    T object;
    auto count = file.read(reinterpret_cast<std::byte *>(&object), offset, sizeof(object));
    if (bytesRead) {
        *bytesRead = (size_t)count;
    }

    return object;
}

bool checkHeader(ElfHeader32 const &header32)
{
    return header32.checkMagic()
           && header32.bitness == Bitness::k32Bit
           && header32.endianness == Endianness::LittleEndian
           && header32.instructionSet == InstructionSet::x86
           && header32.version32 == ElfHeader32::kCurrentVersion
           && (header32.type == Type::Relocatable
               || header32.type == Type::Executable);
}

}

bool Executable::isElf(DirectoryEntry const &entry)
{
    if (!entry.isFile()) {
        return false;
    }

    auto file = sys::FileReader{sys::ArcPtr<sys::InputStream>{entry.fileStream().release()}};

    size_t bytesRead = 0;
    auto const header32 = readObject<ElfHeader32>(file, 0, &bytesRead);

    if (bytesRead != sizeof(header32)) {
        return false;
    }

    return checkHeader(header32);
}

Executable::Executable(DirectoryEntry &entry)
        : _file(sys::ArcPtr<sys::InputStream>{entry.fileStream().release()})
{
    _file.readAll();
    auto header = readObject<ElfHeader32>(_file, 0);
    _sections.reserve(header.sectionEntryCount);
    _segments.reserve(header.programEntryCount);
    loadSections(header.sectionHeaderOffset, header.sectionEntryCount, header.sectionNameEntryIndex);
    loadProgramSegments(header.programHeaderOffset, header.programEntryCount);
    _entry = (EntryType)header.entry;
}

Executable::~Executable()
{
    unloadSegments();
}

size_t Executable::loadSections(size_t offset, size_t entryCount, size_t nameTableIndex)
{
    size_t totalBytesRead = 0;

    if (!_nameTable) {
        loadNameTable(offset, nameTableIndex);
    }

    for (size_t i = 0; i < entryCount; ++i) {
        size_t bytesRead = 0;
        auto const header = readObject<SectionHeader32>(_file, offset, &bytesRead);
        offset += bytesRead;
        totalBytesRead += bytesRead;

        switch (header.type) {
            case SectionHeader32::Type::Null:break;
            case SectionHeader32::Type::ProgBits:break;
            case SectionHeader32::Type::SymbolTable:break;
            case SectionHeader32::Type::StringTable:break;
            case SectionHeader32::Type::RelocAddend:break;
            case SectionHeader32::Type::HashTable:break;
            case SectionHeader32::Type::Dynamic:break;
            case SectionHeader32::Type::Note:break;
            case SectionHeader32::Type::NoBits:break;
            case SectionHeader32::Type::Relocation:break;
            case SectionHeader32::Type::Shlib:break;
            case SectionHeader32::Type::Dynsym:break;
            case SectionHeader32::Type::LoProc:break;
            case SectionHeader32::Type::HiProc:break;
            case SectionHeader32::Type::LoUser:break;
            case SectionHeader32::Type::HiUser:break;
            default: kernel->panic("Unexpected section header type while reading ELF");
        }

        Section section;
        section.vaddress = header.address;
        section.size = header.size;

        if (_nameTable) {
            section.name = (char const *)_nameTable->data + header.nameIndex;
        }

        if (header.type != SectionHeader32::Type::NoBits) {
            section.data = _file.bytes() + header.offset;
        }

        _sections.enqueue(std::move(section));
    }

    return totalBytesRead;
}

void Executable::loadNameTable(size_t offset, size_t entryIndex)
{
    auto const tableOffset = offset + (sizeof(SectionHeader32) * entryIndex);
    size_t bytesRead = 0;
    auto const header = readObject<SectionHeader32>(_file, tableOffset, &bytesRead);
    if (bytesRead != sizeof(SectionHeader32)) {
        return;
    }

    Section section;
    section.vaddress = header.address;
    section.data = _file.bytes() + header.offset;
    section.name = reinterpret_cast<char const *>(section.data) + header.nameIndex;
    _sections.enqueue(std::move(section));
    _nameTable = &_sections.peekBack();
}

size_t Executable::loadProgramSegments(size_t offset, size_t entryCount)
{
    size_t totalBytesRead = 0;

    for (size_t i = 0; i < entryCount; ++i) {
        size_t bytesRead = 0;
        auto const header = readObject<ProgramHeader32>(_file, offset, &bytesRead);
        offset += bytesRead;
        totalBytesRead += bytesRead;

        switch (header.type) {
            case ProgramHeader32::Type::Null:break;
            case ProgramHeader32::Type::Load:break;
            case ProgramHeader32::Type::Dynamic:break;
            case ProgramHeader32::Type::Interpreter:break;
            case ProgramHeader32::Type::Note:break;
            case ProgramHeader32::Type::Shlib:break;
            case ProgramHeader32::Type::ProgramHeader:break;
            case ProgramHeader32::Type::LoProc:break;
            case ProgramHeader32::Type::HiProc:break;
            default: kernel->panic("Unexpected section header type while reading ELF");
        }

        Segment segment;
        segment.vaddress = header.virtAddress;
        segment.alignment = header.alignment;
        segment.data = _file.bytes() + header.offset;
        segment.dataSize = header.sizeInFile;
        segment.memorySize = header.sizeInMemory;
        _segments.enqueue(std::move(segment));
    }

    return totalBytesRead;
}

bool Executable::loadSegments()
{
    if (_isLoaded) {
        return true; // already loaded
    }

    sys::LinkedList<SegmentRAII> cleanup;
    for (auto &ps : _segments) {
        if (ps.alignment == 0x1000) {
            size_t pages = ps.memorySize / 0x1000 + ((ps.memorySize % 0x1000) ? 1 : 0);
            auto mem = kernel->mmu()->palloc((void*)ps.vaddress, pages);
            if (!mem) {
                return false;
            }

            cleanup.enqueue(SegmentRAII{mem, pages});
            memset(mem, 0, ps.memorySize);
            if (ps.dataSize) {
                memcpy(mem, ps.data, ps.dataSize);
            }
        }
    }

    // successfully loaded
    for (auto &c : cleanup) {
        c.disarm();
    }

    _isLoaded = true;

    return true;
}

bool Executable::unloadSegments()
{
    if (!_isLoaded) {
        return true; // already unloaded
    }

    for (auto &ps : _segments) {
        size_t pages = ps.memorySize / 0x1000 + ((ps.memorySize % 0x1000) ? 1 : 0);
        if (kernel->mmu()->pfree((void*)ps.vaddress, pages) != 0) {
            kernel->panic("Error freeing loaded ELF segment memory.");
        }
    }

    _isLoaded = false;
    return true;
}

} // namespace elf
