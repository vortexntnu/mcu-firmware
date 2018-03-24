#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include "ros/ros.h"
#include "vortex_msgs/Pwm.h"

#include <stdio.h>
#include <stdlib.h>     //
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions      
#include <iostream>

#define MAX_PAYLOAD_SIZE 16
#define MAX_MSG_SIZE 21

#define MAGIC_START_BYTE_INDEX 0
#define MAGIC_STOP_BYTE_INDEX 18 // 18 without crc bytes, 20 with crc bytes

#define MSG_PAYLOAD_START_INDEX 2
#define MSG_PAYLOAD_STOP_INDEX 17

#define MSG_TYPE_INDEX 1

class SerialCom
{
        private:
                int m_dev;
                char m_read_buffer[256];
                char m_cmd[MAX_MSG_SIZE] = {0};
        public:
                void callback(const vortex_msgs::Pwm& msg);
                int serial_write(char* cmd, int cmd_size);
                int serial_read();
                SerialCom();
                ~SerialCom();
};

#endif // SERIAL_DEVICE_H