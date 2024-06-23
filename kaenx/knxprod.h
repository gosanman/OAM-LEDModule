#pragma once

#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                            (time & 0x3FFF) * 3600000 ) : 0 )
#define PT_DimCurve_A 0
#define PT_DimCurve_B 1
#define PT_DimCurve_C 2
#define PT_DimCurve_D 3
#define PT_DimCurve_E 4
#define PT_FuncClickAction_none 0
#define PT_FuncClickAction_on 1
#define PT_FuncClickAction_off 2
#define PT_FuncClickAction_toggle 3
//--------------------Allgemein---------------------------
#define MAIN_OpenKnxId 0xAF
#define MAIN_ApplicationNumber 0x01
#define MAIN_ApplicationVersion 0x01
#define MAIN_OrderNumber "OpenKnxLEDDimmer"
#define MAIN_ParameterSize 249
#define MAIN_MaxKoNumber 482


#define APP_Device		0x0000
// Offset: 0, Size: 1 Bit, Text: Gerät
#define ParamAPP_Device knx.paramBit(APP_Device, 0)
#define APP_OperatingMode		0x0000
#define APP_OperatingMode_Shift	3
#define APP_OperatingMode_Mask	0x000F
// Offset: 0, BitOffset: 1, Size: 4 Bit, Text: Betriebsmodus
#define ParamAPP_OperatingMode ((uint)((knx.paramByte(APP_OperatingMode) >> APP_OperatingMode_Shift) & APP_OperatingMode_Mask))
#define APP_PwmFrequenz		0x0001
#define APP_PwmFrequenz_Shift	5
#define APP_PwmFrequenz_Mask	0x07FF
// Offset: 1, Size: 11 Bit, Text: PWM Frequenz
#define ParamAPP_PwmFrequenz ((uint)((knx.paramWord(APP_PwmFrequenz) >> APP_PwmFrequenz_Shift) & APP_PwmFrequenz_Mask))
#define APP_SendMeasuredValues		0x0000
// Offset: 0, BitOffset: 5, Size: 1 Bit, Text: Messwerte zyklisch senden
#define ParamAPP_SendMeasuredValues knx.paramBit(APP_SendMeasuredValues, 5)
#define APP_TempSensorPresent		0x0000
// Offset: 0, BitOffset: 6, Size: 1 Bit, Text: Temperatursensor verbaut
#define ParamAPP_TempSensorPresent knx.paramBit(APP_TempSensorPresent, 6)
#define APP_MeasurementIntervalTime		0x0003
#define APP_MeasurementIntervalTime_Shift	2
#define APP_MeasurementIntervalTime_Mask	0x3FFF
// Offset: 3, Size: 14 Bit, Text: Zeit
#define ParamAPP_MeasurementIntervalTime ((uint)((knx.paramWord(APP_MeasurementIntervalTime) >> APP_MeasurementIntervalTime_Shift) & APP_MeasurementIntervalTime_Mask))
#define ParamAPP_MeasurementIntervalTimeMS (paramDelay(ParamAPP_MeasurementIntervalTime))
#define APP_MeasurementIntervalBase		0x0005
#define APP_MeasurementIntervalBase_Shift	6
#define APP_MeasurementIntervalBase_Mask	0x0003
// Offset: 5, Size: 2 Bit, Text: Zeitbasis
#define ParamAPP_MeasurementIntervalBase ((uint)((knx.paramByte(APP_MeasurementIntervalBase) >> APP_MeasurementIntervalBase_Shift) & APP_MeasurementIntervalBase_Mask))
#define APP_ShuntValue		0x0005
#define APP_ShuntValue_Shift	2
#define APP_ShuntValue_Mask	0x000F
// Offset: 5, BitOffset: 2, Size: 4 Bit, Text: Wert des verbauten Messwiderstand
#define ParamAPP_ShuntValue ((uint)((knx.paramByte(APP_ShuntValue) >> APP_ShuntValue_Shift) & APP_ShuntValue_Mask))
#define APP_DayNight		0x0000
// Offset: 0, BitOffset: 7, Size: 1 Bit, Text: Tag/Nacht Objekt
#define ParamAPP_DayNight knx.paramBit(APP_DayNight, 7)
#define APP_Func1BtnClick		0x0006
#define APP_Func1BtnClick_Shift	4
#define APP_Func1BtnClick_Mask	0x000F
// Offset: 6, Size: 4 Bit, Text: Aktion - Func1 Button Klick
#define ParamAPP_Func1BtnClick ((uint)((knx.paramByte(APP_Func1BtnClick) >> APP_Func1BtnClick_Shift) & APP_Func1BtnClick_Mask))
#define APP_Func1BtnDblClick		0x0006
#define APP_Func1BtnDblClick_Mask	0x000F
// Offset: 6, BitOffset: 4, Size: 4 Bit, Text: Aktion - Func1 Button Doppelklick
#define ParamAPP_Func1BtnDblClick ((uint)((knx.paramByte(APP_Func1BtnDblClick)) & APP_Func1BtnDblClick_Mask))
#define APP_Func1BtnLongClick		0x0007
#define APP_Func1BtnLongClick_Shift	4
#define APP_Func1BtnLongClick_Mask	0x000F
// Offset: 7, Size: 4 Bit, Text: Aktion - Func1 Button Langer Klick
#define ParamAPP_Func1BtnLongClick ((uint)((knx.paramByte(APP_Func1BtnLongClick) >> APP_Func1BtnLongClick_Shift) & APP_Func1BtnLongClick_Mask))
//!< Number: 31, Text: Spannung, Function: Messwert
#define APP_KoVoltageV 31
#define KoAPP_VoltageV knx.getGroupObject(APP_KoVoltageV)
//!< Number: 32, Text: Strom, Function: Messwert
#define APP_KoCurrentA 32
#define KoAPP_CurrentA knx.getGroupObject(APP_KoCurrentA)
//!< Number: 33, Text: Wirkleistung, Function: Messwert
#define APP_KoPowerW 33
#define KoAPP_PowerW knx.getGroupObject(APP_KoPowerW)
//!< Number: 35, Text: Gerätetemperatur, Function: Messwert
#define APP_KoTempC 35
#define KoAPP_TempC knx.getGroupObject(APP_KoTempC)
//!< Number: 40, Text: Tag/Nacht, Function: Allgemein
#define APP_KoDayNight 40
#define KoAPP_DayNight knx.getGroupObject(APP_KoDayNight)

