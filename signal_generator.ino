#include "signal_generator_board.h"
#include <SPI.h>
#include <Wire.h>
#include "Memory.h"

void setup() {
  SignalGeneratorBoard.begin();
}

void loop() {
  SignalGeneratorBoard.Listen();
}

