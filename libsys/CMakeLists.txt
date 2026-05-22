if (NOT DEFINED LIBSYS_TARGET)
    set(LIBSYS_TARGET lambos-libsys)
    message (WARNING "LIBSYS_TARGET was not set by project root. Defaulting to '${LIBSYS_TARGET}'.")
    set(LIBSYS_TARGET ${LIBSYS_TARGET} PARENT_SCOPE)
endif ()

file(GLOB_RECURSE GLOB_INCLUDES include/*.hpp include/*.h)

set (SOURCES
        ${GLOB_INCLUDES} # ok with globbing since they're not translation units
        src/fs/Path.cpp
        src/io/FileReader.cpp
        src/util/StringTokenizer.cpp)

add_library(${LIBSYS_TARGET} STATIC ${SOURCES})
target_include_directories(${LIBSYS_TARGET} PUBLIC include)
target_compile_features(${LIBSYS_TARGET} PUBLIC ${LAMBOS_CXX_STANDARD_FEATURE})
lambos_add_compile_options(${LIBSYS_TARGET})
target_link_libraries(${LIBSYS_TARGET} ${LIBC_TARGET} ${LIBCPP_TARGET})
