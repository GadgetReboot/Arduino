// Modified by Gadget Reboot for a series of demos/tests of audio
// playback and processing.
//
// Flange effect based on Teensy Examples sketch in Arduino.
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

// create audio component objects
AudioPlaySdWav           playWav1;           // SD card wav file player
AudioInputI2S            audioInput;         // audio shield input, mic or line-in selectable
AudioEffectFlange        l_flange;
AudioEffectFlange        r_flange;
AudioMixer4              mixer1;             // mixer to combine wav file and audio shield inputs
AudioMixer4              mixer2;

// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
//AudioOutputAnalog      audioOutput;

// wire up the interfaces between audio components with patch cords
// mixer inputs
AudioConnection          patchCord1(playWav1, 0, mixer1, 0);          // left  channels into mixer 1
AudioConnection          patchCord2(audioInput, 0, mixer1, 1);

AudioConnection          patchCord3(playWav1, 1, mixer2, 0);          // right channels into mixer 2
AudioConnection          patchCord4(audioInput, 1, mixer2, 1);

// mixer outputs
AudioConnection          patchCord5(mixer1, 0, l_flange, 0);          // mixer outputs to flange inputs
AudioConnection          patchCord6(mixer2, 0, r_flange, 0);

// flange between mixer out and audio shield out
AudioConnection          patchCord7(l_flange, 0, audioOutput, 0);     // flange outputs to audio shield out
AudioConnection          patchCord8(r_flange, 0, audioOutput, 1);

// object to allow control of the SGTL5000 audio shield settings
AudioControlSGTL5000     audioShield;

// buttons and potentiometers
#define pot0             A13
#define pot1             A12
#define button0          30
#define button1          29
#define button2          28
#define button3          27

// attach button debouncers to input buttons
Bounce db_button0 = Bounce(button0, 30);
Bounce db_button1 = Bounce(button1, 30);
Bounce db_button2 = Bounce(button2, 30);
Bounce db_button3 = Bounce(button3, 30);

// choose mic or line input for audio shield input path
//const int inputChSelect = AUDIO_INPUT_MIC;
const int inputChSelect = AUDIO_INPUT_LINEIN;

// audio shield volume
int masterVolume        = 0;

// uncomment one set of SD card SPI pins to use
// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN     10
#define SDCARD_MOSI_PIN   7
#define SDCARD_SCK_PIN    14

// Use these with the Teensy 3.5 & 3.6 SD card
//#define SDCARD_CS_PIN    BUILTIN_SDCARD
//#define SDCARD_MOSI_PIN  11  // not actually used
//#define SDCARD_SCK_PIN   13  // not actually used

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13

// wav filenames on SD card for playback
char *wavFiles[]       = {"01.WAV", "02.WAV", "03.WAV", "04.WAV", "05.WAV", "06.WAV", "07.WAV"
                         };
byte wavNum            = 0;      // current wav file index playing from array list
bool wavIsPlaying      = false;  // track if a wav file is currently playing or not

// Flange Effect parameters
#define FLANGE_DELAY_LENGTH (6*AUDIO_BLOCK_SAMPLES)  // number of samples in each delay line

short l_delayline[FLANGE_DELAY_LENGTH];              // allocate delay lines for left and right channels
short r_delayline[FLANGE_DELAY_LENGTH];
int    s_idx           = FLANGE_DELAY_LENGTH / 4;
int    s_depth         = FLANGE_DELAY_LENGTH / 4;
double s_freq          = .1;
bool   flangeActive    = false;                      // track if flange effect is on or off

