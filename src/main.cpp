#include <OpenKNX.h>
#include "LEDModule.h"
#include "MeasuringModule.h"
#include "FrontPanelModule.h"
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
#if defined(BOARD_KNXLED_DK_06_V12) || defined(BOARD_KNXLED_DK_12_V12)
    openknx.addModule(8, openknxFrontPanelModule);
#endif
    openknx.addModule(9, openknxFileTransferModule);
    openknx.setup();
}

void setup1() {
    openknx.setup1();
}

void loop() {
    openknx.loop();
}

void loop1() {
    openknxLEDModule.loop1();
    openknxMeasuringModule.loop1();
#if defined(BOARD_KNXLED_DK_06_V12) || defined(BOARD_KNXLED_DK_12_V12)
    openknxFrontPanelModule.loop1();
#endif
}