#include <Arduino.h>
#include "modules/led_strip.h"
#include "modules/wires.h"
#include <TM1638.h>
#include <IRremote.h>
#include <GyverTimers.h>

#define IR



/*
    commands: group_command
    groups:
        wr = wires connect
        tb = tumblers
        rb = rotator + button
        lk = LED & KEY module
        ir = IR part
    commands:
        start - call only once
        loop - call to run (arg is mode)
        done - call every xx ms after is done
    vars:
        done_n - number of how many times is done

*/

#define LED_PIN 22
#define LED_NUM 30


LedStrip strip = LedStrip(LED_NUM, LED_PIN);


/*

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800); // MOVED

#define strip.RED strip.gamma32(strip.Color(255, 0, 0))               // MOVED
#define strip.GREEN strip.gamma32(strip.Color(0, 255, 0))             // MOVED
#define strip.BLUE strip.gamma32(strip.Color(0, 0, 255))              // MOVED
#define strip.FUKSIA strip.gamma32(strip.Color(255, 0, 255))          // MOVED
#define strip.WHITE strip.gamma32(strip.Color(255, 255, 255))         // MOVED
#define strip.YELLOW strip.gamma32(strip.Color(255, 255, 0))          // MOVED

void strip.setSPixelColor(int n, int r, int g, int b, int steps=30, int del=5){
    uint32_t color = strip.getPixelColor(n);
    uint8_t r_now = color >> 16;
    uint8_t g_now = color >> 8;
    uint8_t b_now = color;
    if(r_now == r && g_now == g && b_now == b){
        return;
    }
    uint8_t r_change = r - r_now;
    uint8_t g_change = g - g_now;
    uint8_t b_change = b - b_now;
    for(int i = 0; i < steps; i++){
        r_now += int(r_change / float(steps));
        g_now += int(g_change / float(steps));
        b_now += int(b_change / float(steps));
        strip.setPixelColor(n, strip.gamma32(strip.Color(r_now, g_now, b_now)));
        strip.show();
        delay(del);
    }
    strip.setPixelColor(n, strip.gamma32(strip.Color(r, g, b)));
    strip.show();
}

*/


uint8_t power_leds[5] = {25, 26, 27, 28, 29};
uint8_t side_leds[2] = {0, 0}; // power 0 to 5
unsigned int power = 0;
bool hardmode = false;
bool easymode = false;


bool debug_msg(String msg){
    Serial.print("[DEBUG]");
    Serial.println(msg);
    return true;
}



// =========== IR
#define IRLED 23
#define IRREC 44
unsigned long encodeCMD(int byte1, int byte2){
  unsigned long res = byte1;
  res <<= 8;
  res |= byte2;
  res <<= 8;
  res |= 0xE8;
  return res;
}

uint32_t IrData = encodeCMD(0xA1, 20);
class transmitter{
    public:
        transmitter(int pin){
            pinMode(pin, OUTPUT);
            IrSender.begin(pin,false);
        }
        void send(uint32_t data){
            IrSender.sendMiles(data, 24);
        }
};

transmitter trans = transmitter(IRLED);

/*
    ================= NOT WORKING YET
*/

bool ir_pause = false;

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

int decodeData(uint8_t address, uint16_t command){
  bool isCommand = true;
  address = reverse(address);
  command = reverse(command);
  if(command < 0b10000000) {isCommand = false;}
  if(isCommand){
    return 0;
  }
  address >>= 2;
  Serial.print("Team: ");
  int team = int(address) >> 4;
  Serial.println(team); // Team
  if(team != 0){
    return command;
  } else {
    return 0;
  }
}


void ir_start(){
   strip.setPixelColor(24, strip.Color(255,255,255));
    IrReceiver.begin(IRREC);
    strip.setPixelColor(24, strip.Color(255,255,255));
}


