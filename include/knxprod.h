#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                            (time & 0x3FFF) * 3600000 ) : 0 )

#define PT_PT_OperatingMode_PT_OperatingMode_EN_0               0
#define PT_PT_OperatingMode_PT_OperatingMode_EN_1               1
#define PT_PT_OperatingMode_PT_OperatingMode_EN_2               2
#define PT_PT_OperatingMode_PT_OperatingMode_EN_3               3
#define PT_PT_OperatingMode_PT_OperatingMode_EN_4               4
#define PT_PT_OperatingMode_PT_OperatingMode_EN_5               5
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_211                 211
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_488                 488
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_600                 600
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_832                 832
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_1000                1000
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_1200                1200
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_0                      0
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_1                      1
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_2                      2
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_3                      3
#define PT_PT_SendMeasuredValues_PT_SendMeasuredValues_EN_1     1
#define PT_PT_SendMeasuredValues_PT_SendMeasuredValues_EN_0     0
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_0   0
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_1   1
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_2   2
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_3   3
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_0                 0
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_3                 3
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_5                 5
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_7                 7
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_10                10
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_15                15
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_20                20
#define PT_PT_TempSensorPresent_PT_TempSensorPresent_EN_0       0
#define PT_PT_TempSensorPresent_PT_TempSensorPresent_EN_1       1
#define PT_PT_ShuntValue_PT_ShuntValue_EN_5                     5
#define PT_PT_ShuntValue_PT_ShuntValue_EN_10                    10
#define PT_PT_UseOnColor_PT_UseOnColor_EN_0                     0
#define PT_PT_UseOnColor_PT_UseOnColor_EN_1                     1

//--------------------Allgemein---------------------------
#define MAIN_OpenKnxId 0xAF
#define MAIN_ApplicationNumber 0x01
#define MAIN_ApplicationVersion 0x01
#define MAIN_OrderNumber "LED01"
#define MAIN_ParameterSize 219
#define MAIN_MaxKoNumber 407

// number of channel to use in DimmerControl
#define CHANNELSHW    6
#define CHANNELSEK    6
#define CHANNELSTW    3
#define CHANNELSRGB   2

// number of ko reserved for a channel
#define KO_PER_CHANNEL_EK       20          //Number of KO per EK channel
#define KO_PER_CHANNEL_TW       30          //Number of KO per TW channel
#define KO_PER_CHANNEL_RGB      30          //Number of KO per RGB channel
// -----------------------------------------------------

//--------------------Kommunikationsobjekte---------------------------
// ComObjects per EK channel
#define KO_OFFSET_EK_SWITCH            0           //Offset for Switch KO
#define KO_OFFSET_EK_DIMABSOLUT        3           //Offset for Dim Absolut KO
#define KO_OFFSET_EK_DIMRELATIV        4           //Offset for Dim Relativ KO
#define KO_OFFSET_EK_STATUSONOFF       14          //Offset for OnOff KO
#define KO_OFFSET_EK_STATUSBRIGHTNESS  15          //Offset for Brightness KO

// ComObjects per TW channel
#define KO_OFFSET_TW_SWITCH            0           //Offset for Switch KO
#define KO_OFFSET_TW_DIMABSOLUT        3           //Offset for Dim Absolut KO
#define KO_OFFSET_TW_DIMKELVIN         5           //Offset for Dim Kelvin KO
#define KO_OFFSET_TW_DIMRELATIV        7           //Offset for Dim Relativ KO
#define KO_OFFSET_TW_STATUSONOFF       24          //Offset for OnOff KO
#define KO_OFFSET_TW_STATUSBRIGHTNESS  25          //Offset for Brightness KO
#define KO_OFFSET_TW_STATUSKELVIN      27          //Offset for Kelvin KO

