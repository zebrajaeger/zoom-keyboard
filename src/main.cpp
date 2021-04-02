#include <Arduino.h>
#include <FreeRTOS.h>

#include <BleKeyboard.h>

#include "touch_button.h"
#include "led_sequence.h"
#include "util.h"

// configurable
const uint16_t sleep_threshold_s = 300;
const int touch_threshold = 30;
const int led_pin = LED_BUILTIN;
const uint8_t space_input = T3;
const uint8_t alt_a_input = T0;
LedSequence seq_on("111000000000000");
LedSequence seq_connected("1010000000");
LedSequence seq_pressing_space("1");
const char *name = "Zoom Keyboard v1.0";

// !configurable
BleKeyboard bleKeyboard(name);
TimerHandle_t xAutoReloadTimer, xLedTimer, xSleepTimer, xTestTimer;
TouchButton alt_a;
TouchButton spacebar;
volatile uint16_t sleep_countdown_s = 0;

RTC_DATA_ATTR int bootCount = 0;

volatile bool sendAltA = false;
volatile bool sendSpaceOn = false;
volatile bool sendSpaceOff = false;

volatile LedSequence *current_sequence = &seq_connected;

/**
 * 
 */
void reset_countdown()
{
    Serial.println('R');
    sleep_countdown_s = 0;
}

void set_default_led_sequence()
{
    current_sequence = bleKeyboard.isConnected() ? &seq_connected : &seq_on;
}

/**
 * 
 */
void setup()
{
    Serial.begin(115200);

    if (bootCount++ == 0)
    {
        delay(1000); // give me time to bring up serial monitor - but only on first boot
    }

    Serial.println(name);
    Serial.println("Boot number: " + String(bootCount));
    bleKeyboard.begin();

    print_wakeup();

    pinMode(led_pin, OUTPUT);

    alt_a.onStateChange([](bool state) {
        reset_countdown();
        if (state)
        {
            sendAltA = true;
        }
    });

    spacebar.onStateChange([](bool state) {
        reset_countdown();
        if (state)
        {
            sendSpaceOn = true;
        }
        else
        {
            sendSpaceOff = true;
        }
    });

    touchAttachInterrupt(
        space_input, [] {}, touch_threshold);
    touchAttachInterrupt(
        alt_a_input, [] {}, touch_threshold);
    esp_sleep_enable_touchpad_wakeup();

    xAutoReloadTimer = xTimerCreate("Button", pdMS_TO_TICKS(5), pdTRUE, 0, [](TimerHandle_t xTimer) {
        spacebar.update(touchRead(space_input) < touch_threshold);
        alt_a.update(touchRead(alt_a_input) < touch_threshold);
    });

    xTimerStart(xAutoReloadTimer, 0);

    xSleepTimer = xTimerCreate("Sleep", pdMS_TO_TICKS(1000), pdTRUE, 0, [](TimerHandle_t xTimer) {
        if (++sleep_countdown_s > sleep_threshold_s)
        {
            Serial.println("Going to sleep now");
            digitalWrite(led_pin, false);
            esp_deep_sleep_start();
        }
    });
    xTimerStart(xSleepTimer, 0);

    xLedTimer = xTimerCreate("Led", pdMS_TO_TICKS(100), pdTRUE, 0, [](TimerHandle_t xTimer) {
        digitalWrite(led_pin, current_sequence->getNextState());
    });
    xTimerStart(xLedTimer, 0);
}

/**
 *
 */
void loop()
{
    if (sendAltA)
    {
        sendAltA = false;
        Serial.println("ALT+A");
        if (bleKeyboard.isConnected())
        {
            Serial.println("SEND");
            bleKeyboard.press(KEY_LEFT_ALT);
            bleKeyboard.write('a');
            bleKeyboard.release(KEY_LEFT_ALT);
        }
    }

    if (sendSpaceOn)
    {
        sendSpaceOn = false;
        {
            current_sequence = &seq_pressing_space;
            Serial.println("SPACE_ON");
            if (bleKeyboard.isConnected())
            {
                Serial.println("SEND");
                bleKeyboard.press(' ');
            }
        }
    }

    if (sendSpaceOff)
    {
        sendSpaceOff = false;
        {
            set_default_led_sequence();
            Serial.println("SPACE_OFF");
            if (bleKeyboard.isConnected())
            {
                Serial.println("SEND");
                bleKeyboard.release(' ');
            }
        }
    }
}