#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                            (time & 0x3FFF) * 3600000 ) : 0 )
#define PT_PT_OperatingMode_PT_OperatingMode_EN_0 0
#define PT_PT_OperatingMode_PT_OperatingMode_EN_1 1
#define PT_PT_OperatingMode_PT_OperatingMode_EN_2 2
#define PT_PT_OperatingMode_PT_OperatingMode_EN_3 3
#define PT_PT_OperatingMode_PT_OperatingMode_EN_4 4
#define PT_PT_OperatingMode_PT_OperatingMode_EN_5 5
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_211 211
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_488 488
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_600 600
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_832 832
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_1000 1000
#define PT_PT_PwmFrequenz_PT_PwmFrequenz_EN_1200 1200
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_0 0
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_1 1
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_2 2
#define PT_PT_ChxDimCurve_PT_DimCurve_EN_3 3
#define PT_PT_SendMeasuredValues_PT_SendMeasuredValues_EN_1 1
#define PT_PT_SendMeasuredValues_PT_SendMeasuredValues_EN_0 0
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_0 0
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_1 1
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_2 2
#define PT_PT_MeasurementInterval_PT_MeasurementInterval_EN_3 3
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_0 0
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_3 3
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_5 5
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_7 7
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_10 10
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_15 15
#define PT_PT_ChxOnOffTime_PT_ChxOnOffTime_EN_20 20
#define PT_PT_TempSensorPresent_PT_TempSensorPresent_EN_0 0
#define PT_PT_TempSensorPresent_PT_TempSensorPresent_EN_1 1
#define PT_PT_ShuntValue_PT_ShuntValue_EN_5 5
#define PT_PT_ShuntValue_PT_ShuntValue_EN_10 10
#define PT_PT_UseOnColor_PT_UseOnColor_EN_0 0
#define PT_PT_UseOnColor_PT_UseOnColor_EN_1 1
#define PT_PT_OnOffYesNo_PT_OnOffYesNo_EN_0 0
#define PT_PT_OnOffYesNo_PT_OnOffYesNo_EN_1 1
//--------------------Allgemein---------------------------
#define MAIN_OpenKnxId 0xAF
#define MAIN_ApplicationNumber 0x01
#define MAIN_ApplicationVersion 0x01
#define MAIN_OrderNumber "LED01"
#define MAIN_ParameterSize 219
#define MAIN_MaxKoNumber 407