int ir_get(int id=-1){
   
    int inner_id = -1;
    IrReceiver.resume();
    debug_msg("Entestrip.RED ir_get");
    while(inner_id < 0){
        if(IrReceiver.decode()){
            Serial.println("Got Shot");
            if(ir_pause){
                debug_msg("ir_get paused");
                IrReceiver.resume();
            }else if(IrReceiver.decodedIRData.protocol == MILES){
                inner_id = decodeData(IrReceiver.decodedIRData.address, IrReceiver.decodedIRData.command);
                
                debug_msg("Got shot from id = " + String(id));
                IrReceiver.resume();
                ir_pause = false;
                if(id != -1 && inner_id != id){
                    inner_id = -1;
                }
                return inner_id;
            }else{
                IrReceiver.resume();
                strip.setPixelColor(24, strip.Color(0,0,0));
                debug_msg("Some shit");
            }
        }
    }
   
}


// =========== WIRE CONNECT

int wr_gr_1[5] = {34, 36, 38, 40, 42};
int wr_gr_2[5] = {35, 37, 39, 41, 43};
uint32_t wr_colors_1[5] = {strip.RED, strip.GREEN, strip.BLUE, strip.FUKSIA, strip.WHITE};
uint32_t wr_colors_2[5] = {strip.RED, strip.GREEN, strip.BLUE, strip.FUKSIA, strip.WHITE};

int wr_colors_1_leds[5] = {23, 22, 21, 20, 19};
int wr_colors_2_leds[5] = {14, 15, 16, 17, 18};
int wr_colors_3_leds[1] = {13};
int wr_done_n = 0;


bool wr_start(){
   
 
 
 
         
   
   
    randomSeed(analogRead(A2));
      
 
  
    for(int i = 0; i < 5; i++){
        int j = random(0, 5);

        
        uint32_t _tmp = wr_colors_1[j];
        wr_colors_1[j] = wr_colors_1[i];
        wr_colors_1[i] = _tmp;

        j = random(0, 5);
        _tmp = wr_colors_2[j];
        wr_colors_2[j] = wr_colors_2[i];
        wr_colors_2[i] = _tmp;
        
    }

    for(int i = 0; i < 5; i++){
        pinMode(wr_gr_1[i], OUTPUT);
        digitalWrite(wr_gr_1[i], LOW);
        pinMode(wr_gr_2[i], INPUT);
        strip.setPixelColor(wr_colors_1_leds[i], wr_colors_1[i]);
        strip.setPixelColor(wr_colors_2_leds[i], wr_colors_2[i]);
        strip.show();
    strip.setPixelColor(13, strip.Color(255,255,255));
     delay (1000);
      strip.setPixelColor(13, strip.Color(0,0,0));
      delay (1000);
  
    }

    debug_msg("wr initialized!");
    return true;
    
}


bool wr_loop(){
    uint8_t done = 0;
    uint8_t sum = 0;



    debug_msg("Wires started!");

    while(done != sum || done != 5){
        done = 0;
        sum = 0;

        for(int i = 0; i < 5; i++){
            digitalWrite(wr_gr_1[i], LOW);
        }

        for(int out = 0; out < 5; out++){
            digitalWrite(wr_gr_1[out], HIGH);

            for(int in = 0; in < 5; in++){
                if(digitalRead(wr_gr_2[in]) == HIGH){
                    sum++;
                    if(wr_colors_1[out] == wr_colors_2[in]){
                        done++;
                    }
                }
            }

            digitalWrite(wr_gr_1[out], LOW);
        }

        debug_msg("done | sum");
        debug_msg(String(done) + String(" | ") + String(sum));
    }
  
    wr_done_n = 1;
    return true;
}


