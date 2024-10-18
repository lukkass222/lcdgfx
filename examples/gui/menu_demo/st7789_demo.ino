/*
    MIT License

    Copyright (c) 2022, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
/**
 *   Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(DIN), D13(CLK)
 *   Attiny SPI PINS:     connect LCD to D4 (D/C), GND (CS), D3 (RES), D1(DIN), D2(CLK)
 *   ESP8266: connect LCD to D1(D/C), D2(CS), RX(RES), D7(DIN), D5(CLK)
 */

/* !!! THIS DEMO RUNS in FULL COLOR MODE */
#ifndef compile

#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "owl.h"

#define TFT_MISO  -1  // Automatically assigned
#define TFT_MOSI  -1  // Automatically assigned
#define TFT_SCLK  -1  // Automatically assigned

#define TFT_CS    SS  // Chip select control pin D8
#define TFT_DC    16  // Data Command control pin
#define TFT_RST   17
#define TFT_BL  2

// The parameters are  RST pin, BUS number, CS pin, DC pin, FREQ (0 means default), CLK pin, MOSI pin
//DisplayST7789_135x240x16_SPI display(3,{-1, 4, 5, 0,-1,-1}); // Use this line for Atmega328p
//DisplayST7789_135x240x16_SPI display(22,{-1, 5, 21, 0, -1, -1}); // Use this line for ESP32 (VSPI) TTGO  (gpio23=RST, gpio5=CE for VSPI, gpio16=D/C)
//DisplayST7789_135x240x16_SPI display(3,{-1, -1, 4, 0, -1, -1}); // FOR ATTINY
//DisplayST7789_135x240x16_SPI display(-1,{-1, 0, 1, 0, -1, -1); // Use this line for nano pi (RST not used, 0=CE, gpio1=D/C)
//DisplayST7789_135x240x16_SPI display(24,{-1, 0, 23, 0,-1,-1}); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//DisplayST7789_135x240x16_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)
//DisplayST7789_135x240x16_SPI display(4,{-1, -1, 5, 0,-1,-1});  // Use this line for ESP8266 Arduino style rst=4, CS=-1, DC=5
                                                               // And ESP8266 RTOS IDF. GPIO4 is D2, GPIO5 is D1 on NodeMCU boards
//DisplayST7789_135x240x16_CustomSPI<EspSpi> display(23, 16, -1, 5, 16, 18, 19, 32000000); // ESP32 TTGO RST, DC, BUS, CS, DC, CLK, MOSI, FREQ

DisplayST7789_240x320x16_SPI display(TFT_RST,{-1, TFT_CS, TFT_DC, 60000000,TFT_SCLK,TFT_MOSI});
/*
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes).
 */
const PROGMEM uint8_t heartImage[8] =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};

