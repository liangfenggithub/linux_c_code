cmake_minimum_required(VERSION 3.16)

# 指定编译文件名, 空格分隔
# file(GLOB SRC_FILE *.c) # 建立变量SRC_FILE为目录下.c文件列表 变量  SRC_FILE 可简化下边指令
set(SRC_LIST main.c)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin) # 指定可执行文件的输出路径
#add_definitions("-Wall -g") #在tasks.json中make任务中指定 F5后会报错，暂时未找到原因
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} pthread uci)