//---------------------Modules----------------------------

//-----Module specific starts
#define BASE_Share_ParamBlockOffset 8
#define BASE_Share_ParamBlockSize 45
#define EK_ParamBlockOffset 53
#define EK_ParamBlockSize 6
#define TW_ParamBlockOffset 125
#define TW_ParamBlockSize 14
#define RGB_ParamBlockOffset 209
#define RGB_ParamBlockSize 10
#define BASE_Share_KoOffset 41
#define BASE_Share_KoBlockSize 11
#define EK_KoOffset 52
#define EK_KoBlockSize 18
#define TW_KoOffset 268
#define TW_KoBlockSize 19
#define RGB_KoOffset 382
#define RGB_KoBlockSize 25

//-----Module: Common Share
#define BASE_StartupDelayBase		0x0000
#define BASE_StartupDelayBase_Shift	6
#define BASE_StartupDelayBase_Mask	0x0003
// UnionOffset: 0, ParaOffset: 0, Size: 2 Bit, Text: Zeitbasis
#define ParamBASE_StartupDelayBase ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_StartupDelayBase)) >> BASE_StartupDelayBase_Shift) & BASE_StartupDelayBase_Mask))
#define BASE_StartupDelayTime		0x0000
#define BASE_StartupDelayTime_Mask	0x3FFF
// UnionOffset: 0, ParaOffset: 0, BitOffset: 2, Size: 14 Bit, Text: Zeit
#define ParamBASE_StartupDelayTime ((uint)((knx.paramWord((BASE_Share_ParamBlockOffset + BASE_StartupDelayTime))) & BASE_StartupDelayTime_Mask))
#define ParamBASE_StartupDelayTimeMS (paramDelay(ParamBASE_StartupDelayTime))
#define BASE_HeartbeatDelayBase		0x0002
#define BASE_HeartbeatDelayBase_Shift	6
#define BASE_HeartbeatDelayBase_Mask	0x0003
// UnionOffset: 0, ParaOffset: 2, Size: 2 Bit, Text: Zeitbasis
#define ParamBASE_HeartbeatDelayBase ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_HeartbeatDelayBase)) >> BASE_HeartbeatDelayBase_Shift) & BASE_HeartbeatDelayBase_Mask))
#define BASE_HeartbeatDelayTime		0x0002
#define BASE_HeartbeatDelayTime_Mask	0x3FFF
// UnionOffset: 0, ParaOffset: 2, BitOffset: 2, Size: 14 Bit, Text: Zeit
#define ParamBASE_HeartbeatDelayTime ((uint)((knx.paramWord((BASE_Share_ParamBlockOffset + BASE_HeartbeatDelayTime))) & BASE_HeartbeatDelayTime_Mask))
#define ParamBASE_HeartbeatDelayTimeMS (paramDelay(ParamBASE_HeartbeatDelayTime))
#define BASE_Timezone		0x0004
#define BASE_Timezone_Shift	3
#define BASE_Timezone_Mask	0x001F
// UnionOffset: 4, ParaOffset: 0, Size: 5 Bit, Text: Zeitzone
#define ParamBASE_Timezone ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_Timezone)) >> BASE_Timezone_Shift) & BASE_Timezone_Mask))
#define BASE_CombinedTimeDate		0x0004
// UnionOffset: 4, ParaOffset: 0, BitOffset: 5, Size: 1 Bit, Text: Empfangen über
#define ParamBASE_CombinedTimeDate knx.paramBit((BASE_Share_ParamBlockOffset + BASE_CombinedTimeDate), 5)
#define BASE_SummertimeAll		0x0004
#define BASE_SummertimeAll_Mask	0x0003
// UnionOffset: 4, ParaOffset: 0, BitOffset: 6, Size: 2 Bit, Text: Sommerzeit ermitteln durch
#define ParamBASE_SummertimeAll ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_SummertimeAll))) & BASE_SummertimeAll_Mask))
#define BASE_SummertimeDE		0x0004
#define BASE_SummertimeDE_Mask	0x0003
// UnionOffset: 4, ParaOffset: 0, BitOffset: 6, Size: 2 Bit, Text: Sommerzeit ermitteln durch
#define ParamBASE_SummertimeDE ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_SummertimeDE))) & BASE_SummertimeDE_Mask))
#define BASE_SummertimeWorld		0x0004
#define BASE_SummertimeWorld_Mask	0x0003
// UnionOffset: 4, ParaOffset: 0, BitOffset: 6, Size: 2 Bit, Text: Sommerzeit ermitteln durch
#define ParamBASE_SummertimeWorld ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_SummertimeWorld))) & BASE_SummertimeWorld_Mask))
#define BASE_SummertimeKO		0x0004
#define BASE_SummertimeKO_Mask	0x0003
// UnionOffset: 4, ParaOffset: 0, BitOffset: 6, Size: 2 Bit, Text: Sommerzeit ermitteln durch
#define ParamBASE_SummertimeKO ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_SummertimeKO))) & BASE_SummertimeKO_Mask))
#define BASE_Latitude		0x0005
// UnionOffset: 5, ParaOffset: 0, Size: 16 Bit (2 Byte), Text: Breitengrad
#define ParamBASE_Latitude knx.paramFloat((BASE_Share_ParamBlockOffset + BASE_Latitude), Float_Enc_IEEE754Single)
#define BASE_Longitude		0x0009
// UnionOffset: 5, ParaOffset: 4, Size: 16 Bit (2 Byte), Text: Längengrad
#define ParamBASE_Longitude knx.paramFloat((BASE_Share_ParamBlockOffset + BASE_Longitude), Float_Enc_IEEE754Single)
#define BASE_Diagnose		0x000D
// UnionOffset: 13, ParaOffset: 0, Size: 1 Bit, Text: Diagnoseobjekt anzeigen
#define ParamBASE_Diagnose knx.paramBit((BASE_Share_ParamBlockOffset + BASE_Diagnose), 0)
#define BASE_Watchdog		0x000D
// UnionOffset: 13, ParaOffset: 0, BitOffset: 1, Size: 1 Bit, Text: Watchdog aktivieren
#define ParamBASE_Watchdog knx.paramBit((BASE_Share_ParamBlockOffset + BASE_Watchdog), 1)
#define BASE_ReadTimeDate		0x000D
// UnionOffset: 13, ParaOffset: 0, BitOffset: 2, Size: 1 Bit, Text: Bei Neustart vom Bus lesen
#define ParamBASE_ReadTimeDate knx.paramBit((BASE_Share_ParamBlockOffset + BASE_ReadTimeDate), 2)
#define BASE_HeartbeatExtended		0x000D
// UnionOffset: 13, ParaOffset: 0, BitOffset: 3, Size: 1 Bit, Text: Erweitertes "In Betrieb"
#define ParamBASE_HeartbeatExtended knx.paramBit((BASE_Share_ParamBlockOffset + BASE_HeartbeatExtended), 3)
#define BASE_ManualSave		0x000D
#define BASE_ManualSave_Mask	0x0007
// UnionOffset: 13, ParaOffset: 0, BitOffset: 5, Size: 3 Bit, Text: Manuelles speichern
#define ParamBASE_ManualSave ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_ManualSave))) & BASE_ManualSave_Mask))
#define BASE_PeriodicSave		0x000E
// UnionOffset: 13, ParaOffset: 1, Size: 8 Bit (1 Byte), Text: Zyklisches speichern
#define ParamBASE_PeriodicSave ((uint)((knx.paramByte((BASE_Share_ParamBlockOffset + BASE_PeriodicSave)))))
//!< Number: 0, Text: In Betrieb, Function: Zyklisch
#define BASE_KoHeartbeat 0
#define KoBASE_Heartbeat knx.getGroupObject(BASE_KoHeartbeat + BASE_Share_KoOffset)
//!< Number: 1, Text: Uhrzeit/Datum, Function: Eingang
#define BASE_KoTime 1
#define KoBASE_Time knx.getGroupObject(BASE_KoTime + BASE_Share_KoOffset)
//!< Number: 2, Text: Datum, Function: Eingang
#define BASE_KoDate 2
#define KoBASE_Date knx.getGroupObject(BASE_KoDate + BASE_Share_KoOffset)
//!< Number: 6, Text: Diagnose, Function: Diagnoseobjekt
#define BASE_KoDiagnose 6
#define KoBASE_Diagnose knx.getGroupObject(BASE_KoDiagnose + BASE_Share_KoOffset)
//!< Number: 9, Text: Sommerzeit aktiv, Function: Eingang
#define BASE_KoIsSummertime 9
#define KoBASE_IsSummertime knx.getGroupObject(BASE_KoIsSummertime + BASE_Share_KoOffset)
//!< Number: 10, Text: Speichern, Function: Eingang
#define BASE_KoManualSave 10
#define KoBASE_ManualSave knx.getGroupObject(BASE_KoManualSave + BASE_Share_KoOffset)

