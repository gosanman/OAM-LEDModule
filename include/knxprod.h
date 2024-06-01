#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                            (time & 0x3FFF) * 3600000 ) : 0 )

#define PT_FuncClickAction_none      0
#define PT_FuncClickAction_on        1
#define PT_FuncClickAction_off       2
#define PT_FuncClickAction_toggle    3

//--------------------Allgemein---------------------------
#define MAIN_OpenKnxId 0xAF
#define MAIN_ApplicationNumber 0x01
#define MAIN_ApplicationVersion 0x01
#define MAIN_OrderNumber "OpenKnxLEDDimmer"
#define MAIN_ParameterSize 783
#define MAIN_MaxKoNumber 698

// max number of channel to use in DimmerControl
#ifdef BOARD_KNXLED_DK_06_V10
    #define MAXCHANNELSHW    6
    #define MAXCHANNELSEK    6
    #define MAXCHANNELSTW    3
    #define MAXCHANNELSRGB   2
    #define MAXCHANNELSCENE  5
#endif
#ifdef BOARD_KNXLED_DK_12_V10
    #define MAXCHANNELSHW    12
    #define MAXCHANNELSEK    12
    #define MAXCHANNELSTW    6
    #define MAXCHANNELSRGB   4
    #define MAXCHANNELSCENE  5
#endif

// number of ko reserved for a channel
#define KO_PER_CHANNEL_EK       30          //Number of KO per EK channel
#define KO_PER_CHANNEL_TW       30          //Number of KO per TW channel
#define KO_PER_CHANNEL_RGB      30          //Number of KO per RGB channel
// -----------------------------------------------------

//--------------------Kommunikationsobjekte---------------------------
// ComObjects per EK channel
#define KO_OFFSET_EK_BLOCK             100         //Offset for Block 
#define KO_OFFSET_EK_SWITCH            1           //Offset for Switch KO
#define KO_OFFSET_EK_DIMABSOLUT        3           //Offset for Dim Absolut KO
#define KO_OFFSET_EK_DIMRELATIV        11          //Offset for Dim Relativ KO
#define KO_OFFSET_EK_STATUSONOFF       17          //Offset for OnOff KO
#define KO_OFFSET_EK_STATUSBRIGHTNESS  18          //Offset for Brightness KO
#define KO_OFFSET_EK_SCENE             28          //Offset for Scene KO 

// ComObjects per TW channel
#define KO_OFFSET_TW_BLOCK             460         //Offset for Block 
#define KO_OFFSET_TW_SWITCH            1           //Offset for Switch KO
#define KO_OFFSET_TW_DIMABSOLUT        3           //Offset for Dim Absolut Brightness KO
#define KO_OFFSET_TW_DIMABSOLUTKELVIN  4           //Offset for Dim Absolut Kelvin KO
#define KO_OFFSET_TW_DIMRELATIV        11          //Offset for Dim Relativ Brithness KO
#define KO_OFFSET_TW_DIMRELATIVKELVIN  12          //Offset for Dim Relativ Kelvin KO
#define KO_OFFSET_TW_STATUSONOFF       17          //Offset for OnOff KO
#define KO_OFFSET_TW_STATUSBRIGHTNESS  18          //Offset for Brightness KO
#define KO_OFFSET_TW_STATUSKELVIN      19          //Offset for Kelvin KO
#define KO_OFFSET_TW_SCENE             28          //Offset for Scene KO 

