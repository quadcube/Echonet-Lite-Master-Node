//
//  main.c
//  Echonet Lite Master Node
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <mysql.h>
#include <my_global.h>
#include "network.h"
#include "error.h"
#include "echonet_lite.h"
#include "EETCC.h"
//#include "EETCC_MATLAB.h"


#define TIME_INTERVAL_MS    10000  //10 seconds per EETCC calculation
#define LINE_BUFSIZE        10
#define DESIRED_TEMPERATURE 25.07

void ECHONETMT_LITE_MAIN_ROUTINE(void);
void ECHONET_LITE_MAIN_ROUTINE(void);
void System_INT_Handler(int sig);
void close_sql(MYSQL *con);
void *SERVER_PORT_LISTEN(void *threadid);
void *ECHONET_LITE_DEVICE_CONTROL(void *threadid);

uint16_t run_counter=1,sql_ID;
char sql[1000];
MYSQL *con;

int main(void)
{
    //Init. setup
    printf("##################################################\n\nEnergy Efficient Thermal Comfort Control (EETCC)\n\n##################################################\n");
    init_network();
    EETCC_init();
    signal(SIGINT,System_INT_Handler);
    printf("MySQL client version: %s\n", mysql_get_client_info());
    con = mysql_init(NULL);	//SQL initialization
    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }
    if (mysql_real_connect(con, "localhost", "tanlim_lab", "eetcc", "EETCC_PROJECT", 0, NULL, 0) == NULL)	//SQL connection
        exit_sql_error(con);
    else
        printf("Connected to MySQL database!\n");
    sql_ID=mysql_insert_id(con);
    //Thread Handler
    struct itimerval it_val;
    /*if (signal(SIGALRM, (void (*)(int))ECHONET_LITE_MAIN_ROUTINE) == SIG_ERR) {
        perror("Error catching SIGALRM for ECHONET_LITE_MAIN_ROUTINE\n");
        exit(1);
    }*/
    if (signal(SIGALRM, (void (*)(int))ECHONETMT_LITE_MAIN_ROUTINE) == SIG_ERR) {
        perror("Error catching SIGALRM for ECHONET_LITE_MAIN_ROUTINE\n");
        exit(1);
    }
    it_val.it_value.tv_sec=TIME_INTERVAL_MS/1000;
    it_val.it_value.tv_usec=(TIME_INTERVAL_MS*1000)%1000000;
    it_val.it_interval = it_val.it_value;
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        perror("Error initializing setitimer()\n");
        exit(1);
    }
    
    pthread_t threads[2];
    if(pthread_create(&threads[0], NULL,SERVER_PORT_LISTEN,(void *)0))
    {
        printf("Error on thread creation for SERVER_PORT_LISTEN\n");
        exit(-1);
    }
    if(pthread_create(&threads[1], NULL,ECHONET_LITE_DEVICE_CONTROL,(void *)1))
    {
        printf("Error on thread creation for ECHONET_LITE_DEVICE_CONTROL\n");
        exit(-1);
    }
    
    while (1) 
        pause();
    return 0;
}

/*
 Multi-thread version
 */
