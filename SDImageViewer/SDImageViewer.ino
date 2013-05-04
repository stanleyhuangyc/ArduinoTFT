// Arduino SD image viewer
// Written by Stanley Huang <stanleyhuangyc@gmail.com>
//
// This program requires the UTFT library.
//

#include <UTFT.h>
#include <SD.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];

// for Arduino 2009/Uno
UTFT myGLCD(ITDB28,19,18,17,16);   // Remember to change the model parameter to suit your display module!

// Uncomment the next line for Arduino Mega
//UTFT myGLCD(ITDB32S,38,39,40,41);   // Remember to change the model parameter to suit your display module!

File root;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void ShowMessage(const char* msg1, const char* msg2 = 0)
{
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(50, 190, 270, 230);
    myGLCD.setColor(255, 255, 255);
    myGLCD.setBackColor(255, 0, 0);
    myGLCD.print(msg1, CENTER, 196);
    if (msg2) myGLCD.print(msg2, CENTER, 210);
}

void LoadImage(File& file)
{
    for (int y = 0; y < SCREEN_HEIGHT && file.available(); y++) {
        uint16_t buf[SCREEN_WIDTH];
        for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
            byte l = file.read();
            byte h = file.read();
            buf[x] = ((uint16_t)h << 8) | l;
        }
        myGLCD.drawPixelLine(0, y, SCREEN_WIDTH, buf);
    }
}

void WalkDirectory(File dir)
{
    for (;;) {
        File entry =  dir.openNextFile();
        if (! entry) {
            // no more files
            break;
        }
        if (entry.isDirectory()) {
            WalkDirectory(entry);
        } else {
            ShowMessage("Loading image from SD card", entry.name());
            LoadImage(entry);
        }
        entry.close();
        delay(2000);
    }
}

void setup()
{
    // Setup the LCD
    myGLCD.InitLCD();
    myGLCD.setFont(SmallFont);
    myGLCD.fillScr(0, 0, 255);

    pinMode(10, OUTPUT);
    if (!SD.begin(10)) {
        ShowMessage("SD not ready");
        return;
    }

    delay(1000);
    root = SD.open("/PICTURE");
    WalkDirectory(root);

    ShowMessage("That's the end of the show", "Press RESET to start over");
}

void loop()
{
}