void setup() {
  Serial.begin(9600);
  Serial.println("Flange Effect Test - SD Card Wav Player and Audio Shield Line In\n");

  // buttons are inputs with pullups
  pinMode(button0, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  // Comment these out if not using the audio adaptor board.
  Serial.print("init audio shield...");
  audioShield.enable();
  audioShield.inputSelect(inputChSelect);  // select mic or line-in for audio shield input source
  audioShield.volume(0.5);
  Serial.println("done.");

  Serial.print("init SD card...");
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message
    Serial.println("Unable to access the SD card.  Program halted.");
    while (1);
  }
  Serial.println("done.");

  Serial.print("init Flange effect...");
  // Set up the flange effect:
  // address of delayline
  // total number of samples in the delay line
  // Index (in samples) into the delay line for the added voice
  // Depth of the flange effect
  // frequency of the flange effect
  l_flange.begin(l_delayline, FLANGE_DELAY_LENGTH, s_idx, s_depth, s_freq);
  r_flange.begin(r_delayline, FLANGE_DELAY_LENGTH, s_idx, s_depth, s_freq);

  // Initially the effect is off.
  l_flange.voices(FLANGE_DELAY_PASSTHRU, 0, 0);
  r_flange.voices(FLANGE_DELAY_PASSTHRU, 0, 0);

  Serial.println("done.");

  Serial.println("Waiting for control input...");

  // reset audio resource usage stats.
  // useful if tracking max usage in main program
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

void playFile(const char *filename)
{
  Serial.print("Start playing file: ");
  Serial.println(filename);

  // start playing the file.
  // sketch continues to run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library to read WAV header info
  delay(5);
}


void loop() {

  // auto select next wav file if current file finishes playing
  // and if playback is enabled
  if ((!(playWav1.isPlaying())) && (wavIsPlaying)) {
    wavNum++;
    if (wavNum > 6) {
      wavNum = 0;
    }
    playFile(wavFiles[wavNum]);
  }

  // update the button debounce status so falling edges
  // can be detected and processed
  db_button0.update();
  db_button1.update();
  db_button2.update();
  db_button3.update();

  // button 0 pressed - toggle playback start/stop for current wav file
  if (db_button0.fallingEdge()) {
    if (playWav1.isPlaying()) {
      playWav1.stop();
      wavIsPlaying = false;
      Serial.println("Playback stopped\n");
    }
    else {
      playFile(wavFiles[wavNum]);
      wavIsPlaying = true;
      Serial.println("Playback started");
    }
    //Serial.print("Audio memory usage max: ");
    //Serial.println(AudioMemoryUsageMax());
  }

  // button 1 pressed - skip track forward
  if (db_button1.fallingEdge()) {
    Serial.println("Skip track forward");
    if (wavNum == 6)
      wavNum = 0;
    else
      wavNum++;
    playFile(wavFiles[wavNum]);
    wavIsPlaying = true;
  }

  // button 2 pressed - skip track backward
  if (db_button2.fallingEdge()) {
    Serial.println("Skip track backward");
    if (wavNum == 0)
      wavNum = 6;
    else
      wavNum--;
    playFile(wavFiles[wavNum]);
    wavIsPlaying = true;
  }

  // button 3 pressed - toggle Flange effect
  if (db_button3.fallingEdge()) {
    if (flangeActive) {
      l_flange.voices(FLANGE_DELAY_PASSTHRU, 0, 0);
      r_flange.voices(FLANGE_DELAY_PASSTHRU, 0, 0);
      Serial.println("Flange OFF\n");
    }
    else {
      l_flange.voices(s_idx, s_depth, s_freq);
      r_flange.voices(s_idx, s_depth, s_freq);
      Serial.println("Flange ON");
    }
    flangeActive = !flangeActive;
  }

  // read volume control pot and set audio shield volume if required
  int vol = analogRead(pot0);
  if (vol != masterVolume) {
    masterVolume = vol;
    audioShield.volume((float)vol / 1023);
  }

  // read Flange control pot and change parameter if required
  if (flangeActive) {
    int reading = map(analogRead(pot1), 0, 1023, 1, 10);
    if ((reading / 10.0) != s_freq) {
      s_freq = reading / 10.0;
      Serial.print("New flange freq: ");
      Serial.println(s_freq);
      l_flange.voices(s_idx, s_depth, s_freq);
      r_flange.voices(s_idx, s_depth, s_freq);
    }
  }

}
