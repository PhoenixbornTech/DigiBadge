//Andon's Color Badge basic program.
//Utilizes the ST7735 driver from Adafruit and the Adafruit Graphics Library
//Documentation can be found here: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
//Due to size constraints (And my own coding abilities), no SD card interactions are programmed in.
//However, the standard SD card library can be used. I've added the SD card chip select pin below for reference.


#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

//#define SD_CS   10  // SD card Chip Select line  
#define TFT_CS    9  // Chip select line for TFT display
#define TFT_RST   8  // Reset line for TFT
#define TFT_DC    7  // Data/command line for TFT
#define TFT_DIM   6  // TFT backlight dimmer output
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

// Mod by FrankkieNL:
// There is no way for the user to enter a name, 
// So Badge 4 should only be used for the Hacker-version,
// Where the user enters his name in this file.
// Set here if this version is a Hacker-version to enable badge 4
char* badgename = "Andon";
int isHacker = 1; //1 = true; 0 = false;

void setup(void) {
  pinMode(TFT_DIM, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
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
  delay(500);
  drawtext("Created in 2015 by", ST7735_WHITE, 0, 16, 1, true);
  drawtext("Jason LeClare.", ST7735_WHITE, 0, 24, 1, true);
  drawtext("http://www.matchfire.net", ST7735_WHITE, 0, 32, 1, true);
  drawtext("Source available at:", ST7735_WHITE, 0, 48, 1, true);
  drawtext("http://bit.ly/1HeVcMC", ST7735_WHITE, 0, 56, 1, true);
  drawtext("For color badge info:", ST7735_WHITE, 0, 72, 1, true);
  drawtext("http://bitly.com/1jY0tzO", ST7735_WHITE, 0, 80, 1, true);
  delay(2500);
  drawBadge(badge);
}
void loop() {
  // First, check button inputs.
  button1state = digitalRead(BUTTON1);
  button2state = digitalRead(BUTTON2);
  // Now that we've read our buttons, see if we need to change anything.
  if (button1state == HIGH) {
    // Button 1 was pressed. We want the next badge.
    badge += 1;
    if (isHacker == 1){
      //Enable 'My name is ...' in Hacker-version
      if (badge > 4) {
        // Don't let the number get too high. There are 4 badges in hacker version
        badge = 1;
      }
    } else {
      if (badge > 3) {
        // Don't let the number get too high. Only 3 badges in non-hacker version
        badge = 1;
      }
    }
    delay(500); // Don't cycle rapidly through them.
    drawBadge(badge);
  }
  if (button2state == HIGH) {
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

void setLight(int l) {
  // Sets the backlight to a specified brightness
  analogWrite(TFT_DIM, (l*20)+55);
  // AnalogWrite uses values from 0-255. As we always want the badge on
  // we set the minimum value to 75, as each bright level is 20.
}

void drawBadge(int b) {
  // Draws a badge given an input number.
  if (b == 1) {
    // Badge number 1 is Green - "Come talk to me!"
    drawGreen();
  }
  else if (b == 2) {
    // Badge number 2 is Yellow - "Do I know you?"
    drawYellow();
  }
  else if (b == 3) {
    // Badge number 3 is Red - "Don't bother me."
    drawRed();
  }
  else if (b == 4) {
    drawMyNameIs();
  }
  else {
    // Default to Red if something gets added up wrong.
    drawRed();
  }
}

//Following are Color Communication Badges.
//See http://blog.bronycon.org/post/92753912478/introducing-color-communications-badges-we-want
void drawGreen() {
  //
  tft.fillScreen(ST7735_GREEN);
  tft.fillCircle(80, 45, 27, ST7735_BLACK);
  tft.fillCircle(80, 45, 24, ST7735_WHITE);
  drawtext("GREEN", ST7735_BLACK, 36, 85, 3, true);
}
void drawYellow() {
  tft.fillScreen(ST7735_YELLOW);
  tft.fillRect(22, 26, 116, 25, ST7735_BLACK);
  tft.fillRect(25, 29, 110, 19, ST7735_WHITE);
  drawtext("YELLOW", ST7735_BLACK, 28, 85, 3, true);
}
void drawRed() {
  tft.fillScreen(ST7735_RED);
  tft.fillRect(53, 18, 54, 54, ST7735_BLACK);
  tft.fillRect(56, 21, 48, 48, ST7735_WHITE);
  drawtext("RED", ST7735_BLACK, 54, 85, 3, true);
}
void drawMyNameIs(){
  tft.fillScreen(ST7735_YELLOW);
  drawtext(badgename, ST7735_BLACK, 28, 85, 3, true);
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
