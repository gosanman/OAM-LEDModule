#include <OpenKNX.h>
#include <LEDModule.h>
#include <MeasuringModule.h>
#include <UpdaterModule.h>

void setup() {
    // setup 1Wire connection for hardware
    Wire1.setSDA(WIRE1_SDA);
    Wire1.setSCL(WIRE1_SCL);
    Wire1.begin();
    Wire1.setClock(400000);

    const uint8_t firmwareRevision = 1;
    openknx.init(firmwareRevision);

    openknx.addModule(1, new LEDModule());
    openknx.addModule(2, new MeasuringModule());
    //openknx.addModule(9, new UpdaterModule());
    openknx.setup();
}

void loop() {
    openknx.loop();
}

void loop1() {
    openknx.loop1();
}