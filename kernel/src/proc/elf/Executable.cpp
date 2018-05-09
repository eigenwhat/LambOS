#include <proc/elf/Executable.hpp>

#include <proc/elf/DataStructures.hpp>

namespace elf {

namespace {

ElfHeader32 readHeader(InputStream &stream, int *bytesRead = nullptr)
{
    ElfHeader32 header;
    auto count = stream.read(reinterpret_cast<uint8_t *>(&header), sizeof(header));
    if (bytesRead) {
        *bytesRead = count;
    }

    return header;
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

    auto * const fstream = entry.fileStream();
    if (!fstream) {
        return false;
    }

    int bytesRead = 0;
    auto const header32 = readHeader(*fstream, &bytesRead);
    if (bytesRead != sizeof(header32)) {
        return false;
    }

    return checkHeader(header32);
}

Executable::Executable(DirectoryEntry &entry)
        : _fileStream(entry.fileStream(), false)
{
    // mark beginning of file as reset point
    _fileStream->mark(_fileStream->available());
}

} // namespace elf
