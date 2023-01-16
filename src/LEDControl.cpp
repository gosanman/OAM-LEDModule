#include <LEDControl.h>

LEDControl *LEDControl::sInstance = nullptr;

LEDControl::LEDControl()
{
    LEDControl::sInstance = this;
}
LEDControl::~LEDControl() {}

GroupObject *LEDControl::getKo(uint16_t numberKo)
{
    return &knx.getGroupObject(numberKo);
}

// !!!!!!!!!!!!!!!!!  Wie kann das in der Klasse->setup definiert werden und in getSingelMeasurment genutzt werden
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(I2C_PCA9685_DEVICE_ADDRESS, Wire1);
// !!!!!!!!!!!!!!!!!  Wie kann das in der Klasse definiert werden


// Core function to set value
void setChannelValue(byte channel, byte value)
{
    pwm.setPin(channel, CIEL_8_12(value));
    
}

// create array of DimmerControls with unique IDs
DimmerControl channels[] = {
    DimmerControl(0),
    DimmerControl(1),
    DimmerControl(2),
    DimmerControl(3),
    DimmerControl(4)};

#define CHANNELS sizeof(channels) / sizeof(DimmerControl)

void LEDControl::setup()
{
    operatinModeSelect = knx.paramByte(PT_OperatingMode);
    pwmFreqSelect = knx.paramByte(PT_PwmFrequenz);

#ifdef KDEBUG_LED
    SERIAL_DEBUG.println("------------------ DEBUG -------------------");
    SERIAL_DEBUG.printf("Operating Mode: %i\n\r", operatinModeSelect);
    SERIAL_DEBUG.printf("PWM frequenz: %i\n\r", PwmFrequenz[pwmFreqSelect]);
    SERIAL_DEBUG.println("--------------------------------------------");
#endif

    // Create dimmer groups based on the parameter PT_OperationMode
    switch (operatinModeSelect)
    {
    case 0: // 5xEK
    {
        channels_ek[0] = new DimChannel_EK(0);
        channels_ek[0]->setup(0, KO_ChannelASwitch);
        channels_ek[1] = new DimChannel_EK(1);
        channels_ek[1]->setup(1, KO_ChannelBSwitch);
        channels_ek[2] = new DimChannel_EK(2);
        channels_ek[2]->setup(2, KO_ChannelCSwitch);
        channels_ek[3] = new DimChannel_EK(3);
        channels_ek[3]->setup(3, KO_ChannelDSwitch);
        channels_ek[4] = new DimChannel_EK(4);
        channels_ek[4]->setup(4, KO_ChannelESwitch);
    }
    break;
    case 1: // 1xRGBCTT
    {
    }
    break;
    case 2: // 1xRGBW and 1xEK
    {
    }
    break;
    case 3: // 1xRGB and 2xEK
    {
    }
    break;
    case 4: // 1xRGB and 1xTW
    {
    }
    break;
    case 5: // 2xTW and 1xEK
    {
    }
    break;
    case 6: // 1xTW and 3xEK
    {
    }
    break;
    default:
        Serial.print("Operation Mode not valide");
        break;
    }

    pwm.begin();
    pwm.setOscillatorFrequency(25000000);

    pwm.setPWMFreq(488); // 1600 is the maximum PWM frequency
    // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
    // some i2c devices dont like this so much so if you're sharing the bus, watch
    // out for this!
    Wire1.setClock(400000);

    // Optional: set minimum possible value (default 0)
    // channels[0].setMinValue(0);
    // Optional: set maximum possible value (default 255)
    // channels[0].setMaxValue(255);


    // Mandatory: set function that should be called to control output (
    for (byte ch = 0; ch < CHANNELS; ch++)
    {
        channels[ch].setValueIdFunction(&setChannelValue);
    }

    // Set all Channels off for a defined status
    for (byte ch = 0; ch < CHANNELS; ch++)
    {
        pwm.setPin(ch, 0);
    }
}

void LEDControl::loop()
{
    task();
    inform();
}

