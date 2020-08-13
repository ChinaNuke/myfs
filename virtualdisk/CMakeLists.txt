aux_source_directory(./src DIR_SRCS)

add_library(virtualdisk ${DIR_SRCS})

target_include_directories(virtualdisk
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

