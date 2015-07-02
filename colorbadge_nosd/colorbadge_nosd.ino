#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

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
int button1state = 0; // For reading Button 1
int button2state = 0; // For reading Button 2

void setup(void) {
  pinMode(TFT_DIM, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  //Rotation 0: Pins are DOWN.
  //Rotation 1: Pins are RIGHT.
  //Rotation 2: Pins are UP.
  //Rotation 3: Pins are LEFT.
  //Default design rotation is 3.
  setLight(bright);
  tft.fillScreen(ST7735_BLACK);
  drawtext("Digibadge Starting...", ST7735_WHITE, 0, 0);
  delay(1000);
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
    if (badge > 3) {
      // Don't let the number get too high. Only 3 badges.
      badge = 1;
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
    char brt[1];
    brt[0] = char(47+bright);
    drawtext(brt, ST7735_BLACK, 1, 1);
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
  if (tft.getRotation() == 0 || tft.getRotation() == 2){
    tft.fillCircle(64, 80, 42, ST7735_WHITE);
    tft.fillCircle(64, 80, 34, ST7735_GREEN);
  }
  else {
    tft.fillCircle(80, 64, 42, ST7735_WHITE);
    tft.fillCircle(80, 64, 34, ST7735_GREEN);
  }
}
void drawYellow() {
  tft.fillScreen(ST7735_YELLOW);
  if (tft.getRotation() == 0 || tft.getRotation() == 2){
    tft.fillRect(22, 68, 84, 16, ST7735_WHITE);
  }
  else {
    tft.fillRect(22, 56, 116, 16, ST7735_WHITE);
  }
}
void drawRed() {
  tft.fillScreen(ST7735_RED);
  if (tft.getRotation() == 0 || tft.getRotation() == 2){
    tft.fillRect(22, 38, 84, 84, ST7735_WHITE);
    tft.fillRect(30, 46, 68, 68, ST7735_RED);
  }
  else {
    tft.fillRect(38, 22, 84, 84, ST7735_WHITE);
    tft.fillRect(46, 30, 68, 68, ST7735_RED);
  }
}

void drawtext(char *text, uint16_t color, int x, int y) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}
