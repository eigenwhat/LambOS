#include <fs/iso9660/DirectoryEntry.hpp>
#include <fs/iso9660/Volume.hpp>
#include <util/StaticList.hpp>
#include <util/LinkedList.hpp>
#include <arch/i386/sys/asm.h>
#include <cstring>
#include <util/StringTokenizer.hpp>

namespace iso9660 {

namespace {

void fixEntryName(char *name, int length)
{
    if (name[length-2] == ';') {
        name[length-2] = 0;
        length -= 2;
    }

    if(length > 1 && name[length-1] == '.') {
        --length;
        name[length] = 0;
    }
}

constexpr size_t sectorsToRead(size_t bytesToRead, size_t sectorSize)
{
    return bytesToRead / sectorSize + ((bytesToRead % sectorSize) ? 1 : 0);
}

enum class RREntry { PX, PN, SL, NM, CL, PL, RE, TF, SF, Unknown };

RREntry decodeSignature(char *ptr)
{
    char sig[3];
    sig[0] = ptr[0];
    sig[1] = ptr[1];
    sig[2] = 0;
    if (!strcmp(sig, "PX")) return RREntry::PX;
    if (!strcmp(sig, "PN")) return RREntry::PN;
    if (!strcmp(sig, "SL")) return RREntry::SL;
    if (!strcmp(sig, "NM")) return RREntry::NM;
    if (!strcmp(sig, "CL")) return RREntry::CL;
    if (!strcmp(sig, "PL")) return RREntry::PL;
    if (!strcmp(sig, "RE")) return RREntry::RE;
    if (!strcmp(sig, "TF")) return RREntry::TF;
    if (!strcmp(sig, "SF")) return RREntry::SF;

    return RREntry::Unknown;
}

String rockRidgeName(rockridge::AltNameEntryInfo *info)
{
    size_t strlen = info->header.length - (size_t)&((decltype(info))0)->name;
    return String(info->name, strlen);
}

rockridge::AltNameEntryInfo *findRockRidgeNameEntry(char *ptr, std::ptrdiff_t len)
{
    using namespace rockridge;
    while (len > 0) {
        auto type = decodeSignature(ptr);
        if (type == RREntry::Unknown) {
            return nullptr;
        }
        auto entryLength = uint8_t(ptr[2]);
        if (type != RREntry::NM) {
            ptr += entryLength;
            len -= entryLength;
            continue;
        }

        // found it
        return reinterpret_cast<AltNameEntryInfo *>(ptr);
    }

    return nullptr;
}

class IsoFileStream : public InputStream
{
  public:
    IsoFileStream(size_t fileSize, size_t bufferSize)
            : _fileSize(fileSize), _buffer(bufferSize) {}

    size_t available() const override { return _fileSize - _pos; }

    Byte read() override
    {
        if (_pos == _fileSize) {
            return kEndOfStream;
        }

        if (_readsUntilInvalid) {
            --_readsUntilInvalid;
        }

        return _buffer[_pos++];
    }

    void mark(size_t readsLeft) override
    {
        _readsUntilInvalid = readsLeft;
        _mark = _pos;
    }

    bool markSupported() const override { return true; }

    void reset() override
    {
        if (_readsUntilInvalid) {
            _pos = _mark;
        }
    }

    std::byte *buffer() { return _buffer.get(); }

  private:
    size_t _fileSize = 0;
    StaticList<std::byte> _buffer;
    size_t _pos = 0;
    size_t _mark = 0;
    size_t _readsUntilInvalid = 0;
};

} // anonymous namespace


DirectoryEntry::DirectoryEntry(iso9660::DirectoryInfo &info, Volume &volume)
        : ::DirectoryEntry(volume,
                           info.flags & 0x2 ? Type::Directory : Type::File,
                           String(info.name, info.nameLength))
        , _extentLba(info.extentLba.lsb)
        , _extentLength(info.extentLength.lsb)
{
    // try to find Rock Ridge name
    auto rrNameEntry = findRockRidgeNameEntry(info.suspStart(), info.suspLength());
    if (rrNameEntry) {
        setName(rockRidgeName(rrNameEntry));
    }
}


::DirectoryEntry *DirectoryEntry::find(char const *path)
{
    static constexpr char const *delimiters = "/";
    // get first element in path
    StringTokenizer tokenizer(path, delimiters);
    return find(tokenizer);
}

::DirectoryEntry *DirectoryEntry::find(StringTokenizer &tokenizer)
{
    auto elem = tokenizer.nextToken();

    // read directory contents
    const auto sectorSize = volume().parentDevice()->sectorSize();
    const auto sectorCount = sectorsToRead(_extentLength, sectorSize);
    StaticList<uint8_t> buf{sectorSize * sectorCount};
    volume().parentDevice()->read(_extentLba, (uint16_t *)buf.get(), sectorCount);

    // go over entries
    auto bytesLeft = _extentLength;
    uint8_t *infoBytes = buf.get();
    DirectoryInfo *info = reinterpret_cast<DirectoryInfo *>(infoBytes);
    while (bytesLeft > 0 && info->length > 0)
    {
        DirectoryEntry entry(*info, (iso9660::Volume&)volume());
        if (!strcmp(entry.name(), elem.cstr())) {
            if (tokenizer.hasNextToken()) {
                return entry.find(tokenizer);
            }

            return new DirectoryEntry(entry);
        }
        infoBytes += info->length;
        info = reinterpret_cast<DirectoryInfo *>(infoBytes);
    }

    return nullptr;
}

Maybe<LinkedList<String>> DirectoryEntry::readdir() const
{
    if (type() != Type::Directory) {
        return Nothing; // not a directory...
    }

    Maybe contents{LinkedList<String>{}};

    // read directory contents
    const auto sectorSize = volume().parentDevice()->sectorSize();
    const auto sectorCount = sectorsToRead(_extentLength, sectorSize);
    StaticList<uint8_t> buf{sectorSize * sectorCount};
    volume().parentDevice()->read(_extentLba, (uint16_t *)buf.get(), sectorCount);

    // go over entries
    auto bytesLeft = _extentLength;
    uint8_t *infoBytes = buf.get();
    DirectoryInfo *info = reinterpret_cast<DirectoryInfo *>(infoBytes);
    while (bytesLeft > 0 && info->length > 0)
    {
        fixEntryName(info->name, info->nameLength);
        if (info->name[0] == 0) {
            contents->insert(".");
        } else if (info->name[0] == 1) {
            contents->insert("..");
        } else {
            DirectoryEntry entry(*info, (iso9660::Volume&)volume());
            contents->insert(String(entry.name()));
        }
        infoBytes += info->length;
        info = reinterpret_cast<DirectoryInfo *>(infoBytes);
    }

    return contents;
}

InputStream *DirectoryEntry::fileStream() const
{
    if (isDir()) return nullptr;

    // build an input stream and some junk
    const auto sectorSize = volume().parentDevice()->sectorSize();
    const auto sectorCount = sectorsToRead(_extentLength, sectorSize);
    auto *fstream = new IsoFileStream(_extentLength, sectorSize*sectorCount);
    volume().parentDevice()->read(_extentLba, (uint16_t *)fstream->buffer(),
                                  sectorCount);

    return fstream;
}

int DirectoryEntry::rmdir(char const *) { return -1; }
int DirectoryEntry::link(char const *, char const *) { return -1; }
int DirectoryEntry::unlink(char const *) { return -1; }
int DirectoryEntry::mkdir(char const *) { return -1; }
int DirectoryEntry::mkfile(char const *) { return -1; }

}
