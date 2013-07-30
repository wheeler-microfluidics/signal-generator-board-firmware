#include "Arduino.h"
#include "signal_generator_board.h"
#include <avr/eeprom.h>
#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Memory.h>

#define P(str) (strcpy_P(p_buffer, PSTR(str)), p_buffer)
char p_buffer[100];

SignalGeneratorClass SignalGeneratorBoard;

const float SignalGeneratorClass::LOG_F_STEP = (log10(SignalGeneratorClass::F_MAX)-log10(SignalGeneratorClass::F_MIN))/255;

prog_char SignalGeneratorClass::PROTOCOL_NAME_[] PROGMEM = "Signal Generator Protocol";
prog_char SignalGeneratorClass::PROTOCOL_VERSION_[] PROGMEM = "0.1";
prog_char SignalGeneratorClass::MANUFACTURER_[] PROGMEM = "Wheeler Microfluidics Lab";
prog_char SignalGeneratorClass::NAME_[] PROGMEM = "Signal Generator Board";
prog_char SignalGeneratorClass::HARDWARE_VERSION_[] PROGMEM = "1.0";
prog_char SignalGeneratorClass::SOFTWARE_VERSION_[] PROGMEM = "0.1";
prog_char SignalGeneratorClass::URL_[] PROGMEM = "http://microfluidics.utoronto.ca/dropbot";

void HandleWireRequest() {
  //Serial.println(P("HandleWireRequest()"));
  if(SignalGeneratorBoard.send_payload_length_) {
    Wire.write((uint8_t*)&SignalGeneratorBoard.bytes_written_,
               sizeof(SignalGeneratorBoard.bytes_written_));
    SignalGeneratorBoard.send_payload_length_ = false;
  } else {
    Wire.write((uint8_t*)SignalGeneratorBoard.buffer_, SignalGeneratorBoard.bytes_written_);
  }
}

void HandleWireReceive(int n_bytes) {
  //Serial.println(P("HandleWireReceive()"));
  SignalGeneratorBoard.cmd_ = Wire.read();
  n_bytes--;
  if(n_bytes<=SignalGeneratorClass::MAX_PAYLOAD_LENGTH) {
    for(int i=0; i<n_bytes; i++) {
      SignalGeneratorBoard.buffer_[i] = Wire.read();
    }
  }
  SignalGeneratorBoard.ProcessWireCommand();
}

void SignalGeneratorClass::begin() {
  pinMode(LTC6903_SS_PIN, OUTPUT);
  pinMode(AD5206_SS_PIN, OUTPUT);
  pinMode(FREQ_RANGE_PIN, OUTPUT);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  Serial.begin(115200);
  LoadConfig();
  DumpConfig();
  Wire.onRequest(HandleWireRequest);
  Wire.onReceive(HandleWireReceive);
  waveform_frequency_ = 1000;
  waveform_voltage_ = 2.5;
  Serial.print(P("ram=")); Serial.println(ram_size(), DEC);
  Serial.print(P(".data=")); Serial.println(data_size(), DEC);
  Serial.print(P(".bss=")); Serial.println(bss_size(), DEC);
  Serial.print(P("heap=")); Serial.println(heap_size(), DEC);
  Serial.print(P("stack=")); Serial.println(stack_size(), DEC);
  Serial.print(P("free memory=")); Serial.println(free_memory(), DEC);
}

void SignalGeneratorClass::Listen() {
  ProcessSerialInput();
}

void SignalGeneratorClass::Serialize(const uint8_t* u,const uint16_t size) {
  for(uint16_t i=0;i<size;i++) {
    buffer_[bytes_written_+i]=u[i];
  }
  bytes_written_+=size;
}

float SignalGeneratorClass::ReadFloat() {
  // TODO check that we're not reading past the end of the buffer
  bytes_read_ += sizeof(float);
  return *(float*)(buffer_+bytes_read_-sizeof(float));
}

uint8_t SignalGeneratorClass::ReadUint8() {
  // TODO check that we're not reading past the end of the buffer
  bytes_read_ += sizeof(uint8_t);
  return *(uint8_t*)(buffer_+bytes_read_-sizeof(uint8_t));
}

void SignalGeneratorClass::DumpConfig() {
  Serial.println(String(name()) + " v" + String(hardware_version()));
  Serial.println(P("Firmware v") + String(software_version()));
  Serial.println(url());
  Serial.println(P("config_version=") + VersionString(ConfigVersion()));
  Serial.println(P("i2c_address=") + String(config_settings_.i2c_address, DEC));
  for(uint8_t i=0; i<6; i++) {
    Serial.println(P("pot[") + String(i) + "]=" + String(config_settings_.pot[i], DEC));
  }
}

