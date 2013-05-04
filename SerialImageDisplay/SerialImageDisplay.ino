// SerialImageDisplay
// Written by Stanley Huang <stanleyhuangyc@gmail.com>
//
// This program requires the UTFT library.
//

#include <UTFT.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];

//File myFile;

// for Arduino Leonardo
UTFT myGLCD(ITDB28,A5,A4,A3,A2);   // Remember to change the model parameter to suit your display module!

// for Arduino 2009/Uno
//UTFT myGLCD(ITDB28,19,18,17,16);   // Remember to change the model parameter to suit your display module!

// for Arduino Mega
//UTFT myGLCD(ITDB32S,38,39,40,41);   // Remember to change the model parameter to suit your display module!

#define SYNC_FRAME_START 'S'
#define SYNC_FRAME_ACK 'A'

void ShowInfo()
{
    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRoundRect(65, 100, 255, 150);

    myGLCD.setColor(255, 255, 255);
    myGLCD.setBackColor(255, 0, 0);
    myGLCD.print("MediaCoder", CENTER, 106);
    myGLCD.print("Arduino Frame Display", CENTER, 119);
    myGLCD.print("Waiting for connection", CENTER, 132);
}

void setup()
{
    Serial.begin(115200);

    // Setup the LCD
    myGLCD.InitLCD();
    myGLCD.setFont(SmallFont);
    myGLCD.fillScr(0, 0, 255);
    ShowInfo();
}

int SerialRead()
{
    unsigned long t = millis();
    while (!Serial.available()) {
        if (millis() - t > 10000)
            return -1;
    }
    return Serial.read();
}

uint16_t SerialReadInt()
{
    byte d[2] = {SerialRead(), SerialRead()};
    return *(uint16_t*)d;
}

void loop()
{
    // wait serial data
    while (!Serial.available());
    // check sync byte
    if (Serial.read() == SYNC_FRAME_START) {
        Serial.write(SYNC_FRAME_ACK);

        // get frame left/top/width/height
        uint16_t sx = SerialReadInt();
        uint16_t sy = SerialReadInt();
        uint16_t w = SerialReadInt();
        uint16_t h = SerialReadInt();

        if (w == 0xffff || h == 0xffff) {
            // timeout
            ShowInfo();
            return;
        }
        Serial.write(SYNC_FRAME_ACK);

        uint16_t pixels[320];
        for (int y = 0; y < h; y++) {
            for (int x = w - 1; x >= 0; x--) {
                int c1;
                int c2;
                if ((c1 = SerialRead()) == -1 || (c2 = SerialRead()) == -1) {
                    // timeout
                    ShowInfo();
                    return;
                }
                pixels[x] = (uint16_t)c2 << 8 | (byte)c1;
            }
            myGLCD.drawPixelLine(sx, sy + y, w, pixels);
            Serial.write(SYNC_FRAME_ACK);
        }
    }
}
