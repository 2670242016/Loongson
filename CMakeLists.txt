cmake_minimum_required(VERSION 3.16)    # 限制最低CMake版本

# 设置C语言编译器、C++编译器的路径和名称
set(CMAKE_C_COMPILER /opt/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.3-1/bin/loongarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /opt/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.3-1/bin/loongarch64-linux-gnu-g++)

# 项目名称
project(main)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)      # 获取包含的源文件
file(GLOB TEST ${PROJECT_SOURCE_DIR}/test/*.cpp)    # 获取包含的源文件
include_directories(${PROJECT_SOURCE_DIR}/include)  # 指定所需头文件路径
include_directories(/opt/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.3-1/loongarch64-linux-gnu/sysroot/usr/include/)
include_directories(./include/control)



link_libraries(pthread)

# 指定OpenCV所用头文件路径
include_directories(/home/zmr/Desktop/opencv_4_10_build/include/opencv4/)
# 找到opencv所需的库文件并保存到变量中
find_library(OPENCV_CORE      opencv_core      PATHS /home/zmr/Desktop/opencv_4_10_build/lib)
find_library(OPENCV_IMGPROC   opencv_imgproc   PATHS /home/zmr/Desktop/opencv_4_10_build/lib)
find_library(OPENCV_HIGHGUI   opencv_highgui   PATHS /home/zmr/Desktop/opencv_4_10_build/lib)
find_library(OPENCV_VIDEOIO   opencv_videoio   PATHS /home/zmr/Desktop/opencv_4_10_build/lib)
find_library(OPENCV_IMGCODECS opencv_imgcodecs PATHS /home/zmr/Desktop/opencv_4_10_build/lib)

# set(CMAKE_PREFIX_PATH "/home/zmr/Desktop/opencv_4_10_build") 

# 若需使用静态库，可带上下面两行程序，具体可参考龙邱B站视频教程
# link_directories(${PROJECT_SOURCE_DIR}/a)
# link_libraries(ss)

# 由源文件生成一个可执行程序
add_executable(main main.cpp ${SRC} ${TEST})

# 连接OpenCV动态库
target_link_libraries(main PUBLIC ${OPENCV_CORE} ${OPENCV_IMGPROC} ${OPENCV_HIGHGUI} ${OPENCV_VIDEOIO} ${OPENCV_IMGCODECS})
# 连接多线程pthread库
target_link_libraries(main PUBLIC pthread)