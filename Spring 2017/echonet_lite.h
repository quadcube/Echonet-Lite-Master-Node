//
//  echonet_lite.h
//  Echonet Lite Test
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#ifndef echonet_lite_h
#define echonet_lite_h

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "network.h"

/*
 Global variable for Echonet Lite Packet status
 */

/*
 Echonet Lite Header 1 (EHD1)
 Note: Conventional Echonet (0b00010000)
 */
#define EHD1_ECHONET                                0x10

/*
 Echonet Lite Header 2 (EHD2)
 Note: Specified message format (FORMAT 1)
       Arbitrary message formar (FORMAT 2)
 */
#define EHD2_FORMAT1                                0x81
#define EHD2_FORMAT2                                0x82

/*
 Echonet Lite Transaction ID (TID)
 */

/*
 Echonet Lite Class Group Code (EOJ)
 */
#define CGC_SENSOR_RELATED                          0x00
#define CGC_AIR_CONDITIONER_RELATED                 0x01
#define CGC_HOUSING_RELATED                         0x02
#define CGC_COOKING_RELATED                         0x03
#define CGC_HEALTH_RELATED                          0x04
#define CGC_MANAGEMENT_RELATED                      0x05
#define CGC_AV_RELATED                              0x06
#define CGC_PROFILE_CLASS                           0x0E
#define CGC_USER_DEFINITION_CLASS                   0x0F

/*
 Echonet Lite Class Code (EOJ)
 Class Group Code = 0x00
 Note: Sensor related device class group
 */
#define CC_GAS_LEAK_SENSOR                          0x01
#define CC_CRIME_PREVENTION_SENSOR                  0x02
#define CC_EMERGENCY_BUTTON                         0x03
#define CC_FIRST_AID_SENSOR                         0x04
#define CC_EARTHQUAKE_SENSOR                        0x05
#define CC_ELECTRIC_LEAK_SENSOR                     0x06
#define CC_HUMAN_DETECTION_SENSOR                   0x07
#define CC_VISITOR_SENSOR                           0x08
#define CC_CALL_SENSOR                              0x09
#define CC_CONDENSATION_SENSOR                      0x0A
#define CC_AIR_POLLUTION_SENSOR                     0x0B
#define CC_OXYGEN_SENSOR                            0x0C
#define CC_ILLUMINANCE_SENSOR                       0x0D
#define CC_SOUND_SENSOR                             0x0E
#define CC_MAILING_SENSOR                           0x0F
#define CC_WEIGHT_SENSOR                            0x10
#define CC_TEMPERTURE_SENSOR                        0x11
#define CC_HUMIDITY_SENSOR                          0x12
#define CC_RAIN_SENSOR                              0x13
#define CC_WATER_LEVEL_SENSOR                       0x14
#define CC_BATH_WATER_LEVEL_SENSOR                  0x15
#define CC_BATH_HEATING_STATUS_SENSOR               0x16
#define CC_WATER_LEAK_SENSOR                        0x17
#define CC_WATER_OVERFLOW_SENSOR                    0x18
#define CC_FIRE_SENSOR                              0x19
#define CC_CIGARETTE_SMOKE_SENSOR                   0x1A
#define CC_CO2_SENSOR                               0x1B
#define CC_GAS_SENSOR                               0x1C
#define CC_VOC_SENSOR                               0x1D
#define CC_DIFFERENTIAL_PRESSURE_SENSOR             0x1E
#define CC_AIR_SPEED_SENSOR                         0x1F
#define CC_ODOR_SENSOR                              0x20
#define CC_FLAME_SENSOR                             0x21
#define CC_ELECTRIC_ENERGY_SENSOR                   0x22
#define CC_CURRENT_VALUE_SENSOR                     0x23
#define CC_DAYLIGHT_SENSOR                          0x24
#define CC_WATER_FLOW_RATE_SENSOR                   0x25
#define CC_MICROMOTION_SENSOR                       0x26
#define CC_PASSAGE_SENSOR                           0x27
#define CC_BED_PRESENCE_SENSOR                      0x28
#define CC_OPEN_CLOSE_SENSOR                        0x29
#define CC_ACTIVITY_AMOUNT_SENSOR                   0x2A
#define CC_HUMAN_BODY_LOCATION_SENSOR               0x2B
#define CC_SNOW_SENSOR                              0x2C
#define CC_AIR_PRESSURE_SENSOR                      0x2D

/*
 Echonet Lite Class Code (EOJ)
 Class Group Code = 0x01
 Note: Air-conditional device class group
 */
#define CC_HOME_AIR_CONDITIONER                     0x30
#define CC_COLD_BLASTER                             0x31
#define CC_ELECTRIC_FAN                             0x32
#define CC_VENTILATION_FAN                          0x33
#define CC_AIR_CONDITIONER_VENTILATION_FAN          0x34
#define CC_AIR_CLEANER                              0x35
#define CC_COLD_BLAST_FAN                           0x36
#define CC_CIRCULATOR                               0x37
#define CC_DEHUMIDIFIER                             0x38
#define CC_HUMIDIFIER                               0x39
#define CC_CEILING_FAN                              0x3A
#define CC_ELECTRIC_KOTATSU                         0x3B
#define CC_ELECTRIC_HEATING_PAD                     0x3C
#define CC_ELECTRIC_BLANKET                         0x3D
#define CC_SPACE_HEATER                             0x3E
#define CC_PANEL_HEATER                             0x3F
#define CC_ELECTRIC_CARPET                          0x40
#define CC_FLOOR_HEATER_0x01                        0x41
#define CC_ELECTRIC_HEATER                          0x42
#define CC_FAN_HEATER                               0x43
#define CC_BATTERY_CHARGER                          0x44
#define CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_INDOOR   0x45
#define CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_OUTDOOR  0x46
#define CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_THERMAL  0x47
#define CC_COMMERCIAL_FAN_COIL_UNIT                 0x48
#define CC_COMMERCIAL_AIRCOND_COLD_SOURCE_CHILLER   0x49
#define CC_COMMERCIAL_AIRCOND_HOT_SOURCE_BOILER     0x50
#define CC_AIRCOND_VAV_FOR_COMMERCIAL_APPLICATIONS  0x51
#define CC_AIRCOND_FOR_COMMERCIAL_APPLICATIONS      0x52
#define CC_UNIT_COOLER                              0x53
#define CC_CONDENSING_UNIT_FOR_COMMERCIAL_APP       0x54
#define CC_ELECTRIC_STORAGE_HEATER                  0x55

