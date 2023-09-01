# LIST(APPEND CMAKE_PROGRAM_PATH "")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION arm)

#SET (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#SET (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#SET (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#include_directories(SYSTEM /usr/local/arm/4.3.1-eabi-armv6/usr/include)

set(CMAKE_AR                arm-none-linux-gnueabihf-ar)
set(CMAKE_ASM_COMPILER      arm-none-linux-gnueabihf-gcc)
set(CMAKE_C_COMPILER        arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER      arm-none-linux-gnueabihf-g++)
set(CMAKE_LINKER            arm-none-linux-gnueabihf-ld)
set(CMAKE_OBJCOPY           arm-none-linux-gnueabihf-objcopy)
set(CMAKE_RANLIB            arm-none-linux-gnueabihf-ranlib)
set(CMAKE_SIZE              arm-none-linux-gnueabihf-size)
set(CMAKE_STIP              arm-none-linux-gnueabihf-strip)
