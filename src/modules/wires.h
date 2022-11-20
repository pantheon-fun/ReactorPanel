#ifndef WIRES_H
#define WIRES_H

#include "led_strip.h"

class Wires{
    public:
        Wires(int wr_gr_1[5], int wr_gr_2[5], int wr_leds_1[5], int wr_leds_2[5], int wr_done_led, LedStrip& strip);
        void Wires_init();
        bool Wires_loop();
        void Wires_done();
    private:
        int wr_gr_1[5], wr_gr_2[5], wr_leds_1[5], wr_leds_2[5];
        uint32_t wr_colors_1[5] = {this->strip.RED, this->strip.GREEN, this->strip.BLUE,
                                                    this->strip.FUKSIA, this->strip.WHITE};
        uint32_t wr_colors_2[5] = {this->strip.RED, this->strip.GREEN, this->strip.BLUE,
                                                    this->strip.FUKSIA, this->strip.WHITE};
        int wr_done_led;
        LedStrip &strip;
};

#endif