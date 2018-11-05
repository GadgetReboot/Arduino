
/*  Teensy 3.6 Audio Library Demo
 * 
 * Portions taken from the Teensy "Examples" and from
 * https://github.com/UECIDE/Teensy3/tree/master/cores/teensy3/files/libraries/Audio/examples/SamplePlayer
 * 
 * Audio samples taken from various freesound.org sources under varying Creative Commons provisions
 * Attribution cited within sketch comments below.
 */
 
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Bounce.h>

// Button input pins on Teensy 3.6
#define sound0Pin 32  
#define sound1Pin 31
#define sound2Pin 30
#define sound3Pin 29
#define sound4Pin 28
#define sound5Pin 27
#define sound6Pin 26
#define sound7Pin 25

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/

// Create the Audio components.  
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;  // six memory players
AudioPlayMemory    sound2;  // for converted wav samples
AudioPlayMemory    sound3;
AudioPlayMemory    sound4;
AudioPlayMemory    sound5;
AudioMixer4        mix1;    // two 4-channel mixers are needed in
AudioMixer4        mix2;    // tandem to combine 6 audio sources
AudioOutputI2S     headphones;
AudioOutputAnalog  dac;     // play to both I2S audio board and on-chip DAC

// Internal synth generated sounds copied from Teensy example audio sketch
AudioSynthSimpleDrum     drum0;          
AudioSynthSimpleDrum     drum1;          
AudioMixer4              mixer1;  // 4 channel mixer to combine sounds and connect to other mixer input

// Create Audio connections between the components
AudioConnection c1(sound0, 0, mix1, 0);
AudioConnection c2(sound1, 0, mix1, 1);
AudioConnection c3(sound2, 0, mix1, 2);
AudioConnection c4(sound3, 0, mix1, 3);
AudioConnection c5(mix1, 0, mix2, 0);   // output of mix1 into 1st input on mix2
AudioConnection c6(sound4, 0, mix2, 1);
AudioConnection c7(sound5, 0, mix2, 2);
AudioConnection c8(mix2, 0, headphones, 0);
AudioConnection c9(mix2, 0, headphones, 1);
AudioConnection c10(mix2, 0, dac, 0);
AudioConnection c11(mixer1, 0, mix2, 3);  // tie in the internal synth mixer output to sample sounds mixer
AudioConnection patchCord1(drum0, 0, mixer1, 0);  
AudioConnection patchCord2(drum1, 0, mixer1, 1);

// Create an object to control the audio shield. 
AudioControlSGTL5000 audioShield;

// Bounce objects to read six pushbuttons (pins 0-5)
Bounce button0 = Bounce(sound0Pin, 5);
Bounce button1 = Bounce(sound1Pin, 5);  // 5 ms debounce time
Bounce button2 = Bounce(sound2Pin, 5);
Bounce button3 = Bounce(sound3Pin, 5);
Bounce button4 = Bounce(sound4Pin, 5);
Bounce button5 = Bounce(sound5Pin, 5);
Bounce button6 = Bounce(sound6Pin, 5);
Bounce button7 = Bounce(sound7Pin, 5);

void setup() {
  // Configure the pushbutton pins for pullups.
  // Each button should connect from the pin to GND.
  pinMode(sound0Pin, INPUT_PULLUP);
  pinMode(sound1Pin, INPUT_PULLUP);
  pinMode(sound2Pin, INPUT_PULLUP);
  pinMode(sound3Pin, INPUT_PULLUP);
  pinMode(sound4Pin, INPUT_PULLUP);
  pinMode(sound5Pin, INPUT_PULLUP);
  pinMode(sound6Pin, INPUT_PULLUP);
  pinMode(sound7Pin, INPUT_PULLUP);
  
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(25);

  // turn on the output
  audioShield.enable();
  audioShield.volume(0.5);

  // by default the Teensy 3.1 DAC uses 3.3Vp-p output
  // if your 3.3V power has noise, switching to the
  // internal 1.2V reference can give you a clean signal
  //dac.analogReference(INTERNAL);

  // reduce the gain on mixer channels, so more than 1
  // sound can play simultaneously without clipping
  mix1.gain(0, 0.4);
  mix1.gain(1, 0.4);
  mix1.gain(2, 0.4);
  mix1.gain(3, 0.4);
  mix2.gain(1, 0.4);
  mix2.gain(2, 0.4);
  mixer1.gain(0, 0.4);
  mixer1.gain(1, 0.4);
   
  // configure what the synth drums will sound like
  drum0.frequency(60);
  drum0.length(300);
  drum0.secondMix(0.0);
  drum0.pitchMod(1.0);
  
  drum1.frequency(60);
  drum1.length(1500);
  drum1.secondMix(0.0);
  drum1.pitchMod(0.55);
  
  AudioInterrupts();
}

void loop() {
  // Update all the button objects
  button0.update();
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();
  button6.update();
  button7.update();

  // When the buttons are pressed, just start a sound playing.
  // The audio library will play each sound through the mixers
  // so any combination can play simultaneously.
  //
  if (button0.fallingEdge()) {
    sound0.play(AudioSampleSnare);
  }
  if (button1.fallingEdge()) {
    sound1.play(AudioSampleTomtom);
  }
  if (button2.fallingEdge()) {
    sound2.play(AudioSampleHihat);
  }
  if (button3.fallingEdge()) {
    sound3.play(AudioSampleKick);
  }
  if (button4.fallingEdge()) {
    // comment this line to work with Teensy 3.0.
    // the Gong sound is very long, too much for 3.0's memory
    sound4.play(AudioSampleGong);
  }
  if (button5.fallingEdge()) {
    sound5.play(AudioSampleCashregister);
  }
  if (button6.fallingEdge()) {
    drum0.noteOn();
  }
    if (button7.fallingEdge()) {
    drum1.noteOn();
  }
}