#define APP_PT_Watchdog		0x0010
// Offset: 16, Size: 1 Bit, Text: Watchdog aktivieren
#define ParamAPP_PT_Watchdog knx.paramBit(16, 0)
#define APP_PT_Diagnose		0x0010
// Offset: 16, BitOffset: 1, Size: 1 Bit, Text: Diagnoseobjekt anzeigen
#define ParamAPP_PT_Diagnose knx.paramBit(16, 1)
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
#define APP_PT_ChAOnBrightness		0x0020
#define APP_PT_ChAOnBrightness_Shift	1
#define APP_PT_ChAOnBrightness_Mask	0x007F
// Offset: 32, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_ChAOnBrightness ((uint)((knx.paramByte(32) >> APP_PT_ChAOnBrightness_Shift) & APP_PT_ChAOnBrightness_Mask))
#define APP_PT_ChARelativDimTime		0x0021
#define APP_PT_ChARelativDimTime_Shift	2
#define APP_PT_ChARelativDimTime_Mask	0x003F
// Offset: 33, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_ChARelativDimTime ((uint)((knx.paramByte(33) >> APP_PT_ChARelativDimTime_Shift) & APP_PT_ChARelativDimTime_Mask))
#define APP_PT_ChAOnOffTime		0x0022
#define APP_PT_ChAOnOffTime_Shift	3
#define APP_PT_ChAOnOffTime_Mask	0x001F
// Offset: 34, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_ChAOnOffTime ((uint)((knx.paramByte(34) >> APP_PT_ChAOnOffTime_Shift) & APP_PT_ChAOnOffTime_Mask))
#define APP_PT_ChADimCurve		0x0023
#define APP_PT_ChADimCurve_Shift	6
#define APP_PT_ChADimCurve_Mask	0x0003
// Offset: 35, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_ChADimCurve ((uint)((knx.paramByte(35) >> APP_PT_ChADimCurve_Shift) & APP_PT_ChADimCurve_Mask))
#define APP_PT_ChBOnBrightness		0x0030
#define APP_PT_ChBOnBrightness_Shift	1
#define APP_PT_ChBOnBrightness_Mask	0x007F
// Offset: 48, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_ChBOnBrightness ((uint)((knx.paramByte(48) >> APP_PT_ChBOnBrightness_Shift) & APP_PT_ChBOnBrightness_Mask))
#define APP_PT_ChBRelativDimTime		0x0031
#define APP_PT_ChBRelativDimTime_Shift	2
#define APP_PT_ChBRelativDimTime_Mask	0x003F
// Offset: 49, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_ChBRelativDimTime ((uint)((knx.paramByte(49) >> APP_PT_ChBRelativDimTime_Shift) & APP_PT_ChBRelativDimTime_Mask))
#define APP_PT_ChBOnOffTime		0x0032
#define APP_PT_ChBOnOffTime_Shift	3
#define APP_PT_ChBOnOffTime_Mask	0x001F
// Offset: 50, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_ChBOnOffTime ((uint)((knx.paramByte(50) >> APP_PT_ChBOnOffTime_Shift) & APP_PT_ChBOnOffTime_Mask))
#define APP_PT_ChBDimCurve		0x0033
#define APP_PT_ChBDimCurve_Mask	0x0003
// Offset: 51, BitOffset: 6, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_ChBDimCurve ((uint)((knx.paramByte(51)) & APP_PT_ChBDimCurve_Mask))
#define APP_PT_ChCOnBrightness		0x0040
#define APP_PT_ChCOnBrightness_Shift	1
#define APP_PT_ChCOnBrightness_Mask	0x007F
// Offset: 64, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_ChCOnBrightness ((uint)((knx.paramByte(64) >> APP_PT_ChCOnBrightness_Shift) & APP_PT_ChCOnBrightness_Mask))
#define APP_PT_ChCRelativDimTime		0x0041
#define APP_PT_ChCRelativDimTime_Shift	2
#define APP_PT_ChCRelativDimTime_Mask	0x003F
// Offset: 65, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_ChCRelativDimTime ((uint)((knx.paramByte(65) >> APP_PT_ChCRelativDimTime_Shift) & APP_PT_ChCRelativDimTime_Mask))
#define APP_PT_ChCOnOffTime		0x0042
#define APP_PT_ChCOnOffTime_Shift	3
#define APP_PT_ChCOnOffTime_Mask	0x001F
// Offset: 66, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_ChCOnOffTime ((uint)((knx.paramByte(66) >> APP_PT_ChCOnOffTime_Shift) & APP_PT_ChCOnOffTime_Mask))
#define APP_PT_ChCDimCurve		0x0043
#define APP_PT_ChCDimCurve_Shift	6
#define APP_PT_ChCDimCurve_Mask	0x0003
// Offset: 67, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_ChCDimCurve ((uint)((knx.paramByte(67) >> APP_PT_ChCDimCurve_Shift) & APP_PT_ChCDimCurve_Mask))
#define APP_PT_ChDOnBrightness		0x0050
#define APP_PT_ChDOnBrightness_Shift	1
#define APP_PT_ChDOnBrightness_Mask	0x007F
// Offset: 80, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_ChDOnBrightness ((uint)((knx.paramByte(80) >> APP_PT_ChDOnBrightness_Shift) & APP_PT_ChDOnBrightness_Mask))
#define APP_PT_ChDRelativDimTime		0x0051
#define APP_PT_ChDRelativDimTime_Shift	2
#define APP_PT_ChDRelativDimTime_Mask	0x003F
// Offset: 81, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_ChDRelativDimTime ((uint)((knx.paramByte(81) >> APP_PT_ChDRelativDimTime_Shift) & APP_PT_ChDRelativDimTime_Mask))
#define APP_PT_ChDOnOffTime		0x0052
#define APP_PT_ChDOnOffTime_Shift	3
#define APP_PT_ChDOnOffTime_Mask	0x001F
// Offset: 82, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_ChDOnOffTime ((uint)((knx.paramByte(82) >> APP_PT_ChDOnOffTime_Shift) & APP_PT_ChDOnOffTime_Mask))
#define APP_PT_ChDDimCurve		0x0053
#define APP_PT_ChDDimCurve_Mask	0x0003
// Offset: 83, BitOffset: 6, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_ChDDimCurve ((uint)((knx.paramByte(83)) & APP_PT_ChDDimCurve_Mask))
#define APP_PT_ChEOnBrightness		0x0060
#define APP_PT_ChEOnBrightness_Shift	1
#define APP_PT_ChEOnBrightness_Mask	0x007F
// Offset: 96, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_ChEOnBrightness ((uint)((knx.paramByte(96) >> APP_PT_ChEOnBrightness_Shift) & APP_PT_ChEOnBrightness_Mask))
#define APP_PT_ChERelativDimTime		0x0061
#define APP_PT_ChERelativDimTime_Shift	2
#define APP_PT_ChERelativDimTime_Mask	0x003F
// Offset: 97, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_ChERelativDimTime ((uint)((knx.paramByte(97) >> APP_PT_ChERelativDimTime_Shift) & APP_PT_ChERelativDimTime_Mask))
#define APP_PT_ChEOnOffTime		0x0062
#define APP_PT_ChEOnOffTime_Shift	3
#define APP_PT_ChEOnOffTime_Mask	0x001F
// Offset: 98, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_ChEOnOffTime ((uint)((knx.paramByte(98) >> APP_PT_ChEOnOffTime_Shift) & APP_PT_ChEOnOffTime_Mask))
#define APP_PT_ChEDimCurve		0x0063
#define APP_PT_ChEDimCurve_Shift	6
#define APP_PT_ChEDimCurve_Mask	0x0003
// Offset: 99, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_ChEDimCurve ((uint)((knx.paramByte(99) >> APP_PT_ChEDimCurve_Shift) & APP_PT_ChEDimCurve_Mask))
#define APP_PT_ChFOnBrightness		0x0070
#define APP_PT_ChFOnBrightness_Shift	1
#define APP_PT_ChFOnBrightness_Mask	0x007F
// Offset: 112, Size: 7 Bit, Text: Einschalthelligkeit
#define ParamAPP_PT_ChFOnBrightness ((uint)((knx.paramByte(112) >> APP_PT_ChFOnBrightness_Shift) & APP_PT_ChFOnBrightness_Mask))
#define APP_PT_ChFRelativDimTime		0x0071
#define APP_PT_ChFRelativDimTime_Shift	2
#define APP_PT_ChFRelativDimTime_Mask	0x003F
// Offset: 113, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_ChFRelativDimTime ((uint)((knx.paramByte(113) >> APP_PT_ChFRelativDimTime_Shift) & APP_PT_ChFRelativDimTime_Mask))
#define APP_PT_ChFOnOffTime		0x0072
#define APP_PT_ChFOnOffTime_Shift	3
#define APP_PT_ChFOnOffTime_Mask	0x001F
// Offset: 114, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_ChFOnOffTime ((uint)((knx.paramByte(114) >> APP_PT_ChFOnOffTime_Shift) & APP_PT_ChFOnOffTime_Mask))
#define APP_PT_ChFDimCurve		0x0073
#define APP_PT_ChFDimCurve_Shift	6
#define APP_PT_ChFDimCurve_Mask	0x0003
// Offset: 115, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_ChFDimCurve ((uint)((knx.paramByte(115) >> APP_PT_ChFDimCurve_Shift) & APP_PT_ChFDimCurve_Mask))
#define APP_PT_TW1ColorTempWW		0x0080
#define APP_PT_TW1ColorTempWW_Shift	4
#define APP_PT_TW1ColorTempWW_Mask	0x0FFF
// Offset: 128, Size: 12 Bit, Text: Farbtemperatur Warmweiß
#define ParamAPP_PT_TW1ColorTempWW ((uint)((knx.paramWord(128) >> APP_PT_TW1ColorTempWW_Shift) & APP_PT_TW1ColorTempWW_Mask))
#define APP_PT_TW1ColorTempKW		0x0082
#define APP_PT_TW1ColorTempKW_Shift	3
#define APP_PT_TW1ColorTempKW_Mask	0x1FFF
// Offset: 130, Size: 13 Bit, Text: Farbtemperatur Kaltweiß
#define ParamAPP_PT_TW1ColorTempKW ((uint)((knx.paramWord(130) >> APP_PT_TW1ColorTempKW_Shift) & APP_PT_TW1ColorTempKW_Mask))
#define APP_PT_TW1OnBrightness		0x0084
#define APP_PT_TW1OnBrightness_Shift	1
#define APP_PT_TW1OnBrightness_Mask	0x007F
// Offset: 132, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamAPP_PT_TW1OnBrightness ((uint)((knx.paramByte(132) >> APP_PT_TW1OnBrightness_Shift) & APP_PT_TW1OnBrightness_Mask))
#define APP_PT_TW1OnColorTemp		0x0085
#define APP_PT_TW1OnColorTemp_Shift	2
#define APP_PT_TW1OnColorTemp_Mask	0x3FFF
// Offset: 133, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur
#define ParamAPP_PT_TW1OnColorTemp ((uint)((knx.paramWord(133) >> APP_PT_TW1OnColorTemp_Shift) & APP_PT_TW1OnColorTemp_Mask))
#define APP_PT_TW1RelativDimTime		0x0087
#define APP_PT_TW1RelativDimTime_Shift	2
#define APP_PT_TW1RelativDimTime_Mask	0x003F
// Offset: 135, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_TW1RelativDimTime ((uint)((knx.paramByte(135) >> APP_PT_TW1RelativDimTime_Shift) & APP_PT_TW1RelativDimTime_Mask))
#define APP_PT_TW1OnOffTime		0x0088
#define APP_PT_TW1OnOffTime_Shift	3
#define APP_PT_TW1OnOffTime_Mask	0x001F
// Offset: 136, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_TW1OnOffTime ((uint)((knx.paramByte(136) >> APP_PT_TW1OnOffTime_Shift) & APP_PT_TW1OnOffTime_Mask))
#define APP_PT_TW1DimCurve		0x0089
#define APP_PT_TW1DimCurve_Shift	6
#define APP_PT_TW1DimCurve_Mask	0x0003
// Offset: 137, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_TW1DimCurve ((uint)((knx.paramByte(137) >> APP_PT_TW1DimCurve_Shift) & APP_PT_TW1DimCurve_Mask))
#define APP_PT_TW2ColorTempWW		0x0090
#define APP_PT_TW2ColorTempWW_Shift	4
#define APP_PT_TW2ColorTempWW_Mask	0x0FFF
// Offset: 144, Size: 12 Bit, Text: Farbtemperatur Warmweiß
#define ParamAPP_PT_TW2ColorTempWW ((uint)((knx.paramWord(144) >> APP_PT_TW2ColorTempWW_Shift) & APP_PT_TW2ColorTempWW_Mask))
#define APP_PT_TW2ColorTempKW		0x0092
#define APP_PT_TW2ColorTempKW_Shift	3
#define APP_PT_TW2ColorTempKW_Mask	0x1FFF
// Offset: 146, Size: 13 Bit, Text: Farbtemperatur Kaltweiß
#define ParamAPP_PT_TW2ColorTempKW ((uint)((knx.paramWord(146) >> APP_PT_TW2ColorTempKW_Shift) & APP_PT_TW2ColorTempKW_Mask))
#define APP_PT_TW2OnBrightness		0x0094
#define APP_PT_TW2OnBrightness_Shift	1
#define APP_PT_TW2OnBrightness_Mask	0x007F
// Offset: 148, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamAPP_PT_TW2OnBrightness ((uint)((knx.paramByte(148) >> APP_PT_TW2OnBrightness_Shift) & APP_PT_TW2OnBrightness_Mask))
#define APP_PT_TW2OnColorTemp		0x0095
#define APP_PT_TW2OnColorTemp_Shift	2
#define APP_PT_TW2OnColorTemp_Mask	0x3FFF
// Offset: 149, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur
#define ParamAPP_PT_TW2OnColorTemp ((uint)((knx.paramWord(149) >> APP_PT_TW2OnColorTemp_Shift) & APP_PT_TW2OnColorTemp_Mask))
#define APP_PT_TW2RelativDimTime		0x0097
#define APP_PT_TW2RelativDimTime_Shift	2
#define APP_PT_TW2RelativDimTime_Mask	0x003F
// Offset: 151, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_TW2RelativDimTime ((uint)((knx.paramByte(151) >> APP_PT_TW2RelativDimTime_Shift) & APP_PT_TW2RelativDimTime_Mask))
#define APP_PT_TW2OnOffTime		0x0098
#define APP_PT_TW2OnOffTime_Shift	3
#define APP_PT_TW2OnOffTime_Mask	0x001F
// Offset: 152, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_TW2OnOffTime ((uint)((knx.paramByte(152) >> APP_PT_TW2OnOffTime_Shift) & APP_PT_TW2OnOffTime_Mask))
#define APP_PT_TW2DimCurve		0x0099
#define APP_PT_TW2DimCurve_Shift	6
#define APP_PT_TW2DimCurve_Mask	0x0003
// Offset: 153, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_TW2DimCurve ((uint)((knx.paramByte(153) >> APP_PT_TW2DimCurve_Shift) & APP_PT_TW2DimCurve_Mask))
#define APP_PT_TW3ColorTempWW		0x00A0
#define APP_PT_TW3ColorTempWW_Shift	4
#define APP_PT_TW3ColorTempWW_Mask	0x0FFF
// Offset: 160, Size: 12 Bit, Text: Farbtemperatur Warmweiß
#define ParamAPP_PT_TW3ColorTempWW ((uint)((knx.paramWord(160) >> APP_PT_TW3ColorTempWW_Shift) & APP_PT_TW3ColorTempWW_Mask))
#define APP_PT_TW3ColorTempKW		0x00A2
#define APP_PT_TW3ColorTempKW_Shift	3
#define APP_PT_TW3ColorTempKW_Mask	0x1FFF
// Offset: 162, Size: 13 Bit, Text: Farbtemperatur Kaltweiß
#define ParamAPP_PT_TW3ColorTempKW ((uint)((knx.paramWord(162) >> APP_PT_TW3ColorTempKW_Shift) & APP_PT_TW3ColorTempKW_Mask))
#define APP_PT_TW3OnBrightness		0x00A4
#define APP_PT_TW3OnBrightness_Shift	1
#define APP_PT_TW3OnBrightness_Mask	0x007F
// Offset: 164, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamAPP_PT_TW3OnBrightness ((uint)((knx.paramByte(164) >> APP_PT_TW3OnBrightness_Shift) & APP_PT_TW3OnBrightness_Mask))
#define APP_PT_TW3OnColorTemp		0x00A5
#define APP_PT_TW3OnColorTemp_Shift	2
#define APP_PT_TW3OnColorTemp_Mask	0x3FFF
// Offset: 165, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur
#define ParamAPP_PT_TW3OnColorTemp ((uint)((knx.paramWord(165) >> APP_PT_TW3OnColorTemp_Shift) & APP_PT_TW3OnColorTemp_Mask))
#define APP_PT_TW3RelativDimTime		0x00A7
#define APP_PT_TW3RelativDimTime_Shift	2
#define APP_PT_TW3RelativDimTime_Mask	0x003F
// Offset: 167, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_TW3RelativDimTime ((uint)((knx.paramByte(167) >> APP_PT_TW3RelativDimTime_Shift) & APP_PT_TW3RelativDimTime_Mask))
#define APP_PT_TW3OnOffTime		0x00A8
#define APP_PT_TW3OnOffTime_Shift	3
#define APP_PT_TW3OnOffTime_Mask	0x001F
// Offset: 168, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_TW3OnOffTime ((uint)((knx.paramByte(168) >> APP_PT_TW3OnOffTime_Shift) & APP_PT_TW3OnOffTime_Mask))
#define APP_PT_TW3DimCurve		0x00A9
#define APP_PT_TW3DimCurve_Shift	6
#define APP_PT_TW3DimCurve_Mask	0x0003
// Offset: 169, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_TW3DimCurve ((uint)((knx.paramByte(169) >> APP_PT_TW3DimCurve_Shift) & APP_PT_TW3DimCurve_Mask))
#define APP_PT_RGB1OnColor		0x00C0
// Offset: 192, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe
#define ParamAPP_PT_RGB1OnColor knx.paramData(192)
#define APP_PT_RGB1UseOnColor		0x00C7
// Offset: 199, Size: 1 Bit, Text: Einschaltverhalten
#define ParamAPP_PT_RGB1UseOnColor knx.paramBit(199, 0)
#define APP_PT_RGB1RelativDimTime		0x00C8
#define APP_PT_RGB1RelativDimTime_Shift	2
#define APP_PT_RGB1RelativDimTime_Mask	0x003F
// Offset: 200, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_RGB1RelativDimTime ((uint)((knx.paramByte(200) >> APP_PT_RGB1RelativDimTime_Shift) & APP_PT_RGB1RelativDimTime_Mask))
#define APP_PT_RGB1OnOffTime		0x00C9
#define APP_PT_RGB1OnOffTime_Shift	3
#define APP_PT_RGB1OnOffTime_Mask	0x001F
// Offset: 201, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_RGB1OnOffTime ((uint)((knx.paramByte(201) >> APP_PT_RGB1OnOffTime_Shift) & APP_PT_RGB1OnOffTime_Mask))
#define APP_PT_RGB1DimCurve		0x00CA
#define APP_PT_RGB1DimCurve_Shift	6
#define APP_PT_RGB1DimCurve_Mask	0x0003
// Offset: 202, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_RGB1DimCurve ((uint)((knx.paramByte(202) >> APP_PT_RGB1DimCurve_Shift) & APP_PT_RGB1DimCurve_Mask))
#define APP_PT_RGB2OnColor		0x00D0
// Offset: 208, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe
#define ParamAPP_PT_RGB2OnColor knx.paramData(208)
#define APP_PT_RGB2UseOnColor		0x00D7
// Offset: 215, Size: 1 Bit, Text: Einschaltverhalten
#define ParamAPP_PT_RGB2UseOnColor knx.paramBit(215, 0)
#define APP_PT_RGB2RelativDimTime		0x00D8
#define APP_PT_RGB2RelativDimTime_Shift	2
#define APP_PT_RGB2RelativDimTime_Mask	0x003F
// Offset: 216, Size: 6 Bit, Text: Dimmgeschwindigkeit Relativ
#define ParamAPP_PT_RGB2RelativDimTime ((uint)((knx.paramByte(216) >> APP_PT_RGB2RelativDimTime_Shift) & APP_PT_RGB2RelativDimTime_Mask))
#define APP_PT_RGB2OnOffTime		0x00D9
#define APP_PT_RGB2OnOffTime_Shift	3
#define APP_PT_RGB2OnOffTime_Mask	0x001F
// Offset: 217, Size: 5 Bit, Text: Ein- und Ausschaltgeschwindigkeit
#define ParamAPP_PT_RGB2OnOffTime ((uint)((knx.paramByte(217) >> APP_PT_RGB2OnOffTime_Shift) & APP_PT_RGB2OnOffTime_Mask))
#define APP_PT_RGB2DimCurve		0x00DA
#define APP_PT_RGB2DimCurve_Shift	6
#define APP_PT_RGB2DimCurve_Mask	0x0003
// Offset: 218, Size: 2 Bit, Text: Dimmkurve
#define ParamAPP_PT_RGB2DimCurve ((uint)((knx.paramByte(218) >> APP_PT_RGB2DimCurve_Shift) & APP_PT_RGB2DimCurve_Mask))
//!< Number: 7, Text: Diagnose, Function: Diagnoseobjekt
#define APP_KoKO_Diagnose 7
#define KoAPP_KO_Diagnose knx.getGroupObject(7)
//!< Number: 20, Text: Spannung, Function: Spannung
#define APP_KoKO_VoltageV 20
#define KoAPP_KO_VoltageV knx.getGroupObject(20)
//!< Number: 21, Text: Strom, Function: Stromwert
#define APP_KoKO_CurrentA 21
#define KoAPP_KO_CurrentA knx.getGroupObject(21)
//!< Number: 22, Text: Wirkleistung, Function: Wirkleistung
#define APP_KoKO_PowerW 22
#define KoAPP_KO_PowerW knx.getGroupObject(22)
//!< Number: 25, Text: Gerätetemperatur, Function: Gerätetemperatur
#define APP_KoKO_TempC 25
#define KoAPP_KO_TempC knx.getGroupObject(25)
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

