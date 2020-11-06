include(${CMAKE_SOURCE_DIR}/programs/AddLambosExecutable.cmake)

add_lambos_executable(NAME proc_test SOURCES main.cpp)
target_link_libraries(proc_test ${LIBSYS_TARGET})