void SignalGeneratorClass::ProcessSerialInput() {
  if(Serial.available()) {
    byte len = Serial.readBytesUntil('\n', buffer_, sizeof(buffer_));
    buffer_[len]=0;
    char* substr;

    if(strcmp(buffer_, P("reset_config()"))==0) {
      LoadConfig(true);
      DumpConfig();
      return;
    }

    if(strcmp(buffer_, P("config()"))==0) {
      DumpConfig();
      return;
    }

    if(strcmp(buffer_, P("i2c_address()"))==0) {
      Serial.println(P("i2c_address=") + String(config_settings_.i2c_address, DEC));
      return;
    }

    substr = strstr(buffer_, P("set_i2c_address("));
    if(substr && substr==buffer_ && substr[strlen(substr)-1] == ')') {
      buffer_[strlen(substr)-1]=0;
      set_i2c_address(atoi(substr+sizeof(P("set_i2c_address("))-1));
      return;
    }

    substr = strstr(buffer_, P("pot("));
    if(substr && substr==buffer_ && substr[strlen(substr)-1] == ')') {
      substr = strstr(buffer_, "(") + 1;
      int32_t index;
      if(NextInt(substr, index)) {
        if(index>=0 && index<6) {
          Serial.println(P("pot[") + String(index) + "]=" + String(config_settings_.pot[index], DEC));
          return;
        }
      }
    }

    substr = strstr(buffer_, P("set_pot("));
    if(substr && substr==buffer_ && substr[strlen(substr)-1] == ')') {
      substr = strstr(buffer_, "(") + 1;
      int32_t index;
      int32_t value;
      if(NextInt(substr, index)==false) {
        Error(1);
        return;
      }
      if(NextInt(substr, value)==false) {
        Error(1);
        return;
      }
      if(index>=0 && index<6 && value>=0 & value < 256) {
        set_pot((uint8_t)index, (uint8_t)value);
        return;
      } else {
        Error(1);
        return;
      }
    }
    
    if(strcmp(buffer_, P("waveform_frequency()"))==0) {
      Serial.print(P("waveform_frequency="));
      Serial.println(waveform_frequency_);
      return;
    }

    substr = strstr(buffer_, P("set_waveform_frequency("));
    if(substr && substr==buffer_ && substr[strlen(substr)-1] == ')') {
      buffer_[strlen(substr)-1]=0;
      set_waveform_frequency(atof(substr+sizeof(P("set_waveform_frequency("))-1));
      return;
    }

    if(strcmp(buffer_, P("waveform_voltage()"))==0) {
      Serial.print(P("waveform_voltage="));
      Serial.println(waveform_voltage_);
      return;
    }

    substr = strstr(buffer_, P("set_waveform_voltage("));
    if(substr && substr==buffer_ && substr[strlen(substr)-1] == ')') {
      buffer_[strlen(substr)-1]=0;
      waveform_voltage_ = atof(substr+sizeof(P("set_waveform_voltage("))-1);
      Serial.print(P("waveform_voltage="));
      Serial.println(waveform_voltage_);
      return;
    }

    if(strcmp(buffer_, P("vout_pk_pk()"))==0) {
      vout_pk_pk();
      return;
    }
    Error(1);
  }
}

void SignalGeneratorClass::ProcessWireCommand() {
  bytes_written_ = 0;
  bytes_read_ = 0;
  send_payload_length_ = true;
  return_code_ = RETURN_GENERAL_ERROR;
  switch(cmd_) {
  case CMD_GET_WAVEFORM_FREQUENCY:
    Serialize(&waveform_frequency_, sizeof(waveform_frequency_));
    return_code_ = RETURN_OK;
    break;
  case CMD_SET_WAVEFORM_FREQUENCY:
    set_waveform_frequency(ReadFloat());
    return_code_ = RETURN_OK;
    break;
  case CMD_GET_WAVEFORM_VOLTAGE:
    Serialize(&waveform_voltage_, sizeof(waveform_voltage_));
    return_code_ = RETURN_OK;
    break;
  case CMD_SET_WAVEFORM_VOLTAGE:
    set_waveform_voltage(ReadFloat());
    return_code_ = RETURN_OK;
    break;
  case CMD_GET_POT_INDEX: {
    uint8_t index = ReadUint8();
    Serialize(&config_settings_.pot[index], sizeof(config_settings_.pot[index]));
    return_code_ = RETURN_OK;
    break;
  }
  case CMD_SET_POT_INDEX: {
    uint8_t index = ReadUint8();
    uint8_t value = ReadUint8();
    set_pot(index, value);
    return_code_ = RETURN_OK;
    break;
  }
  case CMD_MEASURE_VOUT_PK_PK: {
    float v = vout_pk_pk();
    Serialize(&v, sizeof(v));
    return_code_ = RETURN_OK;
  }
  default:
    break;
  }
  Serialize(&return_code_, sizeof(return_code_));
}

void SignalGeneratorClass::Error(uint8_t code) {
  Serial.println(P("Error ") + String(code, DEC));
}

boolean SignalGeneratorClass::NextInt(char* &str, int32_t &value) {
  char* end = strstr(str, ",");
  if(end==NULL) {
    end = strstr(str, ")");
  }
  if(end==NULL) {
    return false;
  }
  char num_str[end-str+1];
  memcpy(num_str, str, end-str);
  num_str[end-str] = 0;
  str = end+1;
  value = atoi(num_str);
  return true;
}

