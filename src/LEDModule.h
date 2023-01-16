#pragma once

#define PT_OperatingMode		        0       // Offset: 0, Size: 8 Bit (1 Byte), Text: Betriebsmodus
#define PT_PwmFrequenz		            1       // Offset: 1, Size: 8 Bit (1 Byte), Text: PWM Frequenz
#define PT_DimCurve		                2       // Offset: 2, Size: 8 Bit (1 Byte), Text: Dimmkurve
#define PT_SendMeasuredValues		    3       // Offset: 3, Size: 8 Bit (1 Byte), Text: Messwerte zyklisch senden
#define PT_MeasurementInterval		    4       // Offset: 4, Size: 8 Bit (1 Byte), Text: Messwerte alle

#define PT_SCBlockOffset                96      // Parameter für Einzelkanal starten bei 50
#define PT_SCBlockSize                  16      // Weitere Kanaele werden mit +16 berechnet
#define PT_SCOnBrightness		        1       // Offset: 97, 113 usw. Size: 8 Bit (1 Byte), Text: Einschalthelligkeit
#define PT_SCRelativDimTime		        2       // Offset: 98, 114 usw. Size: 8 Bit (1 Byte), Text: Dimmgeschwindigkeit Relativ
#define PT_SCOnOffTime		            3       // Offset: 99, 115 usw. Size: 8 Bit (1 Byte), Text: Ein- und Ausschaltgeschwindigkeit
#define PT_SCDimCurve                   4       // Offset: 100, 116 usw. Size: 8 Bit (1 Byte), Text: Dimmkurve

#define PT_TWBlockOffset                50      // Parameter für TW starten bei 50
#define PT_TWBlockSize                  10      // Weitere Kanaele werden mit +10 berechnet
#define PT_TWColorTempWW		        0       // Offset: 50, 60 usw. Size: 16 Bit (2 Byte), Text: Farbtemperatur Warmweiß
#define PT_TWColorTempKW		        2       // Offset: 52, 62 usw. Size: 16 Bit (2 Byte), Text: Farbtemperatur Kaltweiß
#define PT_TWOnBrightness		        6       // Offset: 56, 66 usw. Size: 8 Bit (1 Byte), Text: Einschaltverhalten - Helligkeit
#define PT_TWOnColorTemp		        7       // Offset: 57, 67 usw. Size: 16 Bit (2 Byte), Text: Einschaltverhalten - Farbtemperatur

#define PT_RGBRelativDimTime		    210     // Offset: 210, Size: 8 Bit (1 Byte), Text: Dimmgeschwindigkeit Relativ
#define PT_RGBOnOffTime		            211     // Offset: 211, Size: 8 Bit (1 Byte), Text: Ein- und Ausschaltgeschwindigkeit
#define PT_RGBDimCurve		            212     // Offset: 212, Size: 8 Bit (1 Byte), Text: Dimmkurve
#define PT_RGBOnColor		            213     // Offset: 213, Size: 8 Bit (1 Byte), Text: Farbwert beim Einschalten

#define PT_RGBWOnBrightness		        225     // Offset: 225, Size: 8 Bit (1 Byte), Text: Einschalthelligkeit

#define KO_VoltageV 	                2		//!< Number: 2, Text: Spannung, Function: Spannung
#define KO_CurrentA 	                4		//!< Number: 4, Text: Strom, Function: Stromwert
#define KO_PowerW 	                    6	    //!< Number: 6, Text: Wirkleistung, Function: Wirkleistung

