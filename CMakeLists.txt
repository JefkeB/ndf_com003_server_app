cmake_minimum_required(VERSION 3.15)

# what is the target to build ?
option(TARGET_PC "Using PC toolchain & build for PC" OFF)
option(TARGET_RPI "Using rpi toolchain & build for embedded"  OFF)
option(TARGET_DEBUG "Debug on" OFF)

set(CMAKE_VERBOSE_MAKEFILE OFF)

# will be selected by calling cmake with the -DTARGET_PC=ON
#set(TARGET_PC OFF)
#set(TARGET_RPI OFF)

if(${TARGET_PC})

    message("*** Using x64 toolchain & build for Simulator ***")
    set(CMAKE_TOOLCHAIN_FILE toolchain-x64.cmake)

elseif(${TARGET_RPI})

    message("*** Using rpi toolchain ***")
    #set(CMAKE_TOOLCHAIN_FILE toolchain-rpi.cmake)
    #set(CMAKE_TOOLCHAIN_FILE toolchain-rpi.2.cmake)
    set(CMAKE_TOOLCHAIN_FILE toolchain-rpi.3.cmake)
    set(TARGET_FB ON)

endif()

#
if(TARGET_DEBUG)
    message("*** Build with DEBUG ***")
endif()
 
message("***")


# the project
project(collector LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

#message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/null.cpp "")
add_executable(collector ${CMAKE_CURRENT_BINARY_DIR}/null.cpp)


##
add_subdirectory(source/SunRise)
add_subdirectory(source/SolarPosition)
add_subdirectory(source/Timer)
add_subdirectory(source/Protocol)
add_subdirectory(source/SerialPort)
add_subdirectory(source/Ini)
add_subdirectory(source)

##
target_link_libraries(
    collector 

    -Wl,--whole-archive
    LibSolarPosition
    LibSunRise
    LibTimer
    LibProtocol
    LibSerialPort

    
    pthread
    rt

    -Wl,--no-whole-archive
    LibMain
)

##
target_compile_options(collector
PRIVATE
    -MMD
    -MP
    -ffunction-sections
    -fdata-sections   
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>         
)

# move exe
add_custom_command(TARGET collector 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:collector> .
                )


# copy example config
add_custom_command(TARGET collector 
                   POST_BUILD
                   #COMMAND pwd
                   #copy source/collector.cfg> build/collector.cfg
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/source/collector.cfg ${CMAKE_CURRENT_SOURCE_DIR}/build/collector.cfg 
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMENT "Copy ini example"
                )                   
