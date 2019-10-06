include(${CMAKE_SOURCE_DIR}/programs/AddLambosExecutable.cmake)

set(SOURCES
        src/main.cpp)

add_lambos_executable(NAME kvshell SOURCES ${SOURCES})
target_link_libraries(kvshell ${LIBSYS_TARGET})