/*
 Echonet Lite Class Code (EOJ)
 Class Group Code = 0x02
 Note: Housing/facility device class group
 */
#define CC_ELECTRICALLY_OPERATED_BLIND              0x60
#define CC_ELECTRICALLY_OPERATED_SHUTTER            0x61
#define CC_ELECTRICALLY_OPERATED_CURTAIN            0x62
#define CC_ELECTRICALLY_OPERATED_RAIN_SLIDING_DOOR  0x63
#define CC_ELECTRICALLY_OPERATED_GATE               0x64
#define CC_ELECTRICALLY_OPERATED_WINDOW             0x65
#define CC_AUTOMATICALLY_OPERATED_ENTRANCE_DOOR     0x66
#define CC_GARDEN_SPRINKLER                         0x67
#define CC_FIRE_SPRINKLER                           0x68
#define CC_FOUNTAIN                                 0x69
#define CC_INSTANTANEOUS_WATER_HEATER               0x6A
#define CC_ELECTRIC_WATER_HEATER                    0x6B
#define CC_SOLAR_WATER_HEATER                       0x6C
#define CC_CIRCULATION_PUMP                         0x6D
#define CC_BIDET_EQUIPPED_TOILET                    0x6E
#define CC_ELECTRIC_LOCK                            0x6F
#define CC_GAS_LINE_VALVE                           0x70
#define CC_HOME_SAUNA                               0x71
#define CC_HOT_WATER_GENERATOR                      0x72
#define CC_BATHROOM_DRYER                           0x73
#define CC_HOME_ELEVATOR                            0x74
#define CC_ELECTRICALLY_OPERATED_ROOM_DIVIDER       0x75
#define CC_HORIZONTAL_TRANSFER                      0x76
#define CC_ELECTRICALLY_OPERATED_CLOTH_DRYING_POLE  0x77
#define CC_SEPTIC_TANK                              0x78
#define CC_HOME_SOLAR_POWER_GENERATION              0x79
#define CC_COLD_HOT_WATER_HEAT_SOURCE_EQUIPMENT     0x7A
#define CC_FLOOR_HEATER_0x02                        0x7B
#define CC_FUEL_CELL                                0x7C
#define CC_STORAGE_BATTERY                          0x7D
#define CC_ELECTRIC_VEHICLE_CHARGER_DISCHARGER      0x7E
#define CC_ENGINE_COGENERATION                      0x7F
#define CC_ELECTRIC_ENERGY_METER                    0x80
#define CC_WATER_FLOW_METER                         0x81
#define CC_GAS_METER                                0x82
#define CC_LP_GAS_METER                             0x83
#define CC_CLOCK                                    0x84
#define CC_AUTOMATIC_DOOR                           0x85
#define CC_COMMERCIAL_ELEVATOR                      0x86
#define CC_DISTRIBUTION_PANEL_METERING              0x87
#define CC_LOW_VOLTAGE_SMART_ELECTRIC_ENERGY_METER  0x88
#define CC_SMART_GAS_METER                          0x89
#define CC_HIGH_VOLTAGE_SMART_ELECTRIC_ENERGY_METER 0x8A
#define CC_GENERAL_LIGHTING_CLASS                   0x90
#define CC_SINGLE_FUNCTION_LIGHTING                 0x91
#define CC_EMERGENCY_LIGHTING                       0x99
#define CC_EQUIPMENT_LIGHT                          0x9D
#define CC_BUZZER                                   0xA0

/*
 Echonet Lite Class Code (EOJ)
 Class Group Code = 0x03
 Note: Cooking/Household-related device class group
 */
#define CC_COFFEE_MACHINE                           0xB0
#define CC_COFFEE_MILL                              0xB1
#define CC_ELECTRIC_HOT_WATER_POT                   0xB2
#define CC_ELECTRIC_STOVE                           0xB3
#define CC_TOASTER                                  0xB4
#define CC_JUICER_FOOD_MIXER                        0xB5
#define CC_FOOD_PROCESSOR                           0xB6
#define CC_REFRIGERATOR                             0xB7
#define CC_COMBINATION_MICROWAVE_OVEN               0xB8
#define CC_COOKING_HEATER                           0xB9
#define CC_OVEN                                     0xBA
#define CC_RICE_COOKER                              0xBB
#define CC_ELECTRONIC_JAR                           0xBC
#define CC_DISH_WASHER                              0xBD
#define CC_DISH_DRYER                               0xBE
#define CC_ELECTRIC_RICE_CARD_COOKER                0xBF
#define CC_KEEP_WARM_MACHINE                        0xC0
#define CC_RICE_MILL                                0xC1
#define CC_AUTOMATIC_BREAD_COOKER                   0xC2
#define CC_SLOW_COOKER                              0xC3
#define CC_ELECTRIC_PICKLES_COOKER                  0xC4
#define CC_WASHING_MACHINE                          0xC5
#define CC_CLOTHES_DRYER                            0xC6
#define CC_ELECTRIC_IRON                            0xC7
#define CC_TROUSER_PRESS                            0xC8
#define CC_FUTON_DRYER                              0xC9
#define CC_SMALL_ARTICLE_SHOES_DRYER                0xCA
#define CC_ELECTRIC_VACUUM_CLEANER                  0xCB
#define CC_DISPOSER                                 0xCC
#define CC_ELECTRIC_MOSQUITO_CATCHER                0xCD
#define CC_COMMERCIAL_SHOW_CASE                     0xCE
#define CC_COMMERCIAL_REFRIGERATOR                  0xCF
#define CC_COMMERCIAL_HOT_CASE                      0xD0
#define CC_COMMERCIAL_FRYER                         0xD1
#define CC_COMMERCIAL_MICROWAVE_OVEN                0xD2
#define CC_WASHER_AND_DRYER                         0xD3
#define CC_COMMERCIAL_SHOW_CASE_OUTDOOR_UNIT        0xD4

