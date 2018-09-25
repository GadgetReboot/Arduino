/*
   FFT Audio Spectrum Analizer
   Suitable for signals up to 4.5 KHz
   Sampling frequency on Arduino Uno is approximately 9 KHz

   Adapted from learnelectronics:      https://www.youtube.com/watch?v=5RmQJtE61zE
   Which was adapted from cbm80amiga:  https://www.youtube.com/watch?v=EnvhEgjrHsw

   Uses the FIX_FFT library:    https://www.arduinolibraries.info/libraries/fix_fft

   Gadget Reboot
*/

#include <Wire.h>                                     // I2C library for display
#include "fix_fft.h"                                  // fixed point Fast Fourier Transform library
#include <Adafruit_GFX.h>                             // graphics library for display
#include <Adafruit_SSD1306.h>                         // SSD1306 OLED display library

#define OLED_RESET 4                                  // OLED library likes to see this but it isn't used here with I2C
#define OLED_ADDR 0x3C                                // OLED I2C address
#define audioIn A0                                    // audio input port

Adafruit_SSD1306 display(OLED_RESET);                 // create instance of OLED display

char re[128], im[128];                                // real and imaginary FFT result arrays
byte ylim = 60;                                       // OLED y-axis drawing boundary limit

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);     // init OLED display
  display.clearDisplay();                             // blank the display
  display.setTextSize(1);                             // configure text properties
  display.setTextColor(WHITE);
  analogReference(DEFAULT);                           // use the default analog reference of 5 volts (on 5V Arduino boards)
                                                      // or 3.3 volts (on 3.3V Arduino boards)
};

void loop() {

  // The FFT real/imaginary data are stored in a char data type as a signed -128 to 127 number
  // This allows a waveform to swing centered around a 0 reference data point
  // The ADC returns data between 0-1023 so it is scaled to fit within a char by dividing by 4 and subtracting 128.
  // eg (0 / 4) - 128 = -128 and (1023 / 4) - 128 = 127

  for (byte i = 0; i < 128; i++) {                    // read 128 analog input samples from ADC
    int sample = analogRead(audioIn);
    re[i] = sample / 4 - 128;                         // scale the samples to fit within a char variable
    im[i] = 0;                                        // there are no imaginary samples associated with the time domain so set to 0
  };

  fix_fft(re, im, 7, 0);                              // send the samples for FFT conversion, returning the real/imaginary results in the same arrays

  display.clearDisplay();                             // clear display
  display.setCursor(0, 0);                            // set cursor to top left corner of the display
  display.print("Audio Spectrum");                    // print a header on the top row of the display

  // The data array will contain frequency bin data in locations 0..127 for samples up to the sampling frequency of approx. 9 KHz
  // Each frequency bin will represent a center frequency of approximately (9 KHz / 128 samples) = 70 Hz
  // Due to Nyquist sampling requirements, we can only consider sampled frequency data up to (sampling rate / 2) or (9 KHz / 2) = 4.5 KHz
  // Therefore we only acknowledge the first 64 frequency bins [0..63] = [0..4.5KHz]

  for (byte i = 0; i < 64; i++) {
    int dat = sqrt(re[i] * re[i] + im[i] * im[i]);     // frequency magnitude is the square root of the sum of the squares of the real and imaginary parts of a vector
    display.drawLine(i * 2, ylim, i * 2, ylim - dat, WHITE); // draw bars for each frequency bin from 0 Hz to 4.5 KHz
  };

  display.display();                                   // update the display
};

