#pragma once

#ifdef BOARD_KNXLED_DK_06_V10
    #define DEVICE_ID "LED-DK-06"
    #define HARDWARE_NAME DEVICE_ID
    #define KNX_SERIAL Serial1
    #define KNX_UART_RX_PIN 1               // Use GPIO01 as KNX RX PIN - UART0 TX
    #define KNX_UART_TX_PIN 0               // Use GPIO00 as KNX TX PIN - UART0 RX
    #define SAVE_INTERRUPT_PIN 5            // Use GPIO05 as KNX SAVE PIN
    #define PROG_BUTTON_PIN 7
    #define PROG_LED_PIN 6
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define WIRE1_SDA 14                    // Use GP14 as I2C1 SDA
    #define WIRE1_SCL 15                    // Use GP15 as I2C1 SCL
    #define LED_HW_CHANNEL_COUNT 6
#endif

#ifdef BOARD_KNXLED_DK_12_V10
    #define DEVICE_ID "LED-DK-12"
    #define HARDWARE_NAME DEVICE_ID
    #define KNX_SERIAL Serial1
    #define KNX_UART_RX_PIN 1               // Use GPIO01 as KNX RX PIN - UART0 TX
    #define KNX_UART_TX_PIN 0               // Use GPIO00 as KNX TX PIN - UART0 RX
    #define SAVE_INTERRUPT_PIN 5            // Use GPIO05 as KNX SAVE PIN
    #define PROG_BUTTON_PIN 25
    #define PROG_LED_PIN 26
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define INFO1_LED_PIN 23
    #define INFO1_LED_PIN_ACTIVE_ON HIGH
    #define FUNC1_BUTTON_PIN 24
    #define WIRE1_SDA 14                    // Use GP14 as I2C1 SDA
    #define WIRE1_SCL 15                    // Use GP15 as I2C1 SCL
    #define LED_HW_CHANNEL_COUNT 12
#endif

// For V1.2 of hardware you can add a front pannel display
#ifdef BOARD_KNXLED_DK_06_V12
    #define DEVICE_ID "LED-DK-06"
    #define HARDWARE_NAME DEVICE_ID
    #define KNX_SERIAL Serial1
    #define KNX_UART_RX_PIN 1               // Use GPIO01 as KNX RX PIN - UART0 TX
    #define KNX_UART_TX_PIN 0               // Use GPIO00 as KNX TX PIN - UART0 RX
    #define SAVE_INTERRUPT_PIN 5            // Use GPIO05 as KNX SAVE PIN
    #define PROG_BUTTON_PIN 25
    #define PROG_LED_PIN 26
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define INFO1_LED_PIN 23
    #define INFO1_LED_PIN_ACTIVE_ON HIGH
    #define FUNC1_BUTTON_PIN 24
    #define FRONT_PANEL_PRESENT 1
    #define IO1_PIN 8
    #define IO2_PIN 9
    #define IO3_PIN 10
    #define IO4_PIN 11
    #define WIRE1_SDA 14                    // Use GP14 as I2C1 SDA
    #define WIRE1_SCL 15                    // Use GP15 as I2C1 SCL
    #define LED_HW_CHANNEL_COUNT 6
#endif

#ifdef BOARD_KNXLED_DK_12_V12
    #define DEVICE_ID "LED-DK-12"
    #define HARDWARE_NAME DEVICE_ID
    #define KNX_SERIAL Serial1
    #define KNX_UART_RX_PIN 1               // Use GPIO01 as KNX RX PIN - UART0 TX
    #define KNX_UART_TX_PIN 0               // Use GPIO00 as KNX TX PIN - UART0 RX
    #define SAVE_INTERRUPT_PIN 5            // Use GPIO05 as KNX SAVE PIN
    #define PROG_BUTTON_PIN 25
    #define PROG_LED_PIN 26
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define INFO1_LED_PIN 23
    #define INFO1_LED_PIN_ACTIVE_ON HIGH
    #define FUNC1_BUTTON_PIN 24
    #define FRONT_PANEL_PRESENT 1
    #define IO1_PIN 8
    #define IO2_PIN 9
    #define IO3_PIN 10
    #define IO4_PIN 11
    #define WIRE1_SDA 14                    // Use GP14 as I2C1 SDA
    #define WIRE1_SCL 15                    // Use GP15 as I2C1 SCL
    #define LED_HW_CHANNEL_COUNT 12
#endif

// This is a application board for the REG1 (REG1-APP-6xLED)
#ifdef BOARD_KNXLED_APP_06_V01
    #define DEVICE_ID "LED-APP-06"
    #define HARDWARE_NAME DEVICE_ID
    #define KNX_SERIAL Serial1
    #define KNX_UART_RX_PIN 1               // Use GPIO01 as KNX RX PIN - UART0 TX
    #define KNX_UART_TX_PIN 0               // Use GPIO00 as KNX TX PIN - UART0 RX
    #define SAVE_INTERRUPT_PIN 5            // Use GPIO05 as KNX SAVE PIN
    #define PROG_BUTTON_PIN 23
    #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
    #define PROG_LED_PIN 25
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define INFO1_LED_PIN 6                 // Use as func LED       
    #define INFO1_LED_PIN_ACTIVE_ON HIGH
    #define INFO2_LED_PIN 24                // Use as error LED
    #define INFO2_LED_PIN_ACTIVE_ON HIGH
    #define INFO3_LED_PIN 22                // Use as alert LED
    #define INFO3_LED_PIN_ACTIVE_ON HIGH
    #define FUNC1_BUTTON_PIN 7
    //#define SENSOR_SDA_TX_PIN  8
    //#define SENSOR_SCL_RX_PIN  9
    #define WIRE1_SDA 26                    // Use GP26 as I2C1 SDA
    #define WIRE1_SCL 27                    // Use GP15 as I2C1 SCL
    #define LED_HW_CHANNEL_COUNT 6
#endif

// max number of channel to use in DimmerControl
#if (LED_HW_CHANNEL_COUNT == 6)
    #define MAXCHANNELSHW    6
    #define MAXCHANNELSEK    6
    #define MAXCHANNELSTW    3
    #define MAXCHANNELSRGB   2
    #define MAXCHANNELSCENE  5
    #define MAXCHANNELSHCL   3
#endif
#if (LED_HW_CHANNEL_COUNT == 12)
    #define MAXCHANNELSHW    12
    #define MAXCHANNELSEK    12
    #define MAXCHANNELSTW    6
    #define MAXCHANNELSRGB   4
    #define MAXCHANNELSCENE  5
    #define MAXCHANNELSHCL   3
#endif

const char HWPortsMapping[12] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};   // Mapping of HW ports to letters

enum ChannelType {
    NONE,
    EK,
    TW,
    RGB
};

#define FIRMWARE_NAME "LEDModule"

#define KNX_UART_NUM 0          // Use UART0 for KNX communication

#define I2C_PCA9685_DEVICE_ADDRESS  0x40    // Address of PCA9685 PWM chip
#define I2C_INA22x_DEVICE_ADDRESS   0x41    // Address of INA22x current and power sensor chip
#define I2C_TMP100_DEVICE_ADDRESS   0x48    // Address of TMP100 temperature sensor chip
#define I2C_SSD1306_DEVICE_ADDRESS  0x3C    // Address of SSD1306 OLED display chip