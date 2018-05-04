#include <fs/iso9660/DirectoryEntry.hpp>
#include <fs/iso9660/Volume.hpp>
#include <util/Array.hpp>
#include <cstdio>

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

}

namespace iso9660 {

DirectoryEntry::DirectoryEntry(iso9660::DirectoryInfo &info, Volume &volume)
        : ::DirectoryEntry(volume,
                           info.flags & 0x2 ? Type::Directory : Type::File,
                           String(info.name, info.nameLength))
        , _extentLba(info.extentLba.lsb)
        , _extentLength(info.extentLength.lsb)
{}

List<String> *DirectoryEntry::readdir()
{
    if (type() != Type::Directory) {
        return nullptr; // not a directory...
    }

    LinkedList<String> *contents = new LinkedList<String>;
    const auto sectorSize = volume().parentDevice()->sectorSize();
    auto sectorsToRead = _extentLength / sectorSize
                         + (_extentLength & sectorSize) ? 1 : 0;
    Array<uint8_t> buf{sectorSize*sectorsToRead};
    volume().parentDevice()->read(_extentLba, (uint16_t *)buf.get(), sectorsToRead);
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
//            printf("%x\n", infoBytes);
//            asm volatile ("xchg %bx, %bx");
            contents->insert(String(info->name + info->nameLength + 68, info->nameLength));
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