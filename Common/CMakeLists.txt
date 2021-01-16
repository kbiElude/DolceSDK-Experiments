cmake_minimum_required(VERSION 3.0)

project(VITAEXPERIMENTS-COMMON)

ADD_COMPILE_OPTIONS(-Wno-psabi)

include_directories(${DOLCESDK}/samples/common
                    ${DOLCESDK}/arm-dolce-eabi/include
                    ${VITAEXPERIMENTS-COMMON_SOURCE_DIR}/include
                   )

add_library(Common STATIC
            ../Common/src/baseApp.cpp
            ../Common/src/EGL/eglInstance.cpp
            ../Common/src/OS/event.cpp
            ../Common/src/io.cpp
            ../Common/src/logger.cpp
            ../Common/src/OS/mutex.cpp
            ../Common/src/OS/semaphore.cpp
            ../Common/src/OS/thread.cpp
)

target_link_libraries(Common
                      libScePiglet_stub
                      SceCtrl_stub
                      SceLibc_stub
                      SceLibKernel_stub
                      ScePafStdc_stub
                      SceShaccCg_stub)