// ComObjects per RGB channel
#define KO_OFFSET_RGB_SWITCH                 0           //Offset for Switch KO
#define KO_OFFSET_RGB_COLORRGB               2           //Offset for Color RGB KO
#define KO_OFFSET_RGB_COLORHSV               3           //Offset for Color HSV KO
#define KO_OFFSET_RGB_DIMABSOLUTSHADEH       4           //Offset for Dim Absolut H KO
#define KO_OFFSET_RGB_DIMABSOLUTSATURATIONS  5           //Offset for Dim Absolut S KO
#define KO_OFFSET_RGB_DIMABSOLUTBRIGHTNESSV  6           //Offset for Dim Absolut V KO
#define KO_OFFSET_RGB_DIMRELATIVSHADEH       7           //Offset for Dim Relativ H KO
#define KO_OFFSET_RGB_DIMRELATIVSATURATIONS  8           //Offset for Dim Relativ S KO
#define KO_OFFSET_RGB_DIMRELATIVBRIGHTNESSV  9           //Offset for Dim Relativ V KO
#define KO_OFFSET_RGB_STATUSONOFF            10          //Offset for OnOff KO
#define KO_OFFSET_RGB_STATUSCOLORRGB         11          //Offset for Status Color RGB KO
#define KO_OFFSET_RGB_STATUSCOLORHSV         12          //Offset for Status Color HSV KO
#define KO_OFFSET_RGB_STATUSSHADEH           13          //Offset for Status H KO
#define KO_OFFSET_RGB_STATUSSATURATIONS      14          //Offset for Status S KO
#define KO_OFFSET_RGB_STATUSBRIGHTNESSV      15          //Offset for Status V KO
// -----------------------------------------------------

//--------------------Parameter---------------------------
// Parameter per EK channel
#define EK_ParamBlockOffset             32      // Parameter für Einzelkanal starten bei 32
#define EK_ParamBlockSize               16      // Weitere Kanaele werden mit +16 berechnet
#define EK_ParamOffSetOnBrightness       0      // Offset: 32, 48 usw. Text: Einschalthelligkeit
#define EK_ParamOffSetRelativDimTime     1      // Offset: 33, 49 usw. Text: Dimmgeschwindigkeit Relativ
#define EK_ParamOffSetOnOffTime		     2      // Offset: 34, 50 usw. Text: Ein- und Ausschaltgeschwindigkeit
#define EK_ParamOffSetDimCurve           3      // Offset: 35, 51 usw. Text: Dimmkurve

#define EK_ParamCalcIndex(index) (index + (_channelIndex * EK_ParamBlockSize) + EK_ParamBlockOffset)

// Parameter per TW channel
#define TW_ParamBlockOffset            128      // Parameter für TW Kanal starten bei 128
#define TW_ParamBlockSize               16      // Weitere Kanaele werden mit +16 berechnet
#define TW_ParamColorTempWW	             0      // Offset: 128, 144 usw. Text: Farbtemperatur Warmweiß
#define TW_ParamColorTempKW	             2      // Offset: 130, 146 usw. Text: Farbtemperatur Kaltweiß
#define TW_ParamOnBrightness		     4      // Offset: 132, 148 usw. Text: Einschaltverhalten - Helligkeit
#define TW_ParamOnColorTemp              5      // Offset: 133, 149 usw. Text: Einschaltverhalten - Farbtemperatur
#define TW_ParamRelativDimTime           7      // Offset: 135, 151 usw. Text: Dimmgeschwindigkeit Relativ
#define TW_ParamOnOffTime                8      // Offset: 136, 152 usw. Text: Ein- und Ausschaltgeschwindigkeit
#define TW_ParamDimCurve                 9      // Offset: 137, 153 usw. Text: Dimmkurve

#define TW_ParamCalcIndex(index) (index + (_channelIndex * TW_ParamBlockSize) + TW_ParamBlockOffset)

// Parameter per RGB channel
#define RGB_ParamBlockOffset            192      // Parameter für RGB Kanal starten bei 192
#define RGB_ParamBlockSize               16      // Weitere Kanaele werden mit +16 berechnet
#define RGB_ParamOnColor                  0      // Offset: 192, 208 usw. Text: Einschaltverhalten - Farbe
#define RGB_ParamUseOnColor               7      // Offset: 199, 216 usw. Text: Einschaltverhalten
#define RGB_ParamRelativDimTime           8      // Offset: 200, 216 usw. Text: Dimmgeschwindigkeit Relativ
#define RGB_ParamOnOffTime                9      // Offset: 201, 217 usw. Text: Ein- und Ausschaltgeschwindigkeit
#define RGB_ParamDimCurve                10      // Offset: 202, 218 usw. Text: Dimmkurve

#define RGB_ParamCalcIndex(index) (index + (_channelIndex * RGB_ParamBlockSize) + RGB_ParamBlockOffset)
// -----------------------------------------------------

