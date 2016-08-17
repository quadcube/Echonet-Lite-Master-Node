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
#include <math.h>
#include <my_global.h>
#include "network.h"
#include "error.h"
#include "echonet_lite.h"
#include "EETCC.h"
//#include "EETCC_MATLAB.h"

//EETCC and Echonet Lite Device Setting
#define TIME_INTERVAL_MS            10000  //10 seconds per EETCC calculation
#define TIME_INTERVAL_RETRANSMIT    10  //in seconds
#define TIME_INTERVAL_WINDOW        30  //TIME_INTERVAL_RETRANSMIT + TIME_INTERVAL_WINDOW
#define TIME_INTERVAL_CURTAIN       5   //TIME_INTERVAL_RETRANSMIT + TIME_INTERVAL_CURTAIN
#define DESIRED_TEMPERATURE         25.07

//Main Program Prototype Function
void ECHONETMT_LITE_MAIN_ROUTINE(void);
void ECHONET_LITE_MAIN_ROUTINE(void);
void System_INT_Handler(int sig);
void close_sql(MYSQL *con);
void *SERVER_PORT_LISTEN(void *threadid);
void *ECHONET_DEVICE_CONTROL(void *threadid);

//MySQL global variable
uint16_t run_counter=1,sql_ID;
uint8_t TID_counter=0;
char sql[1000];
MYSQL *con;

//Multi-thread global variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
time_t currentTime1,previousTime1,currentTime2,previousTime2,currentTime3,previousTime3,currentTime4,previousTime4,currentTime5,previousTime5,currentTime6,previousTime6,currentTime7,previousTime7;
float MTUDP_network_timeout_aircond=0,MTUDP_network_timeout_airspeed_indoor=0,MTUDP_network_timeout_airspeed_outdoor=0,MTUDP_network_timeout_temperature_indoor=0,MTUDP_network_timeout_temperature_outdoor=0,MTUDP_network_timeout_window1=0,MTUDP_network_timeout_window2=0,MTUDP_network_timeout_curtain=0;
float MTsensor_AirSpeedIndoor=0,MTsensor_AirSpeedOutdoor=0,MTsensor_AirSpeed1=0,MTsensor_AirSpeed2=0,MTsensor_AirSpeed3=0,MTsensor_TemperatureIndoor=0,MTsensor_HumidityIndoor=0,MTsensor_HumidityOutdoor=0,MTsensor_TemperatureOutdoor=0;
uint8_t MTstate_window1=0,MTstate_window2=0,MTstate_curtain=0,MTstate_aircond=0,MTsetting_aircond_temperature=0,MTaircond_room_humidity=0;
uint8_t MTupdatedstate_window1=0,MTupdatedstate_window2=0,MTupdatedstate_curtain=0,MTupdatedstate_aircond=0;
uint8_t EETCC_ControlSignal=5;
//uint8_t MTsensor_Rain=0;
char UDPnetwork_buffer[DEFAULT_BUFFER_SIZE];
uint16_t UDPpacket_TID;
unsigned long accumulative_power=0;
unsigned long UDPpacket_length;

/*
 Main program
 Note: 1) Connect to localhost MySQL database
       2) Use alarm signal to run EETCC calculation thread every 10 second
       3) UDP listener thread and packet analyzer
       4) Echonet device control thread with retry timer
 */
int main(void)
{
    //Init. setup
    printf("##################################################\n\nEnergy Efficient Thermal Comfort Control (EETCC)\n\n##################################################\n");
    initMT_network();
    EETCC_init();
    signal(SIGINT,System_INT_Handler);  //setup signal to catch CTRL-C command to close MySQL connection and do proper shutdown
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
    if (signal(SIGALRM, (void (*)(int))ECHONETMT_LITE_MAIN_ROUTINE) == SIG_ERR)
    {
        perror("Error catching SIGALRM for ECHONET_LITE_MAIN_ROUTINE\n");
        exit(1);
    }
    it_val.it_value.tv_sec=TIME_INTERVAL_MS/1000;               //timer setup for EETCC algorithm loop
    it_val.it_value.tv_usec=(TIME_INTERVAL_MS*1000)%1000000;    //timer setup for EETCC algorithm loop
    it_val.it_interval = it_val.it_value;                       //timer setup for EETCC algorithm loop
    if (setitimer(ITIMER_REAL, &it_val, NULL) == -1)            //timer setup for EETCC algorithm loop
    {
        perror("Error initializing setitimer()\n");
        exit(1);
    }
    pthread_t threads[2];
    if(pthread_create(&threads[0], NULL,SERVER_PORT_LISTEN,(void *)0))
    {
        printf("Error on thread creation for SERVER_PORT_LISTEN\n");
        exit(-1);
    }
    //Initial UDP packet transmission
    //3 Indoor Airspeed sensor (IP:192.168.2.202) TID: 10
    echonetMT_getiHouse_2_202(10);
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime1);
    pthread_mutex_unlock(&mutex);
    //1 Outdoor Airspeed sensor,1 Outdoor Temperature sensor,1 Outdoor Humidity sensor (IP:192.168.2.145) TID: 20
    echonetMT_getiHouse_2_145(20);
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime2);
    pthread_mutex_unlock(&mutex);
    //1 Indoor Temperature sensor,1 Indoor Humidity sensor (IP:192.168.2.108) TID: 30
    echonetMT_getiHouse_2_108(30);
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime3);
    pthread_mutex_unlock(&mutex);
    //Air Conditioner (IP:192.168.2.174) TID: 40
    echonetMT_getiHouse_2_174(40);
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime4);
    pthread_mutex_unlock(&mutex);
    //2 Switch, Window1 (IP:192.168.2.166) TID: 50
    echonetMT_getiHouse_2_166(50);
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime5);
    pthread_mutex_unlock(&mutex);
    //2 Switch, Window2 (IP:192.168.2.167) TID: 60
    echonetMT_getiHouse_2_167(60);
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime6);
    pthread_mutex_unlock(&mutex);
    //2 Switch, Curtain (IP:192.168.2.158) TID: 70
    echonetMT_getiHouse_2_158(70);
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime7);
    pthread_mutex_unlock(&mutex);
    echonetMT_getObject_powerDistributionMeter_reading("192.168.2.176",256,1,3);
    //sleep 2 second before starting ECHONET DEVICE CONTROL
    sleep(2);
    if(pthread_create(&threads[1], NULL,ECHONET_DEVICE_CONTROL,(void *)0))
    {
        printf("Error on thread creation for ECHONET_DEVICE_CONTROL\n");
        exit(-1);
    }
    while (1) 
        pause();
    return 0;
}

/*
 Multi-thread version EETCC calculation
 Note: 1) send Get request to Echonet devices
       2) retrieve value from global variable and calculate EETCC control state
       3) update SQL database
 */
