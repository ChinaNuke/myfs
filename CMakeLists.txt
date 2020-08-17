cmake_minimum_required(VERSION 3.10)

project(Unix-Filesystem VERSION 0.2)

# 添加库目录
add_subdirectory(myfs)
add_subdirectory(virtualdisk)
# add_subdirectory(myshell)
# add_subdirectory(utils)

# 获取各目录下的源文件列表
# DIR_SRCS包含根目录下除了main.c之外的源文件
aux_source_directory(. DIR_SRCS)
list(REMOVE_ITEM DIR_SRCS ./main.c)
aux_source_directory(tests/ TESTS_SRCS)
aux_source_directory(myshell/ MYSH_SRCS)

# 主程序
add_executable(mysh ${MYSH_SRCS})
target_link_libraries(mysh myfs virtualdisk)
target_include_directories(mysh PUBLIC "${PROJECT_SOURCE_DIR}")

# virtual disk
#add_executable(vdisk virtualdisk/vdisk_main.c)
#target_link_libraries(vdisk virtualdisk)
#target_include_directories(main PUBLIC "${PROJECT_SOURCE_DIR}")

# 测试
#add_executable(testall ${TESTS_SRCS})
#target_link_libraries(testall myfs virtualdisk)
add_executable(testall ${TESTS_SRCS})
target_link_libraries(testall virtualdisk myfs)

enable_testing()
add_test(
  NAME testall
  COMMAND $<TARGET_FILE:testall> --success
  )




# configure_file(PengConfig.h.in PengConfig.h)

# add_executable(testpeng main.c)

# target_link_libraries(testpeng PUBLIC penglibrary)

# target_include_directories(testpeng PUBLIC "${PROJECT_BINARY_DIR}")

# MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
# MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
# MESSAGE(STATUS "DIR_SRCS" ${DIR_SRCS})
