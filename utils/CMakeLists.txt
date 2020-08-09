aux_source_directory(. DIR_SRCS)

add_library(utils ${DIR_SRCS})

target_include_directories(utils
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

