include(${CMAKE_SOURCE_DIR}/programs/AddLambosExecutable.cmake)

add_lambos_executable(NAME proc-test SOURCES main.cpp)
target_link_libraries(proc-test ${LIBSYS_TARGET})