// ComObjects per RGB channel
#define KO_OFFSET_RGB_BLOCK                  640         //Offset for Block 
#define KO_OFFSET_RGB_SWITCH                 1           //Offset for Switch KO
#define KO_OFFSET_RGB_COLORRGB               3           //Offset for Color RGB KO
#define KO_OFFSET_RGB_COLORHSV               4           //Offset for Color HSV KO
#define KO_OFFSET_RGB_DIMABSOLUTSHADEH       5           //Offset for Dim Absolut H KO
#define KO_OFFSET_RGB_DIMABSOLUTSATURATIONS  6           //Offset for Dim Absolut S KO
#define KO_OFFSET_RGB_DIMABSOLUTBRIGHTNESSV  7           //Offset for Dim Absolut V KO
#define KO_OFFSET_RGB_DIMABSOLUTR            8           //Offset for Dim Absolut R KO
#define KO_OFFSET_RGB_DIMABSOLUTG            9           //Offset for Dim Absolut G KO
#define KO_OFFSET_RGB_DIMABSOLUTB            10          //Offset for Dim Absolut B KO
#define KO_OFFSET_RGB_DIMRELATIVSHADEH       11          //Offset for Dim Relativ H KO
#define KO_OFFSET_RGB_DIMRELATIVSATURATIONS  12          //Offset for Dim Relativ S KO
#define KO_OFFSET_RGB_DIMRELATIVBRIGHTNESSV  13          //Offset for Dim Relativ V KO
#define KO_OFFSET_RGB_DIMRELATIVR            14          //Offset for Dim Relativ R KO
#define KO_OFFSET_RGB_DIMRELATIVG            15          //Offset for Dim Relativ G KO
#define KO_OFFSET_RGB_DIMRELATIVB            16          //Offset for Dim Relativ B KO
#define KO_OFFSET_RGB_STATUSONOFF            17          //Offset for OnOff KO
#define KO_OFFSET_RGB_STATUSCOLORRGB         18          //Offset for Status Color RGB KO
#define KO_OFFSET_RGB_STATUSCOLORHSV         19          //Offset for Status Color HSV KO
#define KO_OFFSET_RGB_STATUSSHADEH           20          //Offset for Status H KO
#define KO_OFFSET_RGB_STATUSSATURATIONS      21          //Offset for Status S KO
#define KO_OFFSET_RGB_STATUSBRIGHTNESSV      22          //Offset for Status V KO
#define KO_OFFSET_RGB_STATUSR                23          //Offset for Status R KO
#define KO_OFFSET_RGB_STATUSG                24          //Offset for Status G KO
#define KO_OFFSET_RGB_STATUSB                25          //Offset for Status B KO
#define KO_OFFSET_RGB_SCENE                  28          //Offset for Scene KO 
// -----------------------------------------------------

//--------------------Parameter---------------------------
// Parameter per EK channel
#define EK_ParamBlockOffset             32      // Parameter für Einzelkanal starten bei 32
#define EK_ParamBlockSize               32      // Weitere Kanaele werden mit +32 berechnet
#define EK_ParamOffSetUseOnBrightness    0      // Offset: 32, 64 usw. Text: Einschaltverhalten
#define EK_ParamOffSetOnBrightness       0      // Offset: 32, 64 usw. Text: Einschalthelligkeit
#define EK_ParamOffSetRelativDimTime     1      // Offset: 33, 65 usw. Text: Dimmgeschwindigkeit Relativ
#define EK_ParamOffSetOnOffTime		     2      // Offset: 34, 66 usw. Text: Ein- und Ausschaltgeschwindigkeit
#define EK_ParamOffSetDimCurve           3      // Offset: 35, 67 usw. Text: Dimmkurve

#define EK_ParamCalcIndex(index) (index + (_channelIndex * EK_ParamBlockSize) + EK_ParamBlockOffset)

// Parameter per TW channel
#define TW_ParamBlockOffset            416      // Parameter für TW Kanal starten bei 416
#define TW_ParamBlockSize               32      // Weitere Kanaele werden mit +32 berechnet
#define TW_ParamColorTempWW	             0      // Offset: 416, 448 usw. Text: Farbtemperatur Warmweiß
#define TW_ParamColorTempKW	             2      // Offset: 418, 450 usw. Text: Farbtemperatur Kaltweiß
#define TW_ParamUseOnColorTemp           4      // Offset: 420, 452 usw. Text: Einschaltverhalten
#define TW_ParamOnBrightness		     4      // Offset: 420, 452 usw. Text: Einschaltverhalten - Helligkeit
#define TW_ParamOnColorTemp              5      // Offset: 421, 453 usw. Text: Einschaltverhalten - Farbtemperatur
#define TW_ParamRelativDimTime           7      // Offset: 423, 455 usw. Text: Dimmgeschwindigkeit Relativ
#define TW_ParamOnOffTime                8      // Offset: 424, 456 usw. Text: Ein- und Ausschaltgeschwindigkeit
#define TW_ParamDimCurve                 9      // Offset: 425, 457 usw. Text: Dimmkurve

#define TW_ParamCalcIndex(index) (index + (_channelIndex * TW_ParamBlockSize) + TW_ParamBlockOffset)

