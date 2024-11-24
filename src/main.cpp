#include <OpenKNX.h>
#include "LEDModule.h"
#include "MeasuringModule.h"
#include <FileTransferModule.h>

void setup() {
    // setup 1Wire connection for hardware
    Wire1.setSDA(WIRE1_SDA);
    Wire1.setSCL(WIRE1_SCL);
    Wire1.begin();
    Wire1.setClock(100000);

    const uint8_t firmwareRevision = 1;
    openknx.init(firmwareRevision);

    openknx.addModule(1, openknxLEDModule);
    openknx.addModule(2, openknxMeasuringModule);
    openknx.addModule(9, openknxFileTransferModule);
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