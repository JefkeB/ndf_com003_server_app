#pragma once


#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <string>


#include <termios.h>
//#include <sys/ioctl.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <limits.h>


class RS232 {
    public :

        int Open(const std::string devicePath, int baudrate, const char * mode);
        void Close();

        int Poll(uint8_t * buffer, int size);

        int SendBuf(uint8_t *, int size);
        int SendByte(uint8_t val);

    private :
        int cport = -1;

        struct termios new_port_settings;
        struct termios old_port_settings;
};

#if 0
int RS232_OpenComport(const char * devicePath, int baudrate, const char * mode);
//int RS232_OpenComport(int, int, const char *);
int RS232_PollComport(int, unsigned char *, int);
int RS232_SendByte(int, unsigned char);
int RS232_SendBuf(int, unsigned char *, int);
void RS232_CloseComport(int);
void RS232_cputs(int, const char *);
int RS232_IsDCDEnabled(int);
int RS232_IsCTSEnabled(int);
int RS232_IsDSREnabled(int);
void RS232_enableDTR(int);
void RS232_disableDTR(int);
void RS232_enableRTS(int);
void RS232_disableRTS(int);
#endif

