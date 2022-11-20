#include "led_strip.h"
#include "wires.h"
#include <Arduino.h>


Wires::Wires(int wr_gr_1[5], int wr_gr_2[5], int wr_leds_1[5], int wr_leds_2[5], int wr_done_led, LedStrip& strip){
            
    for(int i = 0; i < 5; i++){
        this->wr_gr_1[i] = wr_gr_1[i];
        this->wr_gr_2[i] = wr_gr_2[i];
        this->wr_leds_1[i] = wr_leds_1[i];
        this->wr_leds_2[i] = wr_leds_2[i];
    }

    this->wr_done_led = wr_done_led;
    this->strip = strip;
}
 
void Wires::Wires_init(){

    randomSeed(analogRead(A2));

    for(int i = 0; i < 5; i++){
        int j = random(0,5);

        uint32_t _tmp = this->wr_colors_1[j];
        this->wr_colors_1[j] = this->wr_colors_1[i];
        this->wr_colors_1[i] = _tmp;

        j = random(0, 5);
        _tmp = this->wr_colors_2[j];
        this->wr_colors_2[j] = this->wr_colors_2[i];
        this->wr_colors_2[i] = _tmp;
    }

    for(int i = 0; i < 5; i++){
        pinMode(this->wr_gr_1[i], OUTPUT);
        digitalWrite(this->wr_gr_1[i], LOW);
        pinMode(this->wr_gr_2[i], INPUT);
        this->strip.setPixelColor(this->wr_leds_1[i], this->wr_colors_1[i]);
        this->strip.setPixelColor(this->wr_leds_2[i], this->wr_colors_2[i]);
        this->strip.show();
        this->strip.setSPixelColor(13, 255, 255, 255, 20, 50);
        this->strip.setSPixelColor(13, 0, 0, 0, 20, 50);
    }

}

bool Wires::Wires_loop(){
    uint8_t done = 0;
    uint8_t sum = 0;

    while(done != sum || done !=5){
        done = 0;
        sum = 0;

        for(int i = 0; i < 5; i++){
            digitalWrite(this->wr_gr_1[i], LOW);
        }

        for(int out = 0; out < 5; out++){
            digitalWrite(this->wr_gr_1[out], HIGH);

            for(int in = 0; in < 5; in++){
                if(digitalRead(this->wr_gr_2[in]) == HIGH){
                    sum++;
                    if(this->wr_colors_1[out] == this->wr_colors_2[in]){
                        done++;
                    }
                }
            }

            digitalWrite(this->wr_gr_1[out], LOW);
        }
    }

    return true;
}

void Wires::Wires_done(){
    this->strip.setPixelColor(this->wr_done_led, this->strip.GREEN);
    
    for(int i = 0; i < 3; i++){
        this->strip.setSPixelsColor(5, this->wr_leds_1, 50, 255, 50, 25, 50);
        this->strip.setSPixelsColor(5, this->wr_leds_2, 50, 255, 50, 25, 50);

        this->strip.setSPixelsColor(5, this->wr_leds_1, 50, 50, 50, 25, 50);
        this->strip.setSPixelsColor(5, this->wr_leds_2, 50, 50, 50, 25, 50);
    }
    this->strip.setSPixelsColor(5, this->wr_leds_1, 50, 255, 50, 25, 50);
    this->strip.setSPixelsColor(5, this->wr_leds_2, 50, 255, 50, 25, 50);
}
    
