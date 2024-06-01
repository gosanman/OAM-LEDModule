#pragma once

#ifdef BOARD_KNXLED_DK_06_V10
    #define SAVE_INTERRUPT_PIN 5
    #define PROG_LED_PIN 6
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define PROG_BUTTON_PIN 7
#endif

#ifdef BOARD_KNXLED_DK_12_V10
    #define PROG_BUTTON_PIN 25
    #define PROG_LED_PIN 26
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define INFO1_LED_PIN 23
    #define INFO1_LED_PIN_ACTIVE_ON HIGH
    #define FUNC1_BUTTON_PIN 24
#endif

//#define PROG_BUTTON_PIN_INTERRUPT_ON FALLING

#define KNX_UART_RX_PIN 1 // Use GP01 as KNX RX
#define KNX_UART_TX_PIN 0 // Use GP00 as KNX TX

#define WIRE1_SDA 14  // Use GP14 as I2C1 SDA
#define WIRE1_SCL 15  // Use GP15 as I2C1 SCL
#define I2C_PCA9685_DEVICE_ADDRESS 0x40 // Address of PCA9685 PWM chip
#define I2C_INA226_DEVICE_ADDRESS 0x41 // Address of INA226 current and power sensor chip
#define I2C_TMP100_DEVICE_ADDRESS 0x48 // Address of TMP100 temperature sensor chip

//#define ENABLE_DEBUG_OUTPUT // Debug output for PCA9685 PWM chip