void ECHONETMT_LITE_MAIN_ROUTINE(void)
{
    //Multi-thread global variable
    float MTsensor_AirSpeedIndoor,MTsensor_AirSpeedOutdoor,MTsensor_AirSpeed1,MTsensor_AirSpeed2,MTsensor_AirSpeed3,MTsensor_TemperatureIndoor,MTsensor_HumidityIndoor,MTsensor_HumidityOutdoor,MTsensor_TemperatureOutdoor;
    
    //EETCC Variable
    float sensor_AirSpeedIndoor,sensor_AirSpeedOutdoor,sensor_AirSpeed1,sensor_AirSpeed2,sensor_AirSpeed3,sensor_TemperatureIndoor,sensor_HumidityIndoor,sensor_HumidityOutdoor,sensor_TemperatureOutdoor,sensor_SolarRadiation,sensor_SolarVoltage,timer,globalA,globalA_delay,Q1,Q2;
    float PMV_gain;
    double PMV1,PMV2,PMV3,PMV4,PMV,PPD,PPD1,PPD2,PPD3,PPD4,draught;
    uint8_t ControlSignal,prev_ControlSignal,index;
    
    //MySQL Variable
    uint8_t state_window1,state_window2,state_curtain,state_aircond,setting_aircond_temperature,aircond_room_humidity;
    
    int8_t aircond_room_temperature,aircond_cooled_air_temperature,aircond_outdoor_air_temperature;
    
    uint8_t UDP_network_timeout_aircond,UDP_network_timeout_airspeed_indoor,UDP_network_timeout_airspeed_outdoor,UDP_network_timeout_temperature_indoor,UDP_network_timeout_temperature_outdoor,UDP_network_timeout_window1,UDP_network_timeout_window2,UDP_network_timeout_curtain;
    
    uint8_t UDP_network_fatal_aircond,UDP_network_fatal_airspeed_indoor,UDP_network_fatal_airspeed_outdoor,UDP_network_fatal_temperature_indoor,UDP_network_fatal_temperature_outdoor,UDP_network_fatal_window1,UDP_network_fatal_window2,UDP_network_fatal_curtain;
    
    float aircond_current_comsumption;
    char scriptOutput[LINE_BUFSIZE];
    
    printf("Start of EETCC routine. No.%d\n",run_counter);
    
    
    
    
    
    
    //get Solar reading from Makino script
    FILE *pipe;
    pipe = popen("~/EETCC/SolarVoltage.sh", "r");
    while (fgets(scriptOutput, LINE_BUFSIZE, pipe) != NULL)
    {
        //printf("Solar voltage: %s\n", scriptOutput);
    }
    pclose(pipe);
    sensor_SolarVoltage=((scriptOutput[1]-'0'))+((scriptOutput[3]-'0')*0.1)+((scriptOutput[4]-'0')*0.01)+((scriptOutput[5]-'0')*0.001)+((scriptOutput[6]-'0')*0.0001);
    if(scriptOutput[0]=='-')
        sensor_SolarVoltage=sensor_SolarVoltage*-1;
    sensor_SolarRadiation=(137598*pow(sensor_SolarVoltage,2))+(3216.1*sensor_SolarVoltage)+0.6377;
    //3 Indoor Airspeed sensor (IP:192.168.2.202) TID: 10
    echonetMT_getiHouse_2_202(10);
    //1 Outdoor Airspeed sensor,1 Outdoor Temperature sensor,1 Outdoor Humidity sensor (IP:192.168.2.145) TID: 20
    echonetMT_getiHouse_2_145(20);
    //1 Indoor Temperature sensor,1 Indoor Humidity sensor (IP:192.168.2.108) TID: 30
    echonetMT_getiHouse_2_108(30);
    //Air Conditioner (IP:192.168.2.174) TID: 40
    //echonetMT_getiHouse_2_174(40, &state_aircond, &aircond_current_comsumption, &aircond_room_humidity, &aircond_room_temperature, &aircond_cooled_air_temperature, &aircond_outdoor_air_temperature, &UDP_network_fatal_aircond);
    //2 Switch, Window1 (IP:192.168.2.166) TID: 50
    echonetMT_getiHouse_2_166(50);
    //2 Switch, Window2 (IP:192.168.2.167) TID: 60
    echonetMT_getiHouse_2_167(60);
    //2 Switch, Curtain (IP:192.168.2.158) TID: 70
    echonetMT_getiHouse_2_158(70);
    
    
    
    
    
    
    
    
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, 0.0, sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV1
    PMV1=EETCC_PMV();
    PPD1=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, 0.0, sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV2
    PMV2=EETCC_PMV();
    PPD2=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*0.8*0.5/0.1), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV3
    PMV3=EETCC_PMV();
    PPD3=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*1.0*0.5/0.1), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV4
    PMV4=EETCC_PMV();
    PPD4=EETCC_PPD();
    Q1=EETCC_Q1(1, sensor_TemperatureIndoor, sensor_TemperatureOutdoor, 3, sensor_SolarRadiation, sensor_SolarRadiation);
    Q2=EETCC_Q2(1, sensor_TemperatureIndoor, sensor_TemperatureOutdoor, 3, sensor_SolarRadiation, sensor_SolarRadiation);
    index=EETCC_index(sensor_TemperatureIndoor, DESIRED_TEMPERATURE, Q1, Q2);
    prev_ControlSignal=EETCC_prev_controlSignal();
    timer=EETCC_timer();
    globalA=EETCC_globalA(PMV);
    globalA_delay=EETCC_globalA_delay();
    ControlSignal=EETCC_controlSignal(PMV1,PMV2,PMV3,PMV4,prev_ControlSignal,timer,index,sensor_AirSpeedOutdoor,sensor_TemperatureIndoor,globalA,globalA_delay);
    if(ControlSignal==3)
        PMV_gain=0.8;
    else if(ControlSignal==4)
        PMV_gain=1;
    else
        PMV_gain=0;
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*PMV_gain*0.5), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);
    PMV=EETCC_PMV();
    PPD=EETCC_PPD();
    draught=EETCC_draught(sensor_TemperatureIndoor, sensor_AirSpeedIndoor);
    
    
    
    sprintf(sql,"INSERT INTO iHouseData_test VALUES(%d,%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%f,%d,%f,%f,%f,%f,%f,NOW())",(sql_ID+run_counter-1),state_window1,state_window2,state_curtain,state_aircond,setting_aircond_temperature,aircond_current_comsumption,aircond_room_humidity,aircond_room_temperature,aircond_cooled_air_temperature,aircond_outdoor_air_temperature,sensor_TemperatureIndoor,sensor_TemperatureOutdoor,sensor_AirSpeedIndoor,sensor_AirSpeedOutdoor,sensor_HumidityIndoor,sensor_HumidityOutdoor,sensor_SolarVoltage,sensor_SolarRadiation,UDP_network_timeout_aircond,UDP_network_timeout_airspeed_indoor,UDP_network_timeout_airspeed_outdoor,UDP_network_timeout_temperature_indoor,UDP_network_timeout_temperature_outdoor,UDP_network_timeout_window1,UDP_network_timeout_window2,UDP_network_timeout_curtain,UDP_network_fatal_aircond,UDP_network_fatal_airspeed_indoor,UDP_network_fatal_airspeed_outdoor,UDP_network_fatal_temperature_indoor,UDP_network_fatal_temperature_outdoor,UDP_network_fatal_window1,UDP_network_fatal_window2,UDP_network_fatal_curtain,PMV,PMV1,PMV2,PMV3,PMV4,PPD,PPD1,PPD2,PPD3,PPD4,ControlSignal,prev_ControlSignal,timer,index,globalA,globalA_delay,Q1,Q2,draught);
    if(mysql_query(con, sql))
    {
        exit_sql_error(con);
    }
    memset(sql,'\0',1000);   //clear SQL command array*/
    
    
    printf("End of EETCC routine. No.%d\n\n",run_counter);
    run_counter++;
}