void LEDControl::processInputKo(GroupObject &iKo)
{
    // check if KO for Channels
    uint16_t ko = iKo.asap();
    if (ko < 100) // KO not for Channels
        return;
    
    operatinModeSelect = knx.paramByte(PT_OperatingMode);
    // send Input KO to the correct dimmer based on the parameter PT_OperationMode
    switch (operatinModeSelect)
    {
    case 0: // 5xEK
    {
        channels_ek[0]->processInputKo(iKo);
        channels_ek[1]->processInputKo(iKo);
        channels_ek[2]->processInputKo(iKo);
        channels_ek[3]->processInputKo(iKo);
        channels_ek[4]->processInputKo(iKo);     
    }
    break;
    case 1: // 1xRGBCTT
    {
    }
    break;
    case 2: // 1xRGBW and 1xEK
    {
    }
    break;
    case 3: // 1xRGB and 2xEK
    {
    }
    break;
    case 4: // 1xRGB and 1xTW
    {
    }
    break;
    case 5: // 2xTW and 1xEK
    {
    }
    break;
    case 6: // 1xTW and 3xEK
    {
    }
    break;
    default:
        Serial.println("Operation Mode not valide for sending KO");
    break;
    }
}

void LEDControl::task()
{
    // task must be executed as often as possible
    for (byte ch = 0; ch < CHANNELS; ch++)
    {
        channels[ch].task();
    }
}

void LEDControl::inform()
{
    // if you want to inform user about current value it's better to check if it really needed:
    for (byte ch = 0; ch < CHANNELS; ch++)
    {
        if (channels[ch].updateAvailable())
        {
            // get the current value and print it
            SERIAL_DEBUG.printf("Current value: %i\n\r", channels[ch].getCurrentValue());
            // reset the update flag
            channels[ch].resetUpdateFlag();
        }
    }
}

void LEDControl::onChannel(uint8_t channel)
{
    channels[channel].taskSoftOn();
}

void LEDControl::offChannel(uint8_t channel)
{
    channels[channel].taskSoftOff();
}

void LEDControl::dimStop(uint8_t channel)
{
    channels[channel].taskStop();
}

void LEDControl::dimUp(uint8_t channel)
{
    channels[channel].taskDimUp();
}

void LEDControl::dimDown(uint8_t channel)
{
    channels[channel].taskDimDown();
}

void LEDControl::dimChannel(uint8_t channel, uint8_t brightness)
{
    channels[channel].taskNewValue(brightness);
}

void LEDControl::dimCCT(uint8_t startChannel, uint8_t brightness, uint16_t kelvin, uint16_t colortempww, uint16_t colortempkw)
{
    uint16_t percentWW = map(kelvin, colortempww, colortempkw, brightness, 0);
    uint16_t percentKW = map(kelvin, colortempww, colortempkw, 0, brightness);

    SERIAL_DEBUG.println("---------------");
    SERIAL_DEBUG.println(percentWW);
    SERIAL_DEBUG.println(percentKW);
    SERIAL_DEBUG.println("---------------");

    // consider gamma value in percentage to make sure the temperature is the same at each dim level and according to gamma
    uint16_t percent = pgm_read_word(&(proztable[brightness][2]));

    SERIAL_DEBUG.println(percent);

    // calculate percentage for each channel
    percentWW = (percent*(6500 - kelvin))/3800;
    percentKW = (percent*(kelvin - 2700))/3800;
    
    SERIAL_DEBUG.println("---------------");
    SERIAL_DEBUG.println(percentWW);
    SERIAL_DEBUG.println(percentKW);
    SERIAL_DEBUG.println("---------------");

    
    channels[startChannel].taskNewValue(percentWW);     // WW
    channels[startChannel + 1].taskNewValue(percentKW); // KW
}

void LEDControl::dimRGB(uint8_t startChannel, uint8_t brightness, uint8_t r, uint8_t g, uint8_t b)
{
    // consider gamma value in percentage to make sure the temperature is the same at each dim level and according to gamma
    //uint16_t percent = pgm_read_word(&(curves[brightness][curve]));

    channels[startChannel].taskNewValue(r * brightness / 100);     // RED
    channels[startChannel + 1].taskNewValue(g * brightness / 100); // GREEN
    channels[startChannel + 2].taskNewValue(b * brightness / 100); // BLUE
}



// DimmerControl Settings

void LEDControl::setChannelDurationRelativ(uint8_t channel, uint16_t value)
{
    // Optional: how long should it take if you dimming up/down (default 5000ms)
    channels[channel].setDurationRelative(value);
}

void LEDControl::setChannelDurationAbsolut(uint8_t channel, uint16_t value)
{
    // Optional: how long should it if you turn on/off? (default 500ms)
    channels[channel].setDurationAbsolute(value);
}

int LEDControl::getChannelDurationRelativ(uint8_t channel)
{
    int time = channels[channel].getDurationRelative();
    return time;
}

int LEDControl::getChannelDurationAbsolut(uint8_t channel)
{
    int time = channels[channel].getDurationAbsolute();
    return time;
}