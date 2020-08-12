aux_source_directory(. DIR_SRCS)

add_library(myshell ${DIR_SRCS})

target_include_directories(myshell
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


