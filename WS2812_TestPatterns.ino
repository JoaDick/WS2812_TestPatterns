/*******************************************************************************

A little sketch to check the functionality of a WS2812 LED strip with an Arduino.
https://github.com/JoaDick/WS2812_TestPatterns

********************************************************************************

MIT License

Copyright (c) 2020 Joachim Dick

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

*******************************************************************************/

#include <FastLED.h>

// Enable one of these according to your LED strip - see also setup().
#define LED_COLOR_ORDER     RGB
//#define LED_COLOR_ORDER     RBG
//#define LED_COLOR_ORDER     GRB
//#define LED_COLOR_ORDER     GBR
//#define LED_COLOR_ORDER     BRG
//#define LED_COLOR_ORDER     BGR

// Connect the LED Strip to that pin.
#define LED_PIN 6

// Connect a pushbutton for selecting the next pattern to that pin (and GND).
#define PIN_BUTTON_NEXT_PATTERN 2

// Connect a pushbutton for selecting the first pattern to that pin (and GND).
#define PIN_BUTTON_FIRST_PATTERN 3

//------------------------------------------------------------------------------

#define LED_TYPE WS2812B
#define NUM_LEDS 300

CRGB leds[NUM_LEDS];

bool lastButtonState = false;

uint8_t patternIndex = 0;

uint16_t animationCounter = 0;
uint8_t animationColorIndex = 0;

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))

//------------------------------------------------------------------------------

void setup()
{
    pinMode(PIN_BUTTON_NEXT_PATTERN, INPUT_PULLUP);
    pinMode(PIN_BUTTON_FIRST_PATTERN, INPUT_PULLUP);

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    Serial.begin(115200);
    Serial.println(F(""));
    Serial.println(F("WS2812 Test Patterns"));
    Serial.println(F(""));
    Serial.println(F("Verify LED_COLOR_ORDER setting:"));
    Serial.println(F("First  LED should be RED"));
    Serial.println(F("Second LED should be GREEN"));
    Serial.println(F("Third  LED should be BLUE"));
    Serial.println(F("If this is not the case, adjust LED_COLOR_ORDER according to the displayed colors."));
    Serial.println(F(""));

    leds[0] = CRGB{0x20, 0, 0};
    leds[1] = CRGB{0, 0x20, 0};
    leds[2] = CRGB{0, 0, 0x20};
    FastLED.show();

    delay(5000);
    selectFirstPattern();
}

//------------------------------------------------------------------------------

void loop()
{
    if (!digitalRead(PIN_BUTTON_FIRST_PATTERN))
    {
        selectFirstPattern();
    }

    const bool buttonState = !digitalRead(PIN_BUTTON_NEXT_PATTERN);
    if (buttonState != lastButtonState)
    {
        if (buttonState == false)
        {
            selectNextPattern();
        }
    }

    lastButtonState = buttonState;

    showPattern();
    delay(10);
}

//------------------------------------------------------------------------------

typedef void (*PatternFct)();

const PatternFct patternFunctions[] =
    {
        &displayRGB,
        &displayRuler,
        &displayAnimation,
        &displayRainbow,

        []() { displayColor(CRGB{0x40, 0x00, 0x00}); }, // red
        []() { displayColor(CRGB{0x40, 0x40, 0x00}); }, // yellow
        []() { displayColor(CRGB{0x00, 0x40, 0x00}); }, // green
        []() { displayColor(CRGB{0x00, 0x40, 0x40}); }, // cyan
        []() { displayColor(CRGB{0x00, 0x00, 0x40}); }, // blue
        []() { displayColor(CRGB{0x40, 0x00, 0x40}); }, // magenta
        []() { displayColor(CRGB{0x40, 0x40, 0x40}); }, // white

        []() { displayColor(CRGB{0xFF, 0x00, 0x00}); }, // red
        []() { displayColor(CRGB{0xFF, 0xFF, 0x00}); }, // yellow
        []() { displayColor(CRGB{0x00, 0xFF, 0x00}); }, // green
        []() { displayColor(CRGB{0x00, 0xFF, 0xFF}); }, // cyan
        []() { displayColor(CRGB{0x00, 0x00, 0xFF}); }, // blue
        []() { displayColor(CRGB{0xFF, 0x00, 0xFF}); }, // magenta
        []() { displayColor(CRGB{0xFF, 0xFF, 0xFF}); }, // white

        nullptr // end
};

