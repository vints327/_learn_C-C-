# 设置CMake的最低版本要求，以确保使用了3.5.0及以上版本的CMake
cmake_minimum_required(VERSION 3.5.0)

# 定义项目名称和版本号，同时指定项目使用的语言
# 这里项目名为helloword，版本号为0.1.0，使用C和C++语言
project(helloword VERSION 0.1.0 LANGUAGES C CXX)

# 添加一个库，名为helloword，源文件为helloword.cpp
# 这里创建的是一个静态库或动态库，具体类型取决于CMake配置和平台约定
add_library(helloword helloword.cpp)