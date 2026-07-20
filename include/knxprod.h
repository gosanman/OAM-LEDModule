#pragma once


#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                         (time & 0x3FFF) * 3600000 ) : 0 )
                                             
#define ETS_ModuleId_NONE 0
#define ETS_ModuleId_BASE 1
#define ETS_ModuleId_UCT 2
#define MAIN_FirmwareName "LED Dimmer"
#define MAIN_OpenKnxId 0xA8
#define MAIN_ApplicationNumber 1
#define MAIN_ApplicationVersion 4
#define MAIN_FirmwareRevision 0
#define MAIN_ApplicationEncoding iso-8859-15
#define MAIN_ParameterSize 814
#define MAIN_MaxKoNumber 678
#define MAIN_OrderNumber "OpenKnxLEDDimmer"
#define BASE_ModuleVersion 21
#define UCT_ModuleVersion 5
// Parameter with single occurrence


#define LED_ControllerType                       0      // 2 Bits, Bit 7-6
#define     LED_ControllerTypeMask 0xC0
#define     LED_ControllerTypeShift 6
#define LED_OperatingMode                        0      // 6 Bits, Bit 5-0
#define     LED_OperatingModeMask 0x3F
#define     LED_OperatingModeShift 0
#define LED_PwmFrequenz                          1      // 11 Bits, Bit 15-5
#define     LED_PwmFrequenzMask 0xFFE0
#define     LED_PwmFrequenzShift 5
#define LED_SendMeasuredValues                   3      // 1 Bit, Bit 7
#define     LED_SendMeasuredValuesMask 0x80
#define     LED_SendMeasuredValuesShift 7
#define LED_TempSensorPresent                    3      // 1 Bit, Bit 6
#define     LED_TempSensorPresentMask 0x40
#define     LED_TempSensorPresentShift 6
#define LED_MeasurementIntervalTime              4      // 14 Bits, Bit 15-2
#define     LED_MeasurementIntervalTimeMask 0xFFFC
#define     LED_MeasurementIntervalTimeShift 2
#define LED_MeasurementIntervalBase              3      // 2 Bits, Bit 5-4
#define     LED_MeasurementIntervalBaseMask 0x30
#define     LED_MeasurementIntervalBaseShift 4
#define LED_ShuntValue                           3      // 4 Bits, Bit 3-0
#define     LED_ShuntValueMask 0x0F
#define     LED_ShuntValueShift 0
#define LED_DayNight                             6      // 1 Bit, Bit 7
#define     LED_DayNightMask 0x80
#define     LED_DayNightShift 7
#define LED_Func1BtnClick                        6      // 4 Bits, Bit 6-3
#define     LED_Func1BtnClickMask 0x78
#define     LED_Func1BtnClickShift 3
#define LED_Func1BtnDblClick                     7      // 4 Bits, Bit 7-4
#define     LED_Func1BtnDblClickMask 0xF0
#define     LED_Func1BtnDblClickShift 4
#define LED_Func1BtnLongClick                    7      // 4 Bits, Bit 3-0
#define     LED_Func1BtnLongClickMask 0x0F
#define     LED_Func1BtnLongClickShift 0
#define LED_AlarmUseVoltage                      6      // 1 Bit, Bit 2
#define     LED_AlarmUseVoltageMask 0x04
#define     LED_AlarmUseVoltageShift 2
#define LED_AlarmUseOverTemp                     6      // 1 Bit, Bit 1
#define     LED_AlarmUseOverTempMask 0x02
#define     LED_AlarmUseOverTempShift 1
#define LED_AlarmUseOverCurrent                  6      // 1 Bit, Bit 0
#define     LED_AlarmUseOverCurrentMask 0x01
#define     LED_AlarmUseOverCurrentShift 0
#define LED_AlarmOverVoltage                     8      // float (2 Byte)
#define LED_AlarmUnderVoltage                   10      // float (2 Byte)
#define LED_AlarmOverTemp                       12      // float (2 Byte)
#define LED_AlarmOverCurrent                    14      // float (2 Byte)
#define LED_DisplayTimeOut                      16      // 10 Bits, Bit 15-6
#define     LED_DisplayTimeOutMask 0xFFC0
#define     LED_DisplayTimeOutShift 6
#define LED_FrontPanelPresent                   18      // 1 Bit, Bit 7
#define     LED_FrontPanelPresentMask 0x80
#define     LED_FrontPanelPresentShift 7
#define LED_FrontPanelControl                   18      // 1 Bit, Bit 6
#define     LED_FrontPanelControlMask 0x40
#define     LED_FrontPanelControlShift 6

// Gerät
#define ParamLED_ControllerType                      ((knx.paramByte(LED_ControllerType) & LED_ControllerTypeMask) >> LED_ControllerTypeShift)
// Betriebsmodus
#define ParamLED_OperatingMode                       (knx.paramByte(LED_OperatingMode) & LED_OperatingModeMask)
// PWM Frequenz
#define ParamLED_PwmFrequenz                         ((knx.paramWord(LED_PwmFrequenz) & LED_PwmFrequenzMask) >> LED_PwmFrequenzShift)
// Messwerte zyklisch senden
#define ParamLED_SendMeasuredValues                  ((bool)(knx.paramByte(LED_SendMeasuredValues) & LED_SendMeasuredValuesMask))
// Temperatursensor verbaut
#define ParamLED_TempSensorPresent                   ((bool)(knx.paramByte(LED_TempSensorPresent) & LED_TempSensorPresentMask))
// Zeit
#define ParamLED_MeasurementIntervalTime             ((knx.paramWord(LED_MeasurementIntervalTime) & LED_MeasurementIntervalTimeMask) >> LED_MeasurementIntervalTimeShift)
// Zeit (in Millisekunden)
#define ParamLED_MeasurementIntervalTimeMS           (paramDelay(knx.paramWord(LED_MeasurementIntervalTime)))
// Zeitbasis
#define ParamLED_MeasurementIntervalBase             ((knx.paramByte(LED_MeasurementIntervalBase) & LED_MeasurementIntervalBaseMask) >> LED_MeasurementIntervalBaseShift)
// Wert des verbauten Messwiderstand
#define ParamLED_ShuntValue                          (knx.paramByte(LED_ShuntValue) & LED_ShuntValueMask)
// Tag/Nacht Objekt
#define ParamLED_DayNight                            ((bool)(knx.paramByte(LED_DayNight) & LED_DayNightMask))
// Aktion - Func1 Button Klick
#define ParamLED_Func1BtnClick                       ((knx.paramByte(LED_Func1BtnClick) & LED_Func1BtnClickMask) >> LED_Func1BtnClickShift)
// Aktion - Func1 Button Doppelklick
#define ParamLED_Func1BtnDblClick                    ((knx.paramByte(LED_Func1BtnDblClick) & LED_Func1BtnDblClickMask) >> LED_Func1BtnDblClickShift)
// Aktion - Func1 Button Langer Klick
#define ParamLED_Func1BtnLongClick                   (knx.paramByte(LED_Func1BtnLongClick) & LED_Func1BtnLongClickMask)
// Spannungsbereich überwachen
#define ParamLED_AlarmUseVoltage                     ((bool)(knx.paramByte(LED_AlarmUseVoltage) & LED_AlarmUseVoltageMask))
// Übertemperatur überwachen
#define ParamLED_AlarmUseOverTemp                    ((bool)(knx.paramByte(LED_AlarmUseOverTemp) & LED_AlarmUseOverTempMask))
// Überstrom überwachen
#define ParamLED_AlarmUseOverCurrent                 ((bool)(knx.paramByte(LED_AlarmUseOverCurrent) & LED_AlarmUseOverCurrentMask))
// Grenzwert Überspannung
#define ParamLED_AlarmOverVoltage                    (knx.paramFloat(LED_AlarmOverVoltage, Float_Enc_DPT9))
// Grenzwert Unterpannung
#define ParamLED_AlarmUnderVoltage                   (knx.paramFloat(LED_AlarmUnderVoltage, Float_Enc_DPT9))
// Temperaturabschaltung bei
#define ParamLED_AlarmOverTemp                       (knx.paramFloat(LED_AlarmOverTemp, Float_Enc_DPT9))
// Grenzwert Überstrom
#define ParamLED_AlarmOverCurrent                    (knx.paramFloat(LED_AlarmOverCurrent, Float_Enc_DPT9))
// Anzeige ausschalten nach
#define ParamLED_DisplayTimeOut                      ((knx.paramWord(LED_DisplayTimeOut) & LED_DisplayTimeOutMask) >> LED_DisplayTimeOutShift)
// Bedienfeld verbaut
#define ParamLED_FrontPanelPresent                   ((bool)(knx.paramByte(LED_FrontPanelPresent) & LED_FrontPanelPresentMask))
// Steuerung über das Bedienfeld zulassen
#define ParamLED_FrontPanelControl                   ((bool)(knx.paramByte(LED_FrontPanelControl) & LED_FrontPanelControlMask))

