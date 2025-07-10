#include "RS485_Wind_Speed_Transmitter.h"
#include <sys/time.h>
#include <fcntl.h>

// Declare Sfd as a global variable
int Sfd = -1;

int main() {
  if (!InitSensor("/dev/ttyUSB0")) {
    printf("Failed to initialize sensor.\n");
    return 1;
  }

  unsigned char Address = 0x04;
  float WindSpeed = readWindSpeed(Address);
  if (WindSpeed >= 0) {
    printf("Wind Speed: %.2f m/s\n", WindSpeed);
  } else {
    printf("Failed to read wind speed.\n");
  }

  return 0;
}

float readWindSpeed(unsigned char Address) {
  unsigned char Data[7] = { 0 }; // Buffer to store received data
  unsigned char COM[8] = { 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00 }; // Command to send
  char ret = 0; // Flag to indicate success
  float WindSpeed = 0; // Variable to store wind speed
  struct timeval curr, curr2; // Time structures for timeout calculation
  long mtime, useconds, seconds; // Variables for time calculations

  gettimeofday(&curr, NULL); // Get the current time

  char ch = 0; // Variable to store received byte
  COM[0] = Address; // Set the address in the command
  addedCRC(COM, 6); // Add CRC to the command

  printf("Sending readWindSpeed command:\n");
  for (int i = 0; i < 8; i++) {
    printf("%02X ", COM[i]);
  }
  printf("\n");

  write(Sfd, COM, 8); // Send the command
  while (!ret) {
    gettimeofday(&curr2, NULL); // Get the current time
    seconds = curr2.tv_sec - curr.tv_sec;
    useconds = curr2.tv_usec - curr.tv_usec;
    mtime = ((seconds)*1000 + useconds / 1000.0) + 0.5;

    if (mtime > 1000) { // Timeout after 1 second
      WindSpeed = -1;
      break;
    }

    delayms(10); // Wait for 10ms
    if (read(Sfd, &ch, 1) == 1) { // Read a byte from the serial port
      printf("Received byte: %02X\n", ch);
      if (ch == Address) { // Check if the byte matches the address
        Data[0] = ch;
        if (read(Sfd, &ch, 1) == 1) {
          if (ch == 0x03) { // Check if the function code is correct
            Data[1] = ch;
            if (read(Sfd, &ch, 1) == 1) {
              if (ch == 0x02) { // Check if the data length is correct
                Data[2] = ch;
                if (read(Sfd, &Data[3], 4) == 4) { // Read the remaining data
                  unsigned int calculatedCRC = CRC16_2(Data, 5);
                  printf("Calculated CRC: %04X, Received CRC: %04X\n", calculatedCRC, (Data[5] << 8) | Data[6]);
                  if (calculatedCRC == (Data[5] << 8 | Data[6])) { // Verify CRC
                    ret = 1;
                    WindSpeed = (Data[3] * 256 + Data[4]) / 10.00; // Calculate wind speed
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return WindSpeed;
}