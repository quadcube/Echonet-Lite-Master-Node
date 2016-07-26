//
//  EETCC.h
//  Echonet Lite Master Node
//
//  Created by QuadCube on 11/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#ifndef EETCC_h
#define EETCC_h

#include <math.h>
#include <stdio.h>
#include <time.h>
#include "network.h"
#include "echonet_lite.h"

#define MINIMUM_TIME_USING_AIRCOND              4   //minutes
#define MINIMUM_TIME_WITHOUT_USING_AIRCOND      3   //minutes

#define CLOTHING_INSULATION                     0.9
#define METABOLIC_RATE                          1.0


void EETCC_init(void);
void EETCC_thermalComfort(float clothing_insulation,float metabolic_rate,float velocity,float temperature_mean_radiation,float relative_humidity,float air_temperature);
double EETCC_PMV(void);
double EETCC_PPD(void);
uint8_t EETCC_prev_controlSignal(void);
double EETCC_draught(float local_air_temperature,float local_mean_air_velocity);

float EETCC_globalA(double PMV);
float EETCC_globalA_delay(void);
float EETCC_Q1(float occupant,float local_air_temperature,float outdoor_air_temperature,float correction,float solar_south,float solar_west);
float EETCC_Q2(float occupant,float local_air_temperature,float outdoor_air_temperature,float correction,float solar_south,float solar_west);
float EETCC_timer(void);
uint8_t EETCC_controlSignal(double PMV1,double PMV2,double PMV3,double PMV4,uint8_t prev_EETCC_controlSignal,float timer,uint8_t index,float local_mean_air_velocity,float temperature_mean_radiation,float GlobalA,float GlobalA_delay);
uint8_t EETCC_index(float temperature_mean_radiation,float desired_temperature,float Q1,float Q2);

#endif /* EETCC_h */
