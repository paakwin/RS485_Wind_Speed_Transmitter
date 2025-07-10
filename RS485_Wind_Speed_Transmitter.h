#ifndef __RS485_WIND_SPEED_TRANSMITTER_H__
#define __RS485_WIND_SPEED_TRANSMITTER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>

// Declare Sfd as an external variable
extern int Sfd;

/**
  @brief Initialize sensor serial port
  @param dev Serial device path
  @return 1: Success, 0: Failure.
*/
char InitSensor(char *dev);

/**
  @brief Delay function
  @param data Delay time in milliseconds
*/
void delayms(int data);

/**
  @brief Modify address
  @param Address1 Current address
  @param Address2 New address
  @return 1: Success, 0: Failure
*/
unsigned char ModifyAddress(unsigned char Address1, unsigned char Address2);

/**
  @brief Add CRC parity bit
  @param buf Data to which parity bits are to be added
  @param len Length of the data
*/
void addedCRC(unsigned char *buf, int len);

/**
  @brief Read wind speed
  @param Address The address to read data from
  @return Wind speed (≥0: Success, -1: Failure)
*/
float readWindSpeed(unsigned char Address);

/**
  @brief Calculate CRC
  @param buf Data to be verified
  @param len Data length
  @return Calculated CRC
*/
unsigned int CRC16_2(unsigned char *buf, int len);

#endif