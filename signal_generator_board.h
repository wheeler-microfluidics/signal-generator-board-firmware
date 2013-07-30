#ifndef _SIGNAL_GENERATOR_BOARD_H_
#define _SIGNAL_GENERATOR_BOARD_H_

#include "Arduino.h"
#include <stdint.h>

// Arduino digital pin 11 (MOSI) to LTC6903 pin 2
// Arduino digital pin 13 (SCK) to LTC6903 pin 3
// Arduino digital pin 2 to LTC6903 pin 4

class SignalGeneratorClass {
public:
  struct version_t {
    uint16_t major;
    uint16_t minor;
    uint16_t micro;
  };
  
  struct config_settings_t {
    version_t version;
    uint8_t i2c_address;
    uint8_t pot[6];
  };
  
  static const uint8_t CMD_GET_WAVEFORM_VOLTAGE =           0xA7;
  static const uint8_t CMD_SET_WAVEFORM_VOLTAGE =           0xA8;
  static const uint8_t CMD_GET_WAVEFORM_FREQUENCY =         0xA9;
  static const uint8_t CMD_SET_WAVEFORM_FREQUENCY =         0xAA;
  static const uint8_t CMD_GET_POT_INDEX =                  0xB7;
  static const uint8_t CMD_SET_POT_INDEX =                  0xB8;
  static const uint8_t CMD_MEASURE_VOUT_PK_PK =             0xB9;
  
  // reserved return codes
  static const uint8_t RETURN_OK =                    0x00;
  static const uint8_t RETURN_GENERAL_ERROR =         0x01;
  static const uint8_t RETURN_UNKNOWN_COMMAND =       0x02;
  static const uint8_t RETURN_TIMEOUT =               0x03;
  static const uint8_t RETURN_NOT_CONNECTED =         0x04;
  static const uint8_t RETURN_BAD_INDEX =             0x05;
  static const uint8_t RETURN_BAD_PACKET_SIZE =       0x06;
  static const uint8_t RETURN_BAD_CRC =               0x07;
  static const uint8_t RETURN_BAD_VALUE =             0x08;
  static const uint8_t RETURN_MAX_PAYLOAD_EXCEEDED =  0x09;
  
  static const uint16_t MAX_PAYLOAD_LENGTH = 100;  
  static const uint32_t BAUD_RATE = 115200;  
  static const int LTC6903_SS_PIN = 2;
  static const int AD5206_SS_PIN = 3;
  static const int FREQ_RANGE_PIN = 4;
  static const float F_MIN = 100.0;
  static const float F_MAX = 25e3;
  static const float LOG_F_STEP;
//  PROGMEM prog_uchar R1[] = { 254, 250, 244, 234, 243, 212, 203, 245, 220, 202, 168, 206, 185, 161, 149, 147, 142, 209, 213, 131, 212, 203, 199, 158, 219, 242, 111, 132, 233, 94, 124, 88, 76, 175, 157, 193, 97, 177, 165, 79, 71, 77, 111, 100, 104, 147, 62, 62, 83, 118, 66, 105, 120, 50, 51, 66, 71, 54, 62, 50, 52, 52, 43, 59, 83, 41, 54, 47, 45, 52, 49, 91, 87, 54, 31, 42, 41, 68, 41, 36, 58, 59, 27, 47, 59, 26, 56, 50, 35, 51, 28, 35, 55, 34, 41, 53, 29, 36, 19, 47, 45, 21, 28, 25, 17, 16, 16, 30, 40, 17, 23, 23, 23, 27, 31, 34, 25, 34, 28, 25, 17, 18, 11, 15, 15, 17, 23, 22, 23, 11, 11, 13, 21, 17, 14, 11, 20, 9, 11, 13, 15, 16, 8, 12, 18, 11, 14, 11, 7, 6, 7, 13, 9, 7, 9, 13, 6, 6, 11, 11, 7, 12, 7, 11, 5, 5, 7, 9, 9, 7, 4, 4, 4, 4, 8, 8, 5, 5, 4, 5, 5, 5, 8, 7, 7, 7, 4, 3, 3, 3, 4, 4, 3, 3, 4, 4, 4, 4, 4, 2, 2, 2, 2, 3, 3, 3, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
//  PROGMEM prog_uchar R2[] = { 255, 249, 244, 244, 225, 247, 247, 196, 209, 218, 251, 196, 209, 230, 238, 231, 229, 149, 140, 218, 129, 129, 126, 152, 105, 91, 190, 153, 83, 197, 143, 193, 214, 89, 95, 74, 141, 74, 76, 152, 162, 143, 95, 101, 93, 63, 143, 137, 98, 66, 113, 68, 57, 131, 123, 91, 81, 102, 85, 101, 93, 89, 103, 72, 49, 95, 69, 76, 76, 63, 64, 33, 33, 51, 85, 60, 59, 34, 54, 59, 35, 33, 69, 38, 29, 63, 28, 30, 41, 27, 47, 36, 22, 34, 27, 20, 35, 27, 49, 19, 19, 39, 28, 30, 42, 43, 41, 21, 15, 34, 24, 23, 22, 18, 15, 13, 17, 12, 14, 15, 21, 19, 30, 21, 20, 17, 12, 12, 11, 22, 21, 17, 10, 12, 14, 17, 9, 19, 15, 12, 10, 9, 17, 11, 7, 11, 8, 10, 15, 17, 14, 7, 10, 12, 9, 6, 12, 12, 6, 6, 9, 5, 8, 5, 10, 10, 7, 5, 5, 6, 10, 9, 9, 9, 4, 4, 6, 6, 7, 5, 5, 5, 3, 3, 3, 3, 5, 6, 6, 6, 4, 4, 5, 5, 3, 3, 3, 3, 3, 5, 5, 5, 5, 3, 3, 3, 2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  
  static const uint16_t EEPROM_CONFIG_SETTINGS = 0;

