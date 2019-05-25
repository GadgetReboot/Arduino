// Based on the Adafruit_NeoMatrix example for tiled NeoPixel matrices.  Scrolls
// text across four 4x4 NeoPixel grids, arranged as an 8x8 matrix and shows
// simple graphics drawing capabilities.
//
// For more details about using the graphics libraries, refer to:
// https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library
// https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
//
// Gadget Reboot

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define dataPin       2   // Arduino pin 2 is Neopixel data out

// This sketch uses four NeoPixel tiles that each contain 4x4 pixels (16 per tile)
// Four tiles are arranged two wide and two tall for an overall 8x8 matrix
// made from four 4x4 tiles stacked in a square
#define matrixWidth   4   // Individual matrix tiles are 4 pixels wide
#define matrixHeight  4   // Individual matrix tiles are 4 pixels high
#define tilesX        2   // Using two 4x4 tiles wide
#define tilesY        2   // Using two 4x4 tiles high

// MATRIX DECLARATION:
// Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = data output pin number
// Parameter 6 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
// Parameter 7 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 pixels)
//   NEO_GRB     Pixels are wired for GRB bitstream (v2 pixels)
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA v1 pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

// Example with four 4x4 matrices (created using NeoPixel flex strip --
// these grids are not a ready-made product).  In this application we'd
// like to arrange the three matrices side-by-side in a wide display.
// The first matrix (tile) will be at the left, and the first pixel within
// that matrix is at the top left.  The matrices use zig-zag line ordering.
// There's only one row here, so it doesn't matter if we declare it in row
// or column order.  The matrices use 800 KHz (v2) pixels that expect GRB
// color data.

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(matrixWidth, matrixHeight, tilesX, tilesY, dataPin,
                            NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
                            NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB + NEO_KHZ800);

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF

// arrays holding color config data for scrolling text and for graphics drawings
const uint16_t textColors[] = { matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };
const uint16_t drawingColors[] = { GREEN, BLUE, CYAN, WHITE, RED, YELLOW, MAGENTA };

String msg     = "GR Matrix";      // text to scroll on display
int    x_size  = matrix.width();   // overall display matrix width
int    y_size  = matrix.height();  // overall display matrix height
int    msglen  = msg.length();     // number of characters in scrolling message
int    x_pos   = matrix.width();   // track where we are in the scrolling message display
int    pass    = 0;                // track how many times message has scrolled

void setup() {
  Serial.begin(9600);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(textColors[0]);
  Serial.println("NeoMatrix pixel grid test");
  Serial.println((String)"Matrix Pixel Width x Height: " + x_size + " x " + y_size);
}

void loop() {
  matrix.fillScreen(0);       // clear display
  matrix.setCursor(x_pos, 0); // set cursor at current horizontal scrolling position and top of display (y=0)
  matrix.print(msg);          // display the text at the current position

  // check if message has finished scrolling off the display
  // if so, change text colors and start over
  // after a few passes, go show some other demo graphics
  // before starting all over again.
  if (--x_pos < -(msglen * 6)) {
    x_pos = matrix.width();
    if (++pass >= 3) pass = 0;
    matrix.setTextColor(textColors[pass]);
    testPixels();       // light up individual pixels at random colors
    delay(1000);
    drawLines();        // show how to draw lines between x,y coordinates
    delay(1000);
    drawRectangles();   // show how to draw rectangles
    delay(1000);
    drawCircles();      // show how to draw circles
    delay(1000);
    drawTriangles();    // show how to draw triangles
    delay(1000);
    drawBars();         // draw some lines resembling an audio spectrum analyzer
    delay(1000);
  }
  matrix.show();
  delay(100);
}

void testPixels() {
  int rnd;
  matrix.fillScreen(0);
  for (int i = 0; i < x_size; i++) {
    for (int j = 0; j < y_size; j++) {
      rnd = random(0, 7);
      matrix.drawPixel(i, j, drawingColors[rnd]);
      matrix.show();
      delay(75);
    }
  }
}

void drawLines() {
  matrix.fillScreen(0);
  matrix.drawLine(0, 0, 7, 7, BLUE);
  matrix.show();
  delay(200);
  matrix.drawLine(7, 0, 0, 7, YELLOW);
  matrix.show();
  delay(200);
  matrix.drawLine(4, 0, 4, 7, RED);
  matrix.show();
  delay(200);
  matrix.drawLine(0, 3, 7, 3, WHITE);
  matrix.show();
  delay(200);
}

void drawRectangles() {
  matrix.fillScreen(0);
  for (int i = 0; i < matrix.width() / 2; i++) {
    matrix.drawRect(i, i, matrix.width() - i * 2 , matrix.width() - i * 2 , drawingColors[i]);
    matrix.show();
    delay(200);
  }
}

void drawCircles() {
  matrix.fillScreen(0);
  for (int i = 0; i < matrix.width() / 2 - 1; i++) {
    matrix.drawCircle(matrix.width() / 2 - 1, matrix.width() / 2 - 1, i + 1, drawingColors[i]);
    matrix.show();
    delay(200);
  }
}

void drawTriangles() {
  matrix.fillScreen(0);
  matrix.drawTriangle(0, 3, 3, 6, 6, 3, GREEN);
  matrix.show();
  delay(200);
  matrix.drawTriangle(6, 1, 6, 7, 3, 4, MAGENTA);
  matrix.show();
  delay(200);
  matrix.drawTriangle(0, 7, 7, 0, 0, 0, YELLOW);
  matrix.show();
  delay(200);
}

void drawBars() {

  int rnd;
  for (int i = 0; i < 10; i++) {
    matrix.fillScreen(0);
    rnd = random(1, 7);
    matrix.drawLine(0, 7, 0, rnd, GREEN);
    matrix.drawPixel(0, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(1, 7, 1, rnd, GREEN);
    matrix.drawPixel(1, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(2, 7, 2, rnd, GREEN);
    matrix.drawPixel(2, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(3, 7, 3, rnd, GREEN);
    matrix.drawPixel(3, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(4, 7, 4, rnd, GREEN);
    matrix.drawPixel(4, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(5, 7, 5, rnd, GREEN);
    matrix.drawPixel(5, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(6, 7, 6, rnd, GREEN);
    matrix.drawPixel(6, rnd - 1, RED);
    matrix.show();

    rnd = random(1, 7);
    matrix.drawLine(7, 7, 7, rnd, GREEN);
    matrix.drawPixel(7, rnd - 1, RED);
    matrix.show();
    delay(300);
  }
}
