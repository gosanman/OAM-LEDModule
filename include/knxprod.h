#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                            (time & 0x3FFF) * 3600000 ) : 0 )

//--------------------Allgemein---------------------------
#define MAIN_OpenKnxId 0xAF
#define MAIN_ApplicationNumber 0x01
#define MAIN_ApplicationVersion 0x01
#define MAIN_OrderNumber "OpenKnxLEDDimmer"
#define MAIN_ParameterSize 648
#define MAIN_MaxKoNumber 698

// max number of channel to use in DimmerControl
#define MAXCHANNELSHW    6
#define MAXCHANNELSEK    6
#define MAXCHANNELSTW    3
#define MAXCHANNELSRGB   2

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
#define KO_OFFSET_TW_DIMKELVIN         4           //Offset for Dim Absolut Kelvin KO
#define KO_OFFSET_TW_DIMRELATIV        11          //Offset for Dim Relativ Brithness KO
//#define KO_OFFSET_TW_DIMRELATIV        12          //Offset for Dim Relativ Kelvin KO
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

//!< Number: 101, Text: EK1: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelASwitch 101
#define KoAPP_KO_ChannelASwitch knx.getGroupObject(101)
//!< Number: 103, Text: EK1: {{0:---}}, Function: Dimmen Absolut
#define APP_KoKO_ChannelADimAbsolute 103
#define KoAPP_KO_ChannelADimAbsolute knx.getGroupObject(103)
//!< Number: 111, Text: EK1: {{0:---}}, Function: Dimmen Relativ
#define APP_KoKO_ChannelADimRelativ 111
#define KoAPP_KO_ChannelADimRelativ knx.getGroupObject(111)
//!< Number: 117, Text: EK1: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelAStatusOnOff 117
#define KoAPP_KO_ChannelAStatusOnOff knx.getGroupObject(117)
//!< Number: 118, Text: EK1: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelAStatusBrightness 118
#define KoAPP_KO_ChannelAStatusBrightness knx.getGroupObject(118)
//!< Number: 128, Text: EK1: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelAScene 128
#define KoAPP_KO_ChannelAScene knx.getGroupObject(128)
//!< Number: 131, Text: EK2: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelBSwitch 131
#define KoAPP_KO_ChannelBSwitch knx.getGroupObject(131)
//!< Number: 133, Text: EK2: {{0:---}}, Function: Dimmen Absolut
#define APP_KoKO_ChannelBDimAbsolute 133
#define KoAPP_KO_ChannelBDimAbsolute knx.getGroupObject(133)
//!< Number: 141, Text: EK2: {{0:---}}, Function: Dimmen Relativ
#define APP_KoKO_ChannelBDimRelativ 141
#define KoAPP_KO_ChannelBDimRelativ knx.getGroupObject(141)
//!< Number: 147, Text: EK2: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelBStatusOnOff 147
#define KoAPP_KO_ChannelBStatusOnOff knx.getGroupObject(147)
//!< Number: 148, Text: EK2: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelBStatusBrightness 148
#define KoAPP_KO_ChannelBStatusBrightness knx.getGroupObject(148)
//!< Number: 158, Text: EK2: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelBScene 158
#define KoAPP_KO_ChannelBScene knx.getGroupObject(158)
//!< Number: 161, Text: EK3: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelCSwitch 161
#define KoAPP_KO_ChannelCSwitch knx.getGroupObject(161)
//!< Number: 163, Text: EK3: {{0:---}}, Function: Dimmen Absolut
#define APP_KoKO_ChannelCDimAbsolute 163
#define KoAPP_KO_ChannelCDimAbsolute knx.getGroupObject(163)
//!< Number: 171, Text: EK3: {{0:---}}, Function: Dimmen Relativ
#define APP_KoKO_ChannelCDimRelativ 171
#define KoAPP_KO_ChannelCDimRelativ knx.getGroupObject(171)
//!< Number: 177, Text: EK3: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelCStatusOnOff 177
#define KoAPP_KO_ChannelCStatusOnOff knx.getGroupObject(177)
//!< Number: 178, Text: EK3: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelCStatusBrightness 178
#define KoAPP_KO_ChannelCStatusBrightness knx.getGroupObject(178)
//!< Number: 188, Text: EK3: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelCScene 188
#define KoAPP_KO_ChannelCScene knx.getGroupObject(188)
//!< Number: 191, Text: EK4: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelDSwitch 191
#define KoAPP_KO_ChannelDSwitch knx.getGroupObject(191)
//!< Number: 193, Text: EK4: {{0:---}}, Function: Dimmen Absolut
#define APP_KoKO_ChannelDDimAbsolute 193
#define KoAPP_KO_ChannelDDimAbsolute knx.getGroupObject(193)
//!< Number: 201, Text: EK4: {{0:---}}, Function: Dimmen Relativ
#define APP_KoKO_ChannelDDimRelativ 201
#define KoAPP_KO_ChannelDDimRelativ knx.getGroupObject(201)
//!< Number: 207, Text: EK4: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelDStatusOnOff 207
#define KoAPP_KO_ChannelDStatusOnOff knx.getGroupObject(207)
//!< Number: 208, Text: EK4 {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelDStatusBrightness 208
#define KoAPP_KO_ChannelDStatusBrightness knx.getGroupObject(208)
//!< Number: 218, Text: EK4: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelDScene 218
#define KoAPP_KO_ChannelDScene knx.getGroupObject(218)
//!< Number: 221, Text: EK5: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelESwitch 221
#define KoAPP_KO_ChannelESwitch knx.getGroupObject(221)
//!< Number: 223, Text: EK5: {{0:---}}, Function: Dimmen Absolut
#define APP_KoKO_ChannelEDimAbsolute 223
#define KoAPP_KO_ChannelEDimAbsolute knx.getGroupObject(223)
//!< Number: 231, Text: EK5: {{0:---}}, Function: Dimmen Relativ
#define APP_KoKO_ChannelEDimRelativ 231
#define KoAPP_KO_ChannelEDimRelativ knx.getGroupObject(231)
//!< Number: 237, Text: EK5: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelEStatusOnOff 237
#define KoAPP_KO_ChannelEStatusOnOff knx.getGroupObject(237)
//!< Number: 238, Text: EK5: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelEStatusBrightness 238
#define KoAPP_KO_ChannelEStatusBrightness knx.getGroupObject(238)
//!< Number: 248, Text: EK5: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelEScene 248
#define KoAPP_KO_ChannelEScene knx.getGroupObject(248)
//!< Number: 251, Text: EK6: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelFSwitch 251
#define KoAPP_KO_ChannelFSwitch knx.getGroupObject(251)
//!< Number: 253, Text: EK6: {{0:---}}, Function: Dimmen Absolut
#define APP_KoKO_ChannelFDimAbsolute 253
#define KoAPP_KO_ChannelFDimAbsolute knx.getGroupObject(253)
//!< Number: 261, Text: EK6: {{0:---}}, Function: Dimmen Relativ
#define APP_KoKO_ChannelFDimRelativ 261
#define KoAPP_KO_ChannelFDimRelativ knx.getGroupObject(261)
//!< Number: 267, Text: EK6: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelFStatusOnOff 267
#define KoAPP_KO_ChannelFStatusOnOff knx.getGroupObject(267)
//!< Number: 268, Text: EK6: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelFStatusBrightness 268
#define KoAPP_KO_ChannelFStatusBrightness knx.getGroupObject(268)
//!< Number: 278, Text: EK6: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelFScene 278
#define KoAPP_KO_ChannelFScene knx.getGroupObject(278)
//!< Number: 461, Text: TW1: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelTW1Switch 461
#define KoAPP_KO_ChannelTW1Switch knx.getGroupObject(461)
//!< Number: 463, Text: TW1: {{0:---}}, Function: Dimmen Absolut Helligkeit
#define APP_KoKO_ChannelTW1DimAbsoluteBrightness 463
#define KoAPP_KO_ChannelTW1DimAbsoluteBrightness knx.getGroupObject(463)
//!< Number: 464, Text: TW1: {{0:---}}, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW1DimAbsoluteColorTemp 464
#define KoAPP_KO_ChannelTW1DimAbsoluteColorTemp knx.getGroupObject(464)
//!< Number: 471, Text: TW1: {{0:---}}, Function: Dimmen Relativ Helligkeit
#define APP_KoKO_ChannelTW1DimRelativBrightness 471
#define KoAPP_KO_ChannelTW1DimRelativBrightness knx.getGroupObject(471)
//!< Number: 472, Text: TW1: {{0:---}}, Function: Dimmen Relativ Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW1DimRelativColorTemp 472
#define KoAPP_KO_ChannelTW1DimRelativColorTemp knx.getGroupObject(472)
//!< Number: 477, Text: TW1: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelTW1StatusOnOff 477
#define KoAPP_KO_ChannelTW1StatusOnOff knx.getGroupObject(477)
//!< Number: 478, Text: TW1: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelTW1StatusBrightness 478
#define KoAPP_KO_ChannelTW1StatusBrightness knx.getGroupObject(478)
//!< Number: 479, Text: TW1: {{0:---}}, Function: Status Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW1StatusColorTemp 479
#define KoAPP_KO_ChannelTW1StatusColorTemp knx.getGroupObject(479)
//!< Number: 488, Text: TW1: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelTW1Scene 488
#define KoAPP_KO_ChannelTW1Scene knx.getGroupObject(488)
//!< Number: 491, Text: TW2: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelTW2Switch 491
#define KoAPP_KO_ChannelTW2Switch knx.getGroupObject(491)
//!< Number: 493, Text: TW2: {{0:---}}, Function: Dimmen Absolut Helligkeit
#define APP_KoKO_ChannelTW2DimAbsoluteBrightness 493
#define KoAPP_KO_ChannelTW2DimAbsoluteBrightness knx.getGroupObject(493)
//!< Number: 494, Text: TW2: {{0:---}}, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW2DimAbsoluteColorTemp 494
#define KoAPP_KO_ChannelTW2DimAbsoluteColorTemp knx.getGroupObject(494)
//!< Number: 501, Text: TW2: {{0:---}}, Function: Dimmen Relativ Helligkeit
#define APP_KoKO_ChannelTW2DimRelativBrightness 501
#define KoAPP_KO_ChannelTW2DimRelativBrightness knx.getGroupObject(501)
//!< Number: 502, Text: TW2: {{0:---}}, Function: Dimmen Relativ Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW2DimRelativColorTemp 502
#define KoAPP_KO_ChannelTW2DimRelativColorTemp knx.getGroupObject(502)
//!< Number: 507, Text: TW2: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelTW2StatusOnOff 507
#define KoAPP_KO_ChannelTW2StatusOnOff knx.getGroupObject(507)
//!< Number: 508, Text: TW2: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelTW2StatusBrightness 508
#define KoAPP_KO_ChannelTW2StatusBrightness knx.getGroupObject(508)
//!< Number: 509, Text: TW2: {{0:---}}, Function: Status Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW2StatusColorTemp 509
#define KoAPP_KO_ChannelTW2StatusColorTemp knx.getGroupObject(509)
//!< Number: 518, Text: TW2: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelTW2Scene 518
#define KoAPP_KO_ChannelTW2Scene knx.getGroupObject(518)
//!< Number: 521, Text: TW3: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelTW3Switch 521
#define KoAPP_KO_ChannelTW3Switch knx.getGroupObject(521)
//!< Number: 523, Text: TW3: {{0:---}}, Function: Dimmen Absolut Helligkeit
#define APP_KoKO_ChannelTW3DimAbsoluteBrightness 523
#define KoAPP_KO_ChannelTW3DimAbsoluteBrightness knx.getGroupObject(523)
//!< Number: 524, Text: TW3: {{0:---}}, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW3DimAbsoluteColorTemp 524
#define KoAPP_KO_ChannelTW3DimAbsoluteColorTemp knx.getGroupObject(524)
//!< Number: 531, Text: TW3: {{0:---}}, Function: Dimmen Relativ Helligkeit
#define APP_KoKO_ChannelTW3DimRelativBrightness 531
#define KoAPP_KO_ChannelTW3DimRelativBrightness knx.getGroupObject(531)
//!< Number: 532, Text: TW3: {{0:---}}, Function: Dimmen Relativ Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW3DimRelativColorTemp 532
#define KoAPP_KO_ChannelTW3DimRelativColorTemp knx.getGroupObject(532)
//!< Number: 537, Text: TW3: {{0:---}}, Function: Status An/Aus
#define APP_KoKO_ChannelTW3StatusOnOff 537
#define KoAPP_KO_ChannelTW3StatusOnOff knx.getGroupObject(537)
//!< Number: 538, Text: TW3: {{0:---}}, Function: Status Helligkeit
#define APP_KoKO_ChannelTW3StatusBrightness 538
#define KoAPP_KO_ChannelTW3StatusBrightness knx.getGroupObject(538)
//!< Number: 539, Text: TW3: {{0:---}}, Function: Status Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW3StatusColorTemp 539
#define KoAPP_KO_ChannelTW3StatusColorTemp knx.getGroupObject(539)
//!< Number: 548, Text: TW3: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelTW3Scene 548
#define KoAPP_KO_ChannelTW3Scene knx.getGroupObject(548)
//!< Number: 641, Text: RGB1: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelRGB1Switch 641
#define KoAPP_KO_ChannelRGB1Switch knx.getGroupObject(641)
//!< Number: 643, Text: RGB1: {{0:---}}, Function: Farbeinstellung (RGB)
#define APP_KoKO_ChannelRGB1ColorRGB 643
#define KoAPP_KO_ChannelRGB1ColorRGB knx.getGroupObject(643)
//!< Number: 644, Text: RGB1: {{0:---}}, Function: Farbeinstellung (HSV)
#define APP_KoKO_ChannelRGB1ColorHSV 644
#define KoAPP_KO_ChannelRGB1ColorHSV knx.getGroupObject(644)
//!< Number: 645, Text: RGB1: {{0:---}}, Function: Dimmen absolut (Farbton H)
#define APP_KoKO_ChannelRGB1DimAbsoluteShadeH 645
#define KoAPP_KO_ChannelRGB1DimAbsoluteShadeH knx.getGroupObject(645)
//!< Number: 646, Text: RGB1: {{0:---}}, Function: Dimmen absolut (Sättigung S)
#define APP_KoKO_ChannelRGB1DimAbsoluteSaturationS 646
#define KoAPP_KO_ChannelRGB1DimAbsoluteSaturationS knx.getGroupObject(646)
//!< Number: 647, Text: RGB1: {{0:---}}, Function: Dimmen absolut (Helligkeit V)
#define APP_KoKO_ChannelRGB1DimAbsoluteBrightnessV 647
#define KoAPP_KO_ChannelRGB1DimAbsoluteBrightnessV knx.getGroupObject(647)
//!< Number: 651, Text: RGB1: {{0:---}}, Function: Dimmen relativ (Farbton H)
#define APP_KoKO_ChannelRGB1DimRelativShadeH 651
#define KoAPP_KO_ChannelRGB1DimRelativShadeH knx.getGroupObject(651)
//!< Number: 652, Text: RGB1: {{0:---}}, Function: Dimmen relativ (Sättigung S)
#define APP_KoKO_ChannelRGB1DimRelativSaturationS 652
#define KoAPP_KO_ChannelRGB1DimRelativSaturationS knx.getGroupObject(652)
//!< Number: 653, Text: RGB1: {{0:---}}, Function: Dimmen relativ (Helligkeit V)
#define APP_KoKO_ChannelRGB1DimRelativBrightnessV 653
#define KoAPP_KO_ChannelRGB1DimRelativBrightnessV knx.getGroupObject(653)
//!< Number: 657, Text: RGB1: {{0:---}}, Function: Staus Ein/Aus
#define APP_KoKO_ChannelRGB1StatusOnOff 657
#define KoAPP_KO_ChannelRGB1StatusOnOff knx.getGroupObject(657)
//!< Number: 658, Text: RGB1: {{0:---}}, Function: 3Byte Status Dimmwert (RGB)
#define APP_KoKO_ChannelRGB1StatusColorRGB 658
#define KoAPP_KO_ChannelRGB1StatusColorRGB knx.getGroupObject(658)
//!< Number: 659, Text: RGB1: {{0:---}}, Function: 3Byte Status Dimmwert (HSV)
#define APP_KoKO_ChannelRGB1StatusColorHSV 659
#define KoAPP_KO_ChannelRGB1StatusColorHSV knx.getGroupObject(659)
//!< Number: 660, Text: RGB1: {{0:---}}, Function: Status Dimmwert (Farbton H)
#define APP_KoKO_ChannelRGB1StatusShadeH 660
#define KoAPP_KO_ChannelRGB1StatusShadeH knx.getGroupObject(660)
//!< Number: 661, Text: RGB1: {{0:---}}, Function: Status Dimmwert (Sättigung S)
#define APP_KoKO_ChannelRGB1StatusSaturationS 661
#define KoAPP_KO_ChannelRGB1StatusSaturationS knx.getGroupObject(661)
//!< Number: 662, Text: RGB1: {{0:---}}, Function: Status Dimmwert (Helligkeit V)
#define APP_KoKO_ChannelRGB1StatusBrightnessV 662
#define KoAPP_KO_ChannelRGB1StatusBrightnessV knx.getGroupObject(662)
//!< Number: 668, Text: RGB1: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelRGB1Scene 668
#define KoAPP_KO_ChannelRGB1Scene knx.getGroupObject(668)
//!< Number: 671, Text: RGB2: {{0:---}}, Function: Schalten
#define APP_KoKO_ChannelRGB2Switch 671
#define KoAPP_KO_ChannelRGB2Switch knx.getGroupObject(671)
//!< Number: 673, Text: RGB2: {{0:---}}, Function: Farbeinstellung (RGB)
#define APP_KoKO_ChannelRGB2ColorRGB 673
#define KoAPP_KO_ChannelRGB2ColorRGB knx.getGroupObject(673)
//!< Number: 674, Text: RGB2: {{0:---}}, Function: Farbeinstellung (HSV)
#define APP_KoKO_ChannelRGB2ColorHSV 674
#define KoAPP_KO_ChannelRGB2ColorHSV knx.getGroupObject(674)
//!< Number: 675, Text: RGB2: {{0:---}}, Function: Dimmen absolut (Farbton H)
#define APP_KoKO_ChannelRGB2DimAbsoluteShadeH 675
#define KoAPP_KO_ChannelRGB2DimAbsoluteShadeH knx.getGroupObject(675)
//!< Number: 676, Text: RGB2: {{0:---}}, Function: Dimmen absolut (Sättigung S)
#define APP_KoKO_ChannelRGB2DimAbsoluteSaturationS 676
#define KoAPP_KO_ChannelRGB2DimAbsoluteSaturationS knx.getGroupObject(676)
//!< Number: 677, Text: RGB2: {{0:---}}, Function: Dimmen absolut (Helligkeit V)
#define APP_KoKO_ChannelRGB2DimAbsoluteBrightnessV 677
#define KoAPP_KO_ChannelRGB2DimAbsoluteBrightnessV knx.getGroupObject(677)
//!< Number: 681, Text: RGB2: {{0:---}}, Function: Dimmen relativ (Farbton H)
#define APP_KoKO_ChannelRGB2DimRelativShadeH 681
#define KoAPP_KO_ChannelRGB2DimRelativShadeH knx.getGroupObject(681)
//!< Number: 682, Text: RGB2: {{0:---}}, Function: Dimmen relativ (Sättigung S)
#define APP_KoKO_ChannelRGB2DimRelativSaturationS 682
#define KoAPP_KO_ChannelRGB2DimRelativSaturationS knx.getGroupObject(682)
//!< Number: 683, Text: RGB2: {{0:---}}, Function: Dimmen relativ (Helligkeit V)
#define APP_KoKO_ChannelRGB2DimRelativBrightnessV 683
#define KoAPP_KO_ChannelRGB2DimRelativBrightnessV knx.getGroupObject(683)
//!< Number: 687, Text: RGB2: {{0:---}}, Function: Staus Ein/Aus
#define APP_KoKO_ChannelRGB2StatusOnOff 687
#define KoAPP_KO_ChannelRGB2StatusOnOff knx.getGroupObject(687)
//!< Number: 688, Text: RGB2: {{0:---}}, Function: 3Byte Status Dimmwert (RGB)
#define APP_KoKO_ChannelRGB2StatusColorRGB 688
#define KoAPP_KO_ChannelRGB2StatusColorRGB knx.getGroupObject(688)
//!< Number: 689, Text: RGB2: {{0:---}}, Function: 3Byte Status Dimmwert (HSV)
#define APP_KoKO_ChannelRGB2StatusColorHSV 689
#define KoAPP_KO_ChannelRGB2StatusColorHSV knx.getGroupObject(689)
//!< Number: 690, Text: RGB2: {{0:---}}, Function: Status Dimmwert (Farbton H)
#define APP_KoKO_ChannelRGB2StatusShadeH 690
#define KoAPP_KO_ChannelRGB2StatusShadeH knx.getGroupObject(690)
//!< Number: 691, Text: RGB2: {{0:---}}, Function: Status Dimmwert (Sättigung S)
#define APP_KoKO_ChannelRGB2StatusSaturationS 691
#define KoAPP_KO_ChannelRGB2StatusSaturationS knx.getGroupObject(691)
//!< Number: 692, Text: RGB2: {{0:---}}, Function: Status Dimmwert (Helligkeit V)
#define APP_KoKO_ChannelRGB2StatusBrightnessV 692
#define KoAPP_KO_ChannelRGB2StatusBrightnessV knx.getGroupObject(692)
//!< Number: 698, Text: RGB2: {{0:---}}, Function: Szenensteuerung
#define APP_KoKO_ChannelRGB2Scene 698
#define KoAPP_KO_ChannelRGB2Scene knx.getGroupObject(698)