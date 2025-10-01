cmake_minimum_required(VERSION 3.12)  # 支持C++11线程库
project(bimax_task LANGUAGES CXX)

# 1. 全局配置
set(CMAKE_CXX_STANDARD 11)            # 强制C++11标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)         # 禁用编译器扩展

# 2. 编译器警告设置
if(MSVC)
    add_compile_options(/W4 /WX)     # Windows: 警告等级4，视警告为错误
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)  # Linux/Mac严格模式
endif()
# 2. 添加SDK库
add_library(circular_buffer STATIC
    sdk/circular_buffer.cpp
)
target_include_directories(circular_buffer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # 头文件目录
)
# 3. 查找线程库（跨平台关键）
find_package(Threads REQUIRED)

# 4. 生成可执行文件
add_executable(bimax_task
    # 如果拆分文件到多个cpp，需在此列出所有源文件
    src/buffer.cpp
)

# 5. 链接线程库
target_link_libraries(bimax_task  PRIVATE Threads::Threads circular_buffer)

# 6. 安装规则（可选）
install(TARGETS bimax_task
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

# 7. 打包支持（可选）
include(InstallRequiredSystemLibraries)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION "1.0.0")
include(CPack)