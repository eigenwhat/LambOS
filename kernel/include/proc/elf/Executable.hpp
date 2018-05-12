#pragma once

#include <fs/DirectoryEntry.hpp>
#include <io/FileReader.hpp>
#include <util/ArrayList.hpp>
#include <Kernel.hpp>

namespace elf {

class Executable : public Object
{
  public:
    struct Section;

    /**
     * Checks if the DirectoryEntry points to a valid ELF binary.
     * This will open the entry's file and read from it, looking for the ELF
     * header.
     * @param entry The entry to check.
     * @return `true` if it's a valid ELF binary, `false` otherwise.
     */
    static bool isElf(DirectoryEntry const &entry);

    /**
     * Reads the referred to ELF binary into memory.
     * @param entry The DirectoryEntry for the binary.
     */
    Executable(DirectoryEntry &entry);

    /** The sections as listed in the binary's section header table. */
    ArrayList<Section> const &sections() const { return _sections; }

    struct Section
    {
        String name;
        uint8_t const *data = nullptr;
        uintptr_t vaddress;

        Section() {}

        // TODO: eliminate once we have proper type traits to SFINAE collections
        Section(Section const&) { kernel->panic("Attempted copy of Section."); }
        Section(Section &&rhs) = default;

        // TODO: eliminate once we have proper type traits to SFINAE collections
        Section &operator=(Section const &)
        {
            kernel->panic("Attempted copy assignment of Section.");
            return *this;
        }
        Section &operator=(Section &&rhs) = default;

        bool operator==(Section const &rhs) const
        {
            return name == rhs.name
                   && vaddress == rhs.vaddress
                   && data == rhs.data;
        }

        bool operator!=(Section const &rhs) const { return !operator==(rhs); }
    };

  private:
    size_t loadSections(size_t offset, size_t entryCount, size_t nameTableIndex);
    void loadNameTable(size_t offset, size_t entryIndex);

    FileReader _file;
    ArrayList<Section> _sections{1};
    Section const *_nameTable = nullptr;
};

}
