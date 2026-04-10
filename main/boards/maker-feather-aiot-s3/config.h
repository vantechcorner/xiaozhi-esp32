#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

/*
 * Cytron Maker Feather AIoT S3 + Cytron ST7789 240x240 (7-pin), INMP441, MAX98357.
 *
 * Cytron display 7-pin header (website):
 *   GND  – common ground with MCU
 *   VCC  – 3.3 V (Cytron also notes 5 V worked on Arduino; keep 3.3 V with ESP32-S3)
 *   SCL  – SPI clock  → Feather D17 (SCK)
 *   SDA  – SPI MOSI   → Feather D8  (MOSI)
 *   RES  – ST7789 reset, active LOW pulse (driver keeps idle HIGH)
 *   DC   – LOW = command, HIGH = data  → Feather D18
 *   BLK  – backlight: if unconnected, backlight stays ON; pull LOW to turn OFF
 *
 * Module has no CS pin (CS tied to GND on PCB). esp_lcd still needs a dummy CS GPIO:
 *   DISPLAY_CS_PIN = D7 — leave D7 unconnected (do not wire to the LCD).
 *
 * Backlight / GPIO11 (choose one hardware setup):
 *   A) BLK → Feather VPeripheral (GPIO11 enables rail): keep DISPLAY_BACKLIGHT_PIN = 11.
 *      Firmware drives GPIO11 high before LCD init; PwmBacklight then controls brightness.
 *   B) Cytron default — BLK unconnected or tied to 3V3 (always on): set
 *      DISPLAY_BACKLIGHT_PIN to GPIO_NUM_NC and remove BLK from VPeripheral if you want
 *      simplest bring-up (no software dimming on that pin).
 */

#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// Mic and speaker share BCLK (SCK) and WS (LRCK); separate data lines -> Duplex I2S
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_41
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_42
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_40
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_48

// On-board: Status LED GPIO2 (single LED, not used by Xiaozhi Led API), BOOT GPIO0, User GPIO3, NeoPixel GPIO46
#define BOOT_BUTTON_GPIO     GPIO_NUM_0
#define USER_BUTTON_GPIO     GPIO_NUM_3
#define NEOPIXEL_GPIO        GPIO_NUM_46

#define DISPLAY_MOSI_PIN GPIO_NUM_8
#define DISPLAY_CLK_PIN  GPIO_NUM_17
#define DISPLAY_DC_PIN   GPIO_NUM_18
#define DISPLAY_RST_PIN  GPIO_NUM_16
// Module CS is tied to GND; esp_lcd still needs a real CS GPIO to toggle the SPI host.
// Use D7 — do NOT wire this to the LCD CS pin (leave D7 unconnected on the breadboard).
#define DISPLAY_CS_PIN   GPIO_NUM_7

#define DISPLAY_SPI_PCLK_HZ (20 * 1000 * 1000)

// BLK: Cytron = LOW turns backlight off → high / rail on = on → invert false is correct.
// Option B (BLK NC or to 3V3): use GPIO_NUM_NC here.
#define DISPLAY_BACKLIGHT_PIN           GPIO_NUM_11
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

#define LCD_TYPE_ST7789_SERIAL
#define DISPLAY_WIDTH           240
#define DISPLAY_HEIGHT          240
#define DISPLAY_MIRROR_X        false
#define DISPLAY_MIRROR_Y        false
#define DISPLAY_SWAP_XY         false
#define DISPLAY_INVERT_COLOR    true
#define DISPLAY_RGB_ORDER       LCD_RGB_ELEMENT_ORDER_RGB
#define DISPLAY_OFFSET_X        0
#define DISPLAY_OFFSET_Y        0
// Typical for 240x240 ST7789 modules without CS (same as bread LCD_ST7789_240X240_7PIN)
#define DISPLAY_SPI_MODE 3

#define LAMP_GPIO GPIO_NUM_NC

#endif  // _BOARD_CONFIG_H_
