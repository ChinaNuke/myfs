cmake_minimum_required(VERSION 3.10)

project(Unix-Filesystem VERSION 0.1)

# 包含库 libmyfs
add_subdirectory(myfs)

aux_source_directory(. DIR_SRCS)
aux_source_directory(utils/ UTILS_SRCS)

add_executable(main ${DIR_SRCS} ${UTILS_SRCS})

# 链接库 libmyfs
target_link_libraries(main PUBLIC myfs)



# configure_file(PengConfig.h.in PengConfig.h)

# add_executable(testpeng main.c)

# target_link_libraries(testpeng PUBLIC penglibrary)

# target_include_directories(testpeng PUBLIC "${PROJECT_BINARY_DIR}")

# MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
# MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