#define LED_KoVoltageV 31
#define LED_KoVoltageVext 32
#define LED_KoCurrentA 33
#define LED_KoCurrentAext 34
#define LED_KoPowerW 35
#define LED_KoPowerWext 36
#define LED_KoActivePowerWh 37
#define LED_KoActivePowerWhext 38
#define LED_KoTempC 39
#define LED_KoDayNight 40
#define LED_KoAlarmOverVoltage 45
#define LED_KoAlarmUnderVoltage 46
#define LED_KoAlarmOverTemp 47
#define LED_KoAlarmOverCurrent 48

// Spannung
#define KoLED_VoltageV                            (knx.getGroupObject(LED_KoVoltageV))
// Spannung Ext
#define KoLED_VoltageVext                         (knx.getGroupObject(LED_KoVoltageVext))
// Strom
#define KoLED_CurrentA                            (knx.getGroupObject(LED_KoCurrentA))
// Strom Ext
#define KoLED_CurrentAext                         (knx.getGroupObject(LED_KoCurrentAext))
// Wirkleistung
#define KoLED_PowerW                              (knx.getGroupObject(LED_KoPowerW))
// Wirkleistung Ext
#define KoLED_PowerWext                           (knx.getGroupObject(LED_KoPowerWext))
// Wirkleistung (Wh)
#define KoLED_ActivePowerWh                       (knx.getGroupObject(LED_KoActivePowerWh))
// Wirkleistung (Wh) Ext
#define KoLED_ActivePowerWhext                    (knx.getGroupObject(LED_KoActivePowerWhext))
// Gerätetemperatur
#define KoLED_TempC                               (knx.getGroupObject(LED_KoTempC))
// Tag/Nacht
#define KoLED_DayNight                            (knx.getGroupObject(LED_KoDayNight))
// Überspannung
#define KoLED_AlarmOverVoltage                    (knx.getGroupObject(LED_KoAlarmOverVoltage))
// Unterspannung
#define KoLED_AlarmUnderVoltage                   (knx.getGroupObject(LED_KoAlarmUnderVoltage))
// Übertemperatur
#define KoLED_AlarmOverTemp                       (knx.getGroupObject(LED_KoAlarmOverTemp))
// Überstrom
#define KoLED_AlarmOverCurrent                    (knx.getGroupObject(LED_KoAlarmOverCurrent))

#define BASE_StartupDelayBase                    19      // 2 Bits, Bit 7-6
#define     BASE_StartupDelayBaseMask 0xC0
#define     BASE_StartupDelayBaseShift 6
#define BASE_StartupDelayTime                    19      // 14 Bits, Bit 13-0
#define     BASE_StartupDelayTimeMask 0x3FFF
#define     BASE_StartupDelayTimeShift 0
#define BASE_HeartbeatDelayBase                  21      // 2 Bits, Bit 7-6
#define     BASE_HeartbeatDelayBaseMask 0xC0
#define     BASE_HeartbeatDelayBaseShift 6
#define BASE_HeartbeatDelayTime                  21      // 14 Bits, Bit 13-0
#define     BASE_HeartbeatDelayTimeMask 0x3FFF
#define     BASE_HeartbeatDelayTimeShift 0
#define BASE_Timezone                            23      // 5 Bits, Bit 7-3
#define     BASE_TimezoneMask 0xF8
#define     BASE_TimezoneShift 3
#define BASE_CombinedTimeDate                    23      // 1 Bit, Bit 2
#define     BASE_CombinedTimeDateMask 0x04
#define     BASE_CombinedTimeDateShift 2
#define BASE_SummertimeAll                       23      // 2 Bits, Bit 1-0
#define     BASE_SummertimeAllMask 0x03
#define     BASE_SummertimeAllShift 0
#define BASE_SummertimeDE                        23      // 2 Bits, Bit 1-0
#define     BASE_SummertimeDEMask 0x03
#define     BASE_SummertimeDEShift 0
#define BASE_SummertimeWorld                     23      // 2 Bits, Bit 1-0
#define     BASE_SummertimeWorldMask 0x03
#define     BASE_SummertimeWorldShift 0
#define BASE_SummertimeKO                        23      // 2 Bits, Bit 1-0
#define     BASE_SummertimeKOMask 0x03
#define     BASE_SummertimeKOShift 0
#define BASE_TimezoneCustom                      24      // char*, 63 Byte
#define     BASE_TimezoneCustomLength 63
#define BASE_Latitude                            88      // float (4 Byte)
#define BASE_Longitude                           92      // float (4 Byte)
#define BASE_Diagnose                            97      // 1 Bit, Bit 7
#define     BASE_DiagnoseMask 0x80
#define     BASE_DiagnoseShift 7
#define BASE_Watchdog                            97      // 1 Bit, Bit 6
#define     BASE_WatchdogMask 0x40
#define     BASE_WatchdogShift 6
#define BASE_ReadTimeDate                        97      // 1 Bit, Bit 5
#define     BASE_ReadTimeDateMask 0x20
#define     BASE_ReadTimeDateShift 5
#define BASE_HeartbeatExtended                   97      // 1 Bit, Bit 4
#define     BASE_HeartbeatExtendedMask 0x10
#define     BASE_HeartbeatExtendedShift 4
#define BASE_InternalTime                        97      // 1 Bit, Bit 3
#define     BASE_InternalTimeMask 0x08
#define     BASE_InternalTimeShift 3
#define BASE_ManualSave                          97      // 3 Bits, Bit 2-0
#define     BASE_ManualSaveMask 0x07
#define     BASE_ManualSaveShift 0
#define BASE_PeriodicSave                        98      // 8 Bits, Bit 7-0
#define BASE_Dummy                               128      // uint8_t
#define BASE_ModuleEnabled_UCT                   129      // 1 Bit, Bit 6
#define     BASE_ModuleEnabled_UCTMask 0x40
#define     BASE_ModuleEnabled_UCTShift 6

