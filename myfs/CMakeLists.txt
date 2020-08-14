aux_source_directory(. DIR_SRCS)

#list(REMOVE_ITEM DIR_SRCS ./dir_entry.c)

add_library(myfs ${DIR_SRCS})

target_include_directories(myfs
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(myfs virtualdisk)

