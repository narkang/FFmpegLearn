
cmake_minimum_required(VERSION 3.4.1)

# 导入FFmpeg的头文件
include_directories(${CMAKE_SOURCE_DIR}/ffmpeg/include)

# 导入FFmpeg的库文件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/ffmpeg/${CMAKE_ANDROID_ARCH_ABI}")

aux_source_directory(. SOURCE)  #导入当前目录的源文件
#add_library(
#        native-lib
#        SHARED
#        native-lib.cpp Audio.cpp CallJava.cpp FFmpeg.cpp Queue.cpp Playstatus.cpp)

add_library(
        native-lib
        SHARED
        ${SOURCE}
)
find_library(
        log-lib
        log )

target_link_libraries(
        native-lib
        -Wl,--start-group
        avcodec avfilter avformat avutil swresample swscale
        -Wl,--end-group
        ${log-lib}
        OpenSLES
        android)