void wr_done(){
    int wr_colors[10] = {19, 18, 17, 16, 15, 14, 20, 21, 22, 23};
    for(int nenadotut = 0; nenadotut < 2; nenadotut++){
        for(int i = 0; i < 10; i++){
            int on = i+1;
            int off = i-1;
            if(on == 10){on = 0;}
            if(off == -1){off = 9;}
            strip.setSPixelColor(wr_colors[off], 0, 0, 0, 30, 2);
            strip.setSPixelColor(wr_colors[on], 0, 255, 0, 30, 2);
        }
    }
    for(int i = 0; i < 10; i++){
        strip.setSPixelColor(wr_colors[i], 0, 255, 0, 30, 2);
    }
    strip.setPixelColor(13, strip.Color(0,0,0));
}



// ====================== TUMBLERS

uint8_t tb_done_n = 0;
uint8_t tb_pins[10] = {25, 27, 29, 31, 33,
                        32, 30, 28, 26, 24};
uint8_t tb_leds[10] = {5, 4, 3, 2, 1,
                        10, 9, 8, 7, 6};
uint32_t tb_colors[10] = { strip.RED, strip.GREEN, strip.BLUE, strip.FUKSIA, strip.WHITE,
                            strip.RED, strip.GREEN, strip.BLUE, strip.FUKSIA, strip.WHITE};
bool tb_data[10];


bool tb_start(){
    randomSeed(analogRead(A5));
    
    for(int i = 0; i < 10; i++){
        pinMode(tb_pins[i], INPUT_PULLUP);
        tb_data[i] = random(0, 2);

        int j = random(0, 10);
        uint32_t _tmp = tb_colors[j];
        tb_colors[j] = tb_colors[i];
        tb_colors[i] = _tmp;

        if(hardmode){
            j = random(0, 10);
            _tmp = tb_pins[j];
            tb_pins[j] = tb_pins[i];
            tb_pins[i] = _tmp;
        }
    }

    debug_msg("Tumblers initialized!");
    return true;
}

bool tb_mode1(){
    debug_msg("Started tb_mode1");
    // mode id G->G
    strip.setPixelColor(side_leds[0], strip.GREEN);
    strip.setPixelColor(side_leds[1], strip.GREEN);

    bool _done = false;
    while(!_done){
        _done = true;

        for(int i = 0; i < 10; i++){
            if(digitalRead(tb_pins[i]) == tb_data[i]){
                strip.setPixelColor(tb_leds[i], tb_colors[i]);
            } else {
                strip.setPixelColor(tb_leds[i], 0);
                _done = false;
            }
        }
        strip.show();
    }

    tb_done_n++;
    return _done;
}

bool tb_mode2(){
    debug_msg("Started tb_mode2");
    static uint32_t tb2_chosen = tb_colors[random(0,10)];
    strip.setPixelColor(side_leds[0], strip.BLUE);
    strip.setPixelColor(side_leds[1], tb2_chosen);
    strip.show();

    bool _done = false;
    while(!_done){
        _done = true;

        for(int i = 0; i < 10; i++){
            if(digitalRead(tb_pins[i]) == tb_data[i]){
                strip.setPixelColor(tb_leds[i], tb_colors[i]);
                if(tb_colors[i] != tb2_chosen){
                    _done = false;
                }
            } else {
                strip.setPixelColor(tb_leds[i], 0);
                if(tb_colors[i] == tb2_chosen){
                    _done = false;
                }
            }
        }
        strip.show();
    }

    tb_done_n++;
    return _done;
}

bool tb_mode3(){
    debug_msg("Started tb_mode3");
    strip.setPixelColor(side_leds[0], strip.GREEN);
    uint8_t sum = 0;
    
    while(sum != 21){
        sum = 0;
        for(int i = 0; i < 10; i++){
            if(digitalRead(tb_pins[i]) == tb_data[i]){
                strip.setPixelColor(tb_leds[i], tb_colors[i]);
                
                if(tb_colors[i] == strip.RED){
                    sum += 5;
                } else if(tb_colors[i] == strip.BLUE){
                    sum += 7;
                } else if(tb_colors[i] == strip.GREEN){
                    sum += 4;
                } else if(tb_colors[i] == strip.FUKSIA){
                    sum += 6;
                } else if(tb_colors[i] == strip.WHITE){
                    sum += 1;
                }

            } else {
                strip.setPixelColor(tb_leds[i], 0);
            }
        }
        strip.show();

        if(sum > 21){
            strip.setPixelColor(side_leds[1], strip.BLUE);
        } else {
            strip.setPixelColor(side_leds[1], strip.RED);
        }
        strip.show();
    }

    tb_done_n++;
    return true;
}