// Parameter per RGB channel
#define RGB_ParamBlockOffset            608      // Parameter für RGB Kanal starten bei 608
#define RGB_ParamBlockSize               32      // Weitere Kanaele werden mit +32 berechnet
#define RGB_ParamOnColor                  0      // Offset: 608, 640 usw. Text: Einschaltverhalten - Farbe
#define RGB_ParamNightColor               3      // Offset: 611, 643 usw. Text: Einschaltverhalten - Farbe (Nacht)
#define RGB_ParamUseOnColor               6      // Offset: 614, 646 usw. Text: Einschaltverhalten
#define RGB_ParamUseNightColor            6      // Offset: 614, 646 usw. Text: Einschaltverhalten Nacht
#define RGB_ParamRelativDimTime           6      // Offset: 614, 646 usw. Text: Dimmgeschwindigkeit Relativ
#define RGB_ParamOnOffTime                7      // Offset: 615, 647 usw. Text: Ein- und Ausschaltgeschwindigkeit
#define RGB_ParamDimCurve                 7      // Offset: 615, 647 usw. Text: Dimmkurve

#define RGB_ParamCalcIndex(index) (index + (_channelIndex * RGB_ParamBlockSize) + RGB_ParamBlockOffset)

// Scene per EK channel
#define SceneEK_ParamBlockOffset        736      // Parameter für EK Scene startet bei 736
#define SceneEK_ParamBlockSize           32      // Weitere Kanaele werden mit +32 berechnet
#define SceneEK_ParamNum                  0      // Offset: +0 1 2 3 4 für Scene A, B, C, D, E 
#define SceneEK_ParamAction               5      // Offset: +0 1 2 3 4 für Action A, B, C, D, E
#define SceneEK_ParamValue               10      // Offset: +0 1 2 3 4 für Value A, B, C, D, E

#define SceneEK_ParamCalcIndex(index) (index + (_channelIndex * SceneEK_ParamBlockSize) + SceneEK_ParamBlockOffset)

// -----------------------------------------------------

//--------------------OpenKNX Common---------------------------
#define APP_PT_Watchdog		0x0010
// Offset: 16, Size: 1 Bit, Text: Watchdog aktivieren
//#define ParamAPP_PT_Watchdog knx.paramBit(16, 0)
#define ParamBASE_Watchdog knx.paramBit(16, 0)
#define APP_PT_Diagnose		0x0010
// Offset: 16, BitOffset: 1, Size: 1 Bit, Text: Diagnoseobjekt anzeigen
#define ParamAPP_PT_Diagnose knx.paramBit(16, 1)
// -----------------------------------------------------

