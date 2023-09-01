add_library(LibProtocol BaseProtocol.cpp CRC16.cpp ProtocolPacket.cpp)

target_include_directories(LibProtocol PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