#define KO_ChannelASwitch 	                101		//!< Number: 101, Text: A:, Function: Schalten
#define KO_ChannelADimAbsolute 	            104		//!< Number: 104, Text: A:, Function: Dimmen Absolut
#define KO_ChannelADimRelativ 	            105		//!< Number: 105, Text: A:, Function: Dimmen Relativ
#define KO_ChannelAStatusOnOff 	            115		//!< Number: 115, Text: A:, Function: Status An/Aus
#define KO_ChannelAStatusBrightness 	    116	    //!< Number: 116, Text: A:, Function: Status Helligkeit
#define KO_ChannelBSwitch 	                121		//!< Number: 121, Text: B:, Function: Schalten
#define KO_ChannelBDimAbsolute      	    124		//!< Number: 124, Text: B:, Function: Dimmen Absolut
#define KO_ChannelBDimRelativ 	            125		//!< Number: 125, Text: B:, Function: Dimmen Relativ
#define KO_ChannelBStatusOnOff 	            135		//!< Number: 135, Text: B:, Function: Status An/Aus
#define KO_ChannelBStatusBrightness 	    136	    //!< Number: 136, Text: B:, Function: Status Helligkeit
#define KO_ChannelCSwitch 	                141		//!< Number: 141, Text: C:, Function: Schalten
#define KO_ChannelCDimAbsolute 	            144		//!< Number: 144, Text: C:, Function: Dimmen Absolut
#define KO_ChannelCDimRelativ 	            145		//!< Number: 145, Text: C:, Function: Dimmen Relativ
#define KO_ChannelCStatusOnOff 	            155		//!< Number: 155, Text: C:, Function: Status An/Aus
#define KO_ChannelCStatusBrightness 	    156	    //!< Number: 156, Text: C:, Function: Status Helligkeit
#define KO_ChannelDSwitch 	                161		//!< Number: 161, Text: D:, Function: Schalten
#define KO_ChannelDDimAbsolute 	            164		//!< Number: 164, Text: D:, Function: Dimmen Absolut
#define KO_ChannelDDimRelativ 	            165		//!< Number: 165, Text: D:, Function: Dimmen Relativ
#define KO_ChannelDStatusOnOff 	            175		//!< Number: 175, Text: D:, Function: Status An/Aus
#define KO_ChannelDStatusBrightness 	    176	    //!< Number: 176, Text: D:, Function: Status Helligkeit
#define KO_ChannelESwitch 	                181		//!< Number: 181, Text: E:, Function: Schalten
#define KO_ChannelEDimAbsolute 	            184		//!< Number: 184, Text: E:, Function: Dimmen Absolut
#define KO_ChannelEDimRelativ 	            185		//!< Number: 185, Text: E:, Function: Dimmen Relativ
#define KO_ChannelEStatusOnOff 	            195	    //!< Number: 195, Text: E:, Function: Status An/Aus
#define KO_ChannelEStatusBrightness 	    196	    //!< Number: 196, Text: E:, Function: Status Helligkeit
#define KO_ChannelRGBSwitch 	            201	    //!< Number: 201, Text: RGB, Function: Schalten
#define KO_ChannelRGBDimAbsoluteRGBW 	    210	    //!< Number: 210, Text: RGB:, Function: Dimmen Absolut RGBW
#define KO_ChannelRGBDimAbsoluteRGB 	    211	    //!< Number: 211, Text: RGB:, Function: Dimmen Absolute RGB
#define KO_ChannelRGBDimAbsoluteHSV 	    212	    //!< Number: 212, Text: RGB:, Function: Dimmen Absolute HSV
#define KO_ChannelRGBDimRelativRGBW 	    221	    //!< Number: 221, Text: RBG:, Function: Dimmen Relativ RGBW
#define KO_ChannelRGBDimRelativRGB 	        222	    //!< Number: 222, Text: RGB:, Function: Dimmen Absolute RGB
#define KO_ChannelRGBDimRelativHSV 	        223	    //!< Number: 223, Text: RGB:, Function: Dimmen Relativ HSV
#define KO_ChannelRGBStatusOnOff 	        241	    //!< Number: 241, Text: RGB:, Function: Status An/Aus
#define KO_ChannelRGBStatusRGBW 	        247	    //!< Number: 247, Text: RGB:, Function: Status RGBW
#define KO_ChannelRGBStatusRGB 	            248	    //!< Number: 248, Text: RGB:, Function: Status RGB
#define KO_ChannelRGBStatusHSV 	            249	    //!< Number: 249, Text: RGB:, Function: Status HSV
#define KO_ChannelRGBDimAbsoluteTW 	        265	    //!< Number: 265, Text: RGB:, Function: Dimmen Absolut Helligkeit TW
#define KO_ChannelRGBDimRelativTW 	        269	    //!< Number: 269, Text: RGB:, Function: Dimmen Relativ Helligkeit TW
#define KO_ChannelRGBStatusTW 	            287	    //!< Number: 287, Text: RGB:, Function: Status Helligkeit TW
#define KO_ChannelTW1Switch 	            262	    //!< Number: 262, Text: TW1:, Function: Schalten
#define KO_ChannelTW1DimAbsoluteBrightness 	265	    //!< Number: 265, Text: TW1:, Function: Dimmen Absolut Helligkeit
#define KO_ChannelTW1DimAbsoluteColorTemp 	267	    //!< Number: 267, Text: TW1:, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define KO_ChannelTW1DimRelativBrightness 	269	    //!< Number: 269, Text: TW1:, Function: Dimmen Relativ Helligkeit
#define KO_ChannelTW1StatusOnOff 	        286	    //!< Number: 286, Text: TW1:, Function: Status An/Aus
#define KO_ChannelTW1StatusBrightness 	    287	    //!< Number: 287, Text: TW1:, Function: Status Helligkeit
#define KO_ChannelTW1StatusColorTemp 	    289	    //!< Number: 289, Text: TW1:, Function: Status Farbtemperatur (Kelvin)
#define KO_ChannelTW2Switch 	            299	    //!< Number: 299, Text: TW2:, Function: Schalten
#define KO_ChannelTW2DimAbsoluteBrightness 	302	    //!< Number: 302, Text: TW2:, Function: Dimmen Absolut Helligkeit
#define KO_ChannelTW2DimAbsoluteColorTemp 	304	    //!< Number: 304, Text: TW2:, Function: Dimmen Absolut Farbtemperatur (Kelvin)
#define KO_ChannelTW2DimRelativBrightness 	306	    //!< Number: 306, Text: TW2:, Function: Dimmen Relativ Helligkeit
#define KO_ChannelTW2StatusOnOff 	        323	    //!< Number: 323, Text: TW2:, Function: Status An/Aus
#define KO_ChannelTW2StatusBrightness 	    324	    //!< Number: 324, Text: TW2:, Function: Status Helligkeit
#define KO_ChannelTW2StatusColorTemp 	    326	    //!< Number: 326, Text: TW2:, Function: Status Farbtemperatur (Kelvin)