//-----Module: EK
#define EK_UseOnValue		0x0000
// Offset: 0, Size: 1 Bit, Text: Einschaltverhalten
#define ParamEK_UseOnValueIndex(X) knx.paramBit((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_UseOnValue), 0)
// Offset: 0, Size: 1 Bit, Text: Einschaltverhalten
#define ParamEK_UseOnValue knx.paramBit((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_UseOnValue), 0)
#define EK_UseNightValue		0x0000
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: Einschaltverhalten - Nacht
#define ParamEK_UseNightValueIndex(X) knx.paramBit((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_UseNightValue), 1)
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: Einschaltverhalten - Nacht
#define ParamEK_UseNightValue knx.paramBit((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_UseNightValue), 1)
#define EK_OnBrightness		0x0001
#define EK_OnBrightness_Shift	1
#define EK_OnBrightness_Mask	0x007F
// Offset: 1, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamEK_OnBrightnessIndex(X) ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_OnBrightness)) >> EK_OnBrightness_Shift) & EK_OnBrightness_Mask))
// Offset: 1, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamEK_OnBrightness ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_OnBrightness)) >> EK_OnBrightness_Shift) & EK_OnBrightness_Mask))
#define EK_NightBrightness		0x0002
#define EK_NightBrightness_Shift	1
#define EK_NightBrightness_Mask	0x007F
// Offset: 2, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit (Nacht)
#define ParamEK_NightBrightnessIndex(X) ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_NightBrightness)) >> EK_NightBrightness_Shift) & EK_NightBrightness_Mask))
// Offset: 2, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit (Nacht)
#define ParamEK_NightBrightness ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_NightBrightness)) >> EK_NightBrightness_Shift) & EK_NightBrightness_Mask))
#define EK_RelativDimTime		0x0003
#define EK_RelativDimTime_Shift	1
#define EK_RelativDimTime_Mask	0x007F
// Offset: 3, Size: 7 Bit, Text: Zeit
#define ParamEK_RelativDimTimeIndex(X) ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_RelativDimTime)) >> EK_RelativDimTime_Shift) & EK_RelativDimTime_Mask))
// Offset: 3, Size: 7 Bit, Text: Zeit
#define ParamEK_RelativDimTime ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_RelativDimTime)) >> EK_RelativDimTime_Shift) & EK_RelativDimTime_Mask))
#define EK_RelativDimBase		0x0000
#define EK_RelativDimBase_Shift	4
#define EK_RelativDimBase_Mask	0x0003
// Offset: 0, BitOffset: 2, Size: 2 Bit, Text: Zeitbasis
#define ParamEK_RelativDimBaseIndex(X) ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_RelativDimBase)) >> EK_RelativDimBase_Shift) & EK_RelativDimBase_Mask))
// Offset: 0, BitOffset: 2, Size: 2 Bit, Text: Zeitbasis
#define ParamEK_RelativDimBase ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_RelativDimBase)) >> EK_RelativDimBase_Shift) & EK_RelativDimBase_Mask))
#define EK_OnOffTime		0x0004
#define EK_OnOffTime_Shift	1
#define EK_OnOffTime_Mask	0x007F
// Offset: 4, Size: 7 Bit, Text: Zeit
#define ParamEK_OnOffTimeIndex(X) ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_OnOffTime)) >> EK_OnOffTime_Shift) & EK_OnOffTime_Mask))
// Offset: 4, Size: 7 Bit, Text: Zeit
#define ParamEK_OnOffTime ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_OnOffTime)) >> EK_OnOffTime_Shift) & EK_OnOffTime_Mask))
#define EK_OnOffBase		0x0000
#define EK_OnOffBase_Shift	2
#define EK_OnOffBase_Mask	0x0003
// Offset: 0, BitOffset: 4, Size: 2 Bit, Text: Zeitbasis
#define ParamEK_OnOffBaseIndex(X) ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_OnOffBase)) >> EK_OnOffBase_Shift) & EK_OnOffBase_Mask))
// Offset: 0, BitOffset: 4, Size: 2 Bit, Text: Zeitbasis
#define ParamEK_OnOffBase ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_OnOffBase)) >> EK_OnOffBase_Shift) & EK_OnOffBase_Mask))
#define EK_DimCurve		0x0005
#define EK_DimCurve_Shift	5
#define EK_DimCurve_Mask	0x0007
// Offset: 5, Size: 3 Bit, Text: Dimmkurve
#define ParamEK_DimCurveIndex(X) ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * X + EK_DimCurve)) >> EK_DimCurve_Shift) & EK_DimCurve_Mask))
// Offset: 5, Size: 3 Bit, Text: Dimmkurve
#define ParamEK_DimCurve ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_DimCurve)) >> EK_DimCurve_Shift) & EK_DimCurve_Mask))
//!< Number: 0, Text: EK{{argChan}}: {{0:---}}, Function: Schalten
#define EK_KoSwitch 0
#define KoEK_SwitchIndex(X) knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * X + EK_KoSwitch)
#define KoEK_Switch knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * channelIndex() + EK_KoSwitch)
//!< Number: 2, Text: EK{{argChan}}: {{0:---}}, Function: Dimmen Absolut
#define EK_KoDimAbsolute 2
#define KoEK_DimAbsoluteIndex(X) knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * X + EK_KoDimAbsolute)
#define KoEK_DimAbsolute knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * channelIndex() + EK_KoDimAbsolute)
//!< Number: 10, Text: EK{{argChan}}: {{0:---}}, Function: Dimmen Relativ
#define EK_KoDimRelativ 10
#define KoEK_DimRelativIndex(X) knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * X + EK_KoDimRelativ)
#define KoEK_DimRelativ knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * channelIndex() + EK_KoDimRelativ)
//!< Number: 16, Text: EK{{argChan}}: {{0:---}}, Function: Status An/Aus
#define EK_KoStatusOnOff 16
#define KoEK_StatusOnOffIndex(X) knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * X + EK_KoStatusOnOff)
#define KoEK_StatusOnOff knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * channelIndex() + EK_KoStatusOnOff)
//!< Number: 17, Text: EK{{argChan}}: {{0:---}}, Function: Status Helligkeit
#define EK_KoStatusBrightness 17
#define KoEK_StatusBrightnessIndex(X) knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * X + EK_KoStatusBrightness)
#define KoEK_StatusBrightness knx.getGroupObject(EK_KoOffset + EK_KoBlockSize * channelIndex() + EK_KoStatusBrightness)

