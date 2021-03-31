cmake_minimum_required(VERSION 3.4.1)

file(GLOB allCpp *.cpp)

add_library(
        native-lib

        SHARED

#        ${allCpp}
        native-lib.cpp
        JNICallback.cpp
        RubyPlayer.cpp
        VideoChannel.cpp
        AudioChannel.cpp
)

find_library(
        log-lib

        log)

# 导入FFmpeg的头文件
include_directories(${CMAKE_SOURCE_DIR}/ffmpeg/include)

# 导入FFmpeg的库文件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/ffmpeg/libs/${CMAKE_ANDROID_ARCH_ABI}")

# 导入Rtmp的库文件
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/rtmp/libs/${CMAKE_ANDROID_ARCH_ABI}")

target_link_libraries(
        native-lib
        ${log-lib}
        -Wl,--start-group
        avcodec avfilter avformat avutil swresample swscale
        -Wl,--end-group
        z
        rtmp
        android
        OpenSLES
)