cmake_minimum_required(VERSION 3.0)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
  endif()
endif()

include("$ENV{DOLCESDK}/share/dolce.cmake" REQUIRED)


project(VITAEXPERIMENTS-ROOT)

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


ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(FlashScreen)