//-----Module: TW
#define TW_ColorTempWW		0x0000
#define TW_ColorTempWW_Shift	4
#define TW_ColorTempWW_Mask	0x0FFF
// Offset: 0, Size: 12 Bit, Text: Farbtemperatur Warmweiß
#define ParamTW_ColorTempWWIndex(X) ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_ColorTempWW)) >> TW_ColorTempWW_Shift) & TW_ColorTempWW_Mask))
// Offset: 0, Size: 12 Bit, Text: Farbtemperatur Warmweiß
#define ParamTW_ColorTempWW ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_ColorTempWW)) >> TW_ColorTempWW_Shift) & TW_ColorTempWW_Mask))
#define TW_ColorTempKW		0x0002
#define TW_ColorTempKW_Shift	3
#define TW_ColorTempKW_Mask	0x1FFF
// Offset: 2, Size: 13 Bit, Text: Farbtemperatur Kaltweiß
#define ParamTW_ColorTempKWIndex(X) ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_ColorTempKW)) >> TW_ColorTempKW_Shift) & TW_ColorTempKW_Mask))
// Offset: 2, Size: 13 Bit, Text: Farbtemperatur Kaltweiß
#define ParamTW_ColorTempKW ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_ColorTempKW)) >> TW_ColorTempKW_Shift) & TW_ColorTempKW_Mask))
#define TW_UseOnValue		0x0004
// Offset: 4, Size: 1 Bit, Text: Einschaltverhalten
#define ParamTW_UseOnValueIndex(X) knx.paramBit((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_UseOnValue), 0)
// Offset: 4, Size: 1 Bit, Text: Einschaltverhalten
#define ParamTW_UseOnValue knx.paramBit((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_UseOnValue), 0)
#define TW_UseNightValue		0x0004
// Offset: 4, BitOffset: 1, Size: 1 Bit, Text: Einschaltverhalten - Nacht
#define ParamTW_UseNightValueIndex(X) knx.paramBit((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_UseNightValue), 1)
// Offset: 4, BitOffset: 1, Size: 1 Bit, Text: Einschaltverhalten - Nacht
#define ParamTW_UseNightValue knx.paramBit((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_UseNightValue), 1)
#define TW_OnBrightness		0x0005
#define TW_OnBrightness_Shift	1
#define TW_OnBrightness_Mask	0x007F
// Offset: 5, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamTW_OnBrightnessIndex(X) ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_OnBrightness)) >> TW_OnBrightness_Shift) & TW_OnBrightness_Mask))
// Offset: 5, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit
#define ParamTW_OnBrightness ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_OnBrightness)) >> TW_OnBrightness_Shift) & TW_OnBrightness_Mask))
#define TW_NightBrightness		0x0006
#define TW_NightBrightness_Shift	1
#define TW_NightBrightness_Mask	0x007F
// Offset: 6, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit (Nacht)
#define ParamTW_NightBrightnessIndex(X) ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_NightBrightness)) >> TW_NightBrightness_Shift) & TW_NightBrightness_Mask))
// Offset: 6, Size: 7 Bit, Text: Einschaltverhalten - Helligkeit (Nacht)
#define ParamTW_NightBrightness ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_NightBrightness)) >> TW_NightBrightness_Shift) & TW_NightBrightness_Mask))
#define TW_OnColorTemp		0x0007
#define TW_OnColorTemp_Shift	2
#define TW_OnColorTemp_Mask	0x3FFF
// Offset: 7, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur
#define ParamTW_OnColorTempIndex(X) ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_OnColorTemp)) >> TW_OnColorTemp_Shift) & TW_OnColorTemp_Mask))
// Offset: 7, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur
#define ParamTW_OnColorTemp ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_OnColorTemp)) >> TW_OnColorTemp_Shift) & TW_OnColorTemp_Mask))
#define TW_NightColorTemp		0x0009
#define TW_NightColorTemp_Shift	2
#define TW_NightColorTemp_Mask	0x3FFF
// Offset: 9, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur (Nacht)
#define ParamTW_NightColorTempIndex(X) ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_NightColorTemp)) >> TW_NightColorTemp_Shift) & TW_NightColorTemp_Mask))
// Offset: 9, Size: 14 Bit, Text: Einschaltverhalten - Farbtemperatur (Nacht)
#define ParamTW_NightColorTemp ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_NightColorTemp)) >> TW_NightColorTemp_Shift) & TW_NightColorTemp_Mask))
#define TW_RelativDimTime		0x000B
#define TW_RelativDimTime_Shift	1
#define TW_RelativDimTime_Mask	0x007F
// Offset: 11, Size: 7 Bit, Text: Zeit
#define ParamTW_RelativDimTimeIndex(X) ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_RelativDimTime)) >> TW_RelativDimTime_Shift) & TW_RelativDimTime_Mask))
// Offset: 11, Size: 7 Bit, Text: Zeit
#define ParamTW_RelativDimTime ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_RelativDimTime)) >> TW_RelativDimTime_Shift) & TW_RelativDimTime_Mask))
#define TW_RelativDimBase		0x0004
#define TW_RelativDimBase_Shift	4
#define TW_RelativDimBase_Mask	0x0003
// Offset: 4, BitOffset: 2, Size: 2 Bit, Text: Zeitbasis
#define ParamTW_RelativDimBaseIndex(X) ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_RelativDimBase)) >> TW_RelativDimBase_Shift) & TW_RelativDimBase_Mask))
// Offset: 4, BitOffset: 2, Size: 2 Bit, Text: Zeitbasis
#define ParamTW_RelativDimBase ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_RelativDimBase)) >> TW_RelativDimBase_Shift) & TW_RelativDimBase_Mask))
#define TW_OnOffTime		0x000C
#define TW_OnOffTime_Shift	1
#define TW_OnOffTime_Mask	0x007F
// Offset: 12, Size: 7 Bit, Text: Zeit
#define ParamTW_OnOffTimeIndex(X) ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_OnOffTime)) >> TW_OnOffTime_Shift) & TW_OnOffTime_Mask))
// Offset: 12, Size: 7 Bit, Text: Zeit
#define ParamTW_OnOffTime ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_OnOffTime)) >> TW_OnOffTime_Shift) & TW_OnOffTime_Mask))
#define TW_OnOffBase		0x0004
#define TW_OnOffBase_Shift	2
#define TW_OnOffBase_Mask	0x0003
// Offset: 4, BitOffset: 4, Size: 2 Bit, Text: Zeitbasis
#define ParamTW_OnOffBaseIndex(X) ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_OnOffBase)) >> TW_OnOffBase_Shift) & TW_OnOffBase_Mask))
// Offset: 4, BitOffset: 4, Size: 2 Bit, Text: Zeitbasis
#define ParamTW_OnOffBase ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_OnOffBase)) >> TW_OnOffBase_Shift) & TW_OnOffBase_Mask))
#define TW_DimCurve		0x000D
#define TW_DimCurve_Shift	5
#define TW_DimCurve_Mask	0x0007
// Offset: 13, Size: 3 Bit, Text: Dimmkurve
#define ParamTW_DimCurveIndex(X) ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * X + TW_DimCurve)) >> TW_DimCurve_Shift) & TW_DimCurve_Mask))
// Offset: 13, Size: 3 Bit, Text: Dimmkurve
#define ParamTW_DimCurve ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_DimCurve)) >> TW_DimCurve_Shift) & TW_DimCurve_Mask))
//!< Number: 0, Text: TW{{argChan}}: {{0:---}}, Function: Schalten
#define TW_KoSwitch 0
#define KoTW_SwitchIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoSwitch)
#define KoTW_Switch knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoSwitch)
//!< Number: 2, Text: TW{{argChan}}: {{0:---}}, Function: Dimmen Absolut Helligkeit
#define TW_KoDimAbsoluteBrightness 2
#define KoTW_DimAbsoluteBrightnessIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoDimAbsoluteBrightness)
#define KoTW_DimAbsoluteBrightness knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoDimAbsoluteBrightness)
//!< Number: 3, Text: TW{{argChan}}: {{0:---}}, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define TW_KoDimAbsoluteColorTemp 3
#define KoTW_DimAbsoluteColorTempIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoDimAbsoluteColorTemp)
#define KoTW_DimAbsoluteColorTemp knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoDimAbsoluteColorTemp)
//!< Number: 10, Text: TW{{argChan}}: {{0:---}}, Function: Dimmen Relativ Helligkeit
#define TW_KoDimRelativBrightness 10
#define KoTW_DimRelativBrightnessIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoDimRelativBrightness)
#define KoTW_DimRelativBrightness knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoDimRelativBrightness)
//!< Number: 11, Text: TW{{argChan}}: {{0:---}}, Function: Dimmen Relativ Anteil (KW)
#define TW_KoDimRelativColorKW 11
#define KoTW_DimRelativColorKWIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoDimRelativColorKW)
#define KoTW_DimRelativColorKW knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoDimRelativColorKW)
//!< Number: 16, Text: TW{{argChan}}: {{0:---}}, Function: Status An/Aus
#define TW_KoStatusOnOff 16
#define KoTW_StatusOnOffIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoStatusOnOff)
#define KoTW_StatusOnOff knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoStatusOnOff)
//!< Number: 17, Text: TW{{argChan}}: {{0:---}}, Function: Status Helligkeit
#define TW_KoStatusBrightness 17
#define KoTW_StatusBrightnessIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoStatusBrightness)
#define KoTW_StatusBrightness knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoStatusBrightness)
//!< Number: 18, Text: TW{{argChan}}: {{0:---}}, Function: Status Farbtemperatur (Kelvin)
#define TW_KoStatusColorTemp 18
#define KoTW_StatusColorTempIndex(X) knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * X + TW_KoStatusColorTemp)
#define KoTW_StatusColorTemp knx.getGroupObject(TW_KoOffset + TW_KoBlockSize * channelIndex() + TW_KoStatusColorTemp)

