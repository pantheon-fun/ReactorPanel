#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "led_strip.h"



LedStrip::LedStrip(int LED_NUM, int LED_PIN, int brightness=220){
    this->strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB+NEO_KHZ800);
    this->strip.begin();
    this->strip.fill(0,0,0);
    this->strip.setBrightness(brightness);
    this->strip.show();
    this->RED = this->strip.gamma32(strip.Color(255, 0, 0));
    this->GREEN = this->strip.gamma32(strip.Color(0, 255, 0));
    this->BLUE = this->strip.gamma32(strip.Color(0, 0, 255));
    this->FUKSIA = this->strip.gamma32(strip.Color(255, 0, 255));
    this->WHITE = this->strip.gamma32(strip.Color(255, 255, 255));
    this->YELLOW = this->strip.gamma32(strip.Color(255, 255, 0));
}

void LedStrip::setSPixelColor(int n, int r, int g, int b, int steps=5, int del=15){
    /*
        Функция плавного изменения цвета светодиода
        int n: номер светодиода
        int r: красный цвет (0-255)
        int g: зеленый цвет (0-255)
        int b: синий цвет (0-255)
        int steps: кол-во шагов (больше ставить не стоит, сломается) по умолчанию = 5
        int delay: задержка между шагами, по умолчанию = 15
    */
    uint32_t color = this->strip.getPixelColor(n);
    uint8_t r_now = color >> 16;
    uint8_t g_now = color >> 8;
    uint8_t b_now = color;

    if(r_now == r && g_now == g && b_now == b){
        return;
    }

    int r_change = r - r_now;
    int g_change = g - g_now;
    int b_change = b - b_now;

    for(int i = 0; i < steps; i++){
        r_now += int(r_change / float(steps));
        g_now += int(g_change / float(steps));
        b_now += int(b_change / float(steps));
                
        if(r_change > 0 && r_now > r){r_now = r;}
        if(g_change > 0 && g_now > g){g_now = g;}
        if(b_change > 0 && b_now > b){b_now = b;}
                
        if(r_change < 0 && r_now < r){r_now = r;}
        if(g_change < 0 && g_now < g){g_now = g;}
        if(b_change < 0 && b_now < b){b_now = b;}

        this->strip.setPixelColor(n, this->strip.gamma32(this->strip.Color(r_now, g_now, b_now)));
        this->strip.show();
        delay(del);
    }
    this->strip.setPixelColor(n, this->strip.gamma32(this->strip.Color(r, g, b)));
    this->strip.show();
}

void LedStrip::setSPixelsColor(int size, int n[], int r, int g, int b, int steps=5, int del=15){
    /*
        Функция плавного изменения цвета нескольких светодиодов (к одному цвету)
        int size: кол-во светодиодов
        int n: массив с номерами светодиодов
        int r: красный цвет (0-255)
        int g: зеленый цвет (0-255)
        int b: синий цвет (0-255)
        int steps: кол-во шагов (больше ставить не стоит, сломается) по умолчанию = 5
        int del: задержка между шагами, по умолчанию = 15
    */
    uint32_t color[size];
    uint8_t r_now[size];
    uint8_t g_now[size];
    uint8_t b_now[size];
    int r_change[size];
    int g_change[size];
    int b_change[size];

    for (int i = 0; i < size; i++){
        color[i] = this->strip.getPixelColor(n);
        r_now[i] = color[i] >> 16;
        g_now[i] = color[i] >> 8;
        b_now[i] = color[i];

        r_change[i] = r - r_now[i];
        g_change[i] = g - g_now[i];
        b_change[i] = b - b_now[i];
    }

    for(int i = 0; i < steps; i++){
        for (int i = 0; i < size; i++){
            r_now[i] += int(r_change[i] / float(steps));
            g_now[i] += int(g_change[i] / float(steps));
            b_now[i] += int(b_change[i] / float(steps));
        
            this->strip.setPixelColor(n[i], this->strip.gamma32(this->strip.Color(
                r_now[i], g_now[i], b_now[i]
            )));
        }
        this->strip.show();
        delay(del);
    }
    this->strip.setPixelColor(n, this->strip.gamma32(this->strip.Color(r, g, b)));
    this->strip.show();
}

void LedStrip::setPixelColor(int n, uint32_t color){
    this->strip.setPixelColor(n, color);
}

uint32_t LedStrip::Color(int r, int g, int b){
    return this->strip.Color(r, g, b);
}

void LedStrip::show(){
    while(!this->strip.canShow()){
        delay(5);
    }
    this->strip.show();
}

