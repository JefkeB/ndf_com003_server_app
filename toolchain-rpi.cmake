# LIST(APPEND CMAKE_PROGRAM_PATH "")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION arm)

#SET (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#SET (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#SET (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#include_directories(SYSTEM /usr/local/arm/4.3.1-eabi-armv6/usr/include)

set(CMAKE_AR                arm-linux-gnueabihf-ar)
set(CMAKE_ASM_COMPILER      arm-linux-gnueabihf-gcc)
set(CMAKE_C_COMPILER        arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER      arm-linux-gnueabihf-g++)
set(CMAKE_LINKER            arm-linux-gnueabihf-ld)
set(CMAKE_OBJCOPY           arm-linux-gnueabihf-objcopy)
set(CMAKE_RANLIB            arm-linux-gnueabihf-ranlib)
set(CMAKE_SIZE              arm-linux-gnueabihf-size)
set(CMAKE_STIP              arm-linux-gnueabihf-strip)