void ECHONETMT_LITE_MAIN_ROUTINE(void)
{
    //EETCC Variable
    float sensor_AirSpeedIndoor,sensor_AirSpeedOutdoor,sensor_AirSpeed1,sensor_AirSpeed2,sensor_AirSpeed3,sensor_TemperatureIndoor,sensor_HumidityIndoor,sensor_HumidityOutdoor,sensor_TemperatureOutdoor,sensor_SolarRadiation,sensor_SolarVoltage,timer,globalA,globalA_delay,Q1,Q2;
    float PMV_gain;
    double PMV1,PMV2,PMV3,PMV4,PMV,PPD,PPD1,PPD2,PPD3,PPD4,draught;
    uint8_t ControlSignal,prev_ControlSignal,index;
    
    //MySQL Variable
    uint8_t state_window1=0,state_window2=0,state_curtain=0,state_aircond=0,setting_aircond_temperature=0,aircond_room_humidity=0;
    
    int8_t aircond_room_temperature=0,aircond_cooled_air_temperature=0,aircond_outdoor_air_temperature=0;
    
    float UDP_network_timeout_aircond=0,UDP_network_timeout_airspeed_indoor=0,UDP_network_timeout_airspeed_outdoor=0,UDP_network_timeout_temperature_indoor=0,UDP_network_timeout_temperature_outdoor=0,UDP_network_timeout_window1=0,UDP_network_timeout_window2=0,UDP_network_timeout_curtain=0;
    
    uint8_t UDP_network_fatal_aircond=0,UDP_network_fatal_airspeed_indoor=0,UDP_network_fatal_airspeed_outdoor=0,UDP_network_fatal_temperature_indoor=0,UDP_network_fatal_temperature_outdoor=0,UDP_network_fatal_window1=0,UDP_network_fatal_window2=0,UDP_network_fatal_curtain=0;
    
    float aircond_current_comsumption=0.0;
    unsigned long Accumulative_power;
    char scriptOutput[10];
    uint8_t TID_counter_local;
    //uint8_t sensor_Rain;
    
    printf("EETCC routine. No.%d\n",run_counter);
    
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    Accumulative_power=accumulative_power;
    sensor_AirSpeedIndoor=(MTsensor_AirSpeed1+MTsensor_AirSpeed2+MTsensor_AirSpeed3)/3;
    sensor_AirSpeedOutdoor=MTsensor_AirSpeedOutdoor;
    sensor_AirSpeed1=MTsensor_AirSpeed1;
    sensor_AirSpeed2=MTsensor_AirSpeed2;
    sensor_AirSpeed3=MTsensor_AirSpeed3;
    sensor_TemperatureIndoor=MTsensor_TemperatureIndoor;
    sensor_HumidityIndoor=MTsensor_HumidityIndoor;
    sensor_HumidityOutdoor=MTsensor_HumidityOutdoor;
    sensor_TemperatureOutdoor=MTsensor_TemperatureOutdoor;
    state_aircond=MTstate_aircond;
    state_curtain=MTstate_curtain;
    state_window1=MTstate_window1;
    state_window2=MTstate_window2;
    TID_counter_local=TID_counter;
    UDP_network_timeout_aircond=MTUDP_network_timeout_aircond;
    UDP_network_timeout_airspeed_indoor=MTUDP_network_timeout_airspeed_indoor;
    UDP_network_timeout_airspeed_outdoor=MTUDP_network_timeout_airspeed_outdoor;
    UDP_network_timeout_temperature_indoor=MTUDP_network_timeout_temperature_indoor;
    UDP_network_timeout_temperature_outdoor=MTUDP_network_timeout_temperature_outdoor;
    UDP_network_timeout_window1=MTUDP_network_timeout_window1;
    UDP_network_timeout_window2=MTUDP_network_timeout_window2;
    UDP_network_timeout_curtain=MTUDP_network_timeout_curtain;
    //sensor_Rain=MTsensor_Rain;
    MTUDP_network_timeout_aircond=MTUDP_network_timeout_airspeed_indoor=MTUDP_network_timeout_airspeed_outdoor=MTUDP_network_timeout_temperature_indoor=MTUDP_network_timeout_temperature_outdoor=MTUDP_network_timeout_window1=MTUDP_network_timeout_window2=MTUDP_network_timeout_curtain=0;
    pthread_mutex_unlock(&mutex);
    
    //get Solar reading from Makino script
    FILE *pipe;
    pipe = popen("~/EETCC/SolarVoltage.sh", "r");
    while (fgets(scriptOutput, 10, pipe) != NULL)
    {
        //printf("Solar voltage: %s\n", scriptOutput);
    }
    pclose(pipe);
    sensor_SolarVoltage=((scriptOutput[1]-'0'))+((scriptOutput[3]-'0')*0.1)+((scriptOutput[4]-'0')*0.01)+((scriptOutput[5]-'0')*0.001)+((scriptOutput[6]-'0')*0.0001);
    if(scriptOutput[0]=='-')
        sensor_SolarVoltage=sensor_SolarVoltage*-1;
    sensor_SolarRadiation=(137598*pow(sensor_SolarVoltage,2))+(3216.1*sensor_SolarVoltage)+0.6377;
    echonetMT_getObject_powerDistributionMeter_reading("192.168.2.176",0x256,1,3);
    //3 Indoor Airspeed sensor (IP:192.168.2.202) TID: 10
    echonetMT_getiHouse_2_202(((TID_counter_local<<8) | 10));
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime1);
    pthread_mutex_unlock(&mutex);
    //1 Outdoor Airspeed sensor,1 Outdoor Temperature sensor,1 Outdoor Humidity sensor (IP:192.168.2.145) TID: 20
    echonetMT_getiHouse_2_145(((TID_counter_local<<8) | 20));
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime2);
    pthread_mutex_unlock(&mutex);
    //1 Indoor Temperature sensor,1 Indoor Humidity sensor (IP:192.168.2.108) TID: 30
    echonetMT_getiHouse_2_108(((TID_counter_local<<8) | 30));
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime3);
    pthread_mutex_unlock(&mutex);
    //Air Conditioner (IP:192.168.2.174) TID: 40
    echonetMT_getiHouse_2_174(((TID_counter_local<<8) | 40));
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime4);
    pthread_mutex_unlock(&mutex);
    //2 Switch, Window1 (IP:192.168.2.166) TID: 50
    echonetMT_getiHouse_2_166(((TID_counter_local<<8) | 50));
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime5);
    pthread_mutex_unlock(&mutex);
    //2 Switch, Window2 (IP:192.168.2.167) TID: 60
    echonetMT_getiHouse_2_167(((TID_counter_local<<8) | 60));
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime6);
    pthread_mutex_unlock(&mutex);
    //2 Switch, Curtain (IP:192.168.2.158) TID: 70
    echonetMT_getiHouse_2_158(((TID_counter_local<<8) | 70));
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    time(&previousTime7);
    pthread_mutex_unlock(&mutex);
    
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, 0.0, sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV1
    PMV1=EETCC_PMV();
    PPD1=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, 0.0, sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV2
    PMV2=EETCC_PMV();
    PPD2=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*0.8*0.5), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV3
    PMV3=EETCC_PMV();
    PPD3=EETCC_PPD();
    EETCC_thermalComfort(CLOTHING_INSULATION, METABOLIC_RATE, (sensor_AirSpeedOutdoor*1.0*0.5), sensor_TemperatureIndoor, sensor_HumidityIndoor, sensor_TemperatureIndoor);   //PMV4
    PMV4=EETCC_PMV();
    PPD4=EETCC_PPD();
    Q1=EETCC_Q1(1, sensor_TemperatureIndoor, sensor_TemperatureOutdoor, 3, sensor_SolarRadiation, sensor_SolarRadiation);
    Q2=EETCC_Q2(1, sensor_TemperatureIndoor, sensor_TemperatureOutdoor, 3, sensor_SolarRadiation, sensor_SolarRadiation);
    index=EETCC_index(sensor_TemperatureIndoor, DESIRED_TEMPERATURE, Q1, Q2);
    prev_ControlSignal=EETCC_prev_controlSignal();
    timer=EETCC_timer();
    globalA=EETCC_globalA(PMV);
    globalA_delay=EETCC_globalA_delay();
    ControlSignal=EETCC_controlSignal_v2(PMV1,PMV2,PMV3,PMV4,prev_ControlSignal,timer,index,(sensor_AirSpeedOutdoor*0.1),sensor_TemperatureIndoor,sensor_TemperatureOutdoor,sensor_SolarRadiation,globalA,globalA_delay);
    //if(sensor_Rain==0x41)
    //{
    //    printf("Rain condition! Calculated control: %d\n",ControlSignal);
    //    switch (ControlSignal) {
    //        case 3:
    //            ControlSignal=0;
    //            break;
                
    //        case 4:
    //            ControlSignal=0;
    //            break;
                
    //        default:
    //            break;
    //    }
    //}
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
    
    sprintf(sql,"INSERT INTO iHouseData_testrun3 VALUES(%d,%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%f,%d,%f,%f,%f,%f,%f,%lu,NOW())",(sql_ID+run_counter-1),state_window1,state_window2,state_curtain,state_aircond,setting_aircond_temperature,aircond_current_comsumption,aircond_room_humidity,aircond_room_temperature,aircond_cooled_air_temperature,aircond_outdoor_air_temperature,sensor_TemperatureIndoor,sensor_TemperatureOutdoor,sensor_AirSpeedIndoor,sensor_AirSpeedOutdoor,sensor_HumidityIndoor,sensor_HumidityOutdoor,sensor_SolarVoltage,sensor_SolarRadiation,UDP_network_timeout_aircond,UDP_network_timeout_airspeed_indoor,UDP_network_timeout_airspeed_outdoor,UDP_network_timeout_temperature_indoor,UDP_network_timeout_temperature_outdoor,UDP_network_timeout_window1,UDP_network_timeout_window2,UDP_network_timeout_curtain,UDP_network_fatal_aircond,UDP_network_fatal_airspeed_indoor,UDP_network_fatal_airspeed_outdoor,UDP_network_fatal_temperature_indoor,UDP_network_fatal_temperature_outdoor,UDP_network_fatal_window1,UDP_network_fatal_window2,UDP_network_fatal_curtain,PMV,PMV1,PMV2,PMV3,PMV4,PPD,PPD1,PPD2,PPD3,PPD4,ControlSignal,prev_ControlSignal,timer,index,globalA,globalA_delay,Q1,Q2,draught,Accumulative_power);
    
    if(mysql_query(con, sql))
    {
        exit_sql_error(con);
    }
    memset(sql,'\0',1000);   //clear SQL command array*/
    
    
    //printf("End of EETCC routine. No.%d\n\n",run_counter);
    
    run_counter++;
    if(TID_counter_local<245)   //reserve 10 TID_counter for ECHONET_DEVICE_CONTROL
        TID_counter_local++;
    else
        TID_counter_local=0;
    //Multi-thread global variable lock to prevent read/write error
    pthread_mutex_lock(&mutex);
    EETCC_ControlSignal=ControlSignal;
    TID_counter=TID_counter_local;
    pthread_mutex_unlock(&mutex);
}