//------------------------------------------------------------------------------

void selectNextPattern()
{
    animationCounter = 0;
    animationColorIndex = 0;
    FastLED.clear();
    if (patternFunctions[++patternIndex] == nullptr)
    {
        selectFirstPattern();
    }
}

//------------------------------------------------------------------------------

void selectFirstPattern()
{
    patternIndex = 0;
    animationCounter = 0;
    animationColorIndex = 0;
    FastLED.clear();
}

//------------------------------------------------------------------------------

void showPattern()
{
    patternFunctions[patternIndex]();
    FastLED.show();
}

//------------------------------------------------------------------------------

void displayRuler()
{
    FastLED.clear();
    for (uint16_t i = 1; i <= NUM_LEDS; ++i)
    {
        // every 100th LED white
        if (i % 100 == 0)
        {
            leds[i - 1] = CRGB{0x80, 0x80, 0x80};
        }
        // every 50th LED blue
        else if (i % 50 == 0)
        {
            leds[i - 1] = CRGB{0, 0, 0x80};
        }
        // every 10th LED red
        else if (i % 10 == 0)
        {
            leds[i - 1] = CRGB{0x20, 0, 0};
        }
        // every 5th LED magenta
        else if (i % 5 == 0)
        {
            leds[i - 1] = CRGB{0x04, 0, 0x04};
        }
        // every 2nd (odd) LED green
        else if (i % 2)
        {
            leds[i - 1] = CRGB{0, 0x04, 0};
        }
    }
}

//------------------------------------------------------------------------------

void displayAnimation()
{
    static const CRGB colorTable[] =
        {
            CRGB{0x20, 0x00, 0x00}, // red
            CRGB{0x20, 0x20, 0x00}, // yellow
            CRGB{0x00, 0x20, 0x00}, // green
            CRGB{0x00, 0x20, 0x20}, // cyan
            CRGB{0x00, 0x00, 0x20}, // blue
            CRGB{0x20, 0x00, 0x20}, // magenta
            CRGB{0x20, 0x20, 0x20}, // white
            CRGB{0x00, 0x00, 0x00}  // black
        };

    leds[animationCounter] = colorTable[animationColorIndex];

    if (++animationCounter >= NUM_LEDS)
    {
        animationCounter = 0;
        if (++animationColorIndex >= ARRAYLEN(colorTable))
        {
            animationColorIndex = 0;
        }
    }
}

//------------------------------------------------------------------------------

void displayRGB()
{
    static const CRGB colorTable[] =
        {
            CRGB{0x10, 0x00, 0x00}, // red
            CRGB{0x00, 0x00, 0x00}, // black
            CRGB{0x00, 0x10, 0x00}, // green
            CRGB{0x00, 0x00, 0x00}, // black
            CRGB{0x00, 0x00, 0x10}, // blue
            CRGB{0x00, 0x00, 0x00}  // black
        };

    for (uint16_t i = 0; i < NUM_LEDS; ++i)
    {
        const uint16_t colorIndex = (i / 5) % ARRAYLEN(colorTable);
        const CRGB color = colorTable[colorIndex];
        const uint16_t ledIndex = (i + animationCounter / 10) % NUM_LEDS;
        leds[ledIndex] = color;
    }

    if (++animationCounter >= NUM_LEDS * 10)
    {
        animationCounter = 0;
    }
}

//------------------------------------------------------------------------------

void displayRainbow()
{
    static uint8_t hue = 0;
    fill_rainbow(leds, NUM_LEDS, hue, 5);
    EVERY_N_MILLISECONDS(20) { hue++; }
}

//------------------------------------------------------------------------------

void displayColor(const CRGB &color)
{
    FastLED.clear();
    for (uint16_t i = 0; i < NUM_LEDS; ++i)
    {
        leds[i] = color;
    }
}

//------------------------------------------------------------------------------
