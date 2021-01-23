cmake_minimum_required(VERSION 3.0)

project(FullScreenPass)

set(ELF "FullScreenPass.elf")
set(SELF "FullScreenPass.bin")

set(VITA_APP_NAME "FullScreenPass")
set(VITA_TITLEID  "ELDK00002")
set(VITA_VERSION  "01.00")


include_directories(${VITAEXPERIMENTS-COMMON_SOURCE_DIR}/include)

add_executable("${ELF}"
               src/app.cpp
               src/main.cpp
              )

target_link_libraries("${ELF}"
                      Common
)

dolce_create_self("${SELF}" "${ELF}")

dolce_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} "${SELF}"
                NAME    ${VITA_APP_NAME}
                VERSION ${VITA_VERSION}
                FILE "$ENV{DOLCESDK}/arm-dolce-eabi/module/libScePiglet.suprx" module/libScePiglet.suprx
                FILE "$ENV{DOLCESDK}/arm-dolce-eabi/module/libshacccg.suprx"   module/libshacccg.suprx
                FILE "${VITAEXPERIMENTS-COMMON_SOURCE_DIR}/meiryo_36.bmp" ./meiryo_36.bmp
                FILE "${VITAEXPERIMENTS-COMMON_SOURCE_DIR}/meiryo_36.dat" ./meiryo_36.dat
)
