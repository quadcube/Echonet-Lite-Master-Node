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
#include "network.h"
#include "error.h"
#include "echonet_lite.h"

int main(int argc, const char * argv[])
{
    int i;
    char buffer[DEFAULT_BUFFER_SIZE];
    unsigned long packet_length;
    //init_network("127.0.0.1");
    init_network();
    echonet_setEHD1();
    echonet_setEHD2(1);
    echonet_setTID(0xABCD);
    echonet_printPacket(40);
    echonet_setSEOJ(CGC_SENSOR_RELATED, CC_TEMPERTURE_SENSOR, 0x00);
    echonet_setESV(0x62);
    echonet_setOPC(6);
    printf("Start of EPC 1\n");
    echonet_setnEPC(1,EPC_OPERATIONAL_STATUS);
    echonet_setnPDC(1,3);
    echonet_printPacket(40);
    printf("Start of EPC 2\n");
    echonet_setnEPC(2,EPC_FAULT_STATUS);
    echonet_printPacket(25);
    echonet_setnPDC(2,1);
    echonet_printPacket(40);
    printf("Start of EPC 3\n");
    echonet_setnEPC(3,EPC_OPERATIONAL_STATUS);
    echonet_printPacket(25);
    echonet_setnPDC(3,3);
    echonet_printPacket(40);
    printf("Start of EPC 4\n");
    echonet_setnEPC(4,EPC_FAULT_STATUS);
    echonet_printPacket(40);
    echonet_setnPDC(4,2);
    echonet_printPacket(40);
    printf("Start of EPC 5\n");
    echonet_setnEPC(5,EPC_OPERATIONAL_STATUS);
    echonet_printPacket(40);
    echonet_setnPDC(5,5);
    echonet_printPacket(40);
    printf("Start of EPC 6\n");
    echonet_setnEPC(6,EPC_FAULT_STATUS);
    echonet_printPacket(40);
    char property_value[echonet_getnPDC(4)];
    for(i=0;i<echonet_getnPDC(5);i++)
    {
        property_value[i]=i;
    }
    echonet_setnEDT(5,property_value);
    echonet_printPacket(40);
    printf("Getting EPC code: 0x%1X\n",echonet_getnEPC(4));
    printf("Getiing TID: 0x%X\n",echonet_getTID());
    printf("Getting EHD1: 0x%X\n",echonet_getEHD1());
    memset(property_value, 0, echonet_getnPDC(5));
    printf("Getting EDT: ");
    for(i=0;i<echonet_getnPDC(5);i++)
    {
        printf("0x%X ",property_value[i]);
    }
    echonet_getnEDT(5, property_value);
    printf("\nGetting EDT: ");
    for(i=0;i<echonet_getnPDC(5);i++)
    {
         printf("0x%X ",property_value[i]);
    }
    
    
    
    
    
    printf("\n\n\n\n");
    echonet_clearPacket_all();
    printf("\n");
    echonet_setEHD1();
    echonet_setEHD2(1);
    echonet_setTID(0x0000);
    echonet_setSEOJ(0x00,0x00,0x00);
    echonet_setDEOJ(CGC_SENSOR_RELATED, CC_TEMPERTURE_SENSOR,0x01);
    //echonet_setDEOJ(CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE);
    echonet_setESV(ESV_Get);
    echonet_setOPC(1);
    echonet_setnEPC(1,0x80);
    echonet_setnPDC(1,0);
    //echonet_setnEPC(2,0xE0);
    //echonet_setnPDC(2,0);
    echonet_routine(14);
    //echonet_getNodeInfo_SNILS("150.65.230.104");
    //echonet_getObject_sensorTemp_reading("150.65.230.104",0x01);
    
    /*while(1)
    {
     
        sleep(10);
    }*/
    
    return 0;
}
