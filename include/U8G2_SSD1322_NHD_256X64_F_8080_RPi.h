#ifndef SL_PI_CPP_U8G2_SSD1322_NHD_256X64_F_8080_H
#define SL_PI_CPP_U8G2_SSD1322_NHD_256X64_F_8080_H

#include <iostream>
#include <cstdio>
#include <U8g2lib.h>
#include <u8g2.h>
#include <u8x8.h>
#include <wiringPi.h>

/**
 * Wrapper class for the SSD1322 display in 8080 mode which uses Raspberry Pi GPIO for communication.
 */
class U8G2_SSD1322_NHD_256X64_F_8080_RPI : public U8G2 {
public:
    U8G2_SSD1322_NHD_256X64_F_8080_RPI(const u8g2_cb_t *rotation, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                                       uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t rw, uint8_t cs,
                                       uint8_t dc, uint8_t reset = U8X8_PIN_NONE) : U8G2() {
        u8g2_Setup_ssd1322_nhd_256x64_f(&u8g2, rotation, u8x8_byte_8bit_8080mode, u8x8_gpio_and_delay_rpi_8080);
        u8x8_SetPin_8Bit_8080(getU8x8(), d0, d1, d2, d3, d4, d5, d6, d7, rw, cs, dc, reset);
    }

private:
    /**
    * Implements the u8x8 communication interface using Raspberry Pi GPIO.
     * Menu functions are not implemented.
     * @param u8x8 u8x8 struct with pin information
     * @param msg action to be performed (transmit message or delay)
     * @param arg_int byte to be transmitted or information about delay
     * @see https://github.com/olikraus/u8g2/wiki/Porting-to-new-MCU-platform
     */
    static uint8_t u8x8_gpio_and_delay_rpi_8080(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
        switch (msg) {
            case U8X8_MSG_GPIO_AND_DELAY_INIT:    // called once during init phase of u8g2/u8x8
                pinMode(u8x8_GetPinValue(u8x8, U8X8_PIN_E), OUTPUT);
                pinMode(u8x8_GetPinValue(u8x8, U8X8_PIN_CS), OUTPUT);
                pinMode(u8x8_GetPinValue(u8x8, U8X8_PIN_DC), OUTPUT);
                for (int i = U8X8_PIN_D0; i <= U8X8_PIN_D7; i++) {
                    pinMode(u8x8_GetPinValue(u8x8, i), OUTPUT);
                }

                break;
            case U8X8_MSG_DELAY_NANO:
                delayMicroseconds(1);
                break;
            case U8X8_MSG_DELAY_10MICRO:        // delay arg_int * 10 micro seconds
                delayMicroseconds(arg_int * 10);
                break;
            case U8X8_MSG_DELAY_MILLI:            // delay arg_int * 1 milli second
                delay(arg_int);
                break;
            case U8X8_MSG_GPIO_D0:                // D0 or SPI clock pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D0), arg_int);
                break;
            case U8X8_MSG_GPIO_D1:                // D1 or SPI data pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D1), arg_int);
                break;
            case U8X8_MSG_GPIO_D2:                // D2 pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D2), arg_int);
                break;
            case U8X8_MSG_GPIO_D3:                // D3 pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D3), arg_int);
                break;
            case U8X8_MSG_GPIO_D4:                // D4 pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D4), arg_int);
                break;
            case U8X8_MSG_GPIO_D5:                // D5 pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D5), arg_int);
                break;
            case U8X8_MSG_GPIO_D6:                // D6 pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D6), arg_int);
                break;
            case U8X8_MSG_GPIO_D7:                // D7 pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_D7), arg_int);
                break;
            case U8X8_MSG_GPIO_E:                // E/WR pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_E), arg_int);
                break;
            case U8X8_MSG_GPIO_CS:                // CS (chip select) pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_CS), arg_int);
                break;
            case U8X8_MSG_GPIO_DC:                // DC (data/cmd, A0, register select) pin: Output level in arg_int
                digitalWrite(u8x8_GetPinValue(u8x8, U8X8_PIN_DC), arg_int);
                break;
            default:
                u8x8_SetGPIOResult(u8x8, 1);            // default return value
                break;
        }
        return 1;
    }
};

#endif