#define KO_PER_CHANNEL_EK       20          //Number of KO per EK channel
#define KO_PER_CHANNEL_TW       5           //Number of KO per TW channel
#define KO_PER_CHANNEL_RGB      5           //Number of KO per RGB channel
#define KO_PER_CHANNEL_RGBW     5           //Number of KO per RGBW channel
#define KO_PER_CHANNEL_RGBCCT   5           //Number of KO per RGBCCT channel

#define KO_OFFSET_EK_SWITCH            0           //Offset for Switch KO
#define KO_OFFSET_EK_DIMABSOLUT        3           //Offset for Dim Absolut KO
#define KO_OFFSET_EK_DIMRELATIV        4           //Offset for Dim Relativ KO
#define KO_OFFSET_EK_STATUSONOFF       14          //Offset for OnOff KO
#define KO_OFFSET_EK_STATUSBRIGHTNESS  15          //Offset for Brightness KO

#define KO_OFFSET_TW_SWITCH            0           //Offset for Switch KO
#define KO_OFFSET_TW_DIMABSOLUT        3           //Offset for Dim Absolut KO
#define KO_OFFSET_TW_DIMKELVIN         5           //Offset for Dim Kelvin KO
#define KO_OFFSET_TW_DIMRELATIV        7           //Offset for Dim Relativ KO
#define KO_OFFSET_TW_STATUSONOFF       24          //Offset for OnOff KO
#define KO_OFFSET_TW_STATUSBRIGHTNESS  25          //Offset for Brightness KO
#define KO_OFFSET_TW_STATUSKELVIN      27          //Offset for Kelvin KO


//Number of channel groups per operating mode -> note use till now
//Operating Mode = 0
#define OM_0_EK_CHANNELS        5   //5xEK
//Operating Mode = 1
#define OM_1_RGBCCT_CHANNELS    1   //1xRGBCTT
//Operating Mode = 2
#define OM_2_EK_CHANNELS        1   //1xEK
#define OM_2_RGBW_CHANNELS      1   //1xRGBW
//Operating Mode = 3
#define OM_3_EK_CHANNELS        2   //2xEK
#define OM_3_RGB_CHANNELS       1   //1xRGB
//Operating Mode = 4
#define OM_4_TW_CHANNELS        2   //1xTW
#define OM_4_RGB_CHANNELS       1   //1xRGB
//Operating Mode = 5
#define OM_5_EK_CHANNELS        1   //1xEK
#define OM_5_TW_CHANNELS        2   //2xTW
//Operating Mode = 6
#define OM_6_EK_CHANNELS        3   //3xEK
#define OM_6_TW_CHANNELS        1   //1xTW

// for future use
#define MAIN_OpenKnxId 0xAF00
#define MAIN_ApplicationNumber 1
#define MAIN_ApplicationVersion 1
#define MAIN_OrderNumber "OAMLEDDK"

//DEBUG options
#define KDEBUG                      // comment this line to disable DEBUG info
#define KDEBUG_KO                   // comment this line to disable COMOBJECT DEBUG info
#define KDEBUG_ME                   // comment this line to disable MEASURING DEBUG info
#define KDEBUG_LED                  // comment this line to disable LED DEBUG info