/*
 Echonet Lite Class Code (EOJ)
 Class Group Code = 0x04
 Note: Health-related device class group
 */
#define CC_WEIGHTING_MACHINE                        0x01
#define CC_CLINICAL_THERMOMETER                     0x02
#define CC_BLOOD_PRESSURE_METER                     0x03
#define CC_BLOOD_SUGAR_METER                        0x04
#define CC_BODY_FAT_METER                           0x05

/*
 Echonet Lite Class Code (EOJ)
 Class Group Code = 0x05
 Note: Management/operation-related device class group
 */
#define CC_SECURE_COMM_SHARED_KEY_SETUP_NODE        0xFC
#define CC_SWITCH                                   0xFD
#define CC_PORTABLE_TERMINAL                        0xFE
#define CC_CONTROLLER                               0xFF

/*
 Echonet Lite Class Code (EOJ)
 Class Group Code = 0x06
 Note: Audiovisual-related device class group
 */
#define CC_DISPLAY                                  0x01
#define CC_TELEVISION                               0x02
#define CC_AUDIO                                    0x03
#define CC_NETWORK_CAMERA                           0x04

/*
 Echonet Lite Class Code (EOJ)
 Note: Class Group Code = 0x0E
 */
#define CC_NODE_PROFILE                             0xF0

/*
 Echonet Lite Instance Code (EOJ)
 Note: 1. Only for Class Code = Profile Class (0x0E)
       2. only for Class Group Code = Node Profile Class (0xF0)
 */
#define IC_GENERAL_NODE                             0x01
#define IC_TRANSMISSION_ONLY_NODE                   0x02

/*
 Echonet Lite Service (ESV)
 Note: Service codes for request
 */
#define ESV_SetI                                    0x60
#define ESV_SetC                                    0x61
#define ESV_Get                                     0x62
#define ESV_INF_REQ                                 0x63
#define ESV_SetGet                                  0x6E

/*
 Echonet Lite Service (ESV)
 Note: Service codes for response/notification
 */
#define ESV_Set_Res                                  0x71
#define ESV_Get_Res                                  0x72
#define ESV_INF                                      0x73
#define ESV_INFC                                     0x74
#define ESV_INFC_Res                                 0x7A
#define ESV_SetGet_Res                               0x7E

/*
 Echonet Lite Service (ESV)
 Note: Service codes for response not possible
 */
#define ESV_SetI_SNA                                 0x50
#define ESV_SetC_SNA                                 0x51
#define ESV_Get_SNA                                  0x52
#define ESV_INF_SNA                                  0x53
#define ESV_SetGet_SNA                               0x5E

/*
 Echonet Lite Processing Target Property Counters (OPC)
 */


/*
 Echonet Lite Property (EPC)
 Note: Super class
 */
#define EPC_OPERATIONAL_STATUS                      0x80
#define EPC_INSTALLATION_LOCATION                   0x81
#define EPC_STANDARD_VERSION_INFORMATION            0x82
#define EPC_IDENTIFICATION_NUMBER                   0x83
#define EPC_MEASURE_INSTANTANEOUS_POWER_CONSUMPTION 0x84
#define EPC_MEASURE_CUMULATIVE_POWER_CONSUMPTION    0x85
#define EPC_MANUFACTURER_FAULT_CODE                 0x86
#define EPC_CURRENT_LIMIT_SETTING                   0x87
#define EPC_FAULT_STATUS                            0x88
#define EPC_FAULT_DESCRIPTION                       0x89
#define EPC_MANUFACTURER_CODE                       0x8A
#define EPC_BUSINESS_FACILITY_CODE                  0x8B
#define EPC_PRODUCT_CODE                            0x8C
#define EPC_PRODUCTION_NUMBER                       0x8D
#define EPC_PRODUCTION_DATE                         0x8E
#define EPC_POWER_SAVING_OPERATIONAL_SETTING        0x8F
#define EPC_REMOTE_CONTROL_SETTING                  0x93
#define EPC_CURRENT_TIME_SETTING                    0x97
#define EPC_CURRENT_DATE_SETTING                    0x98
#define EPC_POWER_LIMIT_SETTING                     0x99
#define EPC_CUMULATIVE_OPERATING_TIME               0x9A
#define EPC_SET_M_PROPERTY_MAP                      0x9B
#define EPC_GET_M_PROPERTY_MAP                      0x9C
#define EPC_STATUS_CHANGE_ANNOUCEMENT_PROPERTY_MAP  0x9D
#define EPC_SET_PROPERTY_MAP                        0x9E
#define EPC_GET_PROPERTY_MAP                        0x9F

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Gas Leak Sensor (0x01)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_GAS_LEAK_OCCURRENCE_STATUS              0xB1
#define EPC_GAS_LEAK_OCCURRENCE_STATUS_RESET        0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Crime Prevention Sensor (0x02)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_INVASION_OCCURRENCE_STATUS              0xB1
#define EPC_INVASION_OCCURRENCE_STATUS_RESET        0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Emergency Button (0x03)
 */
