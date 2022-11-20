Example:

LedStrip strip = LedStrip(LED_NUM, LED_PIN);

strip.RED;
strip.GREEN;
strip.BLUE;
strip.FUKSIA;
strip.WHITE;
strip.YELLOW;

strip.setSPixelColor(0, 255, 0, 0);
strip.setSPixelColor(0, 255, 0, 0, 10, 10);

strip.setSPixelsColor(5, [0,1,2,4,5], 255, 0, 0);
strip.setSPixelsColor(5, [0,1,2,4,5], 255, 0, 0, 10, 10);

uint32_t color = strip.Color(128, 128, 255);
strip.setPixelColor(5, color);

strip.show();

Больше информации в описании функций