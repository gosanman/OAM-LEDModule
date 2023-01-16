#include <Arduino.h>
#include <knx.h>

#include <LEDControl.h>
#include <MeasuringControl.h>

class LEDControl;

LEDControl gLEDControl;
MeasuringControl gMeasuringControl;

void processInputKoCallback(GroupObject &iKo)
{
    //uint16_t lAsap = iKo.asap();
    //SERIAL_DEBUG.printf("KO 1 - LEDModule: %i\n\r", lAsap);
    gLEDControl.processInputKo(iKo);
}

void appSetup()
{
    if (!knx.configured())
        return;

    if (GroupObject::classCallback() == 0)
        GroupObject::classCallback(processInputKoCallback);

    gLEDControl.setup();
    //gMeasuringControl.setup();
}

void appLoop()
{
    if (!knx.configured())
        return;

    gLEDControl.loop();
    //gMeasuringControl.loop();
}