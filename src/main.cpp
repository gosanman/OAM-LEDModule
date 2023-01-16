#include <Arduino.h>
#include <knx.h>

#include <Wire.h>
#include <LEDModule.h>
#include <LEDModuleHardware.h>

void appSetup();
void appLoop();

void setup()
{
  // setup KNX connection
  Serial1.setRX(KNX_UART_RX_PIN);
  Serial1.setTX(KNX_UART_TX_PIN);
  // Setup 1Wire connection
  Wire1.setSDA(WIRE1_SDA);
  Wire1.setSCL(WIRE1_SCL);
  Wire1.begin();
  
  pinMode(PROG_LED_PIN, OUTPUT);
  SERIAL_DEBUG.begin(115200);

#ifdef KDEBUG
  digitalWrite(PROG_LED_PIN, HIGH);
  delay(DEBUG_DELAY);
  digitalWrite(PROG_LED_PIN, LOW);
  
  SERIAL_DEBUG.println("Startup called...");
  ArduinoPlatform::SerialDebug = &SERIAL_DEBUG;
#endif

  // pin or GPIO the programming led is connected to. Default is LED_BUILDIN
  knx.ledPin(PROG_LED_PIN);
  // is the led active on HIGH or low? Default is LOW
  knx.ledPinActiveOn(PROG_LED_PIN_ACTIVE_ON);
  // pin or GPIO programming button is connected to. Default is 0
  knx.buttonPin(PROG_BUTTON_PIN);
  // Is the interrupt created in RISING or FALLING signal? Default is RISING
  // knx.buttonPinInterruptOn(PROG_BUTTON_PIN_INTERRUPT_ON); 

  // read adress table, association table, groupobject table and parameters from eeprom
  knx.readMemory();

  appSetup();

  // start the framework
  knx.start();
}

void loop()
{
  // don't delay here to much. Otherwise you might lose packages or mess up the timing with ETS
  knx.loop();
  appLoop();
}