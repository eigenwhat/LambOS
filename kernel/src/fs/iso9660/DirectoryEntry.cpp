#include <fs/iso9660/DirectoryEntry.hpp>
#include <fs/iso9660/Volume.hpp>
#include <util/Array.hpp>
#include <cstdio>
#include <sys/asm.h>
#include <cstring>

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

enum class RREntry
{
    PX,
    PN,
    SL,
    NM,
    CL,
    PL,
    RE,
    TF,
    SF,
    Unknown
};

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

rockridge::AltNameEntryInfo *findRockRidgeNameEntry(char *ptr, size_t len)
{
    using namespace rockridge;
    while (len > 0) {
        auto type = decodeSignature(ptr);
        if (type == RREntry::Unknown) {
            return nullptr;
        }
        uint8_t entryLength = ptr[2];
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

LinkedList<String> *DirectoryEntry::readdir()
{
    if (type() != Type::Directory) {
        return nullptr; // not a directory...
    }

    LinkedList<String> *contents = new LinkedList<String>;

    // read directory contents
    const auto sectorSize = volume().parentDevice()->sectorSize();
    size_t sectorsToRead = _extentLength / sectorSize
                         + (_extentLength & sectorSize) ? 1 : 0;
    Array<uint8_t> buf{sectorSize*sectorsToRead};
    volume().parentDevice()->read(_extentLba, (uint16_t *)buf.get(), sectorsToRead);

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

int DirectoryEntry::rmdir(char const *) { return -1; }
int DirectoryEntry::link(char const *, char const *) { return -1; }
int DirectoryEntry::unlink(char const *) { return -1; }
int DirectoryEntry::mkdir(char const *) { return -1; }
int DirectoryEntry::mkfile(char const *) { return -1; }

}
