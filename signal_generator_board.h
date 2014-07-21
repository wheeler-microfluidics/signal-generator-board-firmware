#ifndef _SIGNAL_GENERATOR_BOARD_H_
#define _SIGNAL_GENERATOR_BOARD_H_

#include "Arduino.h"
#include <stdint.h>

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
    float hf_amplitude_correction;
    float max_voltage_rms;
  };

  static const uint8_t CMD_GET_WAVEFORM_VOLTAGE =           0xA7;
  static const uint8_t CMD_SET_WAVEFORM_VOLTAGE =           0xA8;
  static const uint8_t CMD_GET_WAVEFORM_FREQUENCY =         0xA9;
  static const uint8_t CMD_SET_WAVEFORM_FREQUENCY =         0xAA;
  static const uint8_t CMD_GET_POT_VALUE =                  0xB7;
  static const uint8_t CMD_SET_POT_VALUE =                  0xB8;
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
  static const int S_SCK_PIN = 5;
  static const int S_MOSI_PIN = 6;

  static const float F_MIN = 100.0;
  static const float F_MAX = 10e3;
  static const float R_MAX = 100e3;
  static const float C1 = 47e-9;
  static const float C2 = 6.8e-9;
  static const float LOG_F_STEP;
  static const char R1_INDEX[] PROGMEM;
  static const char R2_INDEX[] PROGMEM;
  static const char R4_INDEX[] PROGMEM;
  static const char R5_INDEX[] PROGMEM;

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
  const char* prog_string(const char* str) { strcpy_P(buffer_, str); return buffer_; }

  void Listen();
  void set_pot(byte address, byte level, boolean save_to_eeprom=false, boolean display_msg=true);
  void set_waveform_frequency(float freq);
  uint8_t set_waveform_voltage(float vrms);
  void set_i2c_address(uint8_t address);
  void set_hf_amplitude_correction(float correction);
  float vout_pk_pk();
  version_t ConfigVersion();
  void ProcessWireCommand();

  boolean send_payload_length_;
  uint8_t cmd_;
  uint16_t bytes_read_; // bytes that have been read (by Read methods)
  uint16_t bytes_written_; // bytes that have been written (by Serialize method)
  uint16_t payload_length_;
  boolean wire_command_received_;
  char buffer_[MAX_PAYLOAD_LENGTH];
  char p_buffer[100];
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

  static const char SOFTWARE_VERSION_[] PROGMEM;
  static const char NAME_[] PROGMEM;
  static const char HARDWARE_VERSION_[] PROGMEM;
  static const char MANUFACTURER_[] PROGMEM;
  static const char URL_[] PROGMEM;
  static const char PROTOCOL_NAME_[] PROGMEM;
  static const char PROTOCOL_VERSION_[] PROGMEM;
  config_settings_t config_settings_;
  float waveform_frequency_;
  float waveform_voltage_;
  uint8_t pot_[6];
  uint8_t return_code_;
};

extern SignalGeneratorClass SignalGeneratorBoard;

#endif // _SIGNAL_GENERATOR_BOARD_H_
