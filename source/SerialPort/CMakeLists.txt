add_library(LibSerialPort rs232.cpp)

target_include_directories(LibSerialPort PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")