#include <proc/elf/Executable.hpp>

#include <proc/elf/DataStructures.hpp>
#include <Kernel.hpp>

#include <utility>

namespace elf {

namespace {

template <typename T>
T readObject(FileReader &file, size_t offset, size_t *bytesRead = nullptr)
{
    T object;
    auto count = file.read(reinterpret_cast<uint8_t *>(&object), offset, sizeof(object));
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

    auto file = FileReader{Autorelease(entry.fileStream()).get()};

    size_t bytesRead = 0;
    auto const header32 = readObject<ElfHeader32>(file, 0, &bytesRead);

    if (bytesRead != sizeof(header32)) {
        return false;
    }

    return checkHeader(header32);
}

Executable::Executable(DirectoryEntry &entry)
        : _file(Autorelease(entry.fileStream()).get())
{
    _file.readAll();
    auto header = readObject<ElfHeader32>(_file, 0);
    _sections.reserve(header.sectionEntryCount);
    _segments.reserve(header.programEntryCount);
    loadSections(header.sectionHeaderOffset, header.sectionEntryCount, header.sectionNameEntryIndex);
    loadProgramSegments(header.programHeaderOffset, header.programEntryCount);
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

} // namespace elf