//--------------------Konfiguration---------------------------
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
#define APP_PT_EKOnBrightness		0x0020
#define APP_PT_EKOnBrightness_Shift	1
#define APP_PT_EKOnBrightness_Mask	0x007F
// Offset: 32, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_EKOnBrightness ((uint)((knx.paramByte(EK_ParamCalcIndex(EK_ParamOffSetOnBrightness)) >> APP_PT_EKOnBrightness_Shift) & APP_PT_EKOnBrightness_Mask))

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
#define APP_PT_TWColorTempWW		0x0080
#define APP_PT_TWColorTempWW_Shift	4
#define APP_PT_TWColorTempWW_Mask	0x0FFF
// Offset: 128, Size: 12 Bit, Text: Farbtemperatur Warmweiß
#define ParamAPP_PT_TWColorTempWW ((uint)((knx.paramWord(TW_ParamCalcIndex(TW_ParamColorTempWW)) >> APP_PT_TWColorTempWW_Shift) & APP_PT_TWColorTempWW_Mask))

#define APP_PT_TWColorTempKW		0x0082
#define APP_PT_TWColorTempKW_Shift	3
#define APP_PT_TWColorTempKW_Mask	0x1FFF
// Offset: 130, Size: 13 Bit, Text: Farbtemperatur Kaltweiß
#define ParamAPP_PT_TWColorTempKW ((uint)((knx.paramWord(TW_ParamCalcIndex(TW_ParamColorTempKW)) >> APP_PT_TWColorTempKW_Shift) & APP_PT_TWColorTempKW_Mask))

#define APP_PT_TWOnBrightness		0x0084
#define APP_PT_TWOnBrightness_Shift	1
#define APP_PT_TWOnBrightness_Mask	0x007F
// Offset: 132, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamAPP_PT_TWOnBrightness ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamOnBrightness)) >> APP_PT_TWOnBrightness_Shift) & APP_PT_TWOnBrightness_Mask))

#define APP_PT_TWOnColorTemp		0x0085
#define APP_PT_TWOnColorTemp_Shift	2
#define APP_PT_TWOnColorTemp_Mask	0x3FFF
// Offset: 133, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur
#define ParamAPP_PT_TWOnColorTemp ((uint)((knx.paramWord(TW_ParamCalcIndex(TW_ParamOnColorTemp)) >> APP_PT_TWOnColorTemp_Shift) & APP_PT_TWOnColorTemp_Mask))

#define APP_PT_TWRelativDimTime		0x0087
#define APP_PT_TWRelativDimTime_Shift	2
#define APP_PT_TWRelativDimTime_Mask	0x003F
// Offset: 135, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_TWRelativDimTime ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamRelativDimTime)) >> APP_PT_TWRelativDimTime_Shift) & APP_PT_TWRelativDimTime_Mask))

#define APP_PT_TWOnOffTime		0x0088
#define APP_PT_TWOnOffTime_Shift	3
#define APP_PT_TWOnOffTime_Mask	0x001F
// Offset: 136, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_TWOnOffTime ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamOnOffTime)) >> APP_PT_TWOnOffTime_Shift) & APP_PT_TWOnOffTime_Mask))

#define APP_PT_TWDimCurve		0x0089
#define APP_PT_TWDimCurve_Shift	6
#define APP_PT_TWDimCurve_Mask	0x0003
// Offset: 137, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_TWDimCurve ((uint)((knx.paramByte(TW_ParamCalcIndex(TW_ParamDimCurve)) >> APP_PT_TWDimCurve_Shift) & APP_PT_TWDimCurve_Mask))
// -----------------------------------------------------

// ------------------- RGB Channel ----------------------
#define APP_PT_RGBOnColor		0x00C0
// Offset: 192, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe
#define ParamAPP_PT_RGBOnColor knx.paramData(RGB_ParamCalcIndex(RGB_ParamOnColor))

#define APP_PT_RGB1UseOnColor		0x00C7
// Offset: 199, Size: 1 Bit, Text: Einschaltverhalten
#define ParamAPP_PT_RGBUseOnColor knx.paramBit(RGB_ParamCalcIndex(RGB_ParamUseOnColor), 0)

#define APP_PT_RGBRelativDimTime		0x00C8
#define APP_PT_RGBRelativDimTime_Shift	2
#define APP_PT_RGBRelativDimTime_Mask	0x003F
// Offset: 200, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_RGBRelativDimTime ((uint)((knx.paramByte(RGB_ParamCalcIndex(RGB_ParamRelativDimTime)) >> APP_PT_RGBRelativDimTime_Shift) & APP_PT_RGBRelativDimTime_Mask))