//--------------------Konfiguration---------------------------
#define APP_PT_DayNight		0x0004
// Offset: 4, Size: 1 Bit, Text: Tag/Nacht Objekt
#define ParamAPP_PT_DayNight knx.paramBit(4, 0)
#define APP_PT_OperatingMode		0x0000
#define APP_PT_OperatingMode_Shift	5
#define APP_PT_OperatingMode_Mask	0x0007
// Offset: 0, Size: 3 Bit, Text: Betriebsmodus
#define ParamAPP_PT_OperatingMode ((uint)((knx.paramByte(0) >> APP_PT_OperatingMode_Shift) & APP_PT_OperatingMode_Mask))
#define APP_PT_PwmFrequenz		0x0001
#define APP_PT_PwmFrequenz_Shift	5
#define APP_PT_PwmFrequenz_Mask	0x07FF
// Offset: 1, Size: 11 Bit, Text: PWM Frequenz
#define ParamAPP_PT_PwmFrequenz ((uint)((knx.paramWord(1) >> APP_PT_PwmFrequenz_Shift) & APP_PT_PwmFrequenz_Mask))
#define APP_PT_SendMeasuredValues		0x0000
// Offset: 0, BitOffset: 3, Size: 1 Bit, Text: Messwerte zyklisch senden
#define ParamAPP_PT_SendMeasuredValues knx.paramBit(0, 3)
#define APP_PT_TempSensorPresent		0x0000
// Offset: 0, BitOffset: 4, Size: 1 Bit, Text: Temperatursensor verbaut
#define ParamAPP_PT_TempSensorPresent knx.paramBit(0, 4)
#define APP_PT_MeasurementInterval		0x0000
#define APP_PT_MeasurementInterval_Shift	1
#define APP_PT_MeasurementInterval_Mask	0x0003
// Offset: 0, BitOffset: 5, Size: 2 Bit, Text: Messwerte alle
#define ParamAPP_PT_MeasurementInterval ((uint)((knx.paramByte(0) >> APP_PT_MeasurementInterval_Shift) & APP_PT_MeasurementInterval_Mask))
#define APP_PT_ShuntValue		0x0003
#define APP_PT_ShuntValue_Shift	4
#define APP_PT_ShuntValue_Mask	0x000F
// Offset: 3, Size: 4 Bit, Text: Wert des verbauten Messwiderstand
#define ParamAPP_PT_ShuntValue ((uint)((knx.paramByte(3) >> APP_PT_ShuntValue_Shift) & APP_PT_ShuntValue_Mask))
#define APP_PT_Func1BtnClick		0x0004
#define APP_PT_Func1BtnClick_Shift	5
#define APP_PT_Func1BtnClick_Mask	0x0003
// Offset: 4, BitOffset: 1, Size: 2 Bit, Text: Aktion - Func1 Button Klick
#define ParamAPP_PT_Func1BtnClick ((uint)((knx.paramByte(4) >> APP_PT_Func1BtnClick_Shift) & APP_PT_Func1BtnClick_Mask))
#define APP_PT_Func1BtnDblClick		0x0004
#define APP_PT_Func1BtnDblClick_Shift	3
#define APP_PT_Func1BtnDblClick_Mask	0x0003
// Offset: 4, BitOffset: 3, Size: 2 Bit, Text: Aktion - Func1 Button Doppelklick
#define ParamAPP_PT_Func1BtnDblClick ((uint)((knx.paramByte(4) >> APP_PT_Func1BtnDblClick_Shift) & APP_PT_Func1BtnDblClick_Mask))
#define APP_PT_Func1BtnLongClick		0x0004
#define APP_PT_Func1BtnLongClick_Shift	1
#define APP_PT_Func1BtnLongClick_Mask	0x0003
// Offset: 4, BitOffset: 5, Size: 2 Bit, Text: Aktion - Func1 Button Langer Klick
#define ParamAPP_PT_Func1BtnLongClick ((uint)((knx.paramByte(4) >> APP_PT_Func1BtnLongClick_Shift) & APP_PT_Func1BtnLongClick_Mask))
// -----------------------------------------------------

// ------------------- EK Channel ----------------------
#define APP_PT_EKUseOnBrightness		0x0020
// Offset: 32, Size: 1 Bit, Text: Einschaltverhalten
#define ParamAPP_PT_EKUseOnBrightness knx.paramBit(EK_ParamCalcIndex(EK_ParamOffSetUseOnBrightness), 0)

#define APP_PT_EKOnBrightness		0x0020
#define APP_PT_EKOnBrightness_Mask	0x007F
// Offset: 32, BitOffset: 1, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_EKOnBrightness ((uint)((knx.paramByte(EK_ParamCalcIndex(EK_ParamOffSetOnBrightness))) & APP_PT_EKOnBrightness_Mask))

#define APP_PT_EKRelativDimTime		0x0021
#define APP_PT_EKRelativDimTime_Shift	2
#define APP_PT_EKRelativDimTime_Mask	0x003F
// Offset: 33, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_EKRelativDimTime ((uint)((knx.paramByte(EK_ParamCalcIndex(EK_ParamOffSetRelativDimTime)) >> APP_PT_EKRelativDimTime_Shift) & APP_PT_EKRelativDimTime_Mask))

#define APP_PT_EKOnOffTime		0x0022
#define APP_PT_EKOnOffTime_Shift	3
#define APP_PT_EKOnOffTime_Mask	0x001F
// Offset: 34, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_EKOnOffTime ((uint)((knx.paramByte(EK_ParamCalcIndex(EK_ParamOffSetOnOffTime)) >> APP_PT_EKOnOffTime_Shift) & APP_PT_EKOnOffTime_Mask))

#define APP_PT_EKDimCurve		0x0023
#define APP_PT_EKDimCurve_Shift	6
#define APP_PT_EKDimCurve_Mask	0x0003
// Offset: 35, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_EKDimCurve ((uint)((knx.paramByte(EK_ParamCalcIndex(EK_ParamOffSetDimCurve)) >> APP_PT_EKDimCurve_Shift) & APP_PT_EKDimCurve_Mask))
// -----------------------------------------------------

