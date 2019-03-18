// AVR High-voltage Serial Fuse Reprogrammer
//
// Gadget Reboot - modified fuse programming options in menu system
//               - changed 12 volt reset circuit and control logic
//
// This version adapted from the version by Ralph Bacon
// https://github.com/RalphBacon/ATTiny85_Fuse_Resetter
// Other History:
// Adapted from code and design by Paul Willoughby 03/20/2010
// http://www.rickety.us/2010/03/arduino-avr-high-voltage-serial-programmer/
// Inspired by Jeff Keyzer mightyohm.com
// Serial Programming routines from ATtiny25/45/85 datasheet
//
// Fuse Calc:
//   http://www.engbedded.com/fusecalc/


#define  SCI     12    // Target Clock Input
#define  SDO     11    // Target Data Output
#define  SII     10    // Target Instruction Input
#define  SDI      9    // Target Data Input
#define  VCC      8    // Target VCC
#define  RST      7    // Output to control 12V prog voltage to target Reset pin

// required for accessing device fuse locations
#define  LFUSE  0x646C
#define  HFUSE  0x747C
#define  EFUSE  0x666E

// Digispark default fuses
#define  LVAL1  0xE1
#define  HVAL1  0x5D
#define  EVAL1  0xFE

// Digispark fuses with Reset pin enabled
#define  LVAL2  0xE1
#define  HVAL2  0xDD
#define  EVAL2  0xFE

byte targetHVal = 0;
byte targetLVal = 0;
byte targetEVal = 0;
bool readFuseOnly = true;  // just reading, not writing

// Define ATTiny series signatures
#define  ATTINY13   0x9007  // L: 0x6A, H: 0xFF             8 pin
#define  ATTINY24   0x910B  // L: 0x62, H: 0xDF, E: 0xFF   14 pin
#define  ATTINY25   0x9108  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
#define  ATTINY44   0x9207  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
#define  ATTINY45   0x9206  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
#define  ATTINY84   0x930C  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
#define  ATTINY85   0x930B  // L: 0x62, H: 0xDF, E: 0xFF    8 pin

void setup() {
  Serial.begin(9600);

  // configure programming pins
  digitalWrite(RST, LOW);  // turn off 12V
  digitalWrite(VCC, LOW);
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  digitalWrite(SDO, LOW);
  pinMode(VCC, OUTPUT);
  pinMode(RST, OUTPUT);
  pinMode(SDI, OUTPUT);
  pinMode(SII, OUTPUT);
  pinMode(SCI, OUTPUT);
  pinMode(SDO, OUTPUT);     // configured as input when in programming mode

}

void loop() {

  switch (getCommand()) {
    case 48:
      readFuseOnly = true;
      break;
    case 49:
      targetHVal = HVAL1;
      targetLVal = LVAL1;
      targetEVal = EVAL1;
      readFuseOnly = false;
      break;
    case 50:
      targetHVal = HVAL2;
      targetLVal = LVAL2;
      targetEVal = EVAL2;
      readFuseOnly = false;
      break;
    default:
      readFuseOnly = true;
  }

  // power up target device in high voltage programming mode
  pinMode(SDO, OUTPUT);    // Set SDO to output
  digitalWrite(SDI, LOW);
  digitalWrite(SII, LOW);
  digitalWrite(SDO, LOW);
  digitalWrite(RST, LOW);  // 12v Off
  digitalWrite(VCC, HIGH); // Vcc On
  delayMicroseconds(40);
  digitalWrite(RST, HIGH); // 12v On
  delayMicroseconds(10);
  pinMode(SDO, INPUT);     // Set SDO to input
  delayMicroseconds(300);

  unsigned int sig = readSignature();
  Serial.print("Signature: ");
  Serial.println(sig, HEX);
  Serial.println("Reading Fuses...");
  readFuses();

  if (!readFuseOnly) {
    Serial.println("Writing Fuses");

    writeFuse(LFUSE, targetLVal);
    writeFuse(HFUSE, targetHVal);
    writeFuse(EFUSE, targetEVal);

    Serial.println("Reading Fuses...");
    readFuses();
  }

  digitalWrite(SCI, LOW);
  digitalWrite(RST, LOW);   // 12v Off
  delayMicroseconds(40);
  digitalWrite(VCC, LOW);    // Vcc Off

}

byte shiftOut (byte val1, byte val2) {
  int inBits = 0;
  //Wait until SDO goes high
  while (!digitalRead(SDO))
    ;
  unsigned int dout = (unsigned int) val1 << 2;
  unsigned int iout = (unsigned int) val2 << 2;
  for (int ii = 10; ii >= 0; ii--)  {
    digitalWrite(SDI, !!(dout & (1 << ii)));
    digitalWrite(SII, !!(iout & (1 << ii)));
    inBits <<= 1;         inBits |= digitalRead(SDO);
    digitalWrite(SCI, HIGH);
    digitalWrite(SCI, LOW);
  }
  return inBits >> 2;
}

void writeFuse (unsigned int fuse, byte val) {
  shiftOut(0x40, 0x4C);
  shiftOut( val, 0x2C);
  shiftOut(0x00, (byte) (fuse >> 8));
  shiftOut(0x00, (byte) fuse);
}

void readFuses () {
  byte val;

  shiftOut(0x04, 0x4C);  // LFuse
  shiftOut(0x00, 0x68);
  val = shiftOut(0x00, 0x6C);
  Serial.print("LFuse:");
  Serial.print(val, HEX);

  shiftOut(0x04, 0x4C);  // HFuse
  shiftOut(0x00, 0x7A);
  val = shiftOut(0x00, 0x7E);
  Serial.print(", HFuse: ");
  Serial.print(val, HEX);

  shiftOut(0x04, 0x4C);  // EFuse
  shiftOut(0x00, 0x6A);
  val = shiftOut(0x00, 0x6E);
  Serial.print(", EFuse: ");
  Serial.println(val, HEX);
}

unsigned int readSignature () {
  unsigned int sig = 0;
  byte val;
  for (int ii = 1; ii < 3; ii++) {
    shiftOut(0x08, 0x4C);
    shiftOut(  ii, 0x0C);
    shiftOut(0x00, 0x68);
    val = shiftOut(0x00, 0x6C);
    sig = (sig << 8) + val;
  }
  return sig;
}

int getCommand() {

  // We must get a valid input to proceed
  int reply;

  do {
    Serial.println();
    Serial.println("Enter 0 to read fuses (no writing)");
    Serial.println("Enter 1 to use Digispark defaults");
    Serial.println("Enter 2 to use P5 as Reset pin again");
    Serial.println();
    while (!Serial.available()) {
      // Wait for user input
    }
    reply = Serial.read();
  }
  while (!(reply == 48 || reply == 49 || reply == 50));

  return reply;
}