bool tb_loop(int mode){
    
    debug_msg("Started tb_loop");
    if(mode == 0){
        return tb_mode1();
    } else if(mode == 1){
        return tb_mode2();
    } else if(mode == 2){
        return tb_mode3();
    }
}

void tb_done(){
    for(int nenadotut = 0; nenadotut < 2; nenadotut++){
        for(int i = 0; i < 10; i++){
            int on = i+1;
            int off = i-1;
            if(on == 10){on = 0;}
            if(off == -1){off = 9;}
            strip.setSPixelColor(tb_leds[off], 0, 0, 0, 30, 2);
            strip.setSPixelColor(tb_leds[on], 0, 255, 0, 30, 2);
        }
    }
    for(int i = 0; i < 10; i++){
        strip.setSPixelColor(tb_leds[i], 0, 255, 0, 30, 2);
    }
}


// ==================== Rot and Btn

uint8_t rb_pot = A0;
uint8_t rb_btn = 46;
int rb_done_n = 0;
uint16_t rb_pot_diff = 10;

bool rb_start(){
    pinMode(A0, INPUT);
    pinMode(rb_btn, INPUT_PULLUP);
    randomSeed(millis() / analogRead(A3));
    return true;
}


bool rb_waitBtn(){
    debug_msg("WaitButton started");

    int cnt = 0;
    while(digitalRead(rb_btn) != LOW || cnt <= 1){
        delay(15);
        if(digitalRead(rb_btn) == LOW){
            cnt++;
        }
    }
    return true;
}


bool rb_loop(int mode=0){
    
    debug_msg("Started rb_loop");
    int16_t rb_pot_target = random(0, 1024);
    bool _done = false;

    while(!_done){
        int16_t rb_pos = analogRead(rb_pot);
        int GREEN = 0;

        if(abs(rb_pos - rb_pot_target) <= rb_pot_diff){
            _done = true;
        } else {
            _done = false;
        }

        GREEN = 255 - (abs(rb_pos-rb_pot_target));
        if(GREEN < 0){
            GREEN = 0;
        }

        strip.setPixelColor(side_leds[1], strip.Color(255-GREEN, GREEN, 0));
        if(_done){
            strip.setPixelColor(side_leds[1], strip.FUKSIA);
        }
        strip.show();

        if(_done){
            if(digitalRead(rb_btn) == LOW){
                delay(10);
                if(digitalRead(rb_btn) == LOW){
                    return true;
                } else {
                    _done = false;
                }
            } else {
                _done = false;
            }
        }
        Serial.println("rb_pos rb_target diff strip.GREEN");
        Serial.print(rb_pos);
        Serial.print(" ");
        Serial.print(rb_pot_target);
        Serial.print(" ");
        Serial.print(abs(rb_pos-rb_pot_target));
        Serial.print(" ");
        Serial.println(strip.GREEN);

    }
    
    debug_msg("rb_loop done");
    return _done;
}


void rb_done(){
    for(int i = 0; i < 3; i++){
        strip.setSPixelColor(side_leds[1], 0, 0, 0, 30, 5);
        strip.setSPixelColor(side_leds[1], 0, 255, 0, 30, 5);
    }
}


// ================= LED & KEY

uint8_t lk_DIO = 49;
uint8_t lk_CLK = 47;
uint8_t lk_STB = 45;
int lk_done_n = 0;

TM1638 lk_module(lk_DIO, lk_CLK, lk_STB);