String SignalGeneratorClass::VersionString(version_t version) {
  return String(version.major) + "." + String(version.minor) + "." + String(version.micro);
}

SignalGeneratorClass::version_t SignalGeneratorClass::ConfigVersion() {
  version_t config_version;
  eeprom_read_block((void*)&config_version, (void*)EEPROM_CONFIG_SETTINGS, sizeof(version_t));
  return config_version;
}

void SignalGeneratorClass::LoadConfig(bool use_defaults) {
  eeprom_read_block((void*)&config_settings_, (void*)EEPROM_CONFIG_SETTINGS, sizeof(config_settings_t));

  // If we're not at the expected version by the end of the upgrade path,
  // set everything to default values.
  if(!(config_settings_.version.major==0 &&
     config_settings_.version.minor==0 &&
     config_settings_.version.micro==0) || use_defaults) {

    config_settings_.version.major=0;
    config_settings_.version.minor=0;
    config_settings_.version.micro=0;
    config_settings_.i2c_address = 10;
    config_settings_.pot[0] = 128;
    config_settings_.pot[1] = 128;
    config_settings_.pot[2] = 128;
    config_settings_.pot[3] = 128;
    config_settings_.pot[4] = 128;
    config_settings_.pot[5] = 128;
    SaveConfig();
  }
  Wire.begin(config_settings_.i2c_address);
  for(uint8_t i=0; i<6; i++) {
    set_pot(i, config_settings_.pot[i], false);
  }
}

void SignalGeneratorClass::SaveConfig() {
  eeprom_write_block((void*)&config_settings_, (void*)EEPROM_CONFIG_SETTINGS, sizeof(config_settings_t));
}


void SignalGeneratorClass::set_pot(byte index, byte value, boolean display_msg) {
  // take the SS pin low to select the chip:
  digitalWrite(AD5206_SS_PIN, LOW);
  SPI.transfer(index);
  SPI.transfer(value);
  // take the SS pin high to de-select the chip:
  digitalWrite(AD5206_SS_PIN, HIGH);
  config_settings_.pot[index] = (uint8_t)value;
  SaveConfig();
  if(display_msg) {
    Serial.println(P("pot[") + String(index) + "]=" + String(config_settings_.pot[index], DEC));
  }
}


void SignalGeneratorClass::set_waveform_voltage(float vrms) {
  waveform_voltage_ = vrms;
  Serial.print(P("waveform_voltage="));
  Serial.println(waveform_voltage_);
}

void SignalGeneratorClass::set_waveform_frequency(float freq) {
  waveform_frequency_ = freq;
  Serial.print(P("waveform_frequency="));
  Serial.println(waveform_frequency_);

  float scaling = 32;
  
  // frequency=2^oct*2078/(2-dac/1024)
  uint8_t oct = 3.322*log(freq*scaling/1039)/log(10);
  uint16_t dac = round(2048-(2078*pow(2, 10+oct))/(freq*scaling));
  uint8_t cnf = 2; // CLK on, /CLK off
  // msb = OCT3 OCT2 OCT1 OCT0 DAC9 DAC8 DAC7 DAC6
  uint8_t msb = (oct << 4) | (dac >> 6);
  // lsb =  DAC5 DAC4 DAC3 DAC2 DAC1 DAC0 CNF1 CNF0
  uint8_t lsb = (dac << 2) | cnf;
  
  digitalWrite(LTC6903_SS_PIN, LOW);
  SPI.transfer(msb);
  SPI.transfer(lsb);
  digitalWrite(LTC6903_SS_PIN, HIGH);

  if(freq < 10e3) {
    digitalWrite(FREQ_RANGE_PIN, HIGH);
  } else {
    digitalWrite(FREQ_RANGE_PIN, LOW);
  }
  
  byte index = (log10(freq)-log10(F_MIN))/LOG_F_STEP; 


/*
  Serial.print("R1[");
  Serial.print(index, DEC);
  Serial.print("] = ");
  Serial.println(pgm_read_byte_near(R1 + index), DEC);

  Serial.print("R2[");
  Serial.print(index, DEC);
  Serial.print("] = ");
  Serial.println(pgm_read_byte_near(R2 + index), DEC);
  
  set_pot(1, pgm_read_byte_near(R1 + index));
  set_pot(2, pgm_read_byte_near(R2 + index));
*/  
  
}

void SignalGeneratorClass::set_i2c_address(uint8_t address) {
  config_settings_.i2c_address = address;
  Wire.begin(config_settings_.i2c_address);
  Serial.println(P("i2c_address=") + String(config_settings_.i2c_address, DEC));
  SaveConfig();
}

float SignalGeneratorClass::vout_pk_pk() {
  uint16_t min_v = 1024;
  uint16_t max_v = 0;
  uint16_t v;
  
  for(int i=0; i<1000; i++) {
    v = analogRead(0);
    if(v<min_v) {
      min_v = v;
    }
    if(v>max_v) {
      max_v = v;
    }
  }
  float vout = (float)(max_v-min_v) * (5.0/1024.0);
  Serial.print(P("vout_pk_pk()="));
  Serial.println(vout);
  return vout;
}

