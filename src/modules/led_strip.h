#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LedStrip{
    public:
        LedStrip(int LED_NUM, int LED_PIN, int brightness=220);
        void setSPixelColor(int n, int r, int g, int b, int steps=5, int del=15);
        void setSPixelsColor(int size, int n[], int r, int g, int b, int steps=5, int del=15);
        void setPixelColor(int n, uint32_t color);
        uint32_t Color(int r, int g, int b);
        void show();
        uint32_t RED;
        uint32_t GREEN;
        uint32_t BLUE;
        uint32_t FUKSIA;
        uint32_t WHITE;
        uint32_t YELLOW;
    private:
        Adafruit_NeoPixel strip;
};

#endif