# LIST(APPEND CMAKE_PROGRAM_PATH "")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION arm)

#SET (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#SET (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#SET (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#include_directories(SYSTEM /usr/local/arm/4.3.1-eabi-armv6/usr/include)

set(CMAKE_AR                armv6-rpi-linux-gnueabihf-ar)
set(CMAKE_ASM_COMPILER      armv6-rpi-linux-gnueabihf-gcc)
set(CMAKE_C_COMPILER        armv6-rpi-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER      armv6-rpi-linux-gnueabihf-g++)
set(CMAKE_LINKER            armv6-rpi-linux-gnueabihf-ld)
set(CMAKE_OBJCOPY           armv6-rpi-linux-gnueabihf-objcopy)
set(CMAKE_RANLIB            armv6-rpi-linux-gnueabihf-ranlib)
set(CMAKE_SIZE              armv6-rpi-linux-gnueabihf-size)
set(CMAKE_STIP              armv6-rpi-linux-gnueabihf-strip)
