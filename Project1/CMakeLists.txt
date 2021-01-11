cmake_minimum_required(VERSION 3.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
  endif()
endif()

include("$ENV{DOLCESDK}/share/dolce.cmake" REQUIRED)

project(VitaExperiments)

set(ELF "VitaExperiments.elf")
set(SELF "VitaExperiments.bin")

set(VITA_APP_NAME "Vita Experiments")
set(VITA_TITLEID  "ELDK00001")
set(VITA_VERSION  "01.00")

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
                   )

link_directories(${CMAKE_CURRENT_BINARY_DIR}
                )

add_executable("${ELF}"
               src/app.cpp
               src/eglInstance.cpp
               src/io.cpp
               src/main.cpp
               src/mutex.cpp
               src/semaphore.cpp
               src/thread.cpp
              )

target_link_libraries("${ELF}"
                      libScePiglet_stub
                      SceCtrl_stub
                      SceLibc_stub
                      SceLibKernel_stub
                      ScePafStdc_stub
                      SceShaccCg_stub
)

dolce_create_self("${SELF}" "${ELF}")

dolce_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} "${SELF}"
                NAME    ${VITA_APP_NAME}
                VERSION ${VITA_VERSION}
                FILE "$ENV{DOLCESDK}/arm-dolce-eabi/module/libScePiglet.suprx" module/libScePiglet.suprx
                FILE "$ENV{DOLCESDK}/arm-dolce-eabi/module/libshacccg.suprx"   module/libshacccg.suprx
)