// ------------------- TW Channel ----------------------
#define APP_PT_TWColorTempWW		0x01A0
#define APP_PT_TWColorTempWW_Shift	4
#define APP_PT_TWColorTempWW_Mask	0x0FFF
// Offset: 416, Size: 12 Bit, Text: Farbtemperatur Warmweiß
#define ParamAPP_PT_TWColorTempWW ((uint)((knx.paramWord(TW_ParamCalcIndex(TW_ParamColorTempWW)) >> APP_PT_TWColorTempWW_Shift) & APP_PT_TWColorTempWW_Mask))

#define APP_PT_TWColorTempKW		0x01A2
#define APP_PT_TWColorTempKW_Shift	3
#define APP_PT_TWColorTempKW_Mask	0x1FFF
// Offset: 418, Size: 13 Bit, Text: Farbtemperatur Kaltweiß
#define ParamAPP_PT_TWColorTempKW ((uint)((knx.paramWord(TW_ParamCalcIndex(TW_ParamColorTempKW)) >> APP_PT_TWColorTempKW_Shift) & APP_PT_TWColorTempKW_Mask))

#define APP_PT_TWUseOnColorTemp		0x01A4
// Offset: 420, Size: 1 Bit, Text: Einschaltverhalten
#define ParamAPP_PT_TWUseOnColorTemp knx.paramBit(TW_ParamCalcIndex(TW_ParamUseOnColorTemp), 0)

#define APP_PT_TWOnBrightness		0x01A4
#define APP_PT_TWOnBrightness_Mask	0x007F
// Offset: 420, BitOffset: 1, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamAPP_PT_TWOnBrightness ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamOnBrightness))) & APP_PT_TWOnBrightness_Mask))

#define APP_PT_TWOnColorTemp		0x01A5
#define APP_PT_TWOnColorTemp_Shift	2
#define APP_PT_TWOnColorTemp_Mask	0x3FFF
// Offset: 421, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur
#define ParamAPP_PT_TWOnColorTemp ((uint)((knx.paramWord(TW_ParamCalcIndex(TW_ParamOnColorTemp)) >> APP_PT_TWOnColorTemp_Shift) & APP_PT_TWOnColorTemp_Mask))

#define APP_PT_TWRelativDimTime		0x01A7
#define APP_PT_TWRelativDimTime_Shift	2
#define APP_PT_TWRelativDimTime_Mask	0x003F
// Offset: 423, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_TWRelativDimTime ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamRelativDimTime)) >> APP_PT_TWRelativDimTime_Shift) & APP_PT_TWRelativDimTime_Mask))

#define APP_PT_TWOnOffTime		0x01A8
#define APP_PT_TWOnOffTime_Shift	3
#define APP_PT_TWOnOffTime_Mask	0x001F
// Offset: 424, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_TWOnOffTime ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamOnOffTime)) >> APP_PT_TWOnOffTime_Shift) & APP_PT_TWOnOffTime_Mask))

#define APP_PT_TWDimCurve		0x01A9
#define APP_PT_TWDimCurve_Shift	6
#define APP_PT_TWDimCurve_Mask	0x0003
// Offset: 425, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_TWDimCurve ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamDimCurve)) >> APP_PT_TWDimCurve_Shift) & APP_PT_TWDimCurve_Mask))
// -----------------------------------------------------

// ------------------- RGB Channel ----------------------
#define APP_PT_RGBOnColor		0x0260
// Offset: 608, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe
#define ParamAPP_PT_RGBOnColor knx.paramData(RGB_ParamCalcIndex(RGB_ParamOnColor))

#define APP_PT_RGBNightColor		0x0263
// Offset: 611, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe (Nacht)
#define ParamAPP_PT_RGBNightColor knx.paramData(RGB_ParamCalcIndex(RGB_ParamNightColor))

#define APP_PT_RGBUseOnColor		0x0266
// Offset: 614, Size: 1 Bit, Text: Einschaltverhalten
#define ParamAPP_PT_RGBUseOnColor knx.paramBit(RGB_ParamCalcIndex(RGB_ParamUseOnColor), 0)