/*
 Echonet Device Control thread
 Note: 1) Listen for EETCC change in control signal
       2) Keep all Echonet devices in the EETCC state
 */
void *ECHONET_DEVICE_CONTROL(void *threadid)
{
    uint8_t prev_EETCC_controlsignal=0,EETCC_controlsignal,unexpected_state_changed=0;
    uint8_t state_Aircond,state_Window1,state_Window2,state_Curtain;
    uint8_t updatedstate_Aircond=1,updatedstate_Window1=1,updatedstate_Window2=1,updatedstate_Curtain=1;
    time_t currentTime_all,previousTime_aircond,previousTime_window1,previousTime_window2,previousTime_curtain;
    //time_t specialTimer;
    uint16_t echonet_TID=246;
    uint8_t first_run=1,retransmit_flag=0;
    long tid;
    tid = (long)threadid;
    //time(&specialTimer);
    time(&previousTime_aircond);
    time(&previousTime_window1);
    time(&previousTime_window2);
    time(&previousTime_curtain);
    while(1)
    {
        //Multi-thread global variable lock to prevent read/write error
        pthread_mutex_lock(&mutex);
        EETCC_controlsignal=EETCC_ControlSignal;
        state_Aircond=MTstate_aircond;
        state_Window1=MTstate_window1;
        state_Window2=MTstate_window2;
        state_Curtain=MTstate_curtain;
        if(MTupdatedstate_aircond>0)
        {
            MTstate_aircond=state_Aircond=MTupdatedstate_aircond;
            MTupdatedstate_aircond=updatedstate_Aircond=0;
        }
        if(MTupdatedstate_window1>0)
        {
            //MTstate_window1=state_Window1=MTupdatedstate_window1;
            MTupdatedstate_window1=updatedstate_Window1=0;
        }
        if(MTupdatedstate_window2>0)
        {
            //MTstate_window2=state_Window2=MTupdatedstate_window2;
            MTupdatedstate_window2=updatedstate_Window2=0;
        }
        if(MTupdatedstate_curtain>0)
        {
            //MTstate_curtain=state_Curtain=MTupdatedstate_curtain;
            MTupdatedstate_curtain=updatedstate_Curtain=0;
        }
        pthread_mutex_unlock(&mutex);
        
        time(&currentTime_all);
        if(EETCC_controlsignal!=prev_EETCC_controlsignal || unexpected_state_changed>1 || retransmit_flag==1)
        {
            if(unexpected_state_changed>1)
                printf("\n##########################################\n\nUnexpected changes occured in Echonet device state!\n\n##########################################\n");
            else if(retransmit_flag==1)
            {
                printf("\n##########################################\n\nRetransmit packet occured!\n\n##########################################\n");
            }
            else
            {
                printf("\n##########################################\n\nControl Signal changed! State: %d\n\n##########################################\n",(EETCC_controlsignal-1));
            }
            //if((difftime(currentTime_all, specialTimer)>180) || first_run==1)
            //{
            //    time(&specialTimer);
                prev_EETCC_controlsignal=EETCC_controlsignal;
            //}
            unexpected_state_changed=0;
            retransmit_flag=0;
            switch (prev_EETCC_controlsignal)
            {
                case 1:
                    if(state_Aircond==OS_ON && (difftime(currentTime_all,previousTime_aircond)> TIME_INTERVAL_RETRANSMIT|| first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.174", echonet_TID, CGC_AIR_CONDITIONER_RELATED, CC_HOME_AIR_CONDITIONER, 1, OS_OFF);
                        updatedstate_Aircond=1;
                        time(&previousTime_aircond);
                        printf("Disabling air conditional! State: 0\n");
                    }
                    else
                        updatedstate_Aircond=0;
                    if(state_Window1==OS_ON && (difftime(currentTime_all,previousTime_window1)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.166", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window1=1;
                        time(&previousTime_window1);
                        printf("Disabling window1! State: 0\n");
                    }
                    else
                        updatedstate_Window1=0;
                    if(state_Window2==OS_ON && (difftime(currentTime_all,previousTime_window2)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.167", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window2=1;
                        time(&previousTime_window2);
                        printf("Disabling window2! State: 0\n");
                    }
                    else
                        updatedstate_Window2=0;
                    if(state_Curtain==OS_ON && (difftime(currentTime_all,previousTime_curtain)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_CURTAIN) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.158", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Curtain=1;
                        time(&previousTime_curtain);
                        printf("Disabling curtain! State: 0\n");
                    }
                    else
                        updatedstate_Curtain=0;
                    break;
                    
                case 2:
                    if(state_Aircond==OS_ON && (difftime(currentTime_all,previousTime_aircond)>TIME_INTERVAL_RETRANSMIT || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.174", echonet_TID, CGC_AIR_CONDITIONER_RELATED, CC_HOME_AIR_CONDITIONER, 1, OS_OFF);
                        updatedstate_Aircond=1;
                        time(&previousTime_aircond);
                        printf("Disabling air conditional! State: 1\n");
                    }
                    else
                        updatedstate_Aircond=0;
                    if(state_Window1==OS_ON && (difftime(currentTime_all,previousTime_window1)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.166", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window1=1;
                        time(&previousTime_window1);
                        printf("Disabling window1! State: 1\n");
                    }
                    else
                        updatedstate_Window1=0;
                    if(state_Window2==OS_ON && (difftime(currentTime_all,previousTime_window2)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.167", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window2=1;
                        time(&previousTime_window2);
                        printf("Disabling window2! State: 1\n");
                    }
                    else
                        updatedstate_Window2=0;
                    if(state_Curtain==OS_OFF && (difftime(currentTime_all,previousTime_curtain)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_CURTAIN) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.158", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 1, OS_ON);
                        updatedstate_Curtain=1;
                        time(&previousTime_curtain);
                        printf("Enabling curtain! State: 1\n");
                    }
                    else
                        updatedstate_Curtain=0;
                    break;
                    
                case 3:
                    if(state_Aircond==OS_ON && (difftime(currentTime_all,previousTime_aircond)>TIME_INTERVAL_RETRANSMIT || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.174", echonet_TID, CGC_AIR_CONDITIONER_RELATED, CC_HOME_AIR_CONDITIONER, 1, OS_OFF);
                        updatedstate_Aircond=1;
                        time(&previousTime_aircond);
                        printf("Disabling air conditional! State: 2\n");
                    }
                    else
                        updatedstate_Aircond=0;
                    if(state_Window1==OS_OFF && (difftime(currentTime_all,previousTime_window1)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.166", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 1, OS_ON);
                        updatedstate_Window1=1;
                        time(&previousTime_window1);
                        printf("Enabling window1! State: 2\n");
                    }
                    else
                        updatedstate_Window1=0;
                    if(state_Window2==OS_OFF && (difftime(currentTime_all,previousTime_window2)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.167", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 1, OS_ON);
                        updatedstate_Window2=1;
                        time(&previousTime_window2);
                        printf("Enabling window2! State: 2\n");
                    }
                    else
                        updatedstate_Window2=0;
                    if(state_Curtain==OS_ON && (difftime(currentTime_all,previousTime_curtain)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_CURTAIN) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.158", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Curtain=1;
                        time(&previousTime_curtain);
                        printf("Disabling curtain! State: 2\n");
                    }
                    else
                        updatedstate_Curtain=0;
                    break;
                    
                case 4:
                    if(state_Aircond==OS_ON && (difftime(currentTime_all,previousTime_aircond)>TIME_INTERVAL_RETRANSMIT || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.174", echonet_TID, CGC_AIR_CONDITIONER_RELATED, CC_HOME_AIR_CONDITIONER, 1, OS_OFF);
                        updatedstate_Aircond=1;
                        time(&previousTime_aircond);
                        printf("Disabling air conditional! State: 3\n");
                    }
                    else
                        updatedstate_Aircond=0;
                    if(state_Window1==OS_OFF && (difftime(currentTime_all,previousTime_window1)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.166", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 1, OS_ON);
                        updatedstate_Window1=1;
                        time(&previousTime_window1);
                        printf("Enabling window1! State: 3\n");
                    }
                    else
                        updatedstate_Window1=0;
                    if(state_Window2==OS_OFF && (difftime(currentTime_all,previousTime_window2)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.167", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 1, OS_ON);
                        updatedstate_Window2=1;
                        time(&previousTime_window2);
                        printf("Enabling window2! State: 3\n");
                    }
                    else
                        updatedstate_Window2=0;
                    if(state_Curtain==OS_OFF && (difftime(currentTime_all,previousTime_curtain)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_CURTAIN) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.158", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 1, OS_ON);
                        updatedstate_Curtain=1;
                        time(&previousTime_curtain);
                        printf("Enabling curtain! State: 3\n");
                    }
                    else
                        updatedstate_Curtain=0;
                    break;
                    
                case 5:
                    if(state_Aircond==OS_OFF && (difftime(currentTime_all,previousTime_aircond)>TIME_INTERVAL_RETRANSMIT || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.174", echonet_TID, CGC_AIR_CONDITIONER_RELATED, CC_HOME_AIR_CONDITIONER, 1, OS_ON);
                        updatedstate_Aircond=1;
                        time(&previousTime_aircond);
                        printf("Enabling air conditional! State: 4\n");
                    }
                    else
                        updatedstate_Aircond=0;
                    if(state_Window1==OS_ON && (difftime(currentTime_all,previousTime_window1)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.166", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window1=1;
                        time(&previousTime_window1);
                        printf("Disabling window1! State: 4\n");
                    }
                    else
                        updatedstate_Window1=0;
                    if(state_Window2==OS_ON && (difftime(currentTime_all,previousTime_window2)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.167", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window2=1;
                        time(&previousTime_window2);
                        printf("Disabling window2! State: 4\n");
                    }
                    else
                        updatedstate_Window2=0;
                    if(state_Curtain==OS_ON && (difftime(currentTime_all,previousTime_curtain)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_CURTAIN) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.158", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Curtain=1;
                        time(&previousTime_curtain);
                        printf("Disabling curtain! State: 4\n");
                    }
                    else
                        updatedstate_Curtain=0;
                    break;
                    
                case 6:
                    if(state_Aircond==OS_OFF && (difftime(currentTime_all,previousTime_aircond)>TIME_INTERVAL_RETRANSMIT || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.174", echonet_TID, CGC_AIR_CONDITIONER_RELATED, CC_HOME_AIR_CONDITIONER, 1, OS_ON);
                        updatedstate_Aircond=1;
                        time(&previousTime_aircond);
                        printf("Enabling air conditional! State: 5\n");
                    }
                    else
                        updatedstate_Aircond=0;
                    if(state_Window1==OS_ON && (difftime(currentTime_all,previousTime_window1)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.166", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window1=1;
                        time(&previousTime_window1);
                        printf("Disabling window1! State: 5\n");
                    }
                    else
                        updatedstate_Window1=0;
                    if(state_Window2==OS_ON && (difftime(currentTime_all,previousTime_window2)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.167", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 2, OS_ON);
                        updatedstate_Window2=1;
                        time(&previousTime_window2);
                        printf("Disabling window2! State: 5\n");
                    }
                    else
                        updatedstate_Window2=0;
                    if(state_Curtain==OS_OFF && (difftime(currentTime_all,previousTime_curtain)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_CURTAIN) || first_run==1))
                    {
                        echonetMT_setObject_operationalStatus("192.168.2.158", echonet_TID, CGC_MANAGEMENT_RELATED, CC_SWITCH, 1, OS_ON);
                        updatedstate_Curtain=1;
                        time(&previousTime_curtain);
                        printf("Enabling curtain! State: 5\n");
                    }
                    else
                        updatedstate_Curtain=0;
                    break;
                    
                default:
                    break;
            }
        }
        else if((updatedstate_Aircond==1 && (difftime(currentTime_all,previousTime_aircond)>TIME_INTERVAL_RETRANSMIT)) || (updatedstate_Window1==1 && (difftime(currentTime_all,previousTime_window1)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW))) || (updatedstate_Window2==1 && (difftime(currentTime_all,previousTime_window2)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_WINDOW))) || (updatedstate_Curtain==1 && (difftime(currentTime_all,previousTime_curtain)>(TIME_INTERVAL_RETRANSMIT+TIME_INTERVAL_CURTAIN))))
        {
            //routine for re-transmission to ensure device state is correct
            retransmit_flag=1;
        }
        else
        {
            //check current state of the Echonet Lite Devices (only for unexpected changes)
            switch (prev_EETCC_controlsignal)
            {
                case 1:
                    if((state_Aircond==OS_ON && updatedstate_Aircond==0) || (state_Window1==OS_ON && updatedstate_Window1==0) || (state_Window2==OS_ON && updatedstate_Window2==0) || (state_Curtain==OS_ON && updatedstate_Curtain==0))
                        unexpected_state_changed++;
                    break;
                    
                case 2:
                    if((state_Aircond==OS_ON && updatedstate_Aircond==0) || (state_Window1==OS_ON &&updatedstate_Window1==0) || (state_Window2==OS_ON && updatedstate_Window2==0) || (state_Curtain==OS_OFF && updatedstate_Curtain==0))
                        unexpected_state_changed++;
                    break;
                    
                case 3:
                    if((state_Aircond==OS_ON && updatedstate_Aircond==0) || (state_Window1==OS_OFF && updatedstate_Window1==0) || (state_Window2==OS_OFF && updatedstate_Window2==0) || (state_Curtain==OS_ON && updatedstate_Curtain==0))
                        unexpected_state_changed++;
                    break;
                    
                case 4:
                    if((state_Aircond==OS_ON && updatedstate_Aircond==0) || (state_Window1==OS_OFF && updatedstate_Window1==0) || (state_Window2==OS_OFF && updatedstate_Window2==0) || (state_Curtain==OS_OFF && updatedstate_Curtain==0))
                        unexpected_state_changed++;
                    break;
                    
                case 5:
                    if((state_Aircond==OS_OFF && updatedstate_Aircond==0) || (state_Window1==OS_ON && updatedstate_Window1==0) || (state_Window2==OS_ON && updatedstate_Window2==0) || (state_Curtain==OS_ON && updatedstate_Curtain==0))
                        unexpected_state_changed++;
                    break;
                    
                case 6:
                    if((state_Aircond==OS_OFF && updatedstate_Aircond==0) || (state_Window1==OS_ON && updatedstate_Window1==0) || (state_Window2==OS_ON && updatedstate_Window2==0) || (state_Curtain==OS_OFF && updatedstate_Curtain==0))
                        unexpected_state_changed++;
                    break;
                    
                default:
                    break;
            }
        }
        if(first_run==1)
            first_run=0;
        echonetMT_getiHouse_2_166(echonet_TID);
        echonetMT_getiHouse_2_167(echonet_TID);
        echonetMT_getiHouse_2_158(echonet_TID);
        echonetMT_getObject_powerDistributionMeter_reading("192.168.2.176",echonet_TID,1,3);
        sleep(1);
        //sleep 500ms
        //nanosleep((const struct timespec[]){{0, 1000000000L}}, NULL);
    }
    //pthread_exit(NULL);
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

/*
 UDP listener thread
 Note: 1) Listen for UDP packet from Echonet port
       2) Analyze incoming packet and update the respective global variable
 */
void *SERVER_PORT_LISTEN(void *threadid)
{
    uint16_t UDPpacket_TIDraw;
    uint8_t UDPpacket_TID_upper8bit;
    long tid;
    tid = (long)threadid;
    while(1)
    {
        if(udp_port_listener_echonet_lite(UDPnetwork_buffer,&UDPpacket_length)==1)
        {
            //Multi-thread global variable lock to prevent read/write error
            pthread_mutex_lock(&mutex);
            //check if it's Echonet Lite Packet
            if((unsigned)UDPnetwork_buffer[0]==EHD1_ECHONET && (unsigned)UDPnetwork_buffer[1]==EHD2_FORMAT1)
            {
                UDPpacket_TIDraw=((unsigned char)UDPnetwork_buffer[2]<<8 | (unsigned char)UDPnetwork_buffer[3]);
                UDPpacket_TID=UDPpacket_TIDraw&0x00FF;
                UDPpacket_TID_upper8bit=(char)(UDPpacket_TIDraw>>8);
                switch ((unsigned)UDPnetwork_buffer[4])
                {
                    case CGC_SENSOR_RELATED:
                        switch ((unsigned)UDPnetwork_buffer[5])
                    {
                        case CC_GAS_LEAK_SENSOR:
                            
                            break;
                        case CC_CRIME_PREVENTION_SENSOR:
                            
                            break;
                        case CC_EMERGENCY_BUTTON:
                            
                            break;
                        case CC_FIRST_AID_SENSOR:
                            
                            break;
                        case CC_EARTHQUAKE_SENSOR:
                            
                            break;
                        case CC_ELECTRIC_LEAK_SENSOR:
                            
                            break;
                        case CC_HUMAN_DETECTION_SENSOR:
                            
                            break;
                        case CC_VISITOR_SENSOR:
                            
                            break;
                        case CC_CALL_SENSOR:
                            
                            break;
                        case CC_CONDENSATION_SENSOR:
                            
                            break;
                        case CC_AIR_POLLUTION_SENSOR:
                            
                            break;
                        case CC_OXYGEN_SENSOR:
                            
                            break;
                        case CC_ILLUMINANCE_SENSOR:
                            
                            break;
                        case CC_SOUND_SENSOR:
                            
                            break;
                        case CC_MAILING_SENSOR:
                            
                            break;
                        case CC_WEIGHT_SENSOR:
                            
                            break;
                        case CC_TEMPERTURE_SENSOR:
                            if((unsigned)UDPnetwork_buffer[10]==ESV_Get_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                if((unsigned)UDPnetwork_buffer[12]==EPC_MEASURE_TEMPERATURE_VALUE && (unsigned)UDPnetwork_buffer[13]==2)
                                {
                                    switch (UDPpacket_TID)
                                    {
                                        case 21:
                                            MTsensor_TemperatureOutdoor=((float)(UDPnetwork_buffer[14]<<8 | UDPnetwork_buffer[15]))/10;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime2);
                                                MTUDP_network_timeout_temperature_outdoor=(float)difftime(currentTime2,previousTime2);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_temperature_outdoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_temperature_outdoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        case 30:
                                            MTsensor_TemperatureIndoor=((float)(UDPnetwork_buffer[14]<<8 | UDPnetwork_buffer[15]))/10;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime3);
                                                MTUDP_network_timeout_temperature_indoor=(float)difftime(currentTime3,previousTime3);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_temperature_indoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_temperature_indoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        default:
                                            break;
                                    }
                                }
                            }
                            else if((unsigned)UDPnetwork_buffer[10]==ESV_Set_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                
                            }
                            break;
                        case CC_HUMIDITY_SENSOR:
                            if((unsigned)UDPnetwork_buffer[10]==ESV_Get_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                if((unsigned)UDPnetwork_buffer[12]==EPC_MEASURE_HUMIDITY_VALUE && (unsigned)UDPnetwork_buffer[13]==1)
                                {
                                    switch (UDPpacket_TID)
                                    {
                                        case 22:
                                            MTsensor_HumidityOutdoor=((float)UDPnetwork_buffer[14])/100;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime2);
                                                MTUDP_network_timeout_temperature_outdoor=(float)difftime(currentTime2,previousTime2);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_temperature_outdoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_temperature_outdoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        case 31:
                                            MTsensor_HumidityIndoor=((float)UDPnetwork_buffer[14])/100;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime3);
                                                MTUDP_network_timeout_temperature_indoor=(float)difftime(currentTime3,previousTime3);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_temperature_indoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_temperature_indoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        default:
                                            break;
                                    }
                                }
                            }
                            else if((unsigned)UDPnetwork_buffer[10]==ESV_Set_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                
                            }
                            break;
                        case CC_RAIN_SENSOR:
                            //if((unsigned)UDPnetwork_buffer[10]==ESV_Get_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            //{
                                //if((unsigned)UDPnetwork_buffer[12]==0xB1 && (unsigned)UDPnetwork_buffer[13]==1)
                                //{
                                    //switch (UDPpacket_TID)
                                    //{
                                        //case 80:
                                            //MTsensor_Rain=UDPnetwork_buffer[14];
                                            //if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            //{
                                            //    time(&currentTime2);
                                            //    MTUDP_network_timeout_temperature_outdoor=(float)difftime(currentTime2,previousTime2);
                                            //}
                                            //else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            //{
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_temperature_outdoor=0;
                                            //}
                                            //else
                                            //{
                                                //MTUDP_network_timeout_temperature_outdoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            //}
                                            //break;
                                            
                                        //default:
                                            //break;
                                    //}
                                //}
                            //}
                            //break;
                        case CC_WATER_LEVEL_SENSOR:
                            
                            break;
                        case CC_BATH_WATER_LEVEL_SENSOR:
                            
                            break;
                        case CC_BATH_HEATING_STATUS_SENSOR:
                            
                            break;
                        case CC_WATER_LEAK_SENSOR:
                            
                            break;
                        case CC_WATER_OVERFLOW_SENSOR:
                            
                            break;
                        case CC_FIRE_SENSOR:
                            
                            break;
                        case CC_CIGARETTE_SMOKE_SENSOR:
                            
                            break;
                        case CC_CO2_SENSOR:
                            
                            break;
                        case CC_GAS_SENSOR:
                            
                            break;
                        case CC_VOC_SENSOR:
                            
                            break;
                        case CC_DIFFERENTIAL_PRESSURE_SENSOR:
                            
                            break;
                        case CC_AIR_SPEED_SENSOR:
                            if((unsigned)UDPnetwork_buffer[10]==ESV_Get_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                if((unsigned)UDPnetwork_buffer[12]==EPC_MEASURE_AIR_SPEED_VALUE && (unsigned)UDPnetwork_buffer[13]==2)
                                {
                                    switch (UDPpacket_TID)
                                    {
                                        case 10:
                                            MTsensor_AirSpeed1=((float)(UDPnetwork_buffer[14]<<8 | UDPnetwork_buffer[15]))/100;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime1);
                                                MTUDP_network_timeout_airspeed_indoor=(float)difftime(currentTime1,previousTime1);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_airspeed_indoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_airspeed_indoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        case 11:
                                            MTsensor_AirSpeed2=((float)(UDPnetwork_buffer[14]<<8 | UDPnetwork_buffer[15]))/100;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime1);
                                                MTUDP_network_timeout_airspeed_indoor=(float)difftime(currentTime1,previousTime1);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_airspeed_indoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_airspeed_indoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        case 12:
                                            MTsensor_AirSpeed3=((float)(UDPnetwork_buffer[14]<<8 | UDPnetwork_buffer[15]))/100;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime1);
                                                MTUDP_network_timeout_airspeed_indoor=(float)difftime(currentTime1,previousTime1);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_airspeed_indoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_airspeed_indoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        case 20:
                                            MTsensor_AirSpeedOutdoor=((float)(UDPnetwork_buffer[14]<<8 | UDPnetwork_buffer[15]))/100;
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime2);
                                                MTUDP_network_timeout_airspeed_outdoor=(float)difftime(currentTime2,previousTime2);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_airspeed_outdoor=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_airspeed_outdoor=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        default:
                                            break;
                                    }
                                }
                            }
                            else if((unsigned)UDPnetwork_buffer[10]==ESV_Set_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                
                            }
                            break;
                        case CC_ODOR_SENSOR:
                            
                            break;
                        case CC_FLAME_SENSOR:
                            
                            break;
                        case CC_ELECTRIC_ENERGY_SENSOR:
                            
                            break;
                        case CC_CURRENT_VALUE_SENSOR:
                            
                            break;
                        case CC_DAYLIGHT_SENSOR:
                            
                            break;
                        case CC_WATER_FLOW_RATE_SENSOR:
                            
                            break;
                        case CC_MICROMOTION_SENSOR:
                            
                            break;
                        case CC_PASSAGE_SENSOR:
                            
                            break;
                        case CC_BED_PRESENCE_SENSOR:
                            
                            break;
                        case CC_OPEN_CLOSE_SENSOR:
                            
                            break;
                        case CC_ACTIVITY_AMOUNT_SENSOR:
                            
                            break;
                        case CC_HUMAN_BODY_LOCATION_SENSOR:
                            
                            break;
                        case CC_SNOW_SENSOR:
                            
                            break;
                        case CC_AIR_PRESSURE_SENSOR:
                            
                            break;
                        default:
                            printf("Input not defined in Sensor-related class code (0x%1X)\n",(unsigned)UDPnetwork_buffer[5]);
                            break;
                    }
                        break;
                    case CGC_AIR_CONDITIONER_RELATED:
                        switch ((unsigned)UDPnetwork_buffer[5])
                    {
                        case CC_HOME_AIR_CONDITIONER:
                            //setting_aircond_temperature,aircond_room_humidity
                            if((unsigned)UDPnetwork_buffer[10]==ESV_Get_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                if((unsigned)UDPnetwork_buffer[12]==EPC_OPERATIONAL_STATUS && (unsigned)UDPnetwork_buffer[13]==1)
                                {
                                    MTstate_aircond=UDPnetwork_buffer[14];
                                    if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                    {
                                        time(&currentTime4);
                                        MTUDP_network_timeout_aircond=(float)difftime(currentTime4,previousTime4);
                                    }
                                    else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                    {
                                        //no specific assignment for the moment 28/7/2016
                                        //MTUDP_network_timeout_aircond=0;
                                    }
                                    else
                                    {
                                        MTUDP_network_timeout_aircond=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                    }
                                }
                                
                            }
                            else if((unsigned)UDPnetwork_buffer[10]==ESV_Set_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                MTupdatedstate_aircond=UDPnetwork_buffer[14];
                            }
                            break;
                        case CC_COLD_BLASTER:
                            
                            break;
                        case CC_ELECTRIC_FAN:
                            
                            break;
                        case CC_VENTILATION_FAN:
                            
                            break;
                        case CC_AIR_CONDITIONER_VENTILATION_FAN:
                            
                            break;
                        case CC_AIR_CLEANER:
                            
                            break;
                        case CC_COLD_BLAST_FAN:
                            
                            break;
                        case CC_CIRCULATOR:
                            
                            break;
                        case CC_DEHUMIDIFIER:
                            
                            break;
                        case CC_HUMIDIFIER:
                            
                            break;
                        case CC_CEILING_FAN:
                            
                            break;
                        case CC_ELECTRIC_KOTATSU:
                            
                            break;
                        case CC_ELECTRIC_HEATING_PAD:
                            
                            break;
                        case CC_ELECTRIC_BLANKET:
                            
                            break;
                        case CC_SPACE_HEATER:
                            
                            break;
                        case CC_PANEL_HEATER:
                            
                            break;
                        case CC_ELECTRIC_CARPET:
                            
                            break;
                        case CC_FLOOR_HEATER_0x01:
                            
                            break;
                        case CC_ELECTRIC_HEATER:
                            
                            break;
                        case CC_FAN_HEATER:
                            
                            break;
                        case CC_BATTERY_CHARGER:
                            
                            break;
                        case CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_INDOOR:
                            
                            break;
                        case CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_OUTDOOR:
                            
                            break;
                        case CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_THERMAL:
                            
                            break;
                        case CC_COMMERCIAL_FAN_COIL_UNIT:
                            
                            break;
                        case CC_COMMERCIAL_AIRCOND_COLD_SOURCE_CHILLER:
                            
                            break;
                        case CC_COMMERCIAL_AIRCOND_HOT_SOURCE_BOILER:
                            
                            break;
                        case CC_AIRCOND_VAV_FOR_COMMERCIAL_APPLICATIONS:
                            
                            break;
                        case CC_AIRCOND_FOR_COMMERCIAL_APPLICATIONS:
                            
                            break;
                        case CC_UNIT_COOLER:
                            
                            break;
                        case CC_CONDENSING_UNIT_FOR_COMMERCIAL_APP:
                            
                            break;
                        case CC_ELECTRIC_STORAGE_HEATER:
                            
                            break;
                        default:
                            printf("Input not defined in Air Conditioner-related class code (0x%1X)\n",(unsigned)UDPnetwork_buffer[5]);
                            break;
                    }
                        break;
                    case CGC_HOUSING_RELATED:
                        switch ((unsigned)UDPnetwork_buffer[5])
                    {
                        case CC_ELECTRICALLY_OPERATED_BLIND:
                            
                            break;
                        case CC_ELECTRICALLY_OPERATED_SHUTTER:
                            
                            break;
                        case CC_ELECTRICALLY_OPERATED_CURTAIN:
                            
                            break;
                        case CC_ELECTRICALLY_OPERATED_RAIN_SLIDING_DOOR:
                            
                            break;
                        case CC_ELECTRICALLY_OPERATED_GATE:
                            
                            break;
                        case CC_ELECTRICALLY_OPERATED_WINDOW:
                            
                            break;
                        case CC_AUTOMATICALLY_OPERATED_ENTRANCE_DOOR:
                            
                            break;
                        case CC_GARDEN_SPRINKLER:
                            
                            break;
                        case CC_FIRE_SPRINKLER:
                            
                            break;
                        case CC_FOUNTAIN:
                            
                            break;
                        case CC_INSTANTANEOUS_WATER_HEATER:
                            
                            break;
                        case CC_ELECTRIC_WATER_HEATER:
                            
                            break;
                        case CC_SOLAR_WATER_HEATER:
                            
                            break;
                        case CC_CIRCULATION_PUMP:
                            
                            break;
                        case CC_BIDET_EQUIPPED_TOILET:
                            
                            break;
                        case CC_ELECTRIC_LOCK:
                            
                            break;
                        case CC_GAS_LINE_VALVE:
                            
                            break;
                        case CC_HOME_SAUNA:
                            
                            break;
                        case CC_HOT_WATER_GENERATOR:
                            
                            break;
                        case CC_BATHROOM_DRYER:
                            
                            break;
                        case CC_HOME_ELEVATOR:
                            
                            break;
                        case CC_ELECTRICALLY_OPERATED_ROOM_DIVIDER:
                            
                            break;
                        case CC_HORIZONTAL_TRANSFER:
                            
                            break;
                        case CC_ELECTRICALLY_OPERATED_CLOTH_DRYING_POLE:
                            
                            break;
                        case CC_SEPTIC_TANK:
                            
                            break;
                        case CC_HOME_SOLAR_POWER_GENERATION:
                            
                            break;
                        case CC_COLD_HOT_WATER_HEAT_SOURCE_EQUIPMENT:
                            
                            break;
                        case CC_FLOOR_HEATER_0x02:
                            
                            break;
                        case CC_FUEL_CELL:
                            
                            break;
                        case CC_STORAGE_BATTERY:
                            
                            break;
                        case CC_ELECTRIC_VEHICLE_CHARGER_DISCHARGER:
                            
                            break;
                        case CC_ENGINE_COGENERATION:
                            
                            break;
                        case CC_ELECTRIC_ENERGY_METER:
                            
                            break;
                        case CC_WATER_FLOW_METER:
                            
                            break;
                        case CC_GAS_METER:
                            
                            break;
                        case CC_LP_GAS_METER:
                            
                            break;
                        case CC_CLOCK:
                            
                            break;
                        case CC_AUTOMATIC_DOOR:
                            
                            break;
                        case CC_COMMERCIAL_ELEVATOR:
                            
                            break;
                        case CC_DISTRIBUTION_PANEL_METERING:
                            accumulative_power=((unsigned char)UDPnetwork_buffer[14]<<24 | (unsigned char)UDPnetwork_buffer[15]<<16 | (unsigned char)UDPnetwork_buffer[16]<<8 | (unsigned char)UDPnetwork_buffer[17]);
                            break;
                        case CC_LOW_VOLTAGE_SMART_ELECTRIC_ENERGY_METER:
                            
                            break;
                        case CC_SMART_GAS_METER:
                            
                            break;
                        case CC_HIGH_VOLTAGE_SMART_ELECTRIC_ENERGY_METER:
                            
                            break;
                        case CC_GENERAL_LIGHTING_CLASS:
                            
                            break;
                        case CC_SINGLE_FUNCTION_LIGHTING:
                            
                            break;
                        case CC_EMERGENCY_LIGHTING:
                            
                            break;
                        case CC_EQUIPMENT_LIGHT:
                            
                            break;
                        case CC_BUZZER:
                            
                            break;
                        default:
                            printf("Input not defined in Housing/Facility-related class code (0x%1X)\n",(unsigned)UDPnetwork_buffer[5]);
                            break;
                    }
                        break;
                    case CGC_COOKING_RELATED:
                        switch ((unsigned)UDPnetwork_buffer[5])
                    {
                        case CC_COFFEE_MACHINE:
                            
                            break;
                        case CC_COFFEE_MILL:
                            
                            break;
                        case CC_ELECTRIC_HOT_WATER_POT:
                            
                            break;
                        case CC_ELECTRIC_STOVE:
                            
                            break;
                        case CC_TOASTER:
                            
                            break;
                        case CC_JUICER_FOOD_MIXER:
                            
                            break;
                        case CC_FOOD_PROCESSOR:
                            
                            break;
                        case CC_REFRIGERATOR:
                            
                            break;
                        case CC_COMBINATION_MICROWAVE_OVEN:
                            
                            break;
                        case CC_COOKING_HEATER:
                            
                            break;
                        case CC_OVEN:
                            
                            break;
                        case CC_RICE_COOKER:
                            
                            break;
                        case CC_ELECTRONIC_JAR:
                            
                            break;
                        case CC_DISH_WASHER:
                            
                            break;
                        case CC_DISH_DRYER:
                            
                            break;
                        case CC_ELECTRIC_RICE_CARD_COOKER:
                            
                            break;
                        case CC_KEEP_WARM_MACHINE:
                            
                            break;
                        case CC_RICE_MILL:
                            
                            break;
                        case CC_AUTOMATIC_BREAD_COOKER:
                            
                            break;
                        case CC_SLOW_COOKER:
                            
                            break;
                        case CC_ELECTRIC_PICKLES_COOKER:
                            
                            break;
                        case CC_WASHING_MACHINE:
                            
                            break;
                        case CC_CLOTHES_DRYER:
                            
                            break;
                        case CC_ELECTRIC_IRON:
                            
                            break;
                        case CC_TROUSER_PRESS:
                            
                            break;
                        case CC_FUTON_DRYER:
                            
                            break;
                        case CC_SMALL_ARTICLE_SHOES_DRYER:
                            
                            break;
                        case CC_ELECTRIC_VACUUM_CLEANER:
                            
                            break;
                        case CC_DISPOSER:
                            
                            break;
                        case CC_ELECTRIC_MOSQUITO_CATCHER:
                            
                            break;
                        case CC_COMMERCIAL_SHOW_CASE:
                            
                            break;
                        case CC_COMMERCIAL_REFRIGERATOR:
                            
                            break;
                        case CC_COMMERCIAL_HOT_CASE:
                            
                            break;
                        case CC_COMMERCIAL_FRYER:
                            
                            break;
                        case CC_COMMERCIAL_MICROWAVE_OVEN:
                            
                            break;
                        case CC_WASHER_AND_DRYER:
                            
                            break;
                        case CC_COMMERCIAL_SHOW_CASE_OUTDOOR_UNIT:
                            
                            break;
                        default:
                            printf("Input not defined in Cooking/Household-related class code (0x%1X)\n",(unsigned)UDPnetwork_buffer[5]);
                            break;
                    }
                        break;
                    case CGC_HEALTH_RELATED:
                        switch ((unsigned)UDPnetwork_buffer[5])
                    {
                        case CC_WEIGHTING_MACHINE:
                            
                            break;
                        case CC_CLINICAL_THERMOMETER:
                            
                            break;
                        case CC_BLOOD_PRESSURE_METER:
                            
                            break;
                        case CC_BLOOD_SUGAR_METER:
                            
                            break;
                        case CC_BODY_FAT_METER:
                            
                            break;
                        default:
                            printf("Input not defined in Health-related class code (0x%1X)\n",(unsigned)UDPnetwork_buffer[5]);
                            break;
                    }
                        break;
                    case CGC_MANAGEMENT_RELATED:
                        switch ((unsigned)UDPnetwork_buffer[5])
                    {
                        case CC_SECURE_COMM_SHARED_KEY_SETUP_NODE:
                            
                            break;
                        case CC_SWITCH:
                            if((unsigned)UDPnetwork_buffer[10]==ESV_Get_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                if((unsigned)UDPnetwork_buffer[12]==EPC_OPERATIONAL_STATUS && (unsigned)UDPnetwork_buffer[13]==1)
                                {
                                    switch (UDPpacket_TID)
                                    {
                                        /*case 40:
                                            MTstate_aircond=UDPnetwork_buffer[14];
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime4);
                                                MTUDP_network_timeout_aircond=(float)difftime(currentTime4,previousTime4);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_aircond=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_aircond=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;*/
                                            
                                        case 50:
                                            MTstate_window1=UDPnetwork_buffer[14];
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime5);
                                                MTUDP_network_timeout_window1=(float)difftime(currentTime5,previousTime5);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_window1=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_window1=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        case 60:
                                            MTstate_window2=UDPnetwork_buffer[14];
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime6);
                                                MTUDP_network_timeout_window2=(float)difftime(currentTime6,previousTime6);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_window2=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_window2=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        case 70:
                                            MTstate_curtain=UDPnetwork_buffer[14];
                                            if(UDPpacket_TID_upper8bit==TID_counter || UDPpacket_TID_upper8bit==(TID_counter-1) || UDPpacket_TID_upper8bit==245)
                                            {
                                                time(&currentTime7);
                                                MTUDP_network_timeout_curtain=(float)difftime(currentTime7,previousTime7);
                                            }
                                            else if(UDPpacket_TID_upper8bit>245 && UDPpacket_TID_upper8bit<=255)
                                            {
                                                //no specific assignment for the moment 28/7/2016
                                                //MTUDP_network_timeout_curtain=0;
                                            }
                                            else
                                            {
                                                MTUDP_network_timeout_curtain=-abs(UDPpacket_TID_upper8bit-TID_counter);
                                            }
                                            break;
                                            
                                        default:
                                            break;
                                    }
                                }
                            }
                            else if((unsigned)UDPnetwork_buffer[10]==ESV_Set_Res && (unsigned)UDPnetwork_buffer[11]==0x01)
                            {
                                switch (UDPpacket_TID)
                                {
                                    case 50:
                                        MTupdatedstate_window1=UDPnetwork_buffer[14];
                                        break;
                                        
                                    case 60:
                                        MTupdatedstate_window2=UDPnetwork_buffer[14];
                                        break;
                                        
                                    case 70:
                                        MTupdatedstate_curtain=UDPnetwork_buffer[14];
                                        break;
                                        
                                    default:
                                        break;
                                }
                            }
                            break;
                        case CC_PORTABLE_TERMINAL:
                            
                            break;
                        case CC_CONTROLLER:
                            
                            break;
                        default:
                            printf("Input not defined in Management/Operation-related class code (0x%1X)\n",(unsigned)UDPnetwork_buffer[5]);
                            break;
                    }
                        break;
                    case CGC_AV_RELATED:
                        switch ((unsigned)UDPnetwork_buffer[5])
                    {
                        case CC_DISPLAY:
                            
                            break;
                        case CC_TELEVISION:
                            
                            break;
                        case CC_AUDIO:
                            
                            break;
                        case CC_NETWORK_CAMERA:
                            
                            break;
                        default:
                            printf("Input not defined in Audiovisual-related class code (0x%1X)",(unsigned)UDPnetwork_buffer[5]);
                            break;
                    }
                        break;
                    case CGC_PROFILE_CLASS:
                        
                        break;
                    case CGC_USER_DEFINITION_CLASS:
                        
                        break;
                    default:
                        break;
                }
                //end of Echonet packet analyzer
            }
            else
                printf("\n##########################################\n\nNon-Echonet packet received on port: %d\n\n##########################################\n",DEFAULT_ECHONET_PORT);
            pthread_mutex_unlock(&mutex);
        }
    }
    //pthread_exit(NULL);
}

/*
 Single thread version
 */
/*void ECHONET_LITE_MAIN_ROUTINE(void)
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
    char scriptOutput[10];
    
    printf("Start of EETCC routine. No.%d\n",run_counter);
    //get Solar reading from Makino script
    FILE *pipe;
    pipe = popen("~/EETCC/SolarVoltage.sh", "r");
    while (fgets(scriptOutput, 10, pipe) != NULL)
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
    ControlSignal=EETCC_controlSignal_v1(PMV1,PMV2,PMV3,PMV4,prev_ControlSignal,timer,index,sensor_AirSpeedOutdoor,sensor_TemperatureIndoor,globalA,globalA_delay);
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
    memset(sql,'\0',1000);   //clear SQL command array
    
    
    printf("End of EETCC routine. No.%d\n\n",run_counter);
    run_counter++;
}*/