#define EPC_EMERGENCY_OCCURRENCE_STATUS             0xB1
#define EPC_EMERGENCY_OCCURRENCE_STATUS_RESET       0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), First-aid Sensor (0x04)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_FIRST_AID_OCCURRENCE_STATUS             0xB1
#define EPC_FIRST_AID_OCCURRENCE_STATUS_RESET       0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Earthquake Sensor (0x05)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_EARTHQUAKE_OCCURRENCE_STATUS            0xB1
#define EPC_EARTHQUAKE_OCCURRENCE_STATUS_RESET      0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Electric Leak Sensor (0x06)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_ELECTRIC_LEAK_OCCURRENCE_STATUS         0xB1
#define EPC_ELECTRIC_LEAK_OCCURRENCE_STATUS_RESET   0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Human Detection Sensor (0x07)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_HUMAN_DETECTION_STATUS                  0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Visitor Sensor (0x08)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_VISITOR_DETECTION_STATUS                0xB1
#define EPC_VISITOR_DETECTION_HOLDING_TIME          0xBE

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Call Sensor (0x09)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_CALL_STATUS                             0xB1
#define EPC_CALL_HOLDING_TIME                       0xBE

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Condensation Sensor (0x0A)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_CONDENSATION_DETECTION_STATUS           0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Air Pollution Sensor (0x0B)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_AIR_POLLUTION_DETECTION_STATUS          0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Oxygen Sensor (0x0C)
 */
#define EPC_MEASURE_OXYGEN_CONCENTRATION_VALUE      0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Illuminance Sensor (0x0D)
 */
#define EPC_MEASURE_ILLUMINANCE_LUX_VALUE           0xE0
#define EPC_MEASURE_ILLUMINANCE_KILOLUX_VALUE       0xE1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Sound Sensor (0x0E)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_SOUND_DETECTION_STATUS                  0xB1
#define EPC_SOUND_DETECTION_HOLDING_TIME            0xBE

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Mailing Sensor (0x0F)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_MAILING_DETECTION_STATUS                0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Weight Sensor (0x10)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_WEIGHT_DETECTION_STATUS                 0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Temperature Sensor (0x11)
 */
#define EPC_MEASURE_TEMPERATURE_VALUE               0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Temperature Sensor (0x12)
 */
#define EPC_MEASURE_HUMIDITY_VALUE                  0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Rain Sensor (0x13)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_RAIN_DETECTION_STATUS                   0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Water Level Sensor (0x14)
 */
#define EPC_WATER_LEVEL_OVER_DETECTION_THRESHOLD_LEVEL  0xB0
#define EPC_WATER_LEVEL_OVER_DETECTION_STATUS       0xB1
#define EPC_MEASURE_WATER_LEVEL_VALUE               0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Bath Water Level Sensor (0x15)
 */
#define EPC_BATH_WATER_LEVEL_OVER_DETECTION_THRESHOLD_LEVEL 0xB0
#define EPC_BATH_WATER_LEVEL_OVER_DETECTION_STATUS  0xB1
#define EPC_MEASURE_BATH_WATER_LEVEL_VALUE          0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Bath Heating Status Sensor (0x16)
*/
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_BATH_HEATING_DETECTION_STATUS           0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Water Leak Sensor (0x17)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_WATER_LEAK_DETECTION_STATUS             0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Water Overflow Sensor (0x18)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_WATER_OVERFLOW_DETECTION_STATUS         0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Fire Sensor (0x19)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_FIRE_OCCURRENCE_DETECTION_STATUS        0xB1
#define EPC_FIRE_OCCURRENCE_DETECTION_STATUS_RESET  0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Cigarette Smoke Sensor (0x1A)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_SMOKE_DETECTION_STATUS                  0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), CO2 Sensor (0x1B)
 */
#define EPC_MEASURE_CO2_CONCENTRATION_VALUE         0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Gas Sensor (0x1C)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_GAS_DETECTION_STATUS                    0xB1
#define EPC_MEASURE_GAS_CONCENTRATION_VALUE         0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), VOC Sensor (0x1D)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_VOC_DETECTION_STATUS                    0xB1
#define EPC_MEASURE_VOC_CONCENTRATION_VALUE         0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Differential Pressure Sensor (0x1E)
 */
#define EPC_MEASURE_DIFFERENTIAL_PRESSURE_VALUE     0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Air Speed Sensor (0x1F)
 */
#define EPC_MEASURE_AIR_SPEED_VALUE                 0xE0
#define EPC_AIR_FLOW_DIRECTION                      0xE1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Odor Sensor (0x20)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_ODOR_DETECTION_STATUS                   0xB1
#define EPC_MEASURE_ODOR_VALUE                      0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Flame Sensor (0x21)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_FLAME_DETECTION_STATUS                  0xB1
#define EPC_FLAME_DETECTION_STATUS_RESET            0xBF

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Electric Energy Sensor (0x22)
 */
#define EPC_CUMULATIVE_AMOUNT_ELECTRIC_ENERGY                       0xE0
#define EPC_MEDIUM_CAPACITY_SENSOR_INSTANTANEOUS_ELECTRIC_ENERGY    0xE1
#define EPC_SMALL_CAPACITY_SENSOR_INSTATANEOUS_ELECTRIC_ENERGY      0xE2
#define EPC_LARGE_CAPACITY_SENSOR_INSTATANEOUS_ELECTRIC_ENERGY      0xE3
#define EPC_CUMULATIVE_AMOUNT_ELECTRIC_ENERGY_MEASUREMENT_LOG       0xE4
#define EPC_EFFECTIVE_VOLTAGE_VALUE                                 0xE5

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Current Value Sensor (0x23)
 */