  void begin();
  // local accessors
  const char* name() { return prog_string(NAME_); }
  const char* hardware_version() { return prog_string(HARDWARE_VERSION_); }
  const char* url() { return prog_string(URL_); }
  const char* software_version() { return prog_string(SOFTWARE_VERSION_); }
  const char* protocol_name() { return prog_string(PROTOCOL_NAME_); }
  const char* protocol_version() { return prog_string(PROTOCOL_VERSION_); }
  const char* manufacturer() { return prog_string(MANUFACTURER_); }
  const char* prog_string(prog_char* str) { strcpy_P(buffer_, str); return buffer_; }

  void Listen();
  void set_pot(byte address, byte level, boolean display_msg=true);
  void set_waveform_frequency(float freq);
  void set_waveform_voltage(float vrms);
  void set_i2c_address(uint8_t address);
  float vout_pk_pk();
  version_t ConfigVersion();
  void ProcessWireCommand();
  
  boolean send_payload_length_;
  uint8_t cmd_;
  uint16_t bytes_read_; // bytes that have been read (by Read methods)
  uint16_t bytes_written_; // bytes that have been written (by Serialize method)
  char buffer_[MAX_PAYLOAD_LENGTH];
private:
  template<typename T> void Serialize(T data, uint16_t size) {
    Serialize((const uint8_t*)data, size); }
  void Serialize(const uint8_t* u, const uint16_t size);
  const char* ReadString();
  uint16_t ReadUint16();
  int16_t ReadInt16();
  uint8_t ReadUint8();
  int8_t ReadInt8();
  float ReadFloat();  

  String VersionString(version_t version);
  boolean NextInt(char* &str, int32_t &value);
  void ProcessSerialInput();
  void Error(uint8_t code);
  void DumpConfig();
  void LoadConfig(bool use_defaults=false);
  void SaveConfig();

  static prog_char SOFTWARE_VERSION_[] PROGMEM;
  static prog_char NAME_[] PROGMEM;
  static prog_char HARDWARE_VERSION_[] PROGMEM;
  static prog_char MANUFACTURER_[] PROGMEM;
  static prog_char URL_[] PROGMEM;
  static prog_char PROTOCOL_NAME_[] PROGMEM;
  static prog_char PROTOCOL_VERSION_[] PROGMEM;
  config_settings_t config_settings_;
  float waveform_frequency_;
  float waveform_voltage_;
  uint8_t return_code_;
};

extern SignalGeneratorClass SignalGeneratorBoard;

#endif // _SIGNAL_GENERATOR_BOARD_H_
