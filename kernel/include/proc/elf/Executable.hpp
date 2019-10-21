//
// Created by Martin Miralles-Cordal on 5/8/2018.
//

#pragma once

#include <fs/DirectoryEntry.hpp>
#include <io/FileReader.hpp>
#include <util/ArrayList.hpp>
#include <Kernel.hpp>
#include <util/LinkedList.hpp>

namespace elf {

class Executable
{
  public:
    /** The function signature for the program's entry point. */
    using EntryType = int (*)(int, char **);

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
    explicit Executable(DirectoryEntry &entry);

    ~Executable();

    /** The sections as listed in the binary's section header table. */
    [[nodiscard]] ArrayList<Section> const &sections() const { return _sections; }

    /**
     * Loads the program segments into memory.
     * @return `true` if the segments are loaded, `false` otherwise.
     */
    bool loadSegments();

    /**
     * Executes the program.
     * @note VERY TEMPORARY AND VERY SUBJECT TO CHANGE.
     * @return The exit status of the program.
     */
     /** @{ */
    int exec() { return loadSegments() ? _entry(0, nullptr) : -1; }
    int operator()() { return exec(); }
    /** @} */

    /**
     * Unloads the program segments from memory.
     * @return `true` if the segments are unloaded, `false` otherwise.
     */
    bool unloadSegments();

    struct Section
    {
        String name;
        std::byte const *data = nullptr;
        uintptr_t vaddress = 0;
        uint32_t size = 0;

        Section() {}

        Section(Section const &) = delete;
        Section(Section &&rhs) = default;

        Section &operator=(Section const &) = delete;
        Section &operator=(Section &&rhs) = default;

        bool operator==(Section const &rhs) const
        {
            return name == rhs.name
                   && vaddress == rhs.vaddress
                   && data == rhs.data;
        }

        bool operator!=(Section const &rhs) const { return !operator==(rhs); }
    };

    struct Segment
    {
        std::byte const *data = nullptr;
        uintptr_t vaddress = 0;
        uint32_t alignment = 0;
        uint32_t dataSize = 0;
        uint32_t memorySize = 0;

        Segment() {}

        Segment(Segment const &) = delete;
        Segment(Segment &&rhs) = default;

        Segment &operator=(Segment const &) = delete;
        Segment &operator=(Segment &&rhs) = default;

        bool operator==(Segment const &rhs) const
        {
            return vaddress == rhs.vaddress
                   && data == rhs.data;
        }

        bool operator!=(Segment const &rhs) const { return !operator==(rhs); }
    };

  private:
    size_t loadProgramSegments(size_t offset, size_t entryCount);
    size_t loadSections(size_t offset, size_t entryCount, size_t nameTableIndex);
    void loadNameTable(size_t offset, size_t entryIndex);

    FileReader _file;
    ArrayList<Section> _sections{1};
    ArrayList<Segment> _segments{1};
    Section const *_nameTable = nullptr;
    EntryType _entry = nullptr;
    bool _isLoaded = false;
};

}