void System_INT_Handler(int sig)
{
    char input;
    signal(sig, SIG_IGN);
    printf("Quit EETCC Main Routine? [y/n]");
    input=getchar();
    if(input=='y' || input=='Y')
    {
        close_sql(con);
        exit(0);
    }
    else
        signal(SIGINT,System_INT_Handler);
    getchar();
}

void close_sql(MYSQL *con)
{
    mysql_close(con);
}

void *ECHONET_LITE_DEVICE_CONTROL(void *threadid)
{
    long tid;
    tid = (long)threadid;
    
    pthread_exit(NULL);
}

void *SERVER_PORT_LISTEN(void *threadid)
{
    long tid;
    tid = (long)threadid;
    udp_port_listener_echonet_lite();
    pthread_exit(NULL);
}

/*
 Single thread version
 */
void ECHONET_LITE_MAIN_ROUTINE(void)
{
    //EETCC Variable
    float sensor_AirSpeedIndoor,sensor_AirSpeedOutdoor,sensor_AirSpeed1,sensor_AirSpeed2,sensor_AirSpeed3,sensor_TemperatureIndoor,sensor_HumidityIndoor,sensor_HumidityOutdoor,sensor_TemperatureOutdoor,sensor_SolarRadiation,sensor_SolarVoltage,timer,globalA,globalA_delay,Q1,Q2;
    float PMV_gain;
    double PMV1,PMV2,PMV3,PMV4,PMV,PPD,PPD1,PPD2,PPD3,PPD4,draught;
    uint8_t ControlSignal,prev_ControlSignal,index;
    //MySQL Variable
    uint8_t state_window1,state_window2,state_curtain,state_aircond,setting_aircond_temperature,aircond_room_humidity;
    int8_t aircond_room_temperature,aircond_cooled_air_temperature,aircond_outdoor_air_temperature;
    uint8_t UDP_network_timeout_aircond,UDP_network_timeout_airspeed_indoor,UDP_network_timeout_airspeed_outdoor,UDP_network_timeout_temperature_indoor,UDP_network_timeout_temperature_outdoor,UDP_network_timeout_window1,UDP_network_timeout_window2,UDP_network_timeout_curtain;
    uint8_t UDP_network_fatal_aircond,UDP_network_fatal_airspeed_indoor,UDP_network_fatal_airspeed_outdoor,UDP_network_fatal_temperature_indoor,UDP_network_fatal_temperature_outdoor,UDP_network_fatal_window1,UDP_network_fatal_window2,UDP_network_fatal_curtain;
    float aircond_current_comsumption;
    char scriptOutput[LINE_BUFSIZE];
    
    printf("Start of EETCC routine. No.%d\n",run_counter);
    //get Solar reading from Makino script
    FILE *pipe;
    pipe = popen("~/EETCC/SolarVoltage.sh", "r");
    while (fgets(scriptOutput, LINE_BUFSIZE, pipe) != NULL)
    {
        //printf("Solar voltage: %s\n", scriptOutput);
    }
    pclose(pipe);
    sensor_SolarVoltage=((scriptOutput[1]-'0'))+((scriptOutput[3]-'0')*0.1)+((scriptOutput[4]-'0')*0.01)+((scriptOutput[5]-'0')*0.001)+((scriptOutput[6]-'0')*0.0001);
    if(scriptOutput[0]=='-')
        sensor_SolarVoltage=sensor_SolarVoltage*-1;
    sensor_SolarRadiation=(137598*pow(sensor_SolarVoltage,2))+(3216.1*sensor_SolarVoltage)+0.6377;
    //3 Indoor Airspeed sensor (IP:192.168.2.202) TID: 10
    echonet_getiHouse_2_202(10, &sensor_AirSpeed1, &sensor_AirSpeed2, &sensor_AirSpeed3, &UDP_network_fatal_airspeed_indoor);
    //1 Outdoor Airspeed sensor,1 Outdoor Temperature sensor,1 Outdoor Humidity sensor (IP:192.168.2.145) TID: 20
    echonet_getiHouse_2_145(20, &sensor_AirSpeedOutdoor, &sensor_TemperatureOutdoor, &sensor_HumidityOutdoor, &UDP_network_fatal_airspeed_outdoor, &UDP_network_fatal_temperature_outdoor);
    //1 Indoor Temperature sensor,1 Indoor Humidity sensor (IP:192.168.2.108) TID: 30
    echonet_getiHouse_2_108(30, &sensor_TemperatureIndoor, &sensor_HumidityIndoor, &UDP_network_fatal_temperature_indoor);
    //Air Conditioner (IP:192.168.2.174) TID: 40
    //echonet_getiHouse_2_174(40, &state_aircond, &aircond_current_comsumption, &aircond_room_humidity, &aircond_room_temperature, &aircond_cooled_air_temperature, &aircond_outdoor_air_temperature, &UDP_network_fatal_aircond);
    //2 Switch, Window1 (IP:192.168.2.166) TID: 50
    echonet_getiHouse_2_166(50, &state_window1, &UDP_network_fatal_window1);
    //2 Switch, Window2 (IP:192.168.2.167) TID: 60
    echonet_getiHouse_2_167(60, &state_window2, &UDP_network_fatal_window2);
    //2 Switch, Curtain (IP:192.168.2.158) TID: 70
    echonet_getiHouse_2_158(70, &state_curtain, &UDP_network_fatal_curtain);
    
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, 0.0, sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV1
    PMV1=EETCC_PMV();
    PPD1=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, 0.0, sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV2
    PMV2=EETCC_PMV();
    PPD2=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*0.8*0.5/0.1), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV3
    PMV3=EETCC_PMV();
    PPD3=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*1.0*0.5/0.1), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV4
    PMV4=EETCC_PMV();
    PPD4=EETCC_PPD();
    Q1=EETCC_Q1(1, sensor_TemperatureIndoor, sensor_TemperatureOutdoor, 3, sensor_SolarRadiation, sensor_SolarRadiation);
    Q2=EETCC_Q2(1, sensor_TemperatureIndoor, sensor_TemperatureOutdoor, 3, sensor_SolarRadiation, sensor_SolarRadiation);
    index=EETCC_index(sensor_TemperatureIndoor, DESIRED_TEMPERATURE, Q1, Q2);
    prev_ControlSignal=EETCC_prev_controlSignal();
    timer=EETCC_timer();
    globalA=EETCC_globalA(PMV);
    globalA_delay=EETCC_globalA_delay();
    ControlSignal=EETCC_controlSignal(PMV1,PMV2,PMV3,PMV4,prev_ControlSignal,timer,index,sensor_AirSpeedOutdoor,sensor_TemperatureIndoor,globalA,globalA_delay);
    if(ControlSignal==3)
        PMV_gain=0.8;
    else if(ControlSignal==4)
        PMV_gain=1;
    else
        PMV_gain=0;
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*PMV_gain*0.5), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);
    PMV=EETCC_PMV();
    PPD=EETCC_PPD();
    draught=EETCC_draught(sensor_TemperatureIndoor, sensor_AirSpeedIndoor);
    
    
    
    sprintf(sql,"INSERT INTO iHouseData_test VALUES(%d,%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%f,%d,%f,%f,%f,%f,%f,NOW())",(sql_ID+run_counter-1),state_window1,state_window2,state_curtain,state_aircond,setting_aircond_temperature,aircond_current_comsumption,aircond_room_humidity,aircond_room_temperature,aircond_cooled_air_temperature,aircond_outdoor_air_temperature,sensor_TemperatureIndoor,sensor_TemperatureOutdoor,sensor_AirSpeedIndoor,sensor_AirSpeedOutdoor,sensor_HumidityIndoor,sensor_HumidityOutdoor,sensor_SolarVoltage,sensor_SolarRadiation,UDP_network_timeout_aircond,UDP_network_timeout_airspeed_indoor,UDP_network_timeout_airspeed_outdoor,UDP_network_timeout_temperature_indoor,UDP_network_timeout_temperature_outdoor,UDP_network_timeout_window1,UDP_network_timeout_window2,UDP_network_timeout_curtain,UDP_network_fatal_aircond,UDP_network_fatal_airspeed_indoor,UDP_network_fatal_airspeed_outdoor,UDP_network_fatal_temperature_indoor,UDP_network_fatal_temperature_outdoor,UDP_network_fatal_window1,UDP_network_fatal_window2,UDP_network_fatal_curtain,PMV,PMV1,PMV2,PMV3,PMV4,PPD,PPD1,PPD2,PPD3,PPD4,ControlSignal,prev_ControlSignal,timer,index,globalA,globalA_delay,Q1,Q2,draught);
    if(mysql_query(con, sql))
    {
        exit_sql_error(con);
    }
    memset(sql,'\0',1000);   //clear SQL command array*/
    
    
    printf("End of EETCC routine. No.%d\n\n",run_counter);
    run_counter++;
}