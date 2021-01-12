cmake_minimum_required(VERSION 3.0)

project(VITAEXPERIMENTS-COMMON)

set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -std=c++11")
set(DOLCE_ELF_CREATE_FLAGS "${DOLCE_ELF_CREATE_FLAGS} -h 0x800000")
set(DOLCE_MKSFOEX_FLAGS    "${DOLCE_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

#if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    ADD_DEFINITIONS(-DSCE_DBG_LOGGING_ENABLED)
#endif

ADD_COMPILE_OPTIONS(-fno-common -fno-lto -ftree-vectorize -funsafe-math-optimizations -fno-exceptions -fno-rtti)
ADD_DEFINITIONS(-DEGL_NO_X11)
ADD_DEFINITIONS(-D__vita__)
ADD_DEFINITIONS(-D__VITA__)

include_directories(${DOLCESDK}/samples/common
                    ${DOLCESDK}/arm-dolce-eabi/include
                    ${VITAEXPERIMENTS-COMMON_SOURCE_DIR}/include
                   )

add_library(Common STATIC
            ../Common/src/eglInstance.cpp
            ../Common/src/io.cpp
            ../Common/src/mutex.cpp
            ../Common/src/semaphore.cpp
            ../Common/src/thread.cpp
)

target_link_libraries(Common
                      libScePiglet_stub
                      SceCtrl_stub
                      SceLibc_stub
                      SceLibKernel_stub
                      ScePafStdc_stub
                      SceShaccCg_stub)