// Zeitbasis
#define ParamBASE_StartupDelayBase                    ((knx.paramByte(BASE_StartupDelayBase) & BASE_StartupDelayBaseMask) >> BASE_StartupDelayBaseShift)
// Zeit
#define ParamBASE_StartupDelayTime                    (knx.paramWord(BASE_StartupDelayTime) & BASE_StartupDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_StartupDelayTimeMS                  (paramDelay(knx.paramWord(BASE_StartupDelayTime)))
// Zeitbasis
#define ParamBASE_HeartbeatDelayBase                  ((knx.paramByte(BASE_HeartbeatDelayBase) & BASE_HeartbeatDelayBaseMask) >> BASE_HeartbeatDelayBaseShift)
// Zeit
#define ParamBASE_HeartbeatDelayTime                  (knx.paramWord(BASE_HeartbeatDelayTime) & BASE_HeartbeatDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_HeartbeatDelayTimeMS                (paramDelay(knx.paramWord(BASE_HeartbeatDelayTime)))
// Zeitzone
#define ParamBASE_Timezone                            ((knx.paramByte(BASE_Timezone) & BASE_TimezoneMask) >> BASE_TimezoneShift)
// Empfangen über
#define ParamBASE_CombinedTimeDate                    ((bool)(knx.paramByte(BASE_CombinedTimeDate) & BASE_CombinedTimeDateMask))
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeAll                       (knx.paramByte(BASE_SummertimeAll) & BASE_SummertimeAllMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeDE                        (knx.paramByte(BASE_SummertimeDE) & BASE_SummertimeDEMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeWorld                     (knx.paramByte(BASE_SummertimeWorld) & BASE_SummertimeWorldMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeKO                        (knx.paramByte(BASE_SummertimeKO) & BASE_SummertimeKOMask)
// POSIX TZ-String
#define ParamBASE_TimezoneCustom                      (knx.paramData(BASE_TimezoneCustom))
#define ParamBASE_TimezoneCustomStr                   (knx.paramString(BASE_TimezoneCustom, BASE_TimezoneCustomLength))
// Breitengrad
#define ParamBASE_Latitude                            (knx.paramFloat(BASE_Latitude, Float_Enc_IEEE754Single))
// Längengrad
#define ParamBASE_Longitude                           (knx.paramFloat(BASE_Longitude, Float_Enc_IEEE754Single))
// Diagnoseobjekt anzeigen
#define ParamBASE_Diagnose                            ((bool)(knx.paramByte(BASE_Diagnose) & BASE_DiagnoseMask))
// Watchdog aktivieren
#define ParamBASE_Watchdog                            ((bool)(knx.paramByte(BASE_Watchdog) & BASE_WatchdogMask))
// Bei Neustart vom Bus lesen
#define ParamBASE_ReadTimeDate                        ((bool)(knx.paramByte(BASE_ReadTimeDate) & BASE_ReadTimeDateMask))
// Erweitertes "In Betrieb"
#define ParamBASE_HeartbeatExtended                   ((bool)(knx.paramByte(BASE_HeartbeatExtended) & BASE_HeartbeatExtendedMask))
// InternalTime
#define ParamBASE_InternalTime                        ((bool)(knx.paramByte(BASE_InternalTime) & BASE_InternalTimeMask))
// Manuelles speichern
#define ParamBASE_ManualSave                          (knx.paramByte(BASE_ManualSave) & BASE_ManualSaveMask)
// Zyklisches speichern
#define ParamBASE_PeriodicSave                        (knx.paramByte(BASE_PeriodicSave))
// 
#define ParamBASE_Dummy                               (knx.paramByte(BASE_Dummy))
// UCT
#define ParamBASE_ModuleEnabled_UCT                   ((bool)(knx.paramByte(BASE_ModuleEnabled_UCT) & BASE_ModuleEnabled_UCTMask))

#define BASE_KoHeartbeat 1
#define BASE_KoTime 2
#define BASE_KoDate 3
#define BASE_KoDateTime 4
#define BASE_KoIsSummertime 5
#define BASE_KoManualSave 6
#define BASE_KoDiagnose 7

// In Betrieb
#define KoBASE_Heartbeat                           (knx.getGroupObject(BASE_KoHeartbeat))
// Uhrzeit
#define KoBASE_Time                                (knx.getGroupObject(BASE_KoTime))
// Datum
#define KoBASE_Date                                (knx.getGroupObject(BASE_KoDate))
// Uhrzeit/Datum
#define KoBASE_DateTime                            (knx.getGroupObject(BASE_KoDateTime))
// Sommerzeit aktiv
#define KoBASE_IsSummertime                        (knx.getGroupObject(BASE_KoIsSummertime))
// Speichern
#define KoBASE_ManualSave                          (knx.getGroupObject(BASE_KoManualSave))
// Diagnose
#define KoBASE_Diagnose                            (knx.getGroupObject(BASE_KoDiagnose))



#define EK_ChannelCount 12

// Parameter per channel
#define EK_ParamBlockOffset 133
#define EK_ParamBlockSize 21
#define EK_ParamCalcIndex(index) (index + EK_ParamBlockOffset + _channelIndex * EK_ParamBlockSize)

#define EK_UseOnValue                           0      // 1 Bit, Bit 7
#define     EK_UseOnValueMask 0x80
#define     EK_UseOnValueShift 7
#define EK_UseNightValue                        0      // 1 Bit, Bit 6
#define     EK_UseNightValueMask 0x40
#define     EK_UseNightValueShift 6
#define EK_OnBrightness                         1      // 7 Bits, Bit 7-1
#define     EK_OnBrightnessMask 0xFE
#define     EK_OnBrightnessShift 1
#define EK_NightBrightness                      2      // 7 Bits, Bit 7-1
#define     EK_NightBrightnessMask 0xFE
#define     EK_NightBrightnessShift 1
#define EK_RelativDimTime                       3      // 7 Bits, Bit 7-1
#define     EK_RelativDimTimeMask 0xFE
#define     EK_RelativDimTimeShift 1
#define EK_RelativDimBase                       0      // 2 Bits, Bit 5-4
#define     EK_RelativDimBaseMask 0x30
#define     EK_RelativDimBaseShift 4
#define EK_OnOffTime                            4      // 7 Bits, Bit 7-1
#define     EK_OnOffTimeMask 0xFE
#define     EK_OnOffTimeShift 1
#define EK_OnOffBase                            0      // 2 Bits, Bit 3-2
#define     EK_OnOffBaseMask 0x0C
#define     EK_OnOffBaseShift 2
#define EK_DimCurve                             5      // 3 Bits, Bit 7-5
#define     EK_DimCurveMask 0xE0
#define     EK_DimCurveShift 5
#define EK_SceneNumberA                         6      // int8_t
#define EK_SceneNumberB                         7      // int8_t
#define EK_SceneNumberC                         8      // int8_t
#define EK_SceneNumberD                         9      // int8_t
#define EK_SceneNumberE                        10      // int8_t
#define EK_SceneActionA                        11      // 8 Bits, Bit 7-0
#define EK_SceneActionB                        12      // 8 Bits, Bit 7-0
#define EK_SceneActionC                        13      // 8 Bits, Bit 7-0
#define EK_SceneActionD                        14      // 8 Bits, Bit 7-0
#define EK_SceneActionE                        15      // 8 Bits, Bit 7-0
#define EK_SceneBrightnessA                    16      // uint8_t
#define EK_SceneBrightnessB                    17      // uint8_t
#define EK_SceneBrightnessC                    18      // uint8_t
#define EK_SceneBrightnessD                    19      // uint8_t
#define EK_SceneBrightnessE                    20      // uint8_t
#define EK_hclChannel                           0      // 2 Bits, Bit 1-0
#define     EK_hclChannelMask 0x03
#define     EK_hclChannelShift 0
#define EK_hclStart                             1      // 1 Bit, Bit 0
#define     EK_hclStartMask 0x01
#define     EK_hclStartShift 0
#define EK_hclActive                            2      // 1 Bit, Bit 0
#define     EK_hclActiveMask 0x01
#define     EK_hclActiveShift 0
#define EK_hclCheckBrightness                   3      // 1 Bit, Bit 0
#define     EK_hclCheckBrightnessMask 0x01
#define     EK_hclCheckBrightnessShift 0

// Einschaltverhalten
#define ParamEK_UseOnValue                          ((bool)(knx.paramByte(EK_ParamCalcIndex(EK_UseOnValue)) & EK_UseOnValueMask))
// Einschaltverhalten - Nacht
#define ParamEK_UseNightValue                       ((bool)(knx.paramByte(EK_ParamCalcIndex(EK_UseNightValue)) & EK_UseNightValueMask))
// Einschaltverhalten - Helligkeit
#define ParamEK_OnBrightness                        ((knx.paramByte(EK_ParamCalcIndex(EK_OnBrightness)) & EK_OnBrightnessMask) >> EK_OnBrightnessShift)
// Einschaltverhalten - Helligkeit (Nacht)
#define ParamEK_NightBrightness                     ((knx.paramByte(EK_ParamCalcIndex(EK_NightBrightness)) & EK_NightBrightnessMask) >> EK_NightBrightnessShift)
// Zeit
#define ParamEK_RelativDimTime                      ((knx.paramByte(EK_ParamCalcIndex(EK_RelativDimTime)) & EK_RelativDimTimeMask) >> EK_RelativDimTimeShift)
// Zeitbasis
#define ParamEK_RelativDimBase                      ((knx.paramByte(EK_ParamCalcIndex(EK_RelativDimBase)) & EK_RelativDimBaseMask) >> EK_RelativDimBaseShift)
// Zeit
#define ParamEK_OnOffTime                           ((knx.paramByte(EK_ParamCalcIndex(EK_OnOffTime)) & EK_OnOffTimeMask) >> EK_OnOffTimeShift)
// Zeitbasis
#define ParamEK_OnOffBase                           ((knx.paramByte(EK_ParamCalcIndex(EK_OnOffBase)) & EK_OnOffBaseMask) >> EK_OnOffBaseShift)
// Dimmkurve
#define ParamEK_DimCurve                            ((knx.paramByte(EK_ParamCalcIndex(EK_DimCurve)) & EK_DimCurveMask) >> EK_DimCurveShift)
// 
#define ParamEK_SceneNumberA                        ((int8_t)knx.paramByte(EK_ParamCalcIndex(EK_SceneNumberA)))
// 
#define ParamEK_SceneNumberB                        ((int8_t)knx.paramByte(EK_ParamCalcIndex(EK_SceneNumberB)))
// 
#define ParamEK_SceneNumberC                        ((int8_t)knx.paramByte(EK_ParamCalcIndex(EK_SceneNumberC)))
// 
#define ParamEK_SceneNumberD                        ((int8_t)knx.paramByte(EK_ParamCalcIndex(EK_SceneNumberD)))
// 
#define ParamEK_SceneNumberE                        ((int8_t)knx.paramByte(EK_ParamCalcIndex(EK_SceneNumberE)))
// 
#define ParamEK_SceneActionA                        (knx.paramByte(EK_ParamCalcIndex(EK_SceneActionA)))
// 
#define ParamEK_SceneActionB                        (knx.paramByte(EK_ParamCalcIndex(EK_SceneActionB)))
// 
#define ParamEK_SceneActionC                        (knx.paramByte(EK_ParamCalcIndex(EK_SceneActionC)))
// 
#define ParamEK_SceneActionD                        (knx.paramByte(EK_ParamCalcIndex(EK_SceneActionD)))
// 
#define ParamEK_SceneActionE                        (knx.paramByte(EK_ParamCalcIndex(EK_SceneActionE)))
// 
#define ParamEK_SceneBrightnessA                    (knx.paramByte(EK_ParamCalcIndex(EK_SceneBrightnessA)))
// 
#define ParamEK_SceneBrightnessB                    (knx.paramByte(EK_ParamCalcIndex(EK_SceneBrightnessB)))
// 
#define ParamEK_SceneBrightnessC                    (knx.paramByte(EK_ParamCalcIndex(EK_SceneBrightnessC)))
// 
#define ParamEK_SceneBrightnessD                    (knx.paramByte(EK_ParamCalcIndex(EK_SceneBrightnessD)))
// 
#define ParamEK_SceneBrightnessE                    (knx.paramByte(EK_ParamCalcIndex(EK_SceneBrightnessE)))
// Verwende
#define ParamEK_hclChannel                          (knx.paramByte(EK_ParamCalcIndex(EK_hclChannel)) & EK_hclChannelMask)
// HCL anwenden
#define ParamEK_hclStart                            ((bool)(knx.paramByte(EK_ParamCalcIndex(EK_hclStart)) & EK_hclStartMask))
// HCL für diesen Kanal aktivieren
#define ParamEK_hclActive                           ((bool)(knx.paramByte(EK_ParamCalcIndex(EK_hclActive)) & EK_hclActiveMask))
// Helligkeit aktivieren
#define ParamEK_hclCheckBrightness                  ((bool)(knx.paramByte(EK_ParamCalcIndex(EK_hclCheckBrightness)) & EK_hclCheckBrightnessMask))

// deprecated
#define EK_KoOffset 57

// Communication objects per channel (multiple occurrence)
#define EK_KoBlockOffset 57
#define EK_KoBlockSize 28

#define EK_KoCalcNumber(index) (index + EK_KoBlockOffset + _channelIndex * EK_KoBlockSize)
#define EK_KoCalcIndex(number) ((number >= EK_KoCalcNumber(0) && number < EK_KoCalcNumber(EK_KoBlockSize)) ? (number - EK_KoBlockOffset) % EK_KoBlockSize : -1)
#define EK_KoCalcChannel(number) ((number >= EK_KoBlockOffset && number < EK_KoBlockOffset + EK_ChannelCount * EK_KoBlockSize) ? (number - EK_KoBlockOffset) / EK_KoBlockSize : -1)

#define EK_KoSwitch 0
#define EK_KoDimAbsolute 2
#define EK_KoDimRelativ 10
#define EK_KoStatusOnOff 16
#define EK_KoStatusBrightness 17
#define EK_KoSceneNumber 27

// EK%C%:
#define KoEK_Switch                              (knx.getGroupObject(EK_KoCalcNumber(EK_KoSwitch)))
// EK%C%:
#define KoEK_DimAbsolute                         (knx.getGroupObject(EK_KoCalcNumber(EK_KoDimAbsolute)))
// EK%C%:
#define KoEK_DimRelativ                          (knx.getGroupObject(EK_KoCalcNumber(EK_KoDimRelativ)))
// EK%C%:
#define KoEK_StatusOnOff                         (knx.getGroupObject(EK_KoCalcNumber(EK_KoStatusOnOff)))
// EK%C%:
#define KoEK_StatusBrightness                    (knx.getGroupObject(EK_KoCalcNumber(EK_KoStatusBrightness)))
// EK%C%:
#define KoEK_SceneNumber                         (knx.getGroupObject(EK_KoCalcNumber(EK_KoSceneNumber)))

#define TW_ChannelCount 6

// Parameter per channel
#define TW_ParamBlockOffset 385
#define TW_ParamBlockSize 39
#define TW_ParamCalcIndex(index) (index + TW_ParamBlockOffset + _channelIndex * TW_ParamBlockSize)

#define TW_ColorTempWW                          0      // 12 Bits, Bit 15-4
#define     TW_ColorTempWWMask 0xFFF0
#define     TW_ColorTempWWShift 4
#define TW_ColorTempKW                          2      // 13 Bits, Bit 15-3
#define     TW_ColorTempKWMask 0xFFF8
#define     TW_ColorTempKWShift 3
#define TW_UseOnValue                           4      // 1 Bit, Bit 7
#define     TW_UseOnValueMask 0x80
#define     TW_UseOnValueShift 7
#define TW_UseNightValue                        4      // 1 Bit, Bit 6
#define     TW_UseNightValueMask 0x40
#define     TW_UseNightValueShift 6
#define TW_OnBrightness                         5      // 7 Bits, Bit 7-1
#define     TW_OnBrightnessMask 0xFE
#define     TW_OnBrightnessShift 1
#define TW_NightBrightness                      6      // 7 Bits, Bit 7-1
#define     TW_NightBrightnessMask 0xFE
#define     TW_NightBrightnessShift 1
#define TW_OnColorTemp                          7      // uint16_t
#define TW_NightColorTemp                       9      // uint16_t
#define TW_RelativDimTime                      11      // 7 Bits, Bit 7-1
#define     TW_RelativDimTimeMask 0xFE
#define     TW_RelativDimTimeShift 1
#define TW_RelativDimBase                       4      // 2 Bits, Bit 5-4
#define     TW_RelativDimBaseMask 0x30
#define     TW_RelativDimBaseShift 4
#define TW_OnOffTime                           12      // 7 Bits, Bit 7-1
#define     TW_OnOffTimeMask 0xFE
#define     TW_OnOffTimeShift 1
#define TW_OnOffBase                            4      // 2 Bits, Bit 3-2
#define     TW_OnOffBaseMask 0x0C
#define     TW_OnOffBaseShift 2
#define TW_DimCurve                            13      // 3 Bits, Bit 7-5
#define     TW_DimCurveMask 0xE0
#define     TW_DimCurveShift 5
#define TW_SceneNumberA                        14      // int8_t
#define TW_SceneNumberB                        15      // int8_t
#define TW_SceneNumberC                        16      // int8_t
#define TW_SceneNumberD                        17      // int8_t
#define TW_SceneNumberE                        18      // int8_t
#define TW_SceneActionA                        19      // 8 Bits, Bit 7-0
#define TW_SceneActionB                        20      // 8 Bits, Bit 7-0
#define TW_SceneActionC                        21      // 8 Bits, Bit 7-0
#define TW_SceneActionD                        22      // 8 Bits, Bit 7-0
#define TW_SceneActionE                        23      // 8 Bits, Bit 7-0
#define TW_SceneBrightnessA                    24      // uint8_t
#define TW_SceneBrightnessB                    25      // uint8_t
#define TW_SceneBrightnessC                    26      // uint8_t
#define TW_SceneBrightnessD                    27      // uint8_t
#define TW_SceneBrightnessE                    28      // uint8_t
#define TW_SceneKelvinA                        29      // uint16_t
#define TW_SceneKelvinB                        31      // uint16_t
#define TW_SceneKelvinC                        33      // uint16_t
#define TW_SceneKelvinD                        35      // uint16_t
#define TW_SceneKelvinE                        37      // uint16_t
#define TW_hclChannel                           4      // 2 Bits, Bit 1-0
#define     TW_hclChannelMask 0x03
#define     TW_hclChannelShift 0
#define TW_hclStart                             5      // 1 Bit, Bit 0
#define     TW_hclStartMask 0x01
#define     TW_hclStartShift 0
#define TW_hclActive                            6      // 1 Bit, Bit 0
#define     TW_hclActiveMask 0x01
#define     TW_hclActiveShift 0
#define TW_hclCheckTemperature                 11      // 1 Bit, Bit 0
#define     TW_hclCheckTemperatureMask 0x01
#define     TW_hclCheckTemperatureShift 0
#define TW_hclCheckBrightness                  12      // 1 Bit, Bit 0
#define     TW_hclCheckBrightnessMask 0x01
#define     TW_hclCheckBrightnessShift 0

// Farbtemperatur Warmweiß
#define ParamTW_ColorTempWW                         ((knx.paramWord(TW_ParamCalcIndex(TW_ColorTempWW)) & TW_ColorTempWWMask) >> TW_ColorTempWWShift)
// Farbtemperatur Kaltweiß
#define ParamTW_ColorTempKW                         ((knx.paramWord(TW_ParamCalcIndex(TW_ColorTempKW)) & TW_ColorTempKWMask) >> TW_ColorTempKWShift)
// Einschaltverhalten
#define ParamTW_UseOnValue                          ((bool)(knx.paramByte(TW_ParamCalcIndex(TW_UseOnValue)) & TW_UseOnValueMask))
// Einschaltverhalten - Nacht
#define ParamTW_UseNightValue                       ((bool)(knx.paramByte(TW_ParamCalcIndex(TW_UseNightValue)) & TW_UseNightValueMask))
// Einschaltverhalten - Helligkeit
#define ParamTW_OnBrightness                        ((knx.paramByte(TW_ParamCalcIndex(TW_OnBrightness)) & TW_OnBrightnessMask) >> TW_OnBrightnessShift)
// Einschaltverhalten - Helligkeit (Nacht)
#define ParamTW_NightBrightness                     ((knx.paramByte(TW_ParamCalcIndex(TW_NightBrightness)) & TW_NightBrightnessMask) >> TW_NightBrightnessShift)
// Einschaltverhalten - Farbtemperatur
#define ParamTW_OnColorTemp                         (knx.paramWord(TW_ParamCalcIndex(TW_OnColorTemp)))
// Einschaltverhalten - Farbtemperatur (Nacht)
#define ParamTW_NightColorTemp                      (knx.paramWord(TW_ParamCalcIndex(TW_NightColorTemp)))
// Zeit
#define ParamTW_RelativDimTime                      ((knx.paramByte(TW_ParamCalcIndex(TW_RelativDimTime)) & TW_RelativDimTimeMask) >> TW_RelativDimTimeShift)
// Zeitbasis
#define ParamTW_RelativDimBase                      ((knx.paramByte(TW_ParamCalcIndex(TW_RelativDimBase)) & TW_RelativDimBaseMask) >> TW_RelativDimBaseShift)
// Zeit
#define ParamTW_OnOffTime                           ((knx.paramByte(TW_ParamCalcIndex(TW_OnOffTime)) & TW_OnOffTimeMask) >> TW_OnOffTimeShift)
// Zeitbasis
#define ParamTW_OnOffBase                           ((knx.paramByte(TW_ParamCalcIndex(TW_OnOffBase)) & TW_OnOffBaseMask) >> TW_OnOffBaseShift)
// Dimmkurve
#define ParamTW_DimCurve                            ((knx.paramByte(TW_ParamCalcIndex(TW_DimCurve)) & TW_DimCurveMask) >> TW_DimCurveShift)
// 
#define ParamTW_SceneNumberA                        ((int8_t)knx.paramByte(TW_ParamCalcIndex(TW_SceneNumberA)))
// 
#define ParamTW_SceneNumberB                        ((int8_t)knx.paramByte(TW_ParamCalcIndex(TW_SceneNumberB)))
// 
#define ParamTW_SceneNumberC                        ((int8_t)knx.paramByte(TW_ParamCalcIndex(TW_SceneNumberC)))
// 
#define ParamTW_SceneNumberD                        ((int8_t)knx.paramByte(TW_ParamCalcIndex(TW_SceneNumberD)))
// 
#define ParamTW_SceneNumberE                        ((int8_t)knx.paramByte(TW_ParamCalcIndex(TW_SceneNumberE)))
// 
#define ParamTW_SceneActionA                        (knx.paramByte(TW_ParamCalcIndex(TW_SceneActionA)))
// 
#define ParamTW_SceneActionB                        (knx.paramByte(TW_ParamCalcIndex(TW_SceneActionB)))
// 
#define ParamTW_SceneActionC                        (knx.paramByte(TW_ParamCalcIndex(TW_SceneActionC)))
// 
#define ParamTW_SceneActionD                        (knx.paramByte(TW_ParamCalcIndex(TW_SceneActionD)))
// 
#define ParamTW_SceneActionE                        (knx.paramByte(TW_ParamCalcIndex(TW_SceneActionE)))
// 
#define ParamTW_SceneBrightnessA                    (knx.paramByte(TW_ParamCalcIndex(TW_SceneBrightnessA)))
// 
#define ParamTW_SceneBrightnessB                    (knx.paramByte(TW_ParamCalcIndex(TW_SceneBrightnessB)))
// 
#define ParamTW_SceneBrightnessC                    (knx.paramByte(TW_ParamCalcIndex(TW_SceneBrightnessC)))
// 
#define ParamTW_SceneBrightnessD                    (knx.paramByte(TW_ParamCalcIndex(TW_SceneBrightnessD)))
// 
#define ParamTW_SceneBrightnessE                    (knx.paramByte(TW_ParamCalcIndex(TW_SceneBrightnessE)))
// 
#define ParamTW_SceneKelvinA                        (knx.paramWord(TW_ParamCalcIndex(TW_SceneKelvinA)))
// 
#define ParamTW_SceneKelvinB                        (knx.paramWord(TW_ParamCalcIndex(TW_SceneKelvinB)))
// 
#define ParamTW_SceneKelvinC                        (knx.paramWord(TW_ParamCalcIndex(TW_SceneKelvinC)))
// 
#define ParamTW_SceneKelvinD                        (knx.paramWord(TW_ParamCalcIndex(TW_SceneKelvinD)))
// 
#define ParamTW_SceneKelvinE                        (knx.paramWord(TW_ParamCalcIndex(TW_SceneKelvinE)))
// Verwende
#define ParamTW_hclChannel                          (knx.paramByte(TW_ParamCalcIndex(TW_hclChannel)) & TW_hclChannelMask)
// HCL anwenden
#define ParamTW_hclStart                            ((bool)(knx.paramByte(TW_ParamCalcIndex(TW_hclStart)) & TW_hclStartMask))
// HCL für diesen Kanal aktivieren
#define ParamTW_hclActive                           ((bool)(knx.paramByte(TW_ParamCalcIndex(TW_hclActive)) & TW_hclActiveMask))
// Farbtemperatur aktivieren
#define ParamTW_hclCheckTemperature                 ((bool)(knx.paramByte(TW_ParamCalcIndex(TW_hclCheckTemperature)) & TW_hclCheckTemperatureMask))
// Helligkeit aktivieren
#define ParamTW_hclCheckBrightness                  ((bool)(knx.paramByte(TW_ParamCalcIndex(TW_hclCheckBrightness)) & TW_hclCheckBrightnessMask))

// deprecated
#define TW_KoOffset 393

// Communication objects per channel (multiple occurrence)
#define TW_KoBlockOffset 393
#define TW_KoBlockSize 28

#define TW_KoCalcNumber(index) (index + TW_KoBlockOffset + _channelIndex * TW_KoBlockSize)
#define TW_KoCalcIndex(number) ((number >= TW_KoCalcNumber(0) && number < TW_KoCalcNumber(TW_KoBlockSize)) ? (number - TW_KoBlockOffset) % TW_KoBlockSize : -1)
#define TW_KoCalcChannel(number) ((number >= TW_KoBlockOffset && number < TW_KoBlockOffset + TW_ChannelCount * TW_KoBlockSize) ? (number - TW_KoBlockOffset) / TW_KoBlockSize : -1)

#define TW_KoSwitch 0
#define TW_KoDimAbsoluteBrightness 2
#define TW_KoDimAbsoluteColorTemp 3
#define TW_KoDimRelativBrightness 10
#define TW_KoDimRelativColorTemp 11
#define TW_KoStatusOnOff 16
#define TW_KoStatusBrightness 17
#define TW_KoStatusColorTemp 18
#define TW_KoSceneNumber 27

// TW%C%:
#define KoTW_Switch                              (knx.getGroupObject(TW_KoCalcNumber(TW_KoSwitch)))
// TW%C%:
#define KoTW_DimAbsoluteBrightness               (knx.getGroupObject(TW_KoCalcNumber(TW_KoDimAbsoluteBrightness)))
// TW%C%:
#define KoTW_DimAbsoluteColorTemp                (knx.getGroupObject(TW_KoCalcNumber(TW_KoDimAbsoluteColorTemp)))
// TW%C%:
#define KoTW_DimRelativBrightness                (knx.getGroupObject(TW_KoCalcNumber(TW_KoDimRelativBrightness)))
// TW%C%:
#define KoTW_DimRelativColorTemp                 (knx.getGroupObject(TW_KoCalcNumber(TW_KoDimRelativColorTemp)))
// TW%C%:
#define KoTW_StatusOnOff                         (knx.getGroupObject(TW_KoCalcNumber(TW_KoStatusOnOff)))
// TW%C%:
#define KoTW_StatusBrightness                    (knx.getGroupObject(TW_KoCalcNumber(TW_KoStatusBrightness)))
// TW%C%:
#define KoTW_StatusColorTemp                     (knx.getGroupObject(TW_KoCalcNumber(TW_KoStatusColorTemp)))
// TW%C%:
#define KoTW_SceneNumber                         (knx.getGroupObject(TW_KoCalcNumber(TW_KoSceneNumber)))

#define RGB_ChannelCount 4

// Parameter per channel
#define RGB_ParamBlockOffset 619
#define RGB_ParamBlockSize 39
#define RGB_ParamCalcIndex(index) (index + RGB_ParamBlockOffset + _channelIndex * RGB_ParamBlockSize)

#define RGB_UseOnColor                           0      // 1 Bit, Bit 7
#define     RGB_UseOnColorMask 0x80
#define     RGB_UseOnColorShift 7
#define RGB_UseNightColor                        0      // 1 Bit, Bit 6
#define     RGB_UseNightColorMask 0x40
#define     RGB_UseNightColorShift 6
#define RGB_OnColor                              1      // 24 Bits, Bit 31-8
#define     RGB_OnColorMask 0xFFFFFF00
#define     RGB_OnColorShift 8
#define RGB_NightColor                           4      // 24 Bits, Bit 31-8
#define     RGB_NightColorMask 0xFFFFFF00
#define     RGB_NightColorShift 8
#define RGB_RelativDimTime                       7      // 7 Bits, Bit 7-1
#define     RGB_RelativDimTimeMask 0xFE
#define     RGB_RelativDimTimeShift 1
#define RGB_RelativDimBase                       0      // 2 Bits, Bit 5-4
#define     RGB_RelativDimBaseMask 0x30
#define     RGB_RelativDimBaseShift 4
#define RGB_OnOffTime                            8      // 7 Bits, Bit 7-1
#define     RGB_OnOffTimeMask 0xFE
#define     RGB_OnOffTimeShift 1
#define RGB_OnOffBase                            0      // 2 Bits, Bit 3-2
#define     RGB_OnOffBaseMask 0x0C
#define     RGB_OnOffBaseShift 2
#define RGB_DimCurve                             9      // 3 Bits, Bit 7-5
#define     RGB_DimCurveMask 0xE0
#define     RGB_DimCurveShift 5
#define RGB_GammaCorrection                     10      // float (4 Byte)
#define RGB_SceneNumberA                        14      // int8_t
#define RGB_SceneNumberB                        15      // int8_t
#define RGB_SceneNumberC                        16      // int8_t
#define RGB_SceneNumberD                        17      // int8_t
#define RGB_SceneNumberE                        18      // int8_t
#define RGB_SceneActionA                        19      // 8 Bits, Bit 7-0
#define RGB_SceneActionB                        20      // 8 Bits, Bit 7-0
#define RGB_SceneActionC                        21      // 8 Bits, Bit 7-0
#define RGB_SceneActionD                        22      // 8 Bits, Bit 7-0
#define RGB_SceneActionE                        23      // 8 Bits, Bit 7-0
#define RGB_SceneColorA                         24      // 24 Bits, Bit 31-8
#define     RGB_SceneColorAMask 0xFFFFFF00
#define     RGB_SceneColorAShift 8
#define RGB_SceneColorB                         27      // 24 Bits, Bit 31-8
#define     RGB_SceneColorBMask 0xFFFFFF00
#define     RGB_SceneColorBShift 8
#define RGB_SceneColorC                         30      // 24 Bits, Bit 31-8
#define     RGB_SceneColorCMask 0xFFFFFF00
#define     RGB_SceneColorCShift 8
#define RGB_SceneColorD                         33      // 24 Bits, Bit 31-8
#define     RGB_SceneColorDMask 0xFFFFFF00
#define     RGB_SceneColorDShift 8
#define RGB_SceneColorE                         36      // 24 Bits, Bit 31-8
#define     RGB_SceneColorEMask 0xFFFFFF00
#define     RGB_SceneColorEShift 8
#define RGB_hclChannel                           0      // 2 Bits, Bit 1-0
#define     RGB_hclChannelMask 0x03
#define     RGB_hclChannelShift 0
#define RGB_hclStart                             7      // 1 Bit, Bit 0
#define     RGB_hclStartMask 0x01
#define     RGB_hclStartShift 0
#define RGB_hclActive                            8      // 1 Bit, Bit 0
#define     RGB_hclActiveMask 0x01
#define     RGB_hclActiveShift 0
#define RGB_hclCheckTemperature                  9      // 1 Bit, Bit 4
#define     RGB_hclCheckTemperatureMask 0x10
#define     RGB_hclCheckTemperatureShift 4
#define RGB_hclCheckBrightness                   9      // 1 Bit, Bit 3
#define     RGB_hclCheckBrightnessMask 0x08
#define     RGB_hclCheckBrightnessShift 3

// Einschaltverhalten
#define ParamRGB_UseOnColor                          ((bool)(knx.paramByte(RGB_ParamCalcIndex(RGB_UseOnColor)) & RGB_UseOnColorMask))
// Einschaltverhalten - Nacht
#define ParamRGB_UseNightColor                       ((bool)(knx.paramByte(RGB_ParamCalcIndex(RGB_UseNightColor)) & RGB_UseNightColorMask))
// Einschaltverhalten - Farbe
#define ParamRGB_OnColor                             ((knx.paramInt(RGB_ParamCalcIndex(RGB_OnColor)) & RGB_OnColorMask) >> RGB_OnColorShift)
// Einschaltverhalten - Farbe (Nacht)
#define ParamRGB_NightColor                          ((knx.paramInt(RGB_ParamCalcIndex(RGB_NightColor)) & RGB_NightColorMask) >> RGB_NightColorShift)
// Zeit
#define ParamRGB_RelativDimTime                      ((knx.paramByte(RGB_ParamCalcIndex(RGB_RelativDimTime)) & RGB_RelativDimTimeMask) >> RGB_RelativDimTimeShift)
// Zeitbasis
#define ParamRGB_RelativDimBase                      ((knx.paramByte(RGB_ParamCalcIndex(RGB_RelativDimBase)) & RGB_RelativDimBaseMask) >> RGB_RelativDimBaseShift)
// Zeit
#define ParamRGB_OnOffTime                           ((knx.paramByte(RGB_ParamCalcIndex(RGB_OnOffTime)) & RGB_OnOffTimeMask) >> RGB_OnOffTimeShift)
// Zeitbasis
#define ParamRGB_OnOffBase                           ((knx.paramByte(RGB_ParamCalcIndex(RGB_OnOffBase)) & RGB_OnOffBaseMask) >> RGB_OnOffBaseShift)
// Dimmkurve
#define ParamRGB_DimCurve                            ((knx.paramByte(RGB_ParamCalcIndex(RGB_DimCurve)) & RGB_DimCurveMask) >> RGB_DimCurveShift)
// Gamma Korrektur
#define ParamRGB_GammaCorrection                     (knx.paramFloat(RGB_ParamCalcIndex(RGB_GammaCorrection), Float_Enc_IEEE754Single))
// 
#define ParamRGB_SceneNumberA                        ((int8_t)knx.paramByte(RGB_ParamCalcIndex(RGB_SceneNumberA)))
// 
#define ParamRGB_SceneNumberB                        ((int8_t)knx.paramByte(RGB_ParamCalcIndex(RGB_SceneNumberB)))
// 
#define ParamRGB_SceneNumberC                        ((int8_t)knx.paramByte(RGB_ParamCalcIndex(RGB_SceneNumberC)))
// 
#define ParamRGB_SceneNumberD                        ((int8_t)knx.paramByte(RGB_ParamCalcIndex(RGB_SceneNumberD)))
// 
#define ParamRGB_SceneNumberE                        ((int8_t)knx.paramByte(RGB_ParamCalcIndex(RGB_SceneNumberE)))
// 
#define ParamRGB_SceneActionA                        (knx.paramByte(RGB_ParamCalcIndex(RGB_SceneActionA)))
// 
#define ParamRGB_SceneActionB                        (knx.paramByte(RGB_ParamCalcIndex(RGB_SceneActionB)))
// 
#define ParamRGB_SceneActionC                        (knx.paramByte(RGB_ParamCalcIndex(RGB_SceneActionC)))
// 
#define ParamRGB_SceneActionD                        (knx.paramByte(RGB_ParamCalcIndex(RGB_SceneActionD)))
// 
#define ParamRGB_SceneActionE                        (knx.paramByte(RGB_ParamCalcIndex(RGB_SceneActionE)))
// 
#define ParamRGB_SceneColorA                         ((knx.paramInt(RGB_ParamCalcIndex(RGB_SceneColorA)) & RGB_SceneColorAMask) >> RGB_SceneColorAShift)
// 
#define ParamRGB_SceneColorB                         ((knx.paramInt(RGB_ParamCalcIndex(RGB_SceneColorB)) & RGB_SceneColorBMask) >> RGB_SceneColorBShift)
// 
#define ParamRGB_SceneColorC                         ((knx.paramInt(RGB_ParamCalcIndex(RGB_SceneColorC)) & RGB_SceneColorCMask) >> RGB_SceneColorCShift)
// 
#define ParamRGB_SceneColorD                         ((knx.paramInt(RGB_ParamCalcIndex(RGB_SceneColorD)) & RGB_SceneColorDMask) >> RGB_SceneColorDShift)
// 
#define ParamRGB_SceneColorE                         ((knx.paramInt(RGB_ParamCalcIndex(RGB_SceneColorE)) & RGB_SceneColorEMask) >> RGB_SceneColorEShift)
// Verwende
#define ParamRGB_hclChannel                          (knx.paramByte(RGB_ParamCalcIndex(RGB_hclChannel)) & RGB_hclChannelMask)
// HCL anwenden
#define ParamRGB_hclStart                            ((bool)(knx.paramByte(RGB_ParamCalcIndex(RGB_hclStart)) & RGB_hclStartMask))
// HCL für diesen Kanal aktivieren
#define ParamRGB_hclActive                           ((bool)(knx.paramByte(RGB_ParamCalcIndex(RGB_hclActive)) & RGB_hclActiveMask))
// Farbtemperatur aktivieren
#define ParamRGB_hclCheckTemperature                 ((bool)(knx.paramByte(RGB_ParamCalcIndex(RGB_hclCheckTemperature)) & RGB_hclCheckTemperatureMask))
// Helligkeit aktivieren
#define ParamRGB_hclCheckBrightness                  ((bool)(knx.paramByte(RGB_ParamCalcIndex(RGB_hclCheckBrightness)) & RGB_hclCheckBrightnessMask))

// deprecated
#define RGB_KoOffset 561

// Communication objects per channel (multiple occurrence)
#define RGB_KoBlockOffset 561
#define RGB_KoBlockSize 28

#define RGB_KoCalcNumber(index) (index + RGB_KoBlockOffset + _channelIndex * RGB_KoBlockSize)
#define RGB_KoCalcIndex(number) ((number >= RGB_KoCalcNumber(0) && number < RGB_KoCalcNumber(RGB_KoBlockSize)) ? (number - RGB_KoBlockOffset) % RGB_KoBlockSize : -1)
#define RGB_KoCalcChannel(number) ((number >= RGB_KoBlockOffset && number < RGB_KoBlockOffset + RGB_ChannelCount * RGB_KoBlockSize) ? (number - RGB_KoBlockOffset) / RGB_KoBlockSize : -1)

#define RGB_KoSwitch 0
#define RGB_KoColorRGB 2
#define RGB_KoColorHSV 3
#define RGB_KoDimAbsoluteShadeH 4
#define RGB_KoDimAbsoluteSaturationS 5
#define RGB_KoDimAbsoluteBrightnessV 6
#define RGB_KoDimAbsoluteR 7
#define RGB_KoDimAbsoluteG 8
#define RGB_KoDimAbsoluteB 9
#define RGB_KoDimRelativShadeH 10
#define RGB_KoDimRelativSaturationS 11
#define RGB_KoDimRelativBrightnessV 12
#define RGB_KoDimRelativR 13
#define RGB_KoDimRelativG 14
#define RGB_KoDimRelativB 15
#define RGB_KoStatusOnOff 16
#define RGB_KoStatusColorRGB 17
#define RGB_KoStatusColorHSV 18
#define RGB_KoStatusShadeH 19
#define RGB_KoStatusSaturationS 20
#define RGB_KoStatusBrightnessV 21
#define RGB_KoStatusColorR 22
#define RGB_KoStatusColorG 23
#define RGB_KoStatusColorB 24
#define RGB_KoSceneNumber 27

// RGB%C%:
#define KoRGB_Switch                              (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoSwitch)))
// RGB%C%:
#define KoRGB_ColorRGB                            (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoColorRGB)))
// RGB%C%:
#define KoRGB_ColorHSV                            (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoColorHSV)))
// RGB%C%:
#define KoRGB_DimAbsoluteShadeH                   (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimAbsoluteShadeH)))
// RGB%C%:
#define KoRGB_DimAbsoluteSaturationS              (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimAbsoluteSaturationS)))
// RGB%C%:
#define KoRGB_DimAbsoluteBrightnessV              (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimAbsoluteBrightnessV)))
// RGB%C%:
#define KoRGB_DimAbsoluteR                        (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimAbsoluteR)))
// RGB%C%:
#define KoRGB_DimAbsoluteG                        (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimAbsoluteG)))
// RGB%C%:
#define KoRGB_DimAbsoluteB                        (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimAbsoluteB)))
// RGB%C%:
#define KoRGB_DimRelativShadeH                    (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimRelativShadeH)))
// RGB%C%:
#define KoRGB_DimRelativSaturationS               (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimRelativSaturationS)))
// RGB%C%:
#define KoRGB_DimRelativBrightnessV               (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimRelativBrightnessV)))
// RGB%C%:
#define KoRGB_DimRelativR                         (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimRelativR)))
// RGB%C%:
#define KoRGB_DimRelativG                         (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimRelativG)))
// RGB%C%:
#define KoRGB_DimRelativB                         (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoDimRelativB)))
// RGB%C%:
#define KoRGB_StatusOnOff                         (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusOnOff)))
// RGB%C%:
#define KoRGB_StatusColorRGB                      (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusColorRGB)))
// RGB%C%:
#define KoRGB_StatusColorHSV                      (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusColorHSV)))
// RGB%C%:
#define KoRGB_StatusShadeH                        (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusShadeH)))
// RGB%C%:
#define KoRGB_StatusSaturationS                   (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusSaturationS)))
// RGB%C%:
#define KoRGB_StatusBrightnessV                   (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusBrightnessV)))
// RGB%C%:
#define KoRGB_StatusColorR                        (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusColorR)))
// RGB%C%:
#define KoRGB_StatusColorG                        (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusColorG)))
// RGB%C%:
#define KoRGB_StatusColorB                        (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoStatusColorB)))
// RGB%C%:
#define KoRGB_SceneNumber                         (knx.getGroupObject(RGB_KoCalcNumber(RGB_KoSceneNumber)))

#define HCL_ChannelCount 3

// Parameter per channel
#define HCL_ParamBlockOffset 775
#define HCL_ParamBlockSize 13
#define HCL_ParamCalcIndex(index) (index + HCL_ParamBlockOffset + _channelIndex * HCL_ParamBlockSize)

#define HCL_Type                                 0      // 2 Bits, Bit 7-6
#define     HCL_TypeMask 0xC0
#define     HCL_TypeShift 6
#define HCL_colorTempMin                         1      // uint16_t
#define HCL_colorTempMax                         3      // uint16_t
#define HCL_offsetRiseType                       0      // 2 Bits, Bit 5-4
#define     HCL_offsetRiseTypeMask 0x30
#define     HCL_offsetRiseTypeShift 4
#define HCL_offsetRiseMin                        5      // 8 Bits, Bit 7-0
#define HCL_offsetSetType                        0      // 2 Bits, Bit 3-2
#define     HCL_offsetSetTypeMask 0x0C
#define     HCL_offsetSetTypeShift 2
#define HCL_offsetSetMin                         6      // 8 Bits, Bit 7-0
#define HCL_briMin                               7      // 7 Bits, Bit 7-1
#define     HCL_briMinMask 0xFE
#define     HCL_briMinShift 1
#define HCL_briMax                               8      // 7 Bits, Bit 7-1
#define     HCL_briMaxMask 0xFE
#define     HCL_briMaxShift 1
#define HCL_startTimeHour                        9      // uint8_t
#define HCL_startTimeMinute                     10      // uint8_t
#define HCL_endTimeHour                         11      // uint8_t
#define HCL_endTimeMinute                       12      // uint8_t

// Ansteuerung über
#define ParamHCL_Type                                ((knx.paramByte(HCL_ParamCalcIndex(HCL_Type)) & HCL_TypeMask) >> HCL_TypeShift)
// Farbtemperatur Min
#define ParamHCL_colorTempMin                        (knx.paramWord(HCL_ParamCalcIndex(HCL_colorTempMin)))
// Farbtemperatur Max
#define ParamHCL_colorTempMax                        (knx.paramWord(HCL_ParamCalcIndex(HCL_colorTempMax)))
// Verschiebe Sonnenaufgang
#define ParamHCL_offsetRiseType                      ((knx.paramByte(HCL_ParamCalcIndex(HCL_offsetRiseType)) & HCL_offsetRiseTypeMask) >> HCL_offsetRiseTypeShift)
// 
#define ParamHCL_offsetRiseMin                       (knx.paramByte(HCL_ParamCalcIndex(HCL_offsetRiseMin)))
// Verschiebe Sonnenuntergang
#define ParamHCL_offsetSetType                       ((knx.paramByte(HCL_ParamCalcIndex(HCL_offsetSetType)) & HCL_offsetSetTypeMask) >> HCL_offsetSetTypeShift)
// 
#define ParamHCL_offsetSetMin                        (knx.paramByte(HCL_ParamCalcIndex(HCL_offsetSetMin)))
// Helligkeit Min
#define ParamHCL_briMin                              ((knx.paramByte(HCL_ParamCalcIndex(HCL_briMin)) & HCL_briMinMask) >> HCL_briMinShift)
// Helligkeit Max
#define ParamHCL_briMax                              ((knx.paramByte(HCL_ParamCalcIndex(HCL_briMax)) & HCL_briMaxMask) >> HCL_briMaxShift)
// 
#define ParamHCL_startTimeHour                       (knx.paramByte(HCL_ParamCalcIndex(HCL_startTimeHour)))
// 
#define ParamHCL_startTimeMinute                     (knx.paramByte(HCL_ParamCalcIndex(HCL_startTimeMinute)))
// 
#define ParamHCL_endTimeHour                         (knx.paramByte(HCL_ParamCalcIndex(HCL_endTimeHour)))
// 
#define ParamHCL_endTimeMinute                       (knx.paramByte(HCL_ParamCalcIndex(HCL_endTimeMinute)))

// deprecated
#define HCL_KoOffset 673

// Communication objects per channel (multiple occurrence)
#define HCL_KoBlockOffset 673
#define HCL_KoBlockSize 2

#define HCL_KoCalcNumber(index) (index + HCL_KoBlockOffset + _channelIndex * HCL_KoBlockSize)
#define HCL_KoCalcIndex(number) ((number >= HCL_KoCalcNumber(0) && number < HCL_KoCalcNumber(HCL_KoBlockSize)) ? (number - HCL_KoBlockOffset) % HCL_KoBlockSize : -1)
#define HCL_KoCalcChannel(number) ((number >= HCL_KoBlockOffset && number < HCL_KoBlockOffset + HCL_ChannelCount * HCL_KoBlockSize) ? (number - HCL_KoBlockOffset) / HCL_KoBlockSize : -1)

#define HCL_KoStatusColorTemp 0
#define HCL_KoStatusBrightness 1

// HCL%C%:
#define KoHCL_StatusColorTemp                     (knx.getGroupObject(HCL_KoCalcNumber(HCL_KoStatusColorTemp)))
// HCL%C%:
#define KoHCL_StatusBrightness                    (knx.getGroupObject(HCL_KoCalcNumber(HCL_KoStatusBrightness)))



// Header generation for Module 'BASE_KommentarModule'

#define BASE_KommentarModuleCount 0
#define BASE_KommentarModuleModuleParamSize 0
#define BASE_KommentarModuleSubmodulesParamSize 0
#define BASE_KommentarModuleParamSize 0
#define BASE_KommentarModuleParamOffset 814
#define BASE_KommentarModuleCalcIndex(index, m1) (index + BASE_KommentarModuleParamOffset + _channelIndex * BASE_KommentarModuleCount * BASE_KommentarModuleParamSize + m1 * BASE_KommentarModuleParamSize)




// enumeration types


#ifdef MAIN_FirmwareRevision
#ifndef FIRMWARE_REVISION
#define FIRMWARE_REVISION MAIN_FirmwareRevision
#endif
#endif
#ifdef MAIN_FirmwareName
#ifndef FIRMWARE_NAME
#define FIRMWARE_NAME MAIN_FirmwareName
#endif
#endif