#define APP_PT_RGBOnOffTime		0x00C9
#define APP_PT_RGBOnOffTime_Shift	3
#define APP_PT_RGBOnOffTime_Mask	0x001F
// Offset: 201, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_RGBOnOffTime ((uint)((knx.paramByte(RGB_ParamCalcIndex(RGB_ParamOnOffTime)) >> APP_PT_RGBOnOffTime_Shift) & APP_PT_RGBOnOffTime_Mask))

#define APP_PT_RGBDimCurve		0x00CA
#define APP_PT_RGBDimCurve_Shift	6
#define APP_PT_RGBDimCurve_Mask	0x0003
// Offset: 202, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_RGBDimCurve ((uint)((knx.paramByte(RGB_ParamCalcIndex(RGB_ParamDimCurve)) >> APP_PT_RGBDimCurve_Shift) & APP_PT_RGBDimCurve_Mask))
// -----------------------------------------------------

// ------------------- Kommunikationsobjekte ----------------------
//!< Number: 2, Text: Spannung, Function: Spannung
#define APP_KoKO_VoltageV 2
#define KoAPP_KO_VoltageV knx.getGroupObject(2)
//!< Number: 4, Text: Strom, Function: Stromwert
#define APP_KoKO_CurrentA 4
#define KoAPP_KO_CurrentA knx.getGroupObject(4)
//!< Number: 6, Text: Wirkleistung, Function: Wirkleistung
#define APP_KoKO_PowerW 6
#define KoAPP_KO_PowerW knx.getGroupObject(6)
//!< Number: 8, Text: Gerätetemperatur, Function: Gerätetemperatur
#define APP_KoKO_TempC 8
#define KoAPP_KO_TempC knx.getGroupObject(8)
//!< Number: 101, Text: A:, Function: Schalten
#define APP_KoKO_ChannelASwitch 101
#define KoAPP_KO_ChannelASwitch knx.getGroupObject(101)
//!< Number: 104, Text: A:, Function: Dimmen Absolut
#define APP_KoKO_ChannelADimAbsolute 104
#define KoAPP_KO_ChannelADimAbsolute knx.getGroupObject(104)
//!< Number: 105, Text: A:, Function: Dimmen Relativ
#define APP_KoKO_ChannelADimRelativ 105
#define KoAPP_KO_ChannelADimRelativ knx.getGroupObject(105)
//!< Number: 115, Text: A:, Function: Status An/Aus
#define APP_KoKO_ChannelAStatusOnOff 115
#define KoAPP_KO_ChannelAStatusOnOff knx.getGroupObject(115)
//!< Number: 116, Text: A:, Function: Status Helligkeit
#define APP_KoKO_ChannelAStatusBrightness 116
#define KoAPP_KO_ChannelAStatusBrightness knx.getGroupObject(116)
//!< Number: 121, Text: B:, Function: Schalten
#define APP_KoKO_ChannelBSwitch 121
#define KoAPP_KO_ChannelBSwitch knx.getGroupObject(121)
//!< Number: 124, Text: B:, Function: Dimmen Absolut
#define APP_KoKO_ChannelBDimAbsolute 124
#define KoAPP_KO_ChannelBDimAbsolute knx.getGroupObject(124)
//!< Number: 125, Text: B:, Function: Dimmen Relativ
#define APP_KoKO_ChannelBDimRelativ 125
#define KoAPP_KO_ChannelBDimRelativ knx.getGroupObject(125)
//!< Number: 135, Text: B:, Function: Status An/Aus
#define APP_KoKO_ChannelBStatusOnOff 135
#define KoAPP_KO_ChannelBStatusOnOff knx.getGroupObject(135)
//!< Number: 136, Text: B:, Function: Status Helligkeit
#define APP_KoKO_ChannelBStatusBrightness 136
#define KoAPP_KO_ChannelBStatusBrightness knx.getGroupObject(136)
//!< Number: 141, Text: C:, Function: Schalten
#define APP_KoKO_ChannelCSwitch 141
#define KoAPP_KO_ChannelCSwitch knx.getGroupObject(141)
//!< Number: 144, Text: C:, Function: Dimmen Absolut
#define APP_KoKO_ChannelCDimAbsolute 144
#define KoAPP_KO_ChannelCDimAbsolute knx.getGroupObject(144)
//!< Number: 145, Text: C:, Function: Dimmen Relativ
#define APP_KoKO_ChannelCDimRelativ 145
#define KoAPP_KO_ChannelCDimRelativ knx.getGroupObject(145)
//!< Number: 155, Text: C:, Function: Status An/Aus
#define APP_KoKO_ChannelCStatusOnOff 155
#define KoAPP_KO_ChannelCStatusOnOff knx.getGroupObject(155)
//!< Number: 156, Text: C:, Function: Status Helligkeit
#define APP_KoKO_ChannelCStatusBrightness 156
#define KoAPP_KO_ChannelCStatusBrightness knx.getGroupObject(156)
//!< Number: 161, Text: D:, Function: Schalten
#define APP_KoKO_ChannelDSwitch 161
#define KoAPP_KO_ChannelDSwitch knx.getGroupObject(161)
//!< Number: 164, Text: D:, Function: Dimmen Absolut
#define APP_KoKO_ChannelDDimAbsolute 164
#define KoAPP_KO_ChannelDDimAbsolute knx.getGroupObject(164)
//!< Number: 165, Text: D:, Function: Dimmen Relativ
#define APP_KoKO_ChannelDDimRelativ 165
#define KoAPP_KO_ChannelDDimRelativ knx.getGroupObject(165)
//!< Number: 175, Text: D:, Function: Status An/Aus
#define APP_KoKO_ChannelDStatusOnOff 175
#define KoAPP_KO_ChannelDStatusOnOff knx.getGroupObject(175)
//!< Number: 176, Text: D:, Function: Status Helligkeit
#define APP_KoKO_ChannelDStatusBrightness 176
#define KoAPP_KO_ChannelDStatusBrightness knx.getGroupObject(176)
//!< Number: 181, Text: E:, Function: Schalten
#define APP_KoKO_ChannelESwitch 181
#define KoAPP_KO_ChannelESwitch knx.getGroupObject(181)
//!< Number: 184, Text: E:, Function: Dimmen Absolut
#define APP_KoKO_ChannelEDimAbsolute 184
#define KoAPP_KO_ChannelEDimAbsolute knx.getGroupObject(184)
//!< Number: 185, Text: E:, Function: Dimmen Relativ
#define APP_KoKO_ChannelEDimRelativ 185
#define KoAPP_KO_ChannelEDimRelativ knx.getGroupObject(185)
//!< Number: 195, Text: E:, Function: Status An/Aus
#define APP_KoKO_ChannelEStatusOnOff 195
#define KoAPP_KO_ChannelEStatusOnOff knx.getGroupObject(195)
//!< Number: 196, Text: E:, Function: Status Helligkeit
#define APP_KoKO_ChannelEStatusBrightness 196
#define KoAPP_KO_ChannelEStatusBrightness knx.getGroupObject(196)
//!< Number: 201, Text: F:, Function: Schalten
#define APP_KoKO_ChannelFSwitch 201
#define KoAPP_KO_ChannelFSwitch knx.getGroupObject(201)
//!< Number: 204, Text: F:, Function: Dimmen Absolut
#define APP_KoKO_ChannelFDimAbsolute 204
#define KoAPP_KO_ChannelFDimAbsolute knx.getGroupObject(204)
//!< Number: 205, Text: F:, Function: Dimmen Relativ
#define APP_KoKO_ChannelFDimRelativ 205
#define KoAPP_KO_ChannelFDimRelativ knx.getGroupObject(205)
//!< Number: 215, Text: F:, Function: Status An/Aus
#define APP_KoKO_ChannelFStatusOnOff 215
#define KoAPP_KO_ChannelFStatusOnOff knx.getGroupObject(215)
//!< Number: 216, Text: F:, Function: Status Helligkeit
#define APP_KoKO_ChannelFStatusBrightness 216
#define KoAPP_KO_ChannelFStatusBrightness knx.getGroupObject(216)
//!< Number: 262, Text: TW1:, Function: Schalten
#define APP_KoKO_ChannelTW1Switch 262
#define KoAPP_KO_ChannelTW1Switch knx.getGroupObject(262)
//!< Number: 265, Text: TW1:, Function: Dimmen Absolut Helligkeit
#define APP_KoKO_ChannelTW1DimAbsoluteBrightness 265
#define KoAPP_KO_ChannelTW1DimAbsoluteBrightness knx.getGroupObject(265)
//!< Number: 267, Text: TW1:, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW1DimAbsoluteColorTemp 267
#define KoAPP_KO_ChannelTW1DimAbsoluteColorTemp knx.getGroupObject(267)
//!< Number: 269, Text: TW1:, Function: Dimmen Relativ Helligkeit
#define APP_KoKO_ChannelTW1DimRelativBrightness 269
#define KoAPP_KO_ChannelTW1DimRelativBrightness knx.getGroupObject(269)
//!< Number: 286, Text: TW1:, Function: Status An/Aus
#define APP_KoKO_ChannelTW1StatusOnOff 286
#define KoAPP_KO_ChannelTW1StatusOnOff knx.getGroupObject(286)
//!< Number: 287, Text: TW1:, Function: Status Helligkeit
#define APP_KoKO_ChannelTW1StatusBrightness 287
#define KoAPP_KO_ChannelTW1StatusBrightness knx.getGroupObject(287)
//!< Number: 289, Text: TW1:, Function: Status Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW1StatusColorTemp 289
#define KoAPP_KO_ChannelTW1StatusColorTemp knx.getGroupObject(289)
//!< Number: 292, Text: TW2:, Function: Schalten
#define APP_KoKO_ChannelTW2Switch 292
#define KoAPP_KO_ChannelTW2Switch knx.getGroupObject(292)
//!< Number: 295, Text: TW2:, Function: Dimmen Absolut Helligkeit
#define APP_KoKO_ChannelTW2DimAbsoluteBrightness 295
#define KoAPP_KO_ChannelTW2DimAbsoluteBrightness knx.getGroupObject(295)
//!< Number: 297, Text: TW2:, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW2DimAbsoluteColorTemp 297
#define KoAPP_KO_ChannelTW2DimAbsoluteColorTemp knx.getGroupObject(297)
//!< Number: 299, Text: TW2:, Function: Dimmen Relativ Helligkeit
#define APP_KoKO_ChannelTW2DimRelativBrightness 299
#define KoAPP_KO_ChannelTW2DimRelativBrightness knx.getGroupObject(299)
//!< Number: 316, Text: TW2:, Function: Status An/Aus
#define APP_KoKO_ChannelTW2StatusOnOff 316
#define KoAPP_KO_ChannelTW2StatusOnOff knx.getGroupObject(316)
//!< Number: 317, Text: TW2:, Function: Status Helligkeit
#define APP_KoKO_ChannelTW2StatusBrightness 317
#define KoAPP_KO_ChannelTW2StatusBrightness knx.getGroupObject(317)
//!< Number: 319, Text: TW2:, Function: Status Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW2StatusColorTemp 319
#define KoAPP_KO_ChannelTW2StatusColorTemp knx.getGroupObject(319)
//!< Number: 322, Text: TW3:, Function: Schalten
#define APP_KoKO_ChannelTW3Switch 322
#define KoAPP_KO_ChannelTW3Switch knx.getGroupObject(322)
//!< Number: 325, Text: TW3:, Function: Dimmen Absolut Helligkeit
#define APP_KoKO_ChannelTW3DimAbsoluteBrightness 325
#define KoAPP_KO_ChannelTW3DimAbsoluteBrightness knx.getGroupObject(325)
//!< Number: 327, Text: TW3:, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW3DimAbsoluteColorTemp 327
#define KoAPP_KO_ChannelTW3DimAbsoluteColorTemp knx.getGroupObject(327)
//!< Number: 329, Text: TW3:, Function: Dimmen Relativ Helligkeit
#define APP_KoKO_ChannelTW3DimRelativBrightness 329
#define KoAPP_KO_ChannelTW3DimRelativBrightness knx.getGroupObject(329)
//!< Number: 346, Text: TW3:, Function: Status An/Aus
#define APP_KoKO_ChannelTW3StatusOnOff 346
#define KoAPP_KO_ChannelTW3StatusOnOff knx.getGroupObject(346)
//!< Number: 347, Text: TW3:, Function: Status Helligkeit
#define APP_KoKO_ChannelTW3StatusBrightness 347
#define KoAPP_KO_ChannelTW3StatusBrightness knx.getGroupObject(347)
//!< Number: 349, Text: TW3:, Function: Status Farbtemperatur (Kelvin)
#define APP_KoKO_ChannelTW3StatusColorTemp 349
#define KoAPP_KO_ChannelTW3StatusColorTemp knx.getGroupObject(349)
//!< Number: 362, Text: RGB1: RGB / HSV, Function: Schalten
#define APP_KoKO_ChannelRGB1Switch 362
#define KoAPP_KO_ChannelRGB1Switch knx.getGroupObject(362)
//!< Number: 364, Text: RGB1: RGB, Function: Farbeinstellung
#define APP_KoKO_ChannelRGB1ColorRGB 364
#define KoAPP_KO_ChannelRGB1ColorRGB knx.getGroupObject(364)
//!< Number: 365, Text: RGB1: HSV, Function: Farbeinstellung
#define APP_KoKO_ChannelRGB1ColorHSV 365
#define KoAPP_KO_ChannelRGB1ColorHSV knx.getGroupObject(365)
//!< Number: 366, Text: RGB1: HSV Farbton (H), Function: Dimmen absolut
#define APP_KoKO_ChannelRGB1DimAbsoluteShadeH 366
#define KoAPP_KO_ChannelRGB1DimAbsoluteShadeH knx.getGroupObject(366)
//!< Number: 367, Text: RGB1: HSV Sättigung (S), Function: Dimmen absolut
#define APP_KoKO_ChannelRGB1DimAbsoluteSaturationS 367
#define KoAPP_KO_ChannelRGB1DimAbsoluteSaturationS knx.getGroupObject(367)
//!< Number: 368, Text: RGB1: HSV Helligkeit (V), Function: Dimmen absolute
#define APP_KoKO_ChannelRGB1DimAbsoluteBrightnessV 368
#define KoAPP_KO_ChannelRGB1DimAbsoluteBrightnessV knx.getGroupObject(368)
//!< Number: 369, Text: RGB1: HSV Farbton (H), Function: Dimmen relativ
#define APP_KoKO_ChannelRGB1DimRelativShadeH 369
#define KoAPP_KO_ChannelRGB1DimRelativShadeH knx.getGroupObject(369)
//!< Number: 370, Text: RGB1: HSV Sättigung (S), Function: Dimmen relativ
#define APP_KoKO_ChannelRGB1DimRelativSaturationS 370
#define KoAPP_KO_ChannelRGB1DimRelativSaturationS knx.getGroupObject(370)
//!< Number: 371, Text: RGB1: HSV Helligkeit (V), Function: Dimmen relativ
#define APP_KoKO_ChannelRGB1DimRelativBrightnessV 371
#define KoAPP_KO_ChannelRGB1DimRelativBrightnessV knx.getGroupObject(371)
//!< Number: 372, Text: RGB1: RGB / HSV, Function: Staus Ein/Aus
#define APP_KoKO_ChannelRGB1StatusOnOff 372
#define KoAPP_KO_ChannelRGB1StatusOnOff knx.getGroupObject(372)
//!< Number: 373, Text: RGB1: RGB, Function: 3Byte Status Dimmwert
#define APP_KoKO_ChannelRGB1StatusColorRGB 373
#define KoAPP_KO_ChannelRGB1StatusColorRGB knx.getGroupObject(373)
//!< Number: 374, Text: RGB1: HSV, Function: 3Byte Status Dimmwert
#define APP_KoKO_ChannelRGB1StatusColorHSV 374
#define KoAPP_KO_ChannelRGB1StatusColorHSV knx.getGroupObject(374)
//!< Number: 375, Text: RGB1: HSV Farbton (H), Function: Status Dimmwert
#define APP_KoKO_ChannelRGB1StatusShadeH 375
#define KoAPP_KO_ChannelRGB1StatusShadeH knx.getGroupObject(375)
//!< Number: 376, Text: RGB1: HSV Sättigung (S), Function: Status Dimmwert
#define APP_KoKO_ChannelRGB1StatusSaturationS 376
#define KoAPP_KO_ChannelRGB1StatusSaturationS knx.getGroupObject(376)
//!< Number: 377, Text: RGB1: HSV Helligkeit (V), Function: Status Dimmwert
#define APP_KoKO_ChannelRGB1StatusBrightnessV 377
#define KoAPP_KO_ChannelRGB1StatusBrightnessV knx.getGroupObject(377)
//!< Number: 392, Text: RGB2: RGB / HSV, Function: Schalten
#define APP_KoKO_ChannelRGB2Switch 392
#define KoAPP_KO_ChannelRGB2Switch knx.getGroupObject(392)
//!< Number: 394, Text: RGB2: RGB, Function: Farbeinstellung
#define APP_KoKO_ChannelRGB2ColorRGB 394
#define KoAPP_KO_ChannelRGB2ColorRGB knx.getGroupObject(394)
//!< Number: 395, Text: RGB2: HSV, Function: Farbeinstellung
#define APP_KoKO_ChannelRGB2ColorHSV 395
#define KoAPP_KO_ChannelRGB2ColorHSV knx.getGroupObject(395)
//!< Number: 396, Text: RGB2: HSV Farbton (H), Function: Dimmen absolut
#define APP_KoKO_ChannelRGB2DimAbsoluteShadeH 396
#define KoAPP_KO_ChannelRGB2DimAbsoluteShadeH knx.getGroupObject(396)
//!< Number: 397, Text: RGB2: HSV Sättigung (S), Function: Dimmen absolut
#define APP_KoKO_ChannelRGB2DimAbsoluteSaturationS 397
#define KoAPP_KO_ChannelRGB2DimAbsoluteSaturationS knx.getGroupObject(397)
//!< Number: 398, Text: RGB2: HSV Helligkeit (V), Function: Dimmen absolute
#define APP_KoKO_ChannelRGB2DimAbsoluteBrightnessV 398
#define KoAPP_KO_ChannelRGB2DimAbsoluteBrightnessV knx.getGroupObject(398)
//!< Number: 399, Text: RGB2: HSV Farbton (H), Function: Dimmen relativ
#define APP_KoKO_ChannelRGB2DimRelativShadeH 399
#define KoAPP_KO_ChannelRGB2DimRelativShadeH knx.getGroupObject(399)
//!< Number: 400, Text: RGB2: HSV Sättigung (S), Function: Dimmen relativ
#define APP_KoKO_ChannelRGB2DimRelativSaturationS 400
#define KoAPP_KO_ChannelRGB2DimRelativSaturationS knx.getGroupObject(400)
//!< Number: 401, Text: RGB2: HSV Helligkeit (V), Function: Dimmen relativ
#define APP_KoKO_ChannelRGB2DimRelativBrightnessV 401
#define KoAPP_KO_ChannelRGB2DimRelativBrightnessV knx.getGroupObject(401)
//!< Number: 402, Text: RGB2: RGB / HSV, Function: Staus Ein/Aus
#define APP_KoKO_ChannelRGB2StatusOnOff 402
#define KoAPP_KO_ChannelRGB2StatusOnOff knx.getGroupObject(402)
//!< Number: 403, Text: RGB2: RGB, Function: 3Byte Status Dimmwert
#define APP_KoKO_ChannelRGB2StatusColorRGB 403
#define KoAPP_KO_ChannelRGB2StatusColorRGB knx.getGroupObject(403)
//!< Number: 404, Text: RGB2: HSV, Function: 3Byte Status Dimmwert
#define APP_KoKO_ChannelRGB2StatusColorHSV 404
#define KoAPP_KO_ChannelRGB2StatusColorHSV knx.getGroupObject(404)
//!< Number: 405, Text: RGB2: HSV Farbton (H), Function: Status Dimmwert
#define APP_KoKO_ChannelRGB2StatusShadeH 405
#define KoAPP_KO_ChannelRGB2StatusShadeH knx.getGroupObject(405)
//!< Number: 406, Text: RGB2: HSV Sättigung (S), Function: Status Dimmwert
#define APP_KoKO_ChannelRGB2StatusSaturationS 406
#define KoAPP_KO_ChannelRGB2StatusSaturationS knx.getGroupObject(406)
//!< Number: 407, Text: RGB2: HSV Helligkeit (V), Function: Status Dimmwert
#define APP_KoKO_ChannelRGB2StatusBrightnessV 407
#define KoAPP_KO_ChannelRGB2StatusBrightnessV knx.getGroupObject(407)








/*

//DEBUG options
#define KDEBUG                      // comment this line to disable DEBUG info
#define KDEBUG_KO                   // comment this line to disable COMOBJECT DEBUG info
#define KDEBUG_ME                   // comment this line to disable MEASURING DEBUG info
#define KDEBUG_LED                  // comment this line to disable LED DEBUG info
//#define KDEBUG_LED_VALUE          // comment this line to disable LED DEBUG VALUE info
#define ENABLE_DEBUG_OUTPUT         

*/