int lk_get_button_inner(TM1638 module);


int lk_get_button(TM1638 module){
    int btn;
    do{
        btn = lk_get_button_inner(module);
    } while(btn == -1);
    debug_msg("Get button = " + String(btn));
    return btn;
}

int lk_get_button_inner(TM1638 module){
    static unsigned long timer = millis();
    
    if(millis()-timer <= 250){
        return -1;
    }

    byte btns = module.getButtons();
    if(btns == 0){
        return -1;
    }
    for(int i = 0; i < 8; i++){
        if(btns & 0b00000001 == 1){
            timer = millis();
            return i;
        }
        btns >>= 1;
    }

}

void lk_failure(TM1638 module){
    unsigned long timer = millis();
    while(1){
        if(millis()-timer >= 1100){
            return;
        }
        module.setLEDs(0b11111111);
        delay(50);
        module.setLEDs(0);
        delay(50);
    }
}


void lk_win(TM1638 module){
    int l = 0;
    while(1){
        module.setLED(1, l%4);
        module.setLED(1, 7-(l%4));
        delay(100);
        module.setLED(0, l%4);
        module.setLED(0, 7-(l%4));
        l++;
        if(l == 12){
            return;
        }
    }
}


bool lk_mode0(TM1638 module){ // Repeat each one 
    debug_msg("Started lk_repeat");
    
    module.setDisplayToString(" REPEAT ");
    module.setLEDs(0);
    
    int retries = hardmode ? random(5,10) : random(15,20);
    for(int i = 0; i < retries; i++){
        delay(250);
        int led = random(8);
        module.setLED(1, led);
        int btn = -1;
    
        while(btn != led){
            btn = lk_get_button(module);
            if(btn > -1 && btn != led){
                lk_failure(module);
                if(hardmode){
                    return false;
                }
                module.setLEDs(0);
                module.setLED(1, led);
            }
        }
        module.setLEDs(0);
    }
    lk_win(module);
    return true;
}

bool lk_mode1(TM1638 module){ // turn all on 
    debug_msg("Started lk_all_on");

    module.setDisplayToString("ALL   ON");
    int sum = 0;
    module.setLEDs(0);
    bool leds[8];
    
    for(int i = 0; i < 8; i++){
        leds[i] = random(2);
        if(leds[i]){
            module.setLED(1, i);
        }
    }
    unsigned long timer = millis();
    
    while(sum != 8){
        
        if(hardmode && millis() - timer > 10000){
            lk_failure(module);
            return false;
        }
        
        sum = 0;
        int btn = lk_get_button(module);
        leds[btn] = !leds[btn];
        
        for(int i = 0; i < 8; i++){
            if(leds[i]){
                module.setLED(1, i);
                sum++;
            } else {
                module.setLED(0, i);
            }
        }
    }
    lk_win(module);
    return true;
}

bool lk_mode2(TM1638 module){ // SimonSays 
    debug_msg("Started lk_simon");
    
    module.setDisplayToString(" REPEAT ");
    module.setLEDs(0);
    delay(1000);
    randomSeed(analogRead(A3));
    int retries = hardmode ? 5 : 2;
    
    for(int i = 0; i < retries; i++){
        int retries_inner = hardmode ? 5 : 3;
        int leds[retries_inner];
    
        for(int j = 0; j < retries_inner; j++){
            leds[j] = random(8);
            module.setLED(1, leds[j]);
            delay(300);
            module.setLEDs(0);
            delay(150);
        }
    
        for(int j = 0; j < retries_inner; j++){
            int btn = -1;
    
            while(1){
                btn = lk_get_button(module);
                if(btn = leds[j]){
                    module.setLEDs(0b11111111);
                    delay(50);
                    module.setLEDs(0);
                    break;
                } else if(btn == -1){
                    continue;
                } else {
                    lk_failure(module);
                    return false;
                }
            }
        }
    }
    
    lk_win(module);
    return true;
}

