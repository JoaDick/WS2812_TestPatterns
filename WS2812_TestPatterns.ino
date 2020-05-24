

#include <FastLED.h>

#define LED_PIN 6

#define PIN_BUTTON_NEXT 2
#define PIN_BUTTON_RESET 3

//#define LED_COLOR_ORDER RGB
#define LED_COLOR_ORDER GRB

#define LED_TYPE WS2812B
#define NUM_LEDS 300

bool lastButtonState = false;

CRGB leds[NUM_LEDS];

uint8_t patternIndex = 0;

uint16_t animationLedIndex = 0;
uint8_t animationColorIndex = 0;

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))

void resetPattern()
{
    patternIndex = 0;
    animationLedIndex = 0;
    animationColorIndex = 0;
    FastLED.clear();
}

void setup()
{
    pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);
    pinMode(PIN_BUTTON_RESET, INPUT_PULLUP);


    Serial.begin(115200);
    Serial.println(F("Hello, World!"));

    FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    resetPattern();
    FastLED.show();
}

void loop()
{
    if (!digitalRead(PIN_BUTTON_RESET))
    {
        resetPattern();
    }
    const bool buttonState = !digitalRead(PIN_BUTTON_NEXT);
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

typedef void (*PatternFct)();

const PatternFct patternFunctions[] =
    {
        &displayAnimation,
        &displayRGB,
        &displayRuler,

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

void selectNextPattern()
{
    if (patternFunctions[++patternIndex] == nullptr)
    {
        resetPattern();
    }
}

void showPattern()
{
    patternFunctions[patternIndex]();
    FastLED.show();
}

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

    leds[animationLedIndex] = colorTable[animationColorIndex];

    if (++animationLedIndex >= NUM_LEDS)
    {
        animationLedIndex = 0;
        if (++animationColorIndex >= ARRAYLEN(colorTable))
        {
            animationColorIndex = 0;
        }
    }
}

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
        uint16_t index = (i / 5) % ARRAYLEN(colorTable);
        const CRGB color = colorTable[index];
        leds[i] = color;
    }
}

void displayColor(const CRGB &color)
{
    FastLED.clear();
    for (uint16_t i = 0; i < NUM_LEDS; ++i)
    {
        leds[i] = color;
    }
}
