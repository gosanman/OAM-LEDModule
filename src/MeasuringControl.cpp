#include <MeasuringControl.h>

MeasuringControl *MeasuringControl::sInstance = nullptr;

MeasuringControl::MeasuringControl()
{
    MeasuringControl::sInstance = this;
}
MeasuringControl::~MeasuringControl() {}

// !!!!!!!!!!!!!!!!!  Wie kann das in der Klasse->setup definiert werden und in getSingelMeasurment genutzt werden
INA226_WE ina226 = INA226_WE(&Wire1, I2C_INA226_DEVICE_ADDRESS);

void MeasuringControl::setup()
{
    measurementSend = knx.paramByte(PT_SendMeasuredValues);
    measurementSelectInterval = knx.paramByte(PT_MeasurementInterval);

    ina226.init();
    ina226.setAverage(AVERAGE_16); // choose mode and uncomment for change of default
    // ina226.setConversionTime(CONV_TIME_1100); //choose conversion time and uncomment for change of default
    ina226.setMeasureMode(TRIGGERED); // choose mode and uncomment for change of default
    // ina226.setCurrentRange(MA_800); // choose gain and uncomment for change of default
    // or set your own resistor and Ohm and current range in A
    ina226.setResistorRange(0.005, 16.0); // Resistor 0.01 Ohm, Max current 8.0 A, 0,005 Ohm, Max current 16.0 A
    // ina226.setCorrectionFactor(0.95);

#ifdef KDEBUG_ME
    SERIAL_DEBUG.print("Measuring send: ");
    SERIAL_DEBUG.println(measurementSend);
    SERIAL_DEBUG.print("Measuring interval: ");
    SERIAL_DEBUG.println(MeasurementInterval[measurementSelectInterval]);
    SERIAL_DEBUG.println("InitMeasuring KNX - OK");
    SERIAL_DEBUG.println("------------------------------------");
#endif

}

void MeasuringControl::loop()
{
    if (measurementSend == 1)
    {
        if (millis() > (MeasurementInterval[measurementSelectInterval]) + currentTime)
        {
            currentTime = millis();
            getSingleMeasurement();
            //SERIAL_DEBUG.printf("Core temperature: %2.1f C\n\r", analogReadTemp());
        }
    }
}

void MeasuringControl::getSingleMeasurement()
{
    ina226.startSingleMeasurement();
    ina226.readAndClearFlags();
    //shuntVoltage_mV = ina226.getShuntVoltage_mV();
    busVoltage_V = ina226.getBusVoltage_V();
    //current_mA = ina226.getCurrent_mA();
    current_A = ina226.getCurrent_A();
    //power_mW = ina226.getBusPower();
    power_W = ina226.getBusPower_W();
    //loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000);

    #ifdef KDEBUG_ME
        SERIAL_DEBUG.print("BusVoltage: ");
        SERIAL_DEBUG.println(busVoltage_V);
        SERIAL_DEBUG.print("Current: ");
        SERIAL_DEBUG.println(current_A);
        SERIAL_DEBUG.print("Power: ");
        SERIAL_DEBUG.println(power_W);
        SERIAL_DEBUG.println("Measurment - OK");
        SERIAL_DEBUG.println("------------------------------------");
    #endif

    knx.getGroupObject(KO_VoltageV).value(busVoltage_V, DPT_Value_Electric_Potential); //** each value access needs to done with according DPT parameter
    knx.getGroupObject(KO_CurrentA).value(current_A, DPT_Value_Electric_Current);      //** each value access needs to done with according DPT parameter
    knx.getGroupObject(KO_PowerW).value(power_W, DPT_Value_Power);                     //** each value access needs to done with according DPT parameter

    if (!ina226.overflow)
    {
        // Noch zu definieren, evtl. KO für Überlast
        // Serial.println("Values OK - no overflow");
    }
    else
    {
        // Noch zu definieren, evtl. KO für Überlast und abschalten der Dim Funktion
        // Serial.println("Overflow! Choose higher current range");
    }
}