#define EPC_MEASURE_CURRENT_VALUE_1                 0xE0
#define EPC_RATED_VOLTAGE_TO_BE_MEASURED            0xE1
#define EPC_MEASURE_CURRENT_VALUE_2                 0xE2

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Water Flow Rate Sensor (0x25)
 */
#define EPC_CUMULATIVE_FLOW_RATE                    0xE0
#define EPC_FLOW_RATE                               0xE2

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Micromotion Sensor (0x26)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_MICROMOTION_DETECTION_STATUS            0xB1
#define EPC_DETECTION_COUNTER                       0xB2
#define EPC_SAMPLING_COUNT                          0xBC
#define EPC_SAMPLING_CYCLE                          0xBD

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Passage Sensor (0x27)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_PASSAGE_DETECTION_HOLD_TIME             0xBE
#define EPC_PASSAGE_DETECTION_DIRECTION             0xE0

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Bed Presence Sensor (0x28)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_BED_PRESENCE_DETECTION_STATUS           0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Open/Close Sensor (0x29)
*/
#define EPC_DEGREE_OF_OPENING_DETECTION_STATUS_1    0xE0
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_DEGREE_OF_OPENING_DETECTION_STATIS_2    0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Activity Amount Sensor (0x2A)
 */
#define EPC_ACTIVITY_AMOUNT_LEVEL_1                 0xE0
#define EPC_MAXIMUM_NUMBER_OF_HUMAN_BODY_ID         0xE1
#define EPC_ACTIVITY_AMOUNT_LEVEL_2                 0xE2
#define EPC_HUMAN_BODY_EXISTENCE_INFORMATION        0xE3

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Human Body Location Sensor (0x2B)
 */
#define EPC_HUMAN_BODY_DETECTION_LOCATION_1         0xE0
#define EPC_MAXIMUM_NUMBER_OF_HUMAN_BODY_ID         0xE1
#define EPC_HUMAN_BODY_DETECTION_LOCATION_2         0xE2
#define EPC_HUMAN_BODY_EXISTENCE_INFORMATION        0xE3

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Snow Sensor (0x2C)
 */
#define EPC_DETECTION_THRESHOLD_LEVEL               0xB0
#define EPC_SNOW_DETECTION_STATUS                   0xB1

/*
 Echonet Lite Property (EPC)
 Note: Sensor Class (0x00), Air Pressure Sensor (0x2D)
 */
#define EPC_AIR_PRESSURE_MEASUREMENT                0xE0
 
/*
 Echonet Lite Property (EPC)
 Note: Node profile class
 */
#define EPC_UNIQUE_IDENTIFIER_DATA                  0xBF

/*
 Echonet Lite Property (EPC)
 Note: Node profile class
 */
#define EPC_NUMBER_OF_SELF_NODE_INSTANCES           0xD3
#define EPC_NUMBER_OF_SELF_NODE_CLASSES             0xD4
#define EPC_INSTANCE_LIST_NOTIFICATION              0xD5
#define EPC_SELF_NODE_INSTANCE_LIST_S               0xD6
#define EPC_SELF_NODE_CLASS_LIST_S                  0xD7

/*
 Echonet Lite Property (EPC)
 Note: Air Conditioner Related Class (0x01), Home Air Conditioner (0x30)
 */
