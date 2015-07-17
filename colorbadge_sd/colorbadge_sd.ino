
//Andon's Color Badge SD card reading program
//Based heavily on the Adafruit ST7735 BMP loading example
//More information can be found here: https://learn.adafruit.com/1-8-tft-display/displaying-bitmaps
//Images are 24-Bit Bitmaps. I haven't tried anything else.
//Included files in the SDCARD directory should, logically, be put in the SD card's
//base directory before inserting the card into the badge's slot.
//Loading from images is MUCH slower than generating the images!

//This code, as-is, does NOT run on the Regular version.
//It simply uses a bit too much space in global variables for the 168 chip.
//I'm sure someone with more coding ability than I can manage this.

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

//The TFT display and the SD card share the SPI pins
//Other pins are as follows:
#define SD_CS    10 // Chip select line for SD card
#define TFT_CS   9  // Chip select line for TFT display
#define TFT_RST  8  // Reset line for TFT
#define TFT_DC   7  // Data/command line for TFT
#define TFT_DIM  6  // TFT backlight dimmer output
#define BUTTON1   2  // Input Button 1 - Used for switching badges
#define BUTTON2   3  // Input Button 2 - Used for adjusting dimmer output

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int bright = 10; // Ten steps of brightness. Default to 10.
int badge = 2;   // Badge 1 is Green
                 // Badge 2 is Yellow. Start here.
                 // Badge 3 is Red.
                 // Badge 4 is "My name is..."
int button1state = 0; // For reading Button 1
int button2state = 0; // For reading Button 2

void setup(void) {
  pinMode(TFT_DIM, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  //Rotation 0: Pins are DOWN.
  //Rotation 1: Pins are RIGHT.
  //Rotation 2: Pins are UP.
  //Rotation 3: Pins are LEFT.
  //Default design rotation is 1.
  setLight(bright);
  tft.fillScreen(ST7735_BLACK);  
  drawtext("Digibadge Starting...", ST7735_WHITE, 0, 0, 1, true);
  drawtext("SD Card Status: ", ST7735_WHITE, 0, 8, 1, true);
  if (!SD.begin(SD_CS)) {
    drawtext("FAIL", ST7735_RED, 96, 8, 1, true);
  }
  else {
    drawtext("OK", ST7735_WHITE, 96, 8, 1, true);
  }
  delay(500);
  drawtext("Created in 2015 by", ST7735_WHITE, 0, 32, 1, true);
  drawtext("Jason LeClare.", ST7735_WHITE, 0, 40, 1, true);
  drawtext("http://www.matchfire.net", ST7735_WHITE, 0, 48, 1, true);
  drawtext("Source available at:", ST7735_WHITE, 0, 64, 1, true);
  drawtext("http://bit.ly/1HeVcMC", ST7735_WHITE, 0, 72, 1, true);
  drawtext("For color badge info:", ST7735_WHITE, 0, 88, 1, true);
  drawtext("http://bitly.com/1jY0tzO", ST7735_WHITE, 0, 96, 1, true);
  delay(2500);
  drawBadge(badge);
}

void loop() {
  // First, check button inputs.
  button1state = digitalRead(BUTTON1);
  button2state = digitalRead(BUTTON2);
  // Now that we've read our buttons, see if we need to change anything.
  if (button1state == LOW) {
    // Button 1 was pressed. We want the next badge.
    badge += 1;
    if (badge > 3) {
      // Don't let the number get too high. Only 3 badges
      badge = 1;
    }
    delay(500); // Don't cycle rapidly through them.
    drawBadge(badge);
  }
  if (button2state == LOW) {
    // Button 2 was pressed. We want the next brightness value.
    bright += 1;
    if (bright > 10) {
      // Don't let the number get too high. Only 10 steps.
      bright = 1;
    }
    setLight(bright);
    drawnum(bright, ST7735_BLACK, 1, 1, 1, true);
    delay(500); // Don't cycle rapidly through brightness levels.
    drawBadge(badge);    
  }
}

void drawBadge(int b) {
  // Draws a badge given an input number.
  if (b == 1) {
    // Badge number 1 is Green - "Come talk to me!"
    bmpDraw("green.bmp", 0, 0);
  }
  else if (b == 2) {
    // Badge number 2 is Yellow - "Do I know you?"
    bmpDraw("yellow.bmp", 0, 0);
  }
  else if (b == 3) {
    // Badge number 3 is Red - "Don't bother me."
    bmpDraw("red.bmp", 0, 0);
  }
  else {
    // Default to Red if something gets added up wrong.
    bmpDraw("red.bmp", 0, 0);
  }
}

void setLight(int l) {
  // Sets the backlight to a specified brightness
  analogWrite(TFT_DIM, (l*20)+55);
  // AnalogWrite uses values from 0-255. As we always want the badge on
  // we set the minimum value to 75, as each bright level is 20.
}

void drawtext(char *text, uint16_t color, int x, int y, int tsize, boolean wrap) {
  settext(color, tsize, x, y, wrap);
  tft.print(text);
}

void drawnum(int text, uint16_t color, int x, int y, int tsize, boolean wrap) {
  settext(color, tsize, x, y, wrap);
  tft.print(text);
}

void settext(uint16_t color, int tsize, int x, int y, boolean wrap) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(wrap);
  tft.setTextSize(tsize);
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 85 // 85 is about the highest a 328 can go.

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    tft.fillScreen(ST7735_BLACK);
    drawtext("File not found:", ST7735_RED, 0, 0, 1, true);
    drawtext(filename, ST7735_RED, 0, 8, 1, true);
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    (void)read32(bmpFile); // Read & ignore file size
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    // Read DIB header
    (void)read32(bmpFile); // Read & ignore header size.
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) {
    tft.fillScreen(ST7735_BLACK);
    drawtext("Error loading file:", ST7735_RED, 0, 0, 1, true);
    drawtext(filename, ST7735_RED, 0, 8, 1, true);
    drawtext("Format not recognized.", ST7735_RED, 0, 16, 1, true);
  }
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

