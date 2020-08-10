cmake_minimum_required(VERSION 3.4.1)

add_library(jni-example-lib SHARED
        jni_callback.c)

# Include libraries needed for hello-jnicallback lib
target_link_libraries(jni-example-lib
        android
        log)