bool lk_mode3(TM1638 module){ // sum of 21 
    debug_msg("Started lk_21");

    module.clearDisplay();
    module.setDisplayToString("    = 21");
    int sum = 0;
    module.setLEDs(0);
    int leds[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int leds_n[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    for(int i = 0; i < 8; i++){
        int j = random(8);
        int _tmp = leds_n[i];
        leds_n[i] = leds_n[j];
        leds_n[j] = _tmp;
    }
    unsigned long timer = millis();
    
    while(sum != 21){
        if(hardmode && millis() - timer > 20000){
            lk_failure(module);
            return false;
        }
        int btn = lk_get_button(module);
        if(btn < 0){continue;}
        leds[btn] = !leds[btn];
        sum = 0;
    
        for(int i = 0; i < 8; i++){
            if(leds[i]){
                module.setLED(1, i);
                sum += leds_n[i];
            } else {
                module.setLED(0, i);
            }
        }
    
        module.clearDisplayDigit(0,0);
        module.clearDisplayDigit(1, 0);
        module.setDisplayToString(String(sum));
    }
    
    lk_win(module);
    return true;
}

bool lk_mode4(TM1638 module){ // code 
    debug_msg("Started lk_code");
    
    module.setDisplayToString("000000000");
    int sum = 0;
    module.setLEDs(0);
    int digits[8];
    
    for(int i = 0; i < 8; i++){
        digits[i] = random(10);
    }
    
    int lcd_digits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long timer = millis();
    
    while(sum != 8){
        sum= 0;
        if(hardmode && millis() - timer > 20000){
            lk_failure(module);
            return false;
        }
        int btn = lk_get_button(module);
        if(btn == -1){
            continue;
        }
        lcd_digits[btn] = (lcd_digits[btn] + 1) % 10;
        String msg;
    
        for(int i = 0; i < 8; i++){
            msg.concat(lcd_digits[i]);
            if(lcd_digits[i] == digits[i]){
                sum += 1;
                module.setLED(1, i);
            } else {
                module.setLED(0, i);
            }
        }
        module.setDisplayToString(msg);
    }
    
    lk_win(module);
    return true;
}

bool lk_mode5(TM1638 module){ // auto 
    debug_msg("Started lk_auto");
    
    module.clearDisplay();
    module.setLEDs(0);
    unsigned long timer = millis();
    
    for(int i = 0; i < 8; i++){
        module.setDisplayDigit(random(10), i, false);
    }
    
    while(millis() - timer <= 10000){
        module.setLED(random(2), random(8));
        module.setDisplayDigit(random(10), random(8), false);
        delay(10);
    }
    
    delay(100);
    if(hardmode && random(10)+1 > 7){
        lk_failure(module);
        return false;
    }    
    
    lk_win(module);
    return true;
}


bool lk_loop(int mode){ 
    debug_msg("Started lk_loop");
    bool done = false;
    while(!done){
        if(mode == 0){
            done = lk_mode0(lk_module);
        } else if(mode == 1){
            done = lk_mode1(lk_module);
        } else if(mode == 2){
            done = lk_mode2(lk_module);
        } else if(mode == 3){
            done = lk_mode3(lk_module);
        } else if(mode == 4){
            done = lk_mode4(lk_module);
        } else if(mode == 5){
            done = lk_mode5(lk_module);
        }
    }
    return done;
}



void count_power(){
    /*
        power [0; 15]
        0 - strip.RED(1-5) - strip.YELLOW(1-5) - strip.GREEN(1-5)
    */
   static uint8_t counter = 0;
   if(power > counter){
       counter++;
   }


   if(counter > 0 && counter <= 5){
        strip.setSPixelColor(power_leds[counter-1], 255, 0, 0, 30, 10);
   } else if(counter > 5 && counter <= 10){
        strip.setSPixelColor(power_leds[counter-6], 255, 255, 0, 30, 10);
   } else if(counter > 10 && counter <= 15){
        strip.setSPixelColor(power_leds[counter-11], 0, 255, 0, 30, 10);
   }
   strip.show();

   if(power > counter){
       count_power();
   } else if (power == counter){
       return;
   }
}


void setup(){
    pinMode(rb_btn, INPUT_PULLUP);
    pinMode(rb_pot, INPUT_PULLUP);
    for(int i = 0; i < 10; i++){
        pinMode(tb_pins[i], INPUT_PULLUP);
    }
    //strip.setBrightness(128);
    Serial.begin(9600);
    ir_start();
    //strip.begin();
    //strip.setBrightness(128);
    randomSeed(millis() + analogRead(A3));
    int counter = 0;
    for(int i = 0; i < 10; i++){
        if(digitalRead(tb_pins[i])){counter++;}
    }
    debug_msg(String(counter));
    if(counter < 3){easymode = true;}
    if(counter > 7){hardmode = true;}
    uint32_t set_color;
    if(easymode){
        set_color = strip.GREEN;
    } else if(hardmode){
        set_color = strip.RED;
    } else {
        set_color = strip.YELLOW;
    }
    for(int i = 0; i < LED_NUM + 3; i++){
        if(i < LED_NUM){
            strip.setPixelColor(i, set_color);
        }
        if(i-3 >= 0){
            strip.setPixelColor(i-3, 0);
        }
        strip.show();
        delay(50);
    }
    for(int i = 0; i < 5; i++){
        strip.setPixelColor(power_leds[i], 0);
        strip.show();
    }
    
}

/*
    Кнопка - дешифратор(код) - провода - дешифратор(все вкл) -
    тумблера(все вкл) - ротатор - дешифратор(повторить) - 
    тумблера(один цвет) - ротатор - дешифратор(21) -
    тумблера(21) - ротатор - дешифратор(саймон) - дешифратор(авто)

    Кнопка - дешифратор(код) - провода - тумблера(все вкл) - дешифратор(повторить)
*/

void loop(){
    if(easymode){
        
        #ifdef IR
        while(ir_get() < 1){
            
            delay(1);
        }
        #endif

        while(!rb_waitBtn()){ // Кнопка
            delay(5);
        }
        power++;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        //lk_loop(4); // Панель КОД
        power+=2;
        count_power();
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        
        wr_start();
        wr_loop();  // Провода
        wr_done();
        power+=4;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        tb_start();
        tb_loop(0); // Тумблера ВСЕ ВКЛ
        tb_done();
        power+=4;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        lk_loop(0); // Панель ПОВТОР
        power+=2;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        rb_loop(); //  Крутилка
        power+=2;
        count_power();
    } else {
        while(!rb_waitBtn()){ // Кнопка
            delay(5);
        }
        power++;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        lk_loop(4); // Панель КОД
        power++;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        
        wr_start();
        wr_loop();  // Провода
        wr_done();
        power+=2;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        lk_loop(1); // Панель ВСЕ ВКЛ
        power++;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        tb_start();
        tb_loop(0); // Тумблера ВСЕ ВКЛ
        tb_done();
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        rb_loop(); //  Крутилка
        power++;
        count_power();

        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        lk_loop(0); // Панель ПОВТОР
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif

        tb_loop(1); // Тумблера ОДИН ЦВЕТ
        tb_done();
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif

        rb_loop();  // Крутилка
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif

        lk_loop(3); // Панель 21
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif

        tb_loop(2); // Тумблера 21
        tb_done();
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif

        rb_loop();  // Крутилка
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif
        
        while(!lk_loop(2)){} // Панель САЙМОН
        power++;
        count_power();
        
        #ifdef IR
        while(ir_get() < 1){
            delay(1);
        }
        #endif

        lk_loop(5); // Панель АВТО
        power++;
        count_power();
    }
    while(true){
        lk_loop(5);
        trans.send(IrData);
    }
}


