#include "SignalGeneratorBoard.h"
#include "Memory.h"
#include <avr/eeprom.h>
#include <SPI.h>

#define P(str) (strcpy_P(p_buffer_, PSTR(str)), p_buffer_)

SignalGeneratorClass SignalGeneratorBoard;

const float SignalGeneratorClass::LOG_F_STEP = (log10(SignalGeneratorClass::F_MAX)-log10(SignalGeneratorClass::F_MIN))/255;

const char BaseNode::PROTOCOL_NAME_[] PROGMEM = "Extension module protocol";
const char BaseNode::PROTOCOL_VERSION_[] PROGMEM = "0.1";
const char BaseNode::MANUFACTURER_[] PROGMEM = "Wheeler Microfluidics Lab";
const char BaseNode::NAME_[] PROGMEM = "Signal Generator Board";
const char BaseNode::HARDWARE_VERSION_[] PROGMEM = ___HARDWARE_VERSION___;
const char BaseNode::SOFTWARE_VERSION_[] PROGMEM = ___SOFTWARE_VERSION___;
const char BaseNode::URL_[] PROGMEM = "http://microfluidics.utoronto.ca/dropbot";

const char SignalGeneratorClass::R1_INDEX[] PROGMEM = { 255, 255, 255, 255, 251, 247, 242, 249, 234, 239, 237, 203, 242, 185, 186, 189, 166, 202, 161, 146, 188, 138, 131, 197, 239, 181, 133, 122, 217, 113, 153, 222, 171, 159, 187, 149, 148, 146, 150, 128, 131, 186, 103, 151, 94, 89, 141, 155, 84, 100, 115, 83, 104, 147, 138, 128, 107, 108, 74, 96, 119, 82, 76, 89, 70, 98, 122, 77, 79, 70, 66, 80, 73, 93, 67, 48, 85, 55, 87, 71, 57, 61, 71, 55, 70, 75, 88, 43, 55, 68, 47, 40, 64, 55, 61, 48, 36, 39, 33, 37, 37, 44, 29, 41, 27, 40, 51, 39, 46, 33, 37, 33, 49, 49, 32, 30, 29, 23, 25, 23, 34, 40, 34, 19, 33, 36, 32, 19, 34, 19, 19, 20, 28, 17, 23, 23, 19, 27, 16, 30, 22, 17, 25, 23, 25, 24, 23, 17, 23, 15, 23, 14, 20, 17, 12, 19, 13, 13, 15, 11, 15, 12, 11, 20, 12, 11, 12, 19, 12, 9, 13, 10, 9, 10, 9, 13, 14, 11, 13, 16, 9, 15, 10, 7, 7, 7, 6, 6, 9, 13, 8, 8, 10, 11, 8, 7, 6, 8, 8, 5, 5, 5, 7, 5, 5, 6, 6, 5, 6, 6, 6, 5, 8, 8, 8, 5, 4, 4, 5, 5, 5, 6, 3, 3, 3, 3, 5, 5, 3, 3, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 2, 2, 2 };
const char SignalGeneratorClass::R2_INDEX[] PROGMEM = { 255, 255, 255, 255, 251, 246, 242, 227, 233, 220, 214, 241, 195, 246, 236, 224, 246, 195, 236, 251, 188, 247, 251, 161, 128, 163, 214, 225, 122, 226, 161, 107, 134, 139, 114, 138, 134, 131, 123, 139, 131, 89, 155, 102, 158, 161, 98, 86, 153, 124, 104, 139, 107, 73, 75, 78, 90, 86, 121, 90, 70, 98, 102, 84, 103, 71, 55, 84, 79, 86, 88, 70, 74, 56, 75, 101, 55, 82, 50, 59, 71, 64, 53, 66, 50, 45, 37, 73, 55, 43, 60, 68, 41, 46, 40, 49, 63, 56, 64, 55, 53, 43, 63, 43, 63, 41, 31, 39, 32, 43, 37, 40, 26, 25, 37, 38, 38, 46, 41, 43, 28, 23, 26, 45, 25, 22, 24, 39, 21, 36, 35, 32, 22, 35, 25, 24, 28, 19, 31, 16, 21, 26, 17, 18, 16, 16, 16, 21, 15, 22, 14, 22, 15, 17, 23, 14, 20, 19, 16, 21, 15, 18, 19, 10, 16, 17, 15, 9, 14, 18, 12, 15, 16, 14, 15, 10, 9, 11, 9, 7, 12, 7, 10, 14, 13, 13, 14, 14, 9, 6, 9, 9, 7, 6, 8, 9, 10, 7, 7, 11, 10, 10, 7, 9, 9, 7, 7, 8, 6, 6, 6, 7, 4, 4, 4, 6, 7, 7, 5, 5, 5, 4, 7, 7, 7, 7, 4, 4, 6, 6, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 3, 3 };
const char SignalGeneratorClass::R4_INDEX[] PROGMEM = { 1, 8, 8, 191, 3, 2, 191, 89, 6, 3, 3, 175, 109, 6, 178, 6, 170, 23, 5, 151, 24, 89, 140, 34, 218, 15, 87, 101, 10, 15, 18, 9, 11, 15, 94, 25, 21, 147, 151, 67, 12, 11, 15, 36, 56, 27, 34, 12, 14, 35, 187, 69, 38, 241, 17, 46, 178, 151, 119, 31, 109, 40, 146, 89, 110, 78, 221, 20, 33, 129, 89, 61, 199, 119, 147, 32, 151, 89, 134, 81, 159, 50, 22, 91, 30, 193, 71, 35, 193, 41, 97, 89, 136, 27, 25, 33, 28, 199, 178, 217, 163, 107, 69, 37, 87, 89, 63, 179, 109, 41, 184, 147, 178, 91, 48, 43, 125, 37, 31, 85, 109, 157, 235, 46, 108, 58, 89, 117, 79, 40, 95, 170, 77, 95, 71, 74, 91, 67, 68, 94, 100, 36, 177, 45, 46, 58, 91, 64, 147, 231, 64, 200, 151, 71, 39, 167, 134, 161, 121, 133, 55, 238, 206, 157, 75, 76, 173, 60, 181, 68, 101, 72, 71, 94, 158, 54, 167, 194, 155, 99, 109, 195, 89, 179, 68, 105, 215, 82, 239, 89, 122, 171, 56, 236, 169, 67, 70, 101, 203, 110, 163, 213, 60, 145, 99, 83, 135, 129, 87, 91, 89, 104, 221, 74, 223, 173, 199, 66, 93, 117, 92, 63, 137, 81, 178, 96, 115, 65, 65, 251, 170, 89, 125, 161, 67, 77, 62, 131, 123, 211, 209, 163, 181, 206, 235, 87, 68, 151, 216, 71, 137, 81, 83, 187, 69, 84, 158, 76, 164, 163, 174, 210, 170, 80, 173, 78, 178, 69, 213, 100, 74, 87, 95, 89, 134, 184, 129, 124, 188, 181, 100, 78, 239, 104, 102, 99, 101, 205, 92, 110, 179, 100, 156, 97, 146, 141, 147, 217, 154, 86, 163, 215, 243, 107, 77, 131, 142, 164, 251, 122, 118, 169, 134, 197, 161, 89, 83, 130, 133, 120, 171, 179, 119, 177, 200, 213, 157, 169, 139, 89, 152, 126, 105, 108, 155, 182, 181, 193, 136, 154, 107, 163, 151, 138, 197, 170, 94, 161, 158, 188, 216, 201, 179, 220, 232, 217, 146, 255, 103, 163, 199, 135, 195, 166, 178, 195, 235, 162, 136, 170, 96, 134, 107, 131, 164, 197, 100, 214, 178, 96, 132, 117, 142, 97, 103, 152, 97, 164, 169, 229, 185, 239, 178, 170, 99, 119, 203, 245, 220, 151, 156, 122, 101, 137, 214, 164, 197, 176, 195, 130, 119, 125, 135, 217, 153, 194, 205, 141, 146, 193, 178, 112, 185, 249, 129, 171, 153, 116, 122, 202, 158, 133, 199, 248, 132, 175, 149, 189, 238, 113, 184, 219, 126, 220, 206, 208, 210, 231, 160, 213, 192, 122, 157, 238, 222, 226, 187, 166, 116, 163, 170, 148, 117, 135, 125, 177, 184, 181, 222, 142, 239, 197, 137, 194, 121, 183, 246, 133, 144, 153, 209, 208, 163, 129, 170, 129, 185, 142, 235, 198, 174, 204, 225, 243, 151, 184, 146, 200, 173, 244, 137, 130, 197, 219, 181, 150, 213, 193, 138, 131, 193, 218, 158, 216, 215, 132, 205, 208, 202, 141, 145, 249, 237, 190, 143, 151, 152, 241, 173, 177, 162, 215, 190, 154, 219, 156, 145, 136, 171, 243, 139, 190, 198, 177, 151, 205, 178, 190, 237, 167, 190, 176, 204, 169, 142, 147, 189, 156, 215, 189, 200, 211, 229, 151, 239, 164, 179, 175, 145, 152, 198, 242, 202, 213, 205, 215, 184, 173, 167, 154, 161, 171, 245, 218, 225, 185, 194, 176, 178, 160, 155, 170, 196, 248, 217, 192, 154, 179, 221, 167, 234, 151, 215, 205, 162, 184, 214, 182, 154, 158, 165, 217, 251, 203, 164, 249, 205, 212, 163, 191, 236, 235, 191, 200, 211, 208, 197, 188, 191, 238, 178, 191, 242, 241, 211, 181, 183, 191, 208, 222, 213, 244, 223, 214, 238, 205, 177, 194, 199, 185, 213, 242, 191, 187, 169, 218, 223, 197, 239, 171, 184, 208, 169, 223, 210, 231, 196, 200, 240, 175, 234, 193, 236, 230, 205, 232, 191, 203, 185, 198, 247, 234, 191, 216, 223, 246, 255, 206, 197, 200, 255, 175, 188, 203, 173, 199, 250, 208, 188, 220, 206, 249, 201, 196, 250, 231, 220, 210, 232, 241, 177, 186, 246, 245, 178, 247, 221, 211, 255, 197, 199, 191, 255, 208, 195, 197, 255, 195, 203, 206, 195, 254, 235, 208, 216, 198, 191, 192, 230, 191, 240, 190, 206, 192, 221, 231, 192, 224, 191, 207, 197, 216, 200, 248, 214, 206, 193, 212, 192, 250, 244, 215, 234, 250, 213, 193, 194, 245, 206, 235, 251, 215, 194, 210, 205, 238, 243, 196, 229, 254, 235, 197, 239, 210, 197, 230, 210, 236, 207, 240, 238, 235, 203, 239, 206, 234, 220, 239, 215, 209, 241, 246, 217, 205, 202, 215, 210, 224, 203, 237, 221, 254, 247, 219, 231, 241, 247, 235, 208, 205, 238, 233, 250, 218, 237, 230, 217, 212, 235, 244, 219, 248, 215, 211, 241, 254, 217, 212, 222, 226, 218, 243, 215, 234, 245, 214, 239, 247, 244, 223, 217, 247, 244, 225, 235, 214, 244, 227, 220, 215, 225, 229, 221, 223, 227, 237, 248, 217, 242, 221, 223, 245, 241, 222, 247, 219, 233, 250, 226, 244, 252, 250, 247, 243, 223, 226, 229, 233, 252, 249, 230, 241, 225, 249, 231, 254, 251, 249, 245, 254, 251, 231, 244, 226, 251, 248, 237, 227, 230, 248, 227, 228, 231, 235, 255, 254, 232, 236, 255, 230, 233, 238, 255, 231, 234, 240, 231, 254, 250, 232, 232, 254, 250, 255, 255, 254, 250, 255, 255, 235, 239, 235, 235, 254, 241, 236, 236, 254, 249, 255, 255, 254, 247, 238, 238, 239, 239, 239, 239, 254, 255, 255, 255, 254, 241, 241, 241, 242, 255, 255, 255, 243, 255, 255, 255, 254, 244, 244, 244, 254, 245, 245, 245, 246, 246, 246, 246, 254, 255, 255, 255, 248, 255, 255, 255, 254, 249, 249, 249, 250, 255, 255, 255, 254, 255, 255, 255, 252, 252, 252, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
const char SignalGeneratorClass::R5_INDEX[] PROGMEM = { 1, 8, 16, 1, 85, 159, 2, 5, 85, 191, 212, 4, 7, 138, 5, 159, 6, 47, 229, 8, 53, 15, 10, 43, 7, 106, 19, 17, 178, 123, 106, 219, 185, 140, 23, 89, 109, 16, 16, 37, 212, 237, 178, 76, 50, 106, 86, 249, 218, 89, 17, 47, 87, 14, 202, 76, 20, 24, 31, 121, 35, 97, 27, 45, 37, 53, 19, 213, 131, 34, 50, 74, 23, 39, 32, 149, 32, 55, 37, 62, 32, 103, 237, 58, 178, 28, 77, 158, 29, 138, 59, 65, 43, 219, 239, 183, 218, 31, 35, 29, 39, 60, 94, 177, 76, 75, 107, 38, 63, 169, 38, 48, 40, 79, 151, 170, 59, 201, 242, 89, 70, 49, 33, 170, 73, 137, 90, 69, 103, 205, 87, 49, 109, 89, 120, 116, 95, 130, 129, 94, 89, 249, 51, 202, 199, 159, 102, 146, 64, 41, 149, 48, 64, 137, 251, 59, 74, 62, 83, 76, 185, 43, 50, 66, 139, 138, 61, 177, 59, 158, 107, 151, 154, 117, 70, 206, 67, 58, 73, 115, 105, 59, 130, 65, 172, 112, 55, 145, 50, 135, 99, 71, 218, 52, 73, 185, 178, 124, 62, 115, 78, 60, 214, 89, 131, 157, 97, 102, 152, 146, 150, 129, 61, 183, 61, 79, 69, 209, 149, 119, 152, 223, 103, 175, 80, 149, 125, 222, 223, 58, 86, 165, 118, 92, 222, 194, 242, 115, 123, 72, 73, 94, 85, 75, 66, 179, 230, 104, 73, 223, 116, 197, 193, 86, 234, 193, 103, 215, 100, 101, 95, 79, 98, 209, 97, 216, 95, 246, 80, 171, 232, 198, 182, 195, 130, 95, 136, 142, 94, 98, 178, 229, 75, 173, 177, 183, 180, 89, 199, 167, 103, 185, 119, 192, 128, 133, 128, 87, 123, 221, 117, 89, 79, 180, 251, 148, 137, 119, 78, 161, 167, 117, 148, 101, 124, 225, 242, 155, 152, 169, 119, 114, 172, 116, 103, 97, 132, 123, 150, 235, 138, 167, 201, 196, 137, 117, 118, 111, 158, 140, 202, 133, 144, 158, 111, 129, 234, 137, 140, 118, 103, 111, 125, 102, 97, 104, 155, 89, 221, 140, 115, 170, 118, 139, 130, 119, 99, 144, 172, 138, 245, 176, 221, 181, 145, 121, 239, 112, 135, 251, 183, 207, 171, 251, 237, 161, 253, 150, 146, 108, 134, 104, 140, 147, 253, 211, 124, 103, 115, 168, 163, 209, 253, 187, 120, 157, 131, 147, 133, 200, 219, 209, 194, 121, 172, 136, 129, 188, 182, 138, 150, 239, 145, 108, 209, 158, 177, 234, 223, 135, 173, 206, 138, 111, 209, 158, 186, 147, 117, 247, 152, 128, 223, 128, 137, 136, 135, 123, 178, 134, 149, 235, 183, 121, 130, 128, 155, 175, 251, 179, 172, 198, 251, 218, 236, 167, 161, 164, 134, 210, 125, 152, 219, 155, 249, 165, 123, 228, 211, 199, 146, 147, 188, 238, 181, 239, 167, 218, 132, 157, 179, 153, 139, 129, 208, 171, 216, 158, 183, 130, 232, 245, 162, 146, 177, 214, 151, 167, 234, 247, 168, 149, 206, 151, 152, 248, 160, 158, 163, 234, 228, 133, 140, 175, 233, 221, 220, 139, 194, 190, 208, 157, 178, 220, 155, 218, 235, 251, 200, 141, 247, 181, 174, 195, 229, 169, 195, 183, 147, 209, 184, 199, 172, 208, 248, 240, 187, 227, 165, 188, 178, 169, 156, 237, 150, 219, 201, 206, 249, 238, 183, 150, 180, 171, 178, 170, 199, 212, 220, 239, 229, 216, 151, 170, 165, 201, 192, 212, 210, 234, 242, 221, 192, 152, 174, 197, 246, 212, 172, 228, 163, 253, 178, 187, 237, 209, 180, 212, 251, 245, 235, 179, 155, 192, 238, 157, 191, 185, 241, 206, 167, 168, 207, 198, 188, 191, 202, 212, 209, 168, 225, 210, 166, 167, 191, 223, 221, 212, 195, 183, 191, 167, 183, 191, 172, 200, 232, 212, 207, 223, 194, 171, 217, 222, 246, 191, 187, 212, 175, 245, 228, 202, 249, 189, 201, 183, 216, 212, 177, 243, 182, 221, 181, 186, 209, 185, 225, 212, 233, 218, 175, 185, 227, 201, 195, 177, 171, 212, 222, 219, 172, 251, 234, 217, 255, 222, 177, 213, 236, 202, 216, 179, 222, 228, 179, 194, 204, 214, 194, 187, 255, 243, 184, 185, 255, 184, 206, 216, 179, 232, 230, 240, 180, 221, 236, 234, 181, 237, 228, 225, 238, 183, 198, 224, 216, 236, 245, 244, 204, 246, 196, 248, 229, 246, 214, 205, 247, 212, 249, 230, 242, 221, 239, 193, 224, 233, 249, 227, 251, 193, 198, 225, 207, 194, 228, 252, 251, 199, 237, 208, 195, 228, 253, 234, 240, 207, 203, 252, 216, 195, 211, 252, 208, 237, 253, 217, 238, 212, 242, 209, 211, 214, 248, 211, 245, 216, 230, 212, 236, 243, 211, 207, 235, 249, 253, 238, 244, 229, 253, 217, 233, 203, 209, 236, 224, 215, 210, 221, 250, 254, 219, 224, 209, 240, 221, 228, 242, 248, 224, 216, 241, 213, 246, 251, 220, 209, 245, 251, 240, 236, 245, 220, 249, 229, 219, 251, 225, 218, 221, 242, 249, 219, 222, 241, 231, 254, 223, 240, 248, 254, 243, 239, 248, 246, 242, 232, 222, 254, 228, 250, 248, 226, 230, 250, 225, 254, 239, 223, 247, 229, 222, 224, 227, 231, 252, 249, 246, 242, 224, 227, 246, 235, 252, 228, 246, 224, 227, 229, 233, 225, 228, 248, 235, 254, 229, 232, 243, 254, 251, 233, 255, 254, 251, 247, 228, 229, 251, 247, 229, 254, 251, 246, 230, 254, 251, 245, 255, 232, 236, 255, 255, 233, 237, 233, 233, 234, 238, 234, 234, 254, 250, 255, 255, 236, 249, 255, 255, 237, 242, 237, 237, 238, 245, 255, 255, 254, 255, 255, 255, 240, 240, 240, 240, 241, 255, 255, 255, 254, 242, 242, 242, 254, 243, 243, 243, 244, 255, 255, 255, 245, 255, 255, 255, 254, 255, 255, 255, 247, 247, 247, 247, 254, 248, 248, 248, 249, 255, 255, 255, 254, 250, 250, 250, 251, 251, 251, 251, 254, 255, 255, 255, 253, 253, 253, 253, 254, 254, 254, 254, 255, 255, 255, 255, 255 };

void SignalGeneratorClass::begin() {
  BaseNode::begin();

  // initalize SPI parameters
  pinMode(LTC6903_SS_PIN, OUTPUT);
  pinMode(AD5206_SS_PIN, OUTPUT);
  pinMode(FREQ_RANGE_PIN, OUTPUT);
#ifdef SOFT_SPI
  pinMode(S_SCK_PIN, OUTPUT);
  pinMode(S_MOSI_PIN, OUTPUT);
#else
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
#endif // SOFT_SPI

  // initialize all potentiometers
  for(uint8_t i=0; i<6; i++) {
    set_pot(i, config_settings_.pot[i], false, false);
  }

  // set an initial frequency/voltage
  set_waveform_frequency(1000);
  set_waveform_voltage(0);

  Serial.print(P("ram=")); Serial.println(ram_size(), DEC);
  Serial.print(P(".data=")); Serial.println(data_size(), DEC);
  Serial.print(P(".bss=")); Serial.println(bss_size(), DEC);
  Serial.print(P("heap=")); Serial.println(heap_size(), DEC);
  Serial.print(P("stack=")); Serial.println(stack_size(), DEC);
  Serial.print(P("free memory=")); Serial.println(free_memory(), DEC);
}

void SignalGeneratorClass::dump_config() {
  BaseNode::dump_config();
  Serial.println(P("config_version=") + version_string(config_version()));
  for(uint8_t i=0; i<6; i++) {
    Serial.println(P("pot[") + String(i) + "]=" + String(config_settings_.pot[i], DEC));
  }
  Serial.print(P("hf_amplitude_correction="));
  Serial.println(config_settings_.hf_amplitude_correction);
  Serial.print(P("max_voltage_rms="));
  Serial.println(config_settings_.max_voltage_rms);
}


/* If there is a request pending on the serial port, process it. */
bool SignalGeneratorClass::process_serial_input() {
  if (BaseNode::process_serial_input()) {
    return true;
  }
  
  if (match_function(P("set_hf_amplitude_correction("))) {
    float value;
    if (read_float(value)) {
      set_hf_amplitude_correction(value);
      return true;
    }
  }

  if (match_function(P("set_pot("))) {
    int32_t index;
    int32_t value;
    int32_t save_to_eeprom = false;
    if (read_int(index) && read_int(value)) {
      read_int(save_to_eeprom);
      if (index>=0 && index<6 && value>=0 & value < 256) {
        set_pot((uint8_t)index, (uint8_t)value, (bool)save_to_eeprom);
        return true;
      }
    }
  }

  if (match_function(P("set_waveform_frequency("))) {
    float value;
    if (read_float(value)) {
      set_waveform_frequency(value);
      return true;
    }
  }

  if (match_function(P("set_waveform_voltage("))) {
    float value;
    if (read_float(value)) {
      set_waveform_voltage(value);
      return true;
    }
  }

  if(strcmp(buffer_, P("vout_pk_pk()"))==0) {
    vout_pk_pk();
    return true;
  }

  error(1);
}

void SignalGeneratorClass::process_wire_command() {
  bytes_written_ = 0;
  bytes_read_ = 0;
  send_payload_length_ = true;
  return_code_ = RETURN_GENERAL_ERROR;
  switch(cmd_) {
  case CMD_GET_WAVEFORM_FREQUENCY:
    if(payload_length_==0) {
      serialize(&waveform_frequency_, sizeof(waveform_frequency_));
      return_code_ = RETURN_OK;
    } else {
      return_code_ = RETURN_BAD_PACKET_SIZE;
    }
    break;
  case CMD_SET_WAVEFORM_FREQUENCY:
    if(payload_length_==sizeof(float)) {
      set_waveform_frequency(read<float>());
      return_code_ = RETURN_OK;
    } else {
      return_code_ = RETURN_BAD_PACKET_SIZE;
    }
    break;
  case CMD_GET_WAVEFORM_VOLTAGE:
    if(payload_length_==0) {
      serialize(&waveform_voltage_, sizeof(waveform_voltage_));
      return_code_ = RETURN_OK;
    } else {
      return_code_ = RETURN_BAD_PACKET_SIZE;
    }
    break;
  case CMD_SET_WAVEFORM_VOLTAGE:
    if(payload_length_==sizeof(float)) {
      return_code_ = set_waveform_voltage(read<float>());
    } else {
      return_code_ = RETURN_BAD_PACKET_SIZE;
    }
    break;
  case CMD_GET_POT_VALUE:
    if(payload_length_==sizeof(uint8_t)) {
      uint8_t index = read<uint8_t>();
      serialize(&pot_[index], sizeof(pot_[index]));
      return_code_ = RETURN_OK;
    } else {
      return_code_ = RETURN_BAD_PACKET_SIZE;
    }
    break;
  case CMD_SET_POT_VALUE:
    if(payload_length_==2*sizeof(uint8_t)) {
      uint8_t index = read<uint8_t>();
      uint8_t value = read<uint8_t>();
      set_pot(index, value);
      return_code_ = RETURN_OK;
    } else {
      return_code_ = RETURN_BAD_PACKET_SIZE;
    }
    break;
  case CMD_MEASURE_VOUT_PK_PK:
    if(payload_length_==0) {
      float v = vout_pk_pk();
      serialize(&v, sizeof(v));
      return_code_ = RETURN_OK;
    } else {
      return_code_ = RETURN_BAD_PACKET_SIZE;
    }
    break;
  default:
    BaseNode::process_wire_command();
    break;
  }
}

BaseNode::Version SignalGeneratorClass::config_version() {
  Version config_version;
  uint16_t address = EEPROM_CONFIG_SETTINGS + sizeof(base_config_settings_);
  eeprom_read_block((void*)&config_version, (void*)address,
                    sizeof(config_version));
  return config_version;
}

void SignalGeneratorClass::load_config(bool use_defaults) {
  BaseNode::load_config(use_defaults);
  uint16_t address = EEPROM_CONFIG_SETTINGS + sizeof(base_config_settings_);
  eeprom_read_block((void*)&config_settings_, (void*)address, sizeof(config_settings_));

  // If we're not at the expected version by the end of the upgrade path,
  // set everything to default values.
  if(!(config_settings_.version.major==0 &&
     config_settings_.version.minor==0 &&
     config_settings_.version.micro==0) || use_defaults) {

    config_settings_.version.major=0;
    config_settings_.version.minor=0;
    config_settings_.version.micro=0;
    config_settings_.pot[0] = 128;
    config_settings_.pot[1] = 128;
    config_settings_.pot[2] = 128;
    config_settings_.pot[3] = 160;
    config_settings_.pot[4] = 0;
    config_settings_.pot[5] = 0;
    config_settings_.hf_amplitude_correction = 1.3;
    config_settings_.max_voltage_rms = 7;
    save_config();
  }
}

void SignalGeneratorClass::save_config() {
  BaseNode::save_config();
  uint16_t address = EEPROM_CONFIG_SETTINGS + sizeof(base_config_settings_);
  eeprom_write_block((void*)&config_settings_, (void*)address, sizeof(ConfigSettings));
}

void SignalGeneratorClass::set_pot(byte index, byte value, bool save_to_eeprom, bool display_msg) {
  // take the SS pin low to select the chip:
  digitalWrite(AD5206_SS_PIN, LOW);
#ifdef SOFT_SPI
  shiftOutFast(S_MOSI_PIN, S_SCK_PIN, MSBFIRST, index);
  shiftOutFast(S_MOSI_PIN, S_SCK_PIN, MSBFIRST, value);
#else
  SPI.transfer(index);
  SPI.transfer(value);
#endif // SOFT_SPI
  // take the SS pin high to de-select the chip:
  digitalWrite(AD5206_SS_PIN, HIGH);
  pot_[index] = value;
  if(save_to_eeprom) {
    config_settings_.pot[index] = (uint8_t)value;
    save_config();
  }
  if(display_msg) {
    if(save_to_eeprom) {
      Serial.println(P("pot[") + String(index) + "]=" + String(value, DEC) + P(" (save to EEPROM)"));
    } else {
      Serial.println(P("pot[") + String(index) + "]=" + String(value, DEC));
    }
  }
}

uint8_t SignalGeneratorClass::set_waveform_voltage(float vrms) {

  if(vrms<0 || vrms>config_settings_.max_voltage_rms) {
    Serial.print(P("set_waveform_voltage("));
    Serial.print(vrms);
    Serial.println(P(") Error: BAD_VALUE"));
    return RETURN_BAD_VALUE;
  }
  waveform_voltage_ = vrms;
  Serial.print(P("waveform_voltage="));
  Serial.println(waveform_voltage_);
  uint16_t index = round(vrms/config_settings_.max_voltage_rms*1023);
  Serial.println(P("index=") + String(index));
  // only set pots if they have changed
  if(pot_[4] != pgm_read_byte_near(R4_INDEX + index) ||
     pot_[5] != pgm_read_byte_near(R5_INDEX + index)) {
    // if pot 4 is increasing, set pot 5 first
    if(pot_[4] < pgm_read_byte_near(R4_INDEX + index)) {
      set_pot(5, pgm_read_byte_near(R5_INDEX + index));
      set_pot(4, pgm_read_byte_near(R4_INDEX + index));
    } else {
      set_pot(4, pgm_read_byte_near(R4_INDEX + index));
      set_pot(5, pgm_read_byte_near(R5_INDEX + index));
    }
  }
  return RETURN_OK;
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
#ifdef SOFT_SPI
  shiftOutFast(S_MOSI_PIN, S_SCK_PIN, MSBFIRST, msb);
  shiftOutFast(S_MOSI_PIN, S_SCK_PIN, MSBFIRST, lsb);
#else
  SPI.transfer(msb);
  SPI.transfer(lsb);
#endif // SOFT_SPI  
  digitalWrite(LTC6903_SS_PIN, HIGH);

  float FSF;
  float amplitude_correction;
  if(freq <= 1e3) {
    FSF = 1;
    amplitude_correction = 1.0;
    digitalWrite(FREQ_RANGE_PIN, HIGH);
  } else {
    FSF = 10;
    amplitude_correction = config_settings_.hf_amplitude_correction;
    digitalWrite(FREQ_RANGE_PIN, LOW);
  }

  byte index = (log10(freq)-log10(F_MIN*FSF))/LOG_F_STEP;
  set_pot(1, pgm_read_byte_near(R1_INDEX + index));
  set_pot(2, pgm_read_byte_near(R2_INDEX + index));

  float R1 = (float)pgm_read_byte_near(R1_INDEX + index)/255.0*R_MAX;
  float R2 = (float)pgm_read_byte_near(R2_INDEX + index)/255.0*R_MAX;
  float gain = 1/sqrt(pow(1-pow(2*PI*freq/FSF, 2)*R1*R2*C1*C2, 2) + pow(2*PI*freq*C2/FSF*(R1+R2), 2));
  Serial.print("gain=");
  Serial.println(gain);
  set_pot(0, round(128/gain*amplitude_correction));
}

void SignalGeneratorClass::set_hf_amplitude_correction(float correction) {
  config_settings_.hf_amplitude_correction = correction;
  Serial.print(P("hf_amplitude_correction="));
  Serial.println(config_settings_.hf_amplitude_correction);
  save_config();
  // reset the frequency to update amplitude based on the new correction factor
  set_waveform_frequency(waveform_frequency_);
}

float SignalGeneratorClass::vout_pk_pk() {
  uint16_t min_v = 1023;
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
  float vout = (float)(max_v-min_v) * (5.0/1023.0);
  Serial.print(P("vout_pk_pk()="));
  Serial.println(vout);
  return vout;
}
