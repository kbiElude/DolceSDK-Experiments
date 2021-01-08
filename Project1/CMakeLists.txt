cmake_minimum_required(VERSION 3.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
  endif()
endif()

project(VitaExperiments)

include("${DOLCESDK}/share/dolce.cmake" REQUIRED)

set(VITA_APP_NAME "Vita Experiments")
set(VITA_TITLEID  "ELDK00001")
set(VITA_VERSION  "01.00")

set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -std=c++11")
set(DOLCE_ELF_CREATE_FLAGS "${DOLCE_ELF_CREATE_FLAGS} -h 0x800000")
set(DOLCE_MKSFOEX_FLAGS     "${DOLCE_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

include_directories(${DOLCESDK}/samples/common
                   )

link_directories(${CMAKE_CURRENT_BINARY_DIR}
                )

add_executable(${PROJECT_NAME}
               ${DOLCESDK}/samples/common/debugScreen.c
               src/main.cpp
              )

target_link_libraries(${PROJECT_NAME}
                      libScePiglet_stub
                      SceDisplayUser_stub
                      SceShaccCg_stub
                      SceLibKernel_stub
)

dolce_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})

dolce_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
                VERSION ${VITA_VERSION}
                NAME    ${VITA_APP_NAME}
                FILE sce_sys/icon0.png sce_sys/icon0.png
                FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
                FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
                FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)
