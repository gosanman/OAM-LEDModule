#include <OpenKNX.h>
#include "LEDModule.h"
#include "MeasuringModule.h"
#include <FileTransferModule.h>

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
    openknx.addModule(9, new FileTransferModule());
    openknx.setup();
}

void loop() {
    openknx.loop();
}

#ifdef OPENKNX_DUALCORE
    void setup1() {
        openknx.setup1();
    };

    void loop1() {
        openknx.loop1();
    }
#endif