//-----Module: RGB
#define RGB_UseOnColor		0x0000
// Offset: 0, Size: 1 Bit, Text: Einschaltverhalten
#define ParamRGB_UseOnColorIndex(X) knx.paramBit((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_UseOnColor), 0)
// Offset: 0, Size: 1 Bit, Text: Einschaltverhalten
#define ParamRGB_UseOnColor knx.paramBit((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_UseOnColor), 0)
#define RGB_UseNightColor		0x0000
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: Einschaltverhalten - Nacht
#define ParamRGB_UseNightColorIndex(X) knx.paramBit((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_UseNightColor), 1)
// Offset: 0, BitOffset: 1, Size: 1 Bit, Text: Einschaltverhalten - Nacht
#define ParamRGB_UseNightColor knx.paramBit((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_UseNightColor), 1)
#define RGB_OnColor		0x0001
// Offset: 1, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe
#define ParamRGB_OnColorIndex(X) knx.paramData((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_OnColor))
// Offset: 1, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe
#define ParamRGB_OnColor knx.paramData((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_OnColor))
#define RGB_NightColor		0x0004
// Offset: 4, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe (Nacht)
#define ParamRGB_NightColorIndex(X) knx.paramData((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_NightColor))
// Offset: 4, Size: 24 Bit (3 Byte), Text: Einschaltverhalten - Farbe (Nacht)
#define ParamRGB_NightColor knx.paramData((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_NightColor))
#define RGB_RelativDimTime		0x0007
#define RGB_RelativDimTime_Shift	1
#define RGB_RelativDimTime_Mask	0x007F
// Offset: 7, Size: 7 Bit, Text: Zeit
#define ParamRGB_RelativDimTimeIndex(X) ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_RelativDimTime)) >> RGB_RelativDimTime_Shift) & RGB_RelativDimTime_Mask))
// Offset: 7, Size: 7 Bit, Text: Zeit
#define ParamRGB_RelativDimTime ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_RelativDimTime)) >> RGB_RelativDimTime_Shift) & RGB_RelativDimTime_Mask))
#define RGB_RelativDimBase		0x0000
#define RGB_RelativDimBase_Shift	4
#define RGB_RelativDimBase_Mask	0x0003
// Offset: 0, BitOffset: 2, Size: 2 Bit, Text: Zeitbasis
#define ParamRGB_RelativDimBaseIndex(X) ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_RelativDimBase)) >> RGB_RelativDimBase_Shift) & RGB_RelativDimBase_Mask))
// Offset: 0, BitOffset: 2, Size: 2 Bit, Text: Zeitbasis
#define ParamRGB_RelativDimBase ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_RelativDimBase)) >> RGB_RelativDimBase_Shift) & RGB_RelativDimBase_Mask))
#define RGB_OnOffTime		0x0008
#define RGB_OnOffTime_Shift	1
#define RGB_OnOffTime_Mask	0x007F
// Offset: 8, Size: 7 Bit, Text: Zeit
#define ParamRGB_OnOffTimeIndex(X) ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_OnOffTime)) >> RGB_OnOffTime_Shift) & RGB_OnOffTime_Mask))
// Offset: 8, Size: 7 Bit, Text: Zeit
#define ParamRGB_OnOffTime ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_OnOffTime)) >> RGB_OnOffTime_Shift) & RGB_OnOffTime_Mask))
#define RGB_OnOffBase		0x0000
#define RGB_OnOffBase_Shift	2
#define RGB_OnOffBase_Mask	0x0003
// Offset: 0, BitOffset: 4, Size: 2 Bit, Text: Zeitbasis
#define ParamRGB_OnOffBaseIndex(X) ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_OnOffBase)) >> RGB_OnOffBase_Shift) & RGB_OnOffBase_Mask))
// Offset: 0, BitOffset: 4, Size: 2 Bit, Text: Zeitbasis
#define ParamRGB_OnOffBase ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_OnOffBase)) >> RGB_OnOffBase_Shift) & RGB_OnOffBase_Mask))
#define RGB_DimCurve		0x0009
#define RGB_DimCurve_Shift	5
#define RGB_DimCurve_Mask	0x0007
// Offset: 9, Size: 3 Bit, Text: Dimmkurve
#define ParamRGB_DimCurveIndex(X) ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * X + RGB_DimCurve)) >> RGB_DimCurve_Shift) & RGB_DimCurve_Mask))
// Offset: 9, Size: 3 Bit, Text: Dimmkurve
#define ParamRGB_DimCurve ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_DimCurve)) >> RGB_DimCurve_Shift) & RGB_DimCurve_Mask))
//!< Number: 0, Text: RGB{{argChan}}: {{0:---}}, Function: Schalten
#define RGB_KoSwitch 0
#define KoRGB_SwitchIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoSwitch)
#define KoRGB_Switch knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoSwitch)
//!< Number: 2, Text: RGB{{argChan}}: {{0:---}}, Function: Farbeinstellung (RGB)
#define RGB_KoColorRGB 2
#define KoRGB_ColorRGBIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoColorRGB)
#define KoRGB_ColorRGB knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoColorRGB)
//!< Number: 3, Text: RGB{{argChan}}: {{0:---}}, Function: Farbeinstellung (HSV)
#define RGB_KoColorHSV 3
#define KoRGB_ColorHSVIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoColorHSV)
#define KoRGB_ColorHSV knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoColorHSV)
//!< Number: 4, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen absolut (Farbton H)
#define RGB_KoDimAbsoluteShadeH 4
#define KoRGB_DimAbsoluteShadeHIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimAbsoluteShadeH)
#define KoRGB_DimAbsoluteShadeH knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimAbsoluteShadeH)
//!< Number: 5, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen absolut (Sättigung S)
#define RGB_KoDimAbsoluteSaturationS 5
#define KoRGB_DimAbsoluteSaturationSIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimAbsoluteSaturationS)
#define KoRGB_DimAbsoluteSaturationS knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimAbsoluteSaturationS)
//!< Number: 6, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen absolut (Helligkeit V)
#define RGB_KoDimAbsoluteBrightnessV 6
#define KoRGB_DimAbsoluteBrightnessVIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimAbsoluteBrightnessV)
#define KoRGB_DimAbsoluteBrightnessV knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimAbsoluteBrightnessV)
//!< Number: 7, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen absolut (Rot)
#define RGB_KoDimAbsoluteR 7
#define KoRGB_DimAbsoluteRIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimAbsoluteR)
#define KoRGB_DimAbsoluteR knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimAbsoluteR)
//!< Number: 8, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen absolut (Grün)
#define RGB_KoDimAbsoluteG 8
#define KoRGB_DimAbsoluteGIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimAbsoluteG)
#define KoRGB_DimAbsoluteG knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimAbsoluteG)
//!< Number: 9, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen absolut (Blau)
#define RGB_KoDimAbsoluteB 9
#define KoRGB_DimAbsoluteBIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimAbsoluteB)
#define KoRGB_DimAbsoluteB knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimAbsoluteB)
//!< Number: 10, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen relativ (Farbton H)
#define RGB_KoDimRelativShadeH 10
#define KoRGB_DimRelativShadeHIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimRelativShadeH)
#define KoRGB_DimRelativShadeH knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimRelativShadeH)
//!< Number: 11, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen relativ (Sättigung S)
#define RGB_KoDimRelativSaturationS 11
#define KoRGB_DimRelativSaturationSIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimRelativSaturationS)
#define KoRGB_DimRelativSaturationS knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimRelativSaturationS)
//!< Number: 12, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen relativ (Helligkeit V)
#define RGB_KoDimRelativBrightnessV 12
#define KoRGB_DimRelativBrightnessVIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimRelativBrightnessV)
#define KoRGB_DimRelativBrightnessV knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimRelativBrightnessV)
//!< Number: 13, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen relativ (Rot)
#define RGB_KoDimRelativR 13
#define KoRGB_DimRelativRIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimRelativR)
#define KoRGB_DimRelativR knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimRelativR)
//!< Number: 14, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen relativ (Grün)
#define RGB_KoDimRelativG 14
#define KoRGB_DimRelativGIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimRelativG)
#define KoRGB_DimRelativG knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimRelativG)
//!< Number: 15, Text: RGB{{argChan}}: {{0:---}}, Function: Dimmen relativ (Blau)
#define RGB_KoDimRelativB 15
#define KoRGB_DimRelativBIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoDimRelativB)
#define KoRGB_DimRelativB knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoDimRelativB)
//!< Number: 16, Text: RGB{{argChan}}: {{0:---}}, Function: Staus Ein/Aus
#define RGB_KoStatusOnOff 16
#define KoRGB_StatusOnOffIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusOnOff)
#define KoRGB_StatusOnOff knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusOnOff)
//!< Number: 17, Text: RGB{{argChan}}: {{0:---}}, Function: Status Dimmwert (RGB)
#define RGB_KoStatusColorRGB 17
#define KoRGB_StatusColorRGBIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusColorRGB)
#define KoRGB_StatusColorRGB knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusColorRGB)
//!< Number: 18, Text: RGB{{argChan}}: {{0:---}}, Function: Status Dimmwert (HSV)
#define RGB_KoStatusColorHSV 18
#define KoRGB_StatusColorHSVIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusColorHSV)
#define KoRGB_StatusColorHSV knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusColorHSV)
//!< Number: 19, Text: RGB{{argChan}}: {{0:---}}, Function: Status Dimmwert (Farbton H)
#define RGB_KoStatusShadeH 19
#define KoRGB_StatusShadeHIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusShadeH)
#define KoRGB_StatusShadeH knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusShadeH)
//!< Number: 20, Text: RGB{{argChan}}: {{0:---}}, Function: Status Dimmwert (Sättigung S)
#define RGB_KoStatusSaturationS 20
#define KoRGB_StatusSaturationSIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusSaturationS)
#define KoRGB_StatusSaturationS knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusSaturationS)
//!< Number: 21, Text: RGB{{argChan}}: {{0:---}}, Function: Status Dimmwert (Helligkeit V)
#define RGB_KoStatusBrightnessV 21
#define KoRGB_StatusBrightnessVIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusBrightnessV)
#define KoRGB_StatusBrightnessV knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusBrightnessV)
//!< Number: 22, Text: RGB{{argChan}}: {{0:---}}, Function: Status Farbe (Rot)
#define RGB_KoStatusColorR 22
#define KoRGB_StatusColorRIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusColorR)
#define KoRGB_StatusColorR knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusColorR)
//!< Number: 23, Text: RGB{{argChan}}: {{0:---}}, Function: Status Farbe (Grün)
#define RGB_KoStatusColorG 23
#define KoRGB_StatusColorGIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusColorG)
#define KoRGB_StatusColorG knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusColorG)
//!< Number: 24, Text: RGB{{argChan}}: {{0:---}}, Function: Status Farbe (Blau)
#define RGB_KoStatusColorB 24
#define KoRGB_StatusColorBIndex(X) knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * X + RGB_KoStatusColorB)
#define KoRGB_StatusColorB knx.getGroupObject(RGB_KoOffset + RGB_KoBlockSize * channelIndex() + RGB_KoStatusColorB)