#define EPC_AIRCOND_OPERATION_POWER_SAVING                          0x8F
#define EPC_AIRCOND_OPERATION_MODE_SETTING                          0xB0
#define EPC_AIRCOND_AUTOMATIC_TEMPERATURE_CONTROL_SETTING           0xB1
#define EPC_AIRCOND_NORMAL_HIGHSPEED_SILENT_OPERATION_SETTING       0xB2
#define EPC_AIRCOND_SET_TEMPERATURE_VALUE                           0xB3
#define EPC_AIRCOND_SET_RELATIVE_HUMIDITY_IN_DEHUMIDIFYING_MODE     0xB4
#define EPC_AIRCOND_SET_TEMPERATURE_IN_COOLING_MODE                 0xB5
#define EPC_AIRCOND_SET_TEMPERATURE_IN_HEATING_MODE                 0xB6
#define EPC_AIRCOND_SET_TEMPERATURE_IN_DEHUMIDIFYING_MODE           0xB7
#define EPC_AIRCOND_RATED_POWER_CONSUMPTION                         0xB8
#define EPC_AIRCOND_MEASURED_VALUE_OF_CURRENT_CONSUMPTION           0xB9
#define EPC_AIRCOND_MEASURED_VALUE_OF_ROOM_RELATIVE_HUMIDITY        0xBA
#define EPC_AIRCOND_MEASURED_VALUE_OF_ROOM_TEMPERATURE              0xBB
#define EPC_AIRCOND_SET_TEMPERATURE_OF_USER_REMOTE_CONTROL          0xBC
#define EPC_AIRCOND_MEASURED_COOLED_AIR_TEMPERATURE                 0xBD
#define EPC_AIRCOND_MEASURED_OUTDOOR_AIR_TEMPERATURE                0xBE
#define EPC_AIRCOND_RELATIVE_TEMPERATURE_SETTING                    0xBF
#define EPC_AIRCOND_AIRFLOW_RATE_SETTING                            0xA0
#define EPC_AIRCOND_AUTOMATIC_CONTROL_AIRFLOW_DIRECTION_SETTING     0xA1
#define EPC_AIRCOND_AUTOMATIC_SWING_AIRFLOW_SETTING                 0xA3
#define EPC_AIRCOND_AIRFLOW_DIRECTION_VERTICAL_SETTING              0xA4
#define EPC_AIRCOND_AIRFLOW_DIRECTION_HORIZONTAL_SETTING            0xA5
#define EPC_AIRCOND_SPECIAL_STATE                                   0xAA
#define EPC_AIRCOND_NON_PRIORITY_STATE                              0xAB
#define EPC_AIRCOND_VENTILATION_FUNCTION_SETTING                    0xC0
#define EPC_AIRCOND_HUMIDIFIER_FUNCTION_SETTING                     0xC1
#define EPC_AIRCOND_VENTILATION_AIR_FLOW_RATE_SETTING               0xC2
#define EPC_AIRCOND_DEGREE_OF_HUMIDIFICATION_SETTING                0xC4
#define EPC_AIRCOND_MOUNTED_AIR_CLEANING_METHOD                     0xC6
#define EPC_AIRCOND_AIR_PURIFIER_FUNCTION_SETTING                   0xC7
#define EPC_AIRCOND_MOUNTED_AIR_REFRESH_METHOD                      0xC8
#define EPC_AIRCOND_AIR_REFRESHER_FUNCTION_SETTING                  0xC9
#define EPC_AIRCOND_MOUNTED_SELF_CLEANING_METHOD                    0xCA
#define EPC_AIRCOND_SELF_CLEANING_FUNCTION_SETTING                  0xCB
#define EPC_AIRCOND_SPECIAL_FUNCTION_SETTING                        0xCC
#define EPC_AIRCOND_OPERATION_STATUS_OF_COMPONENTS                  0xCD
#define EPC_AIRCOND_THERMOSTAT_SETTING_OVERRIDE_FUNCTION            0xCE
#define EPC_AIRCOND_AIR_PURIFICATION_MODE_SETTING                   0xCF
#define EPC_AIRCOND_ON_TIMER_BASED_RESERVATION_SETTING              0x90
#define EPC_AIRCOND_ON_TIMER_SETTING_TIME                           0x91
#define EPC_AIRCOND_ON_TIMER_SETTING_RELATIVE_TIME                  0x92
#define EPC_AIRCOND_OFF_TIMER_BASED_RESERVATION_SETTING             0x94
#define EPC_AIRCOND_OFF_TIMER_SETTING_TIME                          0x95
#define EPC_AIRCOND_OFF_TIMER_SETTING_RELATIVE_TIME                 0x96

/*
 Echonet Lite Operation Status (Property)
 */
#define OS_ON                                       0x30
#define OS_OFF                                      0x31

/*
 Echonet Lite Installation Location (Property)
 According to Specification Appendix Table 2-2
 Note: Location number set to 000 (last 3 bit are not specified)
 */
#define IL_LIVING_ROOM                              0b00001000
#define IL_DINING_ROOM                              0b00010000
#define IL_KITCHEN                                  0b00011000
#define IL_BATHROOM                                 0b00100000
#define IL_LAVATORY                                 0b00101000
#define IL_CHANGING_ROOM                            0b00110000
#define IL_PASSAGEWAY                               0b00111000
#define IL_ROOM                                     0b01000000
#define IL_STAIRWAY                                 0b01001000
#define IL_FRONT_DOOR                               0b01010000
#define IL_STORE_ROOM                               0b01011000
#define IL_GARDEN                                   0b01100000
#define IL_GARAGE                                   0b01101000
#define IL_BALCONY                                  0b01110000
#define IL_OTHERS                                   0b01111000
#define IL_INSTALLATION_LOCATION_NOT_SPECIFIED      0b00000000
#define IL_INSTALLATION_LOCATION_INDEFINITE         0b11111111
#define IL_POSITION_INFORMATION                     0b00000001

/*
 Echonet Lite Standard Version Information (Property)
 */


/*
 Echonet Lite Fault Status (Property)
 */
#define FS_FAULT_OCCURED                            0x41
#define FS_NO_FAULT_OCCURED                         0x42

/*
 Echonet Lite Fault Description (Property)
 Note: 2-byte,lower byte specified
 */
