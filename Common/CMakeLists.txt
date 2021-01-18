cmake_minimum_required(VERSION 3.0)

project(VITAEXPERIMENTS-COMMON)

ADD_COMPILE_OPTIONS(-Wno-psabi)

include_directories(${DOLCESDK}/samples/common
                    ${DOLCESDK}/arm-dolce-eabi/include
                    ${VITAEXPERIMENTS-COMMON_SOURCE_DIR}/include
                   )

add_library(Common STATIC
            src/baseApp.cpp
            src/EGL/eglInstance.cpp
            src/ES/framebuffer.cpp
            src/ES/framebuffer_create_info.cpp
            src/ES/renderbuffer.cpp
            src/ES/renderbuffer_create_info.cpp
            src/ES/shader.cpp
            src/ES/shader_create_info.cpp
            src/ES/texture.cpp
            src/ES/texture_create_info.cpp
            src/OS/event.cpp
            src/io.cpp
            src/logger.cpp
            src/OS/mutex.cpp
            src/OS/semaphore.cpp
            src/OS/thread.cpp
)

target_link_libraries(Common
                      libScePiglet_stub
                      SceCtrl_stub
                      SceLibc_stub
                      SceLibKernel_stub
                      ScePafStdc_stub
                      SceShaccCg_stub)