const PROGMEM uint8_t heartImage8[ 8 * 8 ] =
{
    0x00, 0xE0, 0xE0, 0x00, 0x00, 0xE5, 0xE5, 0x00,
    0xE0, 0xC0, 0xE0, 0xE0, 0xE0, 0xEC, 0xEC, 0xE5,
    0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE5, 0xEC, 0xE5,
    0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE5, 0xE0,
    0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xE0, 0xE0, 0x00,
    0x00, 0x00, 0x80, 0xE0, 0xE0, 0xE0, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xE0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


#ifndef lcd_gfx_use_menu_add

LcdGfxButton m_button_left(("left"),{{0,0},{40,14}});
LcdGfxButton m_button_mid(("mid"), {{41,0},{80,14}});
LcdGfxButton m_button_right(("right"), {{81,0},{127,14}});

LcdGfxButton* m_ButtonArray[] = {
    &m_button_left,
    &m_button_mid,
    &m_button_right,
};

const char *menuItems[] =
{
    "draw bitmap",
    "sprites",
    "fonts",
    "nano canvas",
    "draw lines",
};

#endif

LcdGfxMenu menu;

static bool bitmapDemo()
{
    display.clear();
    display.setColor(RGB_COLOR16(64,64,255));
    display.drawBitmap1(0, 0, 128, 64, Owl);
    display.drawBitmap8(0, 0, 8, 8, heartImage8);
    display.setColor(RGB_COLOR16(255,64,64));
    display.drawBitmap1(0, 16, 8, 8, heartImage);
    lcd_delay(3000);
    return true;
}

/* Sprites are not implemented for color modes.
 * But there is NanoEngine support
 * To make example clear, we use lambda as function pointer. Since lambda can be
 * passed to function only if it doesn't capture, all variables should be global.
 * Refer to C++ documentation.
 */
NanoPoint sprite;
NanoEngine16<DisplayST7789_240x320x16_SPI> engine( display );
//NanoEngine16<DisplayST7789_135x240x16_CustomSPI<EspSpi>> engine( display );
static bool spriteDemo()
{
    // We not need to clear screen, engine will do it for us
    engine.begin();
    // Force engine to refresh the screen
    engine.refresh();
    // Set function to draw our sprite
    engine.drawCallback( []()->bool {
        engine.getCanvas().clear();
        engine.getCanvas().setColor( RGB_COLOR16(255, 32, 32) );
        engine.getCanvas().drawBitmap1( sprite.x, sprite.y, 8, 8, heartImage );
        return true;
    } );
    sprite.x = 0;
    sprite.y = 0;
    for (int i=0; i<250; i++)
    {
        lcd_delay(15);
        // Tell the engine to refresh screen at old sprite position
        engine.refresh( sprite.x, sprite.y, sprite.x + 8 - 1, sprite.y + 8 - 1 );
        sprite.x++;
        if (sprite.x >= display.width())
        {
            sprite.x = 0;
        }
        sprite.y++;
        if (sprite.y >= display.height())
        {
            sprite.y = 0;
        }
        // Tell the engine to refresh screen at new sprite position
        engine.refresh( sprite.x, sprite.y, sprite.x + 8 - 1, sprite.y + 8 - 1 );
        // Do refresh required parts of screen
        engine.display();
    }
    return true;
}

static bool textDemo()
{
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    display.setColor(RGB_COLOR16(255,255,0));
    display.printFixed(0,  8, "Normal text", STYLE_NORMAL);
    display.setColor(RGB_COLOR16(0,255,0));
    display.printFixed(0, 16, "bold text?", STYLE_BOLD);
    display.setColor(RGB_COLOR16(0,255,255));
    display.printFixed(0, 24, "Italic text?", STYLE_ITALIC);
    display.setColor(RGB_COLOR16(255,255,255));
    display.invertColors();
    display.printFixed(0, 32, "Inverted bold?", STYLE_BOLD);
    display.invertColors();
    lcd_delay(3000);
    return true;
}

static bool canvasDemo()
{
    NanoCanvas<64,16,1> canvas;
    display.setColor(RGB_COLOR16(0,255,0));
    display.clear();
    canvas.clear();
    canvas.fillRect(10, 3, 80, 5);
    display.drawCanvas((display.width()-64)/2, 1, canvas);
    lcd_delay(500);
    canvas.fillRect(50, 1, 60, 15);
    display.drawCanvas((display.width()-64)/2, 1, canvas);
    lcd_delay(1500);
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.printFixed(20, 1, " DEMO ", STYLE_BOLD );
    display.drawCanvas((display.width()-64)/2, 1, canvas);
    lcd_delay(3000);
    return true;
}

static bool drawLinesDemo()
{
    display.clear();
    display.setColor(RGB_COLOR16(255,0,0));
    for (int y = 0; y < display.height(); y += 8)
    {
        display.drawLine(0,0, display.width() -1, y);
    }
    display.setColor(RGB_COLOR16(0,255,0));
    for (int x = display.width() - 1; x > 7; x -= 8)
    {
        display.drawLine(0,0, x, display.height() - 1);
    }
    lcd_delay(3000);
    return true;
}

#ifndef lcd_gfx_use_menu_add
bool (*funcArraySubMenus[])(void) = {
    bitmapDemo,
    spriteDemo,
    textDemo,
    canvasDemo,
    drawLinesDemo,
};
#endif

void setup()
{
    Serial.begin(9600);
    display.begin();

    lcd_gpioMode( TFT_BL, LCD_GPIO_OUTPUT );
    lcd_gpioWrite( TFT_BL, LCD_HIGH );
    display.setFixedFont(ssd1306xled_font6x8);

    display.fill( 0x0000 );

#ifndef lcd_gfx_use_menu_add

    menu.setItens(lcd_gfx_array_length(menuItems), menuItems, funcArraySubMenus);
    menu.setButtons(lcd_gfx_array_length(m_ButtonArray), m_ButtonArray);
#else
        menu.addItem(("draw bitmap"), &bitmapDemo);
        menu.addItem(("sprites"), &spriteDemo);
        menu.addItem(("fonts"), &textDemo);
        menu.addItem(("nano canvas"), &canvasDemo);
        menu.addItem(("draw lines"), &drawLinesDemo);

        menu.addButton(("left"),{{0,0},{40,14}} );
        menu.addButton(("mid"), {{81,0},{127,14}});
        menu.addButton(("right"), {{41,0},{80,14}});
#endif

    menu.setTopOffset(16);

#ifdef lcd_gfx_menu_custom_colors
    menu.setSelectionColor(RGB_COLOR16(255,255,0));
    menu.setTextSelectionColor(RGB_COLOR16(0,255,0));
    menu.setBackgroundColor(RGB_COLOR16(255,127,127));
#endif

    menu.show( display );
}

uint8_t rotation = 0;

void loop()
{
    lcd_delay(1000);

    if (!menu.isButtonSelected())
    {
        menu.getItem()();

        display.setColor(RGB_COLOR16(255, 255, 255));
        display.clear();
        menu.show(display);
    }

    //if ((menu.size() - 1) == menu.selection())
    if (m_button_right.isActive())
    {
        display.getInterface().setRotation((++rotation) & 0x03);
        menu.setRect();
        menu.show(display);
    }

    lcd_delay(500);
    menu.down();
    menu.show(display);
}
#endif