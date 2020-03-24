# set binary name
set(BinName "CPPStart")


cmake_minimum_required(VERSION 3.5.1)

#1.set binary name
PROJECT(${BinName})

#2.add include directory
INCLUDE_DIRECTORIES(./include)
INCLUDE_DIRECTORIES(./system/include)
INCLUDE_DIRECTORIES(./git/include)

#3.get project path automaticly
get_filename_component(PROJECT_DIR $ENV{PWD} DIRECTORY)

#4.add all source directory
aux_source_directory(. SRC_LIST)
aux_source_directory(./system SRC_LIST)
aux_source_directory(./git SRC_LIST)

#5.add source directory with binary
add_executable(${BinName} ${SRC_LIST})

#6.link lib file on the binary(if needed)
 target_link_libraries (${BinName} ${PROJECT_DIR}/git/lib/libgit2.so)