#define FD_NO_FAULT                                 0x00
#define FD_RECOVERABLE_FAULT1                       0x01
#define FD_RECOVERABLE_FAULT2                       0x02
#define FD_RECOVERABLE_FAULT3                       0x03
#define FD_RECOVERABLE_FAULT4                       0x04
#define FD_RECOVERABLE_FAULT5                       0x05
#define FD_RECOVERABLE_FAULT6                       0x06
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT1           0x0A
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT2           0x0B
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT3           0x0C
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT4           0x0D
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT5           0x0E
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT6           0x0F
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT7           0x10
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT8           0x11
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT9           0x12
#define FD_REQUIRE_REPAIR_ABNORMAL_EVENT10          0x13
#define FD_REQUIRE_REPAIR_SWITCH_FAULT1             0x14
#define FD_REQUIRE_REPAIR_SWITCH_FAULT2             0x15
#define FD_REQUIRE_REPAIR_SWITCH_FAULT3             0x16
#define FD_REQUIRE_REPAIR_SWITCH_FAULT4             0x17
#define FD_REQUIRE_REPAIR_SWITCH_FAULT5             0x18
#define FD_REQUIRE_REPAIR_SWITCH_FAULT6             0x19
#define FD_REQUIRE_REPAIR_SWITCH_FAULT7             0x1A
#define FD_REQUIRE_REPAIR_SWITCH_FAULT8             0x1B
#define FD_REQUIRE_REPAIR_SWITCH_FAULT9             0x1C
#define FD_REQUIRE_REPAIR_SWITCH_FAULT10            0x1D
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT1      0x1E
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT2      0x1F
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT3      0x20
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT4      0x21
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT5      0x22
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT6      0x23
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT7      0x24
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT8      0x25
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT9      0x26
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT10     0x27
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT11     0x28
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT12     0x29
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT13     0x2A
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT14     0x2B
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT15     0x2C
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT16     0x2D
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT17     0x2E
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT18     0x2F
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT19     0x30
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT20     0x31
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT21     0x32
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT22     0x33
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT23     0x34
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT24     0x35
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT25     0x36
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT26     0x37
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT27     0x38
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT28     0x39
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT29     0x3A
#define FD_REQUIRE_REPAIR_SENSOR_SYSTEM_FAULT30     0x3B
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT1          0x3C
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT2          0x3D
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT3          0x3E
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT4          0x3F
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT5          0x40
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT6          0x41
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT7          0x42
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT8          0x43
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT9          0x44
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT10         0x45
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT11         0x46
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT12         0x47
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT13         0x48
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT14         0x49
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT15         0x4A
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT16         0x4B
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT17         0x4C
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT18         0x4D
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT19         0x4E
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT20         0x4F
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT21         0x50
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT22         0x51
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT23         0x52
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT24         0x53
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT25         0x54
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT26         0x55
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT27         0x56
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT28         0x57
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT29         0x58
#define FD_REQUIRE_REPAIR_COMPONENT_FAULT30         0x59
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT1      0x5A
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT2      0x5B
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT3      0x5C
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT4      0x5D
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT5      0x5E
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT6      0x5F
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT7      0x60
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT8      0x61
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT9      0x62
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT10     0x63
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT11     0x64
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT12     0x65
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT13     0x66
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT14     0x67
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT15     0x68
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT16     0x69
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT17     0x6A
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT18     0x6B
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT19     0x6C
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT20     0x6D
#define FD_REQUIRE_REPAIR_CIRCUIT_BOARD_FAULT21     0x6E
#define FD_FAULT_OCCURED_NOT_ABLE_TO_DETERMINE      0x03FF

/*
 Echonet Lite Manufacturer Code (Property)
 */


/*
 Echonet Lite Business Facility Code (Property)
 */


/*
 Echonet Lite Product Code (Property)
 */


/*
 Echonet Lite Production Number (Property)
 */


/*
 Echonet Lite Production Date (Property)
 */


/*
 Echonet Lite Property Map (Property)
 */


/*
 Echonet Lite Identification Number (Property)
 */


/*
 Echonet Lite Manufacturer Fault Code (Property)
 */


/*
 Echonet Lite Current Limit Setting (Property)
 */


/*
 Echonet Lite Power Saving Operation Setting (Property)
 */
#define PSOS_POWER_SAVING_ON                        0x41
#define PSOS_POWER_SAVING_OFF                       0x42

/*
 Echonet Lite Remote Control Setting (Property)
 */
#define RCS_NOT_THROUGH_PUBLIC_NETWORK              0x41
#define RCS_THROUGH_PUBLIC_NETWORK                  0x42

/*
 Echonet Lite Cumulative Operating Time (Property)
 */
#define COT_SECONDS                                 0x41
#define COT_MINUTES                                 0x42
#define COT_HOURS                                   0x43
#define COT_DAYS                                    0x44

/*
 Echonet Lite Current Time Setting (Property)
 */


/*
 Echonet Lite Current Date Setting (Property)
 */


/*
 Echonet Lite Measured Instantaneous Power Consumption (Property)
 */
#define MIPC_UNDERFLOW                              0xFFFE
#define MIPC_OVERFLOW                               0xFFFF

/*
 Echonet Lite Measured Cumulative Power Consumption (Property)
 */


/*
 Echonet Lite Power Limit Setting (Property)
 */


/*
 Echonet Lite Prototype Functions
 */
//Echonet Lite Packet
void echonet_setEHD1(void);
uint8_t echonet_getEHD1(void);
void echonet_setEHD2(uint8_t value);
uint8_t echonet_getEHD2(void);
void echonet_setTID(uint16_t value);
uint16_t echonet_getTID(void);
void echonet_setSEOJ(char class_group_code,char class_code,char instance_code);
void echonet_getSEOJ(char *class_group_code,char *class_code,char *instance_code);
void echonet_setDEOJ(char class_group_code,char class_code,char instance_code);
void echonet_getDEOJ(char *class_group_code,char *class_code,char *instance_code);
void echonet_setESV(char service_code);
uint8_t echonet_getESV(void);
void echonet_setOPC(char value);
uint8_t echonet_getOPC(void);
void echonet_setnEPC(char n,char epc_code);
uint8_t echonet_getnEPC(char n);
void echonet_setnPDC(char n,char value);
uint8_t echonet_getnPDC(char n);
void echonet_setnEDT(char n,char *property_data);
void echonet_getnEDT(char n,char *property_data);
void echonet_setPacket(char ehd1,char ehd2,uint16_t tid,char seoj_cgc,char seoj_cc,char seoj_ic,char deoj_cgc,char deoj_cc,char deoj_ic,char esv,char opc);
void echonet_setPacket_getNSNI(void);
void echonet_setPacket_getNSNC(void);
void echonet_setPacket_getILN(void);
void echonet_setPacket_getSNILS(void);
void echonet_setPacket_getSNCLS(void);
void echonet_setPacket_getObject_operationalStatus(void);
void echonet_setPacket_getObject_sensorTemp(void);
void echonet_setPacket_getObject_sensorHumid(void);
void echonet_setPacket_getObject_sensorIllum_lux(void);
void echonet_setPacket_getObject_sensorIllum_kilolux(void);
void echonet_setPacket_getObject_sensorAirSpeed(void);
void echonet_setPacket_getObject_sensorAirSpeed_direction(void);
void echonet_setPacket_setObject_operationalStatus(void);
void echonet_setPacket_getObject_aircondCurrentConsumption(void);
void echonet_setPacket_getObject_aircondRoomHumidity(void);
void echonet_setPacket_getObject_aircondRoomTemperature(void);
void echonet_setPacket_getObject_aircondOutdoorAirTemperature(void);