#define APP_PT_RGBUseNightColor		0x0266
// Offset: 614, BitOffset: 1, Size: 1 Bit, Text: Einschaltverhalten Nacht
#define ParamAPP_PT_RGBUseNightColor knx.paramBit(RGB_ParamCalcIndex(RGB_ParamUseNightColor), 1)

#define APP_PT_RGBRelativDimTime		0x0266
#define APP_PT_RGBRelativDimTime_Mask	0x003F
// Offset: 614, BitOffset: 2, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_RGBRelativDimTime ((uint)((knx.paramByte(RGB_ParamCalcIndex(RGB_ParamRelativDimTime))) & APP_PT_RGBRelativDimTime_Mask))

#define APP_PT_RGBOnOffTime		0x0267
#define APP_PT_RGBOnOffTime_Shift	3
#define APP_PT_RGBOnOffTime_Mask	0x001F
// Offset: 615, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_RGBOnOffTime ((uint)((knx.paramByte(RGB_ParamCalcIndex(RGB_ParamOnOffTime)) >> APP_PT_RGBOnOffTime_Shift) & APP_PT_RGBOnOffTime_Mask))

#define APP_PT_RGBDimCurve		0x0267
#define APP_PT_RGBDimCurve_Shift	1
#define APP_PT_RGBDimCurve_Mask	0x0003
// Offset: 615, BitOffset: 5, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_RGBDimCurve ((uint)((knx.paramByte(RGB_ParamCalcIndex(RGB_ParamDimCurve)) >> APP_PT_RGBDimCurve_Shift) & APP_PT_RGBDimCurve_Mask))
// -----------------------------------------------------

// ------------------- Scene EK Channel ----------------------
#define APP_PT_EKSzNum		0x02E0
#define APP_PT_EKSzNum_Shift	1
#define APP_PT_EKSzNum_Mask	0x007F
// Offset: 736, Size: 7 Bit, Text: 
#define ParamAPP_PT_EKSzNum ((uint)((knx.paramByte(SceneEK_ParamCalcIndex(SceneEK_ParamNum + i)) >> APP_PT_EKSzNum_Shift) & APP_PT_EKSzNum_Mask))

#define APP_PT_EKSzAction		0x02E5
#define APP_PT_EKSzAction_Shift	5
#define APP_PT_EKSzAction_Mask	0x0007
// Offset: 741, Size: 3 Bit, Text: 
#define ParamAPP_PT_EKSzAction ((uint)((knx.paramByte(SceneEK_ParamCalcIndex(SceneEK_ParamAction + i)) >> APP_PT_EKSzAction_Shift) & APP_PT_EKSzAction_Mask))

#define APP_PT_EKSzValue		0x02EA
#define APP_PT_EKSzValue_Shift	1
#define APP_PT_EKSzValue_Mask	0x007F
// Offset: 746, Size: 7 Bit, Text: 
#define ParamAPP_PT_EKSzValue ((uint)((knx.paramByte(SceneEK_ParamCalcIndex(SceneEK_ParamValue + i)) >> APP_PT_EKSzValue_Shift) & APP_PT_EKSzValue_Mask))
// -----------------------------------------------------

//--------------------OpenKNX Common---------------------------
#define BASE_KoDiagnose 7
// Diagnose
#define KoBASE_Diagnose (knx.getGroupObject(BASE_KoDiagnose))
// -----------------------------------------------------

// ------------------- Kommunikationsobjekte ----------------------
//!< Number: 20, Text: Messwerte, Function: Spannung
#define APP_KoKO_VoltageV 20
#define KoAPP_KO_VoltageV knx.getGroupObject(20)
//!< Number: 21, Text: Messwerte, Function: Strom
#define APP_KoKO_CurrentA 21
#define KoAPP_KO_CurrentA knx.getGroupObject(21)
//!< Number: 22, Text: Messwerte, Function: Wirkleistung
#define APP_KoKO_PowerW 22
#define KoAPP_KO_PowerW knx.getGroupObject(22)
//!< Number: 25, Text: Messwerte, Function: Gerätetemperatur
#define APP_KoKO_TempC 25
#define KoAPP_KO_TempC knx.getGroupObject(25)
//!< Number: 30, Text: Allgemein, Function: Tag/Nacht
#define APP_KoKO_DayNight 30
#define KoAPP_KO_DayNight knx.getGroupObject(30)
// -----------------------------------------------------