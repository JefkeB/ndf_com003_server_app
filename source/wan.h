#pragma once

#include <stdio.h>
#include <ctime>

#include <chrono>

#include "Protocol/BaseProtocol.h"
#include "Protocol/ProtocolPacket.h"

#include "SerialPort/rs232.h"



//
//
//
class TWAN : public TBaseProtocol {
	public :
		TProtocolPacketTx txPacket;
		
		TWAN() {
			txPacket.setBuffer(txBuffer, sizeof(txBuffer));
		}


        bool open(const std::string devicePath) {

            // settings for the uart

            const int bdrate = 19200;       	
            const char mode[]={ '8', 'N', '1', 0 };		
			
            return rs232.Open(devicePath, bdrate, mode) == 0;
        }

		
		void close()
		{
			printf("WAN: Close\r\n");
			rs232.Close();
		}

        
	    unsigned char buf[4096];
        void takt()
        {
            auto n = rs232.Poll(buf, 4095);

            if(n > 0) {
                for(int idx = 0; idx < n; idx++)
                {
                    auto kar = buf[idx];
                    //printf("%02x ", kar); fflush(stdout);
                    Receive(kar);
                }
            }
        }
		
		
		void sendPing() {
			txPacket.begin(0x01, 0x05, 0x00, TProtocolPacketTx::TTxMode::SET);
			
			SendPacket(txPacket);
		}
		

		void sendTick() {
			txPacket.begin(0xFF, 0x12, 0x00, TProtocolPacketTx::TTxMode::SET);
			
			
			std::time_t result = std::time(nullptr);

			// next day start position
//			result += (17 * 60 * 60);

			txPacket.addUint32((uint32_t)(result));
			
			SendPacket(txPacket);
			//printf("%ld\r\n", (uint32_t)(result));
			
		}
	
	private :
			RS232 rs232;
            uint8_t txBuffer[128];
            const int cport_nr = -1;

			void HandlePacket(uint8_t *buffer, int size) final {	
			
				TProtocolPacketRx rx(buffer, size);			
				
				printf("wan: packet l:%3d  adr:%3d   ", size, rx.getAddress());
				for(auto i = 0; i < size; i++) {
					printf("%02X ", buffer[i]);
				}
				printf("\r\n");
			}
			
			void Send(uint8_t *buffer, int size) final {
				rs232.SendBuf(buffer, size);
			}

};


extern TWAN protocol;
