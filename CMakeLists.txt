cmake_minimum_required(VERSION 3.8)

project(atomic)

enable_testing()

set(CMAKE_CXX_STANDARD 20)

find_package(MPI REQUIRED)
find_package(Threads REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

add_compile_options("-std=c++2a" "-fconcepts" "-O3")
#add_compile_options("-std=c++2a" "-fconcepts")

#add_definitions("-DDEBUG_COMMENTS")

#sets the executable in main folder
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories("include")
include_directories("fmt/include")

#add_compile_options("-g" "-Wall" "-Wpedantic" "-Wextra" "-fexceptions" "-pthread" "-fconcepts")

file(GLOB Sources "src/*.cpp")

add_executable(a.out ${Sources}
        ${CMAKE_CURRENT_SOURCE_DIR}/fmt/src/format.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/fmt/src/os.cc)

target_link_libraries(a.out ${CMAKE_THREAD_LIBS_INIT}
        ${MPI_LIBRARIES}
        )


if (MPI_COMPILE_FLAGS)
    set_target_properties(a.out PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif ()


add_test(myTest mpirun -n 4 a.out)