//Echonet Lite Get
void echonet_getNodeInfo_NSNI(char *ip_addr,uint16_t TID);
void echonet_getNodeInfo_NSNC(char *ip_addr,uint16_t TID);
void echonet_getNodeInfo_ILN(char *ip_addr,uint16_t TID);
void echonet_getNodeInfo_SNILS(char *ip_addr,uint16_t TID);
void echonet_getNodeInfo_SNCLS(char *ip_addr,uint16_t TID);
uint8_t echonet_getObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code);
float echonet_getObject_sensorTemp_reading(char *ip_addr,uint16_t TID,unsigned char instance_code);
float echonet_getObject_sensorHumid_reading(char *ip_addr,uint16_t TID,unsigned char instance_code);
uint16_t echonet_getObject_sensorIllum_reading(char *ip_addr,uint16_t TID,unsigned char instance_code);
float echonet_getObject_sensorAirSpeed_reading(char *ip_addr,uint16_t TID,unsigned char instance_code);
uint16_t echonet_getObject_sensorAirSpeed_direction(char *ip_addr,uint16_t TID,unsigned char instance_code);
float echonet_getObject_aircondCurrentConsumption(char *ip_addr,uint16_t TID,unsigned char instance_code);
uint8_t echonet_getObject_aircondRoomHumidity(char *ip_addr,uint16_t TID,unsigned char instance_code);
int8_t echonet_getObject_aircondRoomTemperature(char *ip_addr,uint16_t TID,unsigned char instance_code);
int8_t echonet_getObject_aircondCooledAirTemperature(char *ip_addr,uint16_t TID,unsigned char instance_code);
int8_t echonet_getObject_aircondOutdoorAirTemperature(char *ip_addr,uint16_t TID,unsigned char instance_code);

//Echonet Lite Get (Multi-thread Version)
void echonetMT_getObject_sensorAirSpeed_reading(char *ip_addr,uint16_t TID,unsigned char instance_code);
void echonetMT_getObject_sensorTemp_reading(char *ip_addr,uint16_t TID,unsigned char instance_code);
void echonetMT_getObject_sensorHumid_reading(char *ip_addr,uint16_t TID,unsigned char instance_code);
void echonetMT_getObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code);
void echonetMT_getObject_powerDistributionMeter_reading(char *ip_addr,uint16_t TID,unsigned char instance_code,uint8_t channel);

//Echonet Lite Set
uint8_t echonet_setObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code,uint8_t operation_status);
void echonetMT_setObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code,uint8_t operation_status);
void echonetMT_setObject_param_1(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code,unsigned char epc,unsigned char operation_status);

//Echonet Lite Packet Check
uint8_t echonet_checkPacket_NodeProfileClass(void);

//Echonet Lite iHouse
void echonet_getiHouse_2_202(uint16_t TID,float *sensor_AirSpeed1,float *sensor_AirSpeed2,float *sensor_AirSpeed3,uint8_t *UDP_network_fatal_airspeed_indoor);
void echonet_getiHouse_2_145(uint16_t TID,float *sensor_AirSpeedOutdoor,float *sensor_TemperatureOutdoor,float *sensor_HumidityOutdoor,uint8_t *UDP_network_fatal_airspeed_outdoor,uint8_t *UDP_network_fatal_temperature_outdoor);
void echonet_getiHouse_2_108(uint16_t TID,float *sensor_TemperatureIndoor,float *sensor_HumidityIndoor,uint8_t *UDP_network_fatal_temperature_indoor);
void echonet_getiHouse_2_174(uint16_t TID,uint8_t *state_aircond,float *aircond_current_consumption,uint8_t *aircond_room_humidity,int8_t *aircond_room_temperature,int8_t *aircond_cooled_air_temperature,int8_t *aircond_outdoor_air_temperature,uint8_t *UDP_network_fatal_aircond);
void echonet_getiHouse_2_166(uint16_t TID,uint8_t *state_window1,uint8_t *UDP_network_fatal_window1);
void echonet_getiHouse_2_167(uint16_t TID,uint8_t *state_window2,uint8_t *UDP_network_fatal_window2);
void echonet_getiHouse_2_158(uint16_t TID,uint8_t *state_curtain,uint8_t *UDP_network_fatal_curtain);

//Echonet Lite iHouse (Multi-thread version)
void echonetMT_getiHouse_2_202(uint16_t TID);
void echonetMT_getiHouse_2_145(uint16_t TID);
void echonetMT_getiHouse_2_108(uint16_t TID);
void echonetMT_getiHouse_2_174(uint16_t TID);
void echonetMT_getiHouse_2_166(uint16_t TID);
void echonetMT_getiHouse_2_167(uint16_t TID);
void echonetMT_getiHouse_2_158(uint16_t TID);
//void echonetMT_getiHouse_2_146(uint16_t TID);

//Echonet Lite Detail Print
void echonet_savePacket(void);
void echonet_printDeviceClassGroup(unsigned char class_group_code);
void echonet_printDeviceClass(unsigned char class_group_code,unsigned char class_code);
void echonet_printPacket(uint16_t value);

//Echonet Lite Packet Clear
void echonet_clearPacket_pdata(void);
void echonet_clearPacket_edata(void);
void echonet_clearPacket_all(void);



#endif /* echonet_lite_h */
