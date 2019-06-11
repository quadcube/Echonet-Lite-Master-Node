//
//  echonet_lite.c
//  Echonet Lite Test
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#include "echonet_lite.h"

/*
 Echonet Lite Packet
 |   EHD1   |   EHD2   |   TID   |   EDATA (SEOJ,DEOJ,ESV,OPC,EPC1,PDC1,EDT1,...)   |
 */
//#define EHD1                echonet_packet[0]
//#define EHD2                echonet_packet[1]
//#define ESV                 echonet_packet[10]
//#define OPC                 echonet_packet[11]

char echonet_packet[DEFAULT_BUFFER_SIZE];
char prev_echonet_packet[12];

/*
 Set Echonet Lite Header 1
 */
void echonet_setEHD1(void)
{
    echonet_packet[0]=EHD1_ECHONET;
}

/*
 Get Echonet Lite Header 1
 */
uint8_t echonet_getEHD1(void)
{
    return echonet_packet[0];
}

/*
 Set Echonet Lite Header 2
 Note: EHD2 default format 1 (specified message format)
 */
void echonet_setEHD2(uint8_t value)
{
    if(value==1)
        echonet_packet[1]=EHD2_FORMAT1;
    else if(value==2)
        echonet_packet[1]=EHD2_FORMAT2;
    else
    {
        //invalid input,revert to default: FORMAT1
        echonet_packet[1]=EHD2_FORMAT1;
        printf("Invalid EHD format! Default selected: FORMAT 1");
    }
}

/*
 Get Echonet Lite Header 2
 */
uint8_t echonet_getEHD2(void)
{
    return echonet_packet[1];
}

/*
 Set Echonet Lite Transaction ID
 Note: TID 2 bytes
 */
void echonet_setTID(uint16_t value)
{
    echonet_packet[2]=(char)(value>>8);
    echonet_packet[3]=value&0xFF;
}

/*
 Get Echonet Lite Transaction ID
 Note: TID 2 bytes
 */
uint16_t echonet_getTID(void)
{
    return ((unsigned char)echonet_packet[2]<<8 | ((unsigned char)echonet_packet[3] & 0xFF));
}

/*
 Set Echonet Lite Object (Source)
 Note: 1. EOJ 3 bytes
       2. echonet_setSEOJ(CG_xx,CC_xx,IC_xx)
 */
void echonet_setSEOJ(char class_group_code,char class_code,char instance_code)
{
    echonet_packet[4]=class_group_code;
    echonet_packet[5]=class_code;
    echonet_packet[6]=instance_code;
}

/*
 Get Echonet Lite Object (Source)
 */
void echonet_getSEOJ(char *class_group_code,char *class_code,char *instance_code)
{
    class_group_code=&echonet_packet[4];
    class_code=&echonet_packet[5];
    instance_code=&echonet_packet[6];
}

/*
 Set Echonet Lite Object (Destination)
 Note: 1. EOJ 3 bytes
 2. echonet_setDEOJ(CG_xx,CC_xx,IC_xx)
 */
void echonet_setDEOJ(char class_group_code,char class_code,char instance_code)
{
    echonet_packet[7]=class_group_code;
    echonet_packet[8]=class_code;
    echonet_packet[9]=instance_code;
}

/*
 Get Echonet Lite Object (Destination)
 */
void echonet_getDEOJ(char *class_group_code,char *class_code,char *instance_code)
{
    class_group_code=&echonet_packet[7];
    class_code=&echonet_packet[8];
    instance_code=&echonet_packet[9];
}

/*
 Set Echonet Lite Service
 Note: 1. Request
       2. Response/Notification
       3. Response not possible
 */
void echonet_setESV(char service_code)
{
    echonet_packet[10]=service_code;
}

/*
 Get Echonet Lite Service
 */
uint8_t echonet_getESV(void)
{
    return echonet_packet[10];
}

/*
 Set Echonet Lite Processing Target Property Counters
 Note: 1. Linked to nEPC,nPDC,nEDT
       2. Set OPC before setting nEPC,nPDC,nEDT
 */
void echonet_setOPC(char value)
{
    echonet_packet[11]=value;
}

/*
 Get Echonet Lite Processing Target Property Counters
 */
uint8_t echonet_getOPC(void)
{
    return echonet_packet[11];
}

/*
 Set Echonet Lite Property
 Note: 1. 1 byte (X1-class group code,X2-class code)
       2. Refer to Table 3.12 EPC Code Allocation Table
       3. Value n>0,n<=OPC value (Request 1,2,3,...,n)
       4. echonet_setnEPC(OPC limited n,EPC_xx)
 */
void echonet_setnEPC(char n,char epc_code)
{
    int i,j=0,counter=0;
    if(n<=echonet_packet[11] && n!=0)
    {
        if(n==1)
            echonet_packet[12]=epc_code;
        else
        {
            for(i=1;i<n;i++)
            {
                counter+=(unsigned char)echonet_packet[12+counter+i+j];
                if((n-1)==i)
                    break;
                j++;
            }
            echonet_packet[12+counter+(i*2)]=epc_code;
        }
        
    }
    else
        printf("Input value is larger than OPC value/Input value is zero!");
}

/*
 Get Echonet Lite Property
 Note: Error,return 0
 */
uint8_t echonet_getnEPC(char n)
{
    int i,j=0,counter=0;
    if(n<=echonet_packet[11] && n!=0)
    {
        if(n==1)
            return echonet_packet[12];
        else
        {
            for(i=1;i<n;i++)
            {
                counter+=(unsigned char)echonet_packet[12+counter+i+j];
                if((n-1)==i)
                    break;
                j++;
            }
            return echonet_packet[12+counter+(i*2)];
        }
        
    }
    else
    {
        printf("Input value is larger than OPC value/Input value is zero!");
        return 0;
    }
}

/*
 Set Echonet Lite Property Data Counter
 Note: 1. Value n>0,n<=OPC value (Request 1,2,3,...,n)
       2. echonet_setnPDC(OPC limited n,value)
 */
void echonet_setnPDC(char n,char value)
{
    int i,j=0,counter=0;
    if(n<=echonet_packet[11] && n!=0)
    {
        if(n==1)
            echonet_packet[13]=value;
        else
        {
            for(i=1;i<n;i++)
            {
                counter+=(unsigned char)echonet_packet[12+counter+i+j];
                if((n-1)==i)
                    break;
                j++;
            }
            echonet_packet[13+counter+(i*2)]=value;
        }
    }
    else
        printf("Input value is larger than OPC value/Input value is zero!");
}

/*
 Get Echonet Lite Property Data Counter
 Note: Error,return 0
 */
uint8_t echonet_getnPDC(char n)
{
    int i,j=0,counter=0;
    if(n<=echonet_packet[11] && n!=0)
    {
        if(n==1)
            return echonet_packet[13];
        else
        {
            for(i=1;i<n;i++)
            {
                counter+=(unsigned char)echonet_packet[12+counter+i+j];
                if((n-1)==i)
                    break;
                j++;
            }
            return echonet_packet[13+counter+(i*2)];
        }
    }
    else
    {
        printf("Input value is larger than OPC value/Input value is zero!");
        return 0;
    }
}

/*
 Set Echonet Lite Property Value
 Note:
 */
void echonet_setnEDT(char n,char *property_data)
{
    int i,j=0,counter=0;
    if(n<=echonet_packet[11] && n!=0)
    {
        if(n==1)
            for(i=0;i<echonet_getnPDC(n);i++)
                echonet_packet[14+i]=property_data[i];
        else
        {
            for(i=1;i<n;i++)
            {
                counter+=(unsigned char)echonet_packet[12+counter+i+j];
                if((n-1)==i)
                    break;
                j++;
            }
            for(j=0;j<echonet_getnPDC(n);j++)
                echonet_packet[14+counter+(i*2)+j]=property_data[j];
        }
    }
    else
        printf("Input value is larger than OPC value/Input value is zero!");
}

/*
 Get Echonet Lite Property Value
 Note:
 */
void echonet_getnEDT(char n,char *property_data)
{
    int i,j=0,counter=0;
    if(n<=echonet_packet[11] && n!=0)
    {
        if(n==1)
            for(i=0;i<echonet_getnPDC(n);i++)
                property_data[i]=echonet_packet[14+i];
        else
        {
            for(i=1;i<n;i++)
            {
                counter+=(unsigned char)echonet_packet[12+counter+i+j];
                if((n-1)==i)
                    break;
                j++;
            }
            for(j=0;j<echonet_getnPDC(n);j++)
                property_data[j]=echonet_packet[14+counter+(i*2)+j];
        }
    }
    else
        printf("Input value is larger than OPC value/Input value is zero!");
}

/*
 Set Echonet Lite Packet (without Echonet Lite Property Setting and Data: EPC,PDC,EDT)
 Note:
 */
void echonet_setPacket(char ehd1,char ehd2,uint16_t tid,char seoj_cgc,char seoj_cc,char seoj_ic,char deoj_cgc,char deoj_cc,char deoj_ic,char esv,char opc)
{
    echonet_packet[0]=ehd1;
    echonet_packet[1]=ehd2;
    echonet_packet[2]=(char)(tid>>8);
    echonet_packet[3]=tid&0xFF;
    echonet_packet[4]=seoj_cgc;
    echonet_packet[5]=seoj_cc;
    echonet_packet[6]=seoj_ic;
    echonet_packet[7]=deoj_cgc;
    echonet_packet[8]=deoj_cc;
    echonet_packet[9]=deoj_ic;
    echonet_packet[10]=esv;
    echonet_packet[11]=opc;
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Number of Self-Node Instances
 Note:
 */
void echonet_setPacket_getNSNI(void)
{
    echonet_setnEPC(1,EPC_NUMBER_OF_SELF_NODE_INSTANCES);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Number of Self-Node Classes
 Note:
 */
void echonet_setPacket_getNSNC(void)
{
    echonet_setnEPC(1,EPC_NUMBER_OF_SELF_NODE_CLASSES);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Instance List Notification
 Note:
 */
void echonet_setPacket_getILN(void)
{
    echonet_packet[12]=EPC_INSTANCE_LIST_NOTIFICATION;
    echonet_packet[13]=0;
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Self-Node Instance List S
 Note:
 */
void echonet_setPacket_getSNILS(void)
{
    echonet_packet[12]=EPC_SELF_NODE_INSTANCE_LIST_S;
    echonet_packet[13]=0;
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Self-Node Class List S
 Note:
 */
void echonet_setPacket_getSNCLS(void)
{
    echonet_setnEPC(1,EPC_SELF_NODE_CLASS_LIST_S);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Object (Operational Status)
 Note: Usable for all objects,regardless of group classes
 */
void echonet_setPacket_getObject_operationalStatus(void)
{
    echonet_setnEPC(1,EPC_OPERATIONAL_STATUS);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Object (Sensor Reading)
 Note: Temperature Sensor (Class Group Code: 0x00 Class Code: 0x11)
 */
void echonet_setPacket_getObject_sensorTemp(void)
{
    echonet_setnEPC(1,EPC_MEASURE_TEMPERATURE_VALUE);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Object (Sensor Reading)
 Note: Humidity Sensor (Class Group Code: 0x00 Class Code: 0x12)
 */
void echonet_setPacket_getObject_sensorHumid(void)
{
    echonet_setnEPC(1,EPC_MEASURE_HUMIDITY_VALUE);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Object (Sensor Reading)
 Note: Illuminance Sensor (Class Group Code: 0x00 Class Code: 0x0D)
 */
void echonet_setPacket_getObject_sensorIllum_lux(void)
{
    echonet_setnEPC(1, EPC_MEASURE_ILLUMINANCE_LUX_VALUE);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Object (Sensor Reading)
 Note: Illuminance Sensor (Class Group Code: 0x00 Class Code: 0x0D)
 */
void echonet_setPacket_getObject_sensorIllum_kilolux(void)
{
    echonet_setnEPC(1, EPC_MEASURE_ILLUMINANCE_KILOLUX_VALUE);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Object (Sensor Reading)
 Note: Air Speed Sensor (Class Group Code: 0x00 Class Code: 0x1F)
 */
void echonet_setPacket_getObject_sensorAirSpeed(void)
{
    echonet_setnEPC(1, EPC_MEASURE_AIR_SPEED_VALUE);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Get Echonet Lite Object (Sensor Reading)
 Note: 1. Air Speed Sensor (Class Group Code: 0x00 Class Code: 0x1F)
       2. Air Speed Direction
 */
void echonet_setPacket_getObject_sensorAirSpeed_direction(void)
{
    echonet_setnEPC(1, EPC_AIR_FLOW_DIRECTION);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Set Echonet Lite Object (Object Operation Status)
 Note: 1. 
 */
void echonet_setPacket_setObject_operationalStatus(void)
{
    echonet_setnEPC(1, EPC_OPERATIONAL_STATUS);
    echonet_setnPDC(1,1);
}

/*
 Set Echonet Lite Packet to Set Echonet Lite Object (Air Conditioner Current Consumption)
 Note: 1.
 */
void echonet_setPacket_getObject_aircondCurrentConsumption(void)
{
    echonet_setnEPC(1, EPC_AIRCOND_MEASURED_VALUE_OF_CURRENT_CONSUMPTION);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Set Echonet Lite Object (Air Conditioner Room Relative Humidity)
 Note: 1.
 */
void echonet_setPacket_getObject_aircondRoomHumidity(void)
{
    echonet_setnEPC(1, EPC_AIRCOND_MEASURED_VALUE_OF_ROOM_RELATIVE_HUMIDITY);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Set Echonet Lite Object (Air Conditioner Room Temperature)
 Note: 1.
 */
void echonet_setPacket_getObject_aircondRoomTemperature(void)
{
    echonet_setnEPC(1, EPC_AIRCOND_MEASURED_VALUE_OF_ROOM_TEMPERATURE);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Set Echonet Lite Object (Air Conditioner Cooled Air Temperature)
 Note: 1.
 */
void echonet_setPacket_getObject_aircondCooledAirTemperature(void)
{
    echonet_setnEPC(1, EPC_AIRCOND_MEASURED_COOLED_AIR_TEMPERATURE);
    echonet_setnPDC(1,0);
}

/*
 Set Echonet Lite Packet to Set Echonet Lite Object (Air Conditioner Outdoor Air Temperature)
 Note: 1.
 */
void echonet_setPacket_getObject_aircondOutdoorAirTemperature(void)
{
    echonet_setnEPC(1, EPC_AIRCOND_MEASURED_OUTDOOR_AIR_TEMPERATURE);
    echonet_setnPDC(1,0);
}

/*
 Get Echonet Lite Number of Self-Node Instances
 Note:
 */
void echonet_getNodeInfo_NSNI(char *ip_addr,uint16_t TID)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE, ESV_Get, 1);
    echonet_setPacket_getNSNI();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
        echonet_printPacket(echonet_length);
    else
        printf("Network error!\n");
}

/*
 Get Echonet Lite Number of Self-Node Classes
 Note:
 */
void echonet_getNodeInfo_NSNC(char *ip_addr,uint16_t TID)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE, ESV_Get, 1);
    echonet_setPacket_getNSNC();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
        echonet_printPacket(echonet_length);
    else
        printf("Network error!\n");
}

/*
 Get Echonet Lite Instance List Notification
 Note:
 */
void echonet_getNodeInfo_ILN(char *ip_addr,uint16_t TID)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE, ESV_Get, 1);
    echonet_setPacket_getILN();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
        echonet_printPacket(echonet_length);
    else
        printf("Network error!\n");
}

/*
 Get Echonet Lite Self-Node Instance List S
 Note: 1. Check Node Profile Class (Detailed Specifications)
       2. 1st byte = Total number of instances, 2nd to 253th bytes: Echonet Object code (3 bytes each)
 */
void echonet_getNodeInfo_SNILS(char *ip_addr,uint16_t TID)
{
    int i;
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE, ESV_Get, 1);
    echonet_setPacket_getSNILS();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        if(echonet_packet[11]==1)
        {
            printf("Number of instances: %d\n",(unsigned)echonet_packet[14]);
            for(i=1;i<echonet_packet[13];i+=3)
            {
                echonet_printDeviceClassGroup(echonet_packet[14+i]);
                echonet_printDeviceClass(echonet_packet[14+i],echonet_packet[15+i]);
                printf("Instance Code: %d\n\n",(unsigned)echonet_packet[16+i]);
            }
        }
        else
            printf("OPC>1 is not defined!");
    }
    else
        printf("Network error!\n");
}

/*
 Get Echonet Lite Self-Node Class List S
 Note:
 */
void echonet_getNodeInfo_SNCLS(char *ip_addr,uint16_t TID)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE, ESV_Get, 1);
    echonet_setPacket_getSNCLS();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
        echonet_printPacket(echonet_length);
    else
        printf("Network error!\n");
}

/*
 Get Echonet Lite Object (Object Operation Status)
 Note: 1.
 */
uint8_t echonet_getObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,deoj_cgc,deoj_cg,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_operationalStatus();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        return echonet_packet[14];
    }
    else
        return 0;
    
}

/*
 Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Temperature Sensor (Class Group Code: 0x00 Class Code:0x11)
       2. Acquire only one sensor reading
       3. Return float value,converted to degree Celcius
 */
float echonet_getObject_sensorTemp_reading(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    float result=0;
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_TEMPERTURE_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorTemp();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        result=(float)(echonet_packet[14]<<8 | (echonet_packet[15] & 0xFF))/10;
        return result;
    }
    else
        return 0;
}

/*
 Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Humidity Sensor (Class Group Code: 0x00 Class Code: 0x12)
       2. Acquire only one sensor reading
       3. Return float value,converted to 0.0~1.0 range
*/
float echonet_getObject_sensorHumid_reading(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_HUMIDITY_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorHumid();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        return (float)echonet_packet[14]/100;
    }
    else
        return 0;
}

/*
 Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Illumination Sensor (Class Group Code: 0x00 Class Code: 0x0D)
       2. Acquire only one sensor reading
       3. Return float
 */
uint16_t echonet_getObject_sensorIllum_reading(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    uint16_t result=0;
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_ILLUMINANCE_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorIllum_lux();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        result=(echonet_packet[14]<<8 | (echonet_packet[15] & 0xFF));
        if(result==0xFFFF)
        {
            echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_ILLUMINANCE_SENSOR,instance_code, ESV_Get, 1);
            echonet_setPacket_getObject_sensorIllum_kilolux();
            send_udp(echonet_packet,&echonet_length,ip_addr);
            echonet_savePacket();
            recv_udp(echonet_packet,&echonet_length,ip_addr);
            echonet_printPacket(echonet_length);
            result=(echonet_packet[14]<<8 | (echonet_packet[15] & 0xFF));
            if(result==0xFFFF)
                printf("Illuminance Sensor Kilo Lux Overflow!\n");
        }
        return result;
    }
    else
        return 0;
}

/*
 Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Air Speed Sensor (Class Group Code: 0x00 Class Code: 0x1F)
 */
float echonet_getObject_sensorAirSpeed_reading(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    uint16_t result=0;
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_AIR_SPEED_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorAirSpeed();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        result=(echonet_packet[14]<<8 | (echonet_packet[15] & 0xFF));
        return ((float)result/100);
    }
    else
        return 0;
}

/*
 Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Air Speed Sensor (Class Group Code: 0x00 Class Code: 0x1F)
       2. Air Speed Direction (0-360 degree)
 */
uint16_t echonet_getObject_sensorAirSpeed_direction(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    uint16_t result=0;
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_AIR_SPEED_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorAirSpeed_direction();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        result=(echonet_packet[14]<<8 | (echonet_packet[15] & 0xFF));
        return result;
    }
    else
        return 0;
}

/*
 Get Echonet Lite Object (Air Conditioner Only)
 Note: 1. Home Air Conditioner (Class Group Code: 0x01 Class Code: 0x30)
       2. Acquire air conditioner current consumption reading
       3. Return float value,unit=0.1A and returns -1 if there's network error
 */
float echonet_getObject_aircondCurrentConsumption(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    uint16_t result=0;
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_aircondCurrentConsumption();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        result=(echonet_packet[14]<<8 | (echonet_packet[15] & 0xFF));
        return ((float)result/10);
    }
    else
        return -1;
}

/*
 Get Echonet Lite Object (Air Conditioner Only)
 Note: 1. Home Air Conditioner (Class Group Code: 0x01 Class Code: 0x30)
       2. Acquire air conditioner room humidity reading
       3. Return 0~100%, return 255 for error
 */
uint8_t echonet_getObject_aircondRoomHumidity(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_aircondRoomHumidity();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        return echonet_packet[14];
    }
    else
        return 255;
}

/*
 Get Echonet Lite Object (Air Conditioner Only)
 Note: 1. Home Air Conditioner (Class Group Code: 0x01 Class Code: 0x30)
       2. Acquire air conditioner room temperature reading
       3. Return -127~125 Celcius,return 0xFF for error
 */
int8_t echonet_getObject_aircondRoomTemperature(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_aircondRoomTemperature();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        return echonet_packet[14];
    }
    else
        return -128;
}

/*
 Get Echonet Lite Object (Air Conditioner Only)
 Note: 1. Home Air Conditioner (Class Group Code: 0x01 Class Code: 0x30)
       2. Acquire air conditioner cooled air temperature reading
       3. Return -127~125 Celcius,return 0xFF for error
 */
int8_t echonet_getObject_aircondCooledAirTemperature(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_aircondCooledAirTemperature();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        return echonet_packet[14];
    }
    else
        return -128;
}

/*
 Get Echonet Lite Object (Air Conditioner Only)
 Note: 1. Home Air Conditioner (Class Group Code: 0x01 Class Code: 0x30)
       2. Acquire air conditioner outdoor air temperature reading
       3. Return -127~125 Celcius,return 0xFF for error
 */
int8_t echonet_getObject_aircondOutdoorAirTemperature(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_aircondOutdoorAirTemperature();
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        return echonet_packet[14];
    }
    else
        return -128;
}

/*
 (Multi-thread version) Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Air Speed Sensor (Class Group Code: 0x00 Class Code: 0x1F)
 */
void echonetMT_getObject_sensorAirSpeed_reading(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_AIR_SPEED_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorAirSpeed();
    send_udp(echonet_packet,&echonet_length,ip_addr);
}

/*
 (Multi-thread version) Get Echonet Lite Object (Object Operation Status)
 Note: 1.
 */
void echonetMT_getObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,deoj_cgc,deoj_cg,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_operationalStatus();
    send_udp(echonet_packet,&echonet_length,ip_addr);
}

/*
 (Multi-thread version) Get Echonet Lite Object (Power Distribution Board)
 Note: 1. Power Distribution Board Metering (Class Group Code: 0x02 Class Code: 0x87)
       2. Acquire single channel cumulative amount of electric energy (kWh)
 */
void echonetMT_getObject_powerDistributionMeter_reading(char *ip_addr,uint16_t TID,unsigned char instance_code,uint8_t channel)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_HOUSING_RELATED, CC_DISTRIBUTION_PANEL_METERING,instance_code, ESV_Get, 1);
    echonet_setnEPC(1,(0xD0+(channel-1)));
    echonet_setnPDC(1,0);
    send_udp(echonet_packet,&echonet_length,ip_addr);
}

/*
 (Multi-thread version) Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Temperature Sensor (Class Group Code: 0x00 Class Code: 0x11)
       2. Acquire only one sensor reading
 */
void echonetMT_getObject_sensorTemp_reading(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_TEMPERTURE_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorTemp();
    send_udp(echonet_packet,&echonet_length,ip_addr);
}

/*
 (Multi-thread version) Get Echonet Lite Object (Sensor Reading Only)
 Note: 1. Humidity Sensor (Class Group Code: 0x00 Class Code: 0x12)
       2. Acquire only one sensor reading
 */
void echonetMT_getObject_sensorHumid_reading(char *ip_addr,uint16_t TID,unsigned char instance_code)
{
    unsigned long echonet_length=14;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, CC_HUMIDITY_SENSOR,instance_code, ESV_Get, 1);
    echonet_setPacket_getObject_sensorHumid();
    send_udp(echonet_packet,&echonet_length,ip_addr);
}

/*
 Set Echonet Lite Object (Object Operational Status)
 Note: 1.
 */
uint8_t echonet_setObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code,uint8_t operation_status)
{
    unsigned long echonet_length=15;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,deoj_cgc,deoj_cg,instance_code, ESV_SetC, 1);
    echonet_setPacket_setObject_operationalStatus();
    echonet_packet[14]=operation_status;
    send_udp(echonet_packet,&echonet_length,ip_addr);
    echonet_savePacket();
    if(recv_udp(echonet_packet,&echonet_length,ip_addr)!=-1)
    {
        echonet_printPacket(echonet_length);
        return echonet_packet[14];
    }
    else
        return 0;
}

/*
 (Multi-thread version) Set Echonet Lite Object (Object Operational Status)
 Note: 1.
 */
void echonetMT_setObject_operationalStatus(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code,uint8_t operation_status)
{
    unsigned long echonet_length=15;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,deoj_cgc,deoj_cg,instance_code, ESV_SetC, 1);
    echonet_setPacket_setObject_operationalStatus();
    echonet_packet[14]=operation_status;
    send_udp(echonet_packet,&echonet_length,ip_addr);
}

/*
 (Multi-thread version) Set Echonet Lite Object (Universal Object Parameter)
 Note: 1.
 */
void echonetMT_setObject_param_1(char *ip_addr,uint16_t TID,unsigned char deoj_cgc,unsigned char deoj_cg,unsigned char instance_code,unsigned char epc,unsigned char operation_status)
{
    unsigned long echonet_length=15;
    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,deoj_cgc,deoj_cg,instance_code, ESV_SetC, 1);
    echonet_setnEPC(1, epc);
    echonet_setnPDC(1,1);
    echonet_packet[14]=operation_status;
    send_udp(echonet_packet,&echonet_length,ip_addr);
}

/*
 Check Echonet Lite Packet Node Profile Class
 Note:
 */
uint8_t echonet_checkPacket_NodeProfileClass(void)
{
    if(((unsigned)echonet_packet[4]==CGC_PROFILE_CLASS && (unsigned)echonet_packet[5]==CC_NODE_PROFILE && (unsigned)echonet_packet[6]==IC_GENERAL_NODE) && ((unsigned)echonet_packet[7]==CGC_PROFILE_CLASS && (unsigned)echonet_packet[8]==CC_NODE_PROFILE && (unsigned)echonet_packet[9]==IC_GENERAL_NODE))
        return 1;
    else
        return 0;
}

/*
 iHouse sensor node (IP: 192.168.2.202)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 3 airspeed sensor
 */
void echonet_getiHouse_2_202(uint16_t TID,float *sensor_AirSpeed1,float *sensor_AirSpeed2,float *sensor_AirSpeed3,uint8_t *UDP_network_fatal_airspeed_indoor)
{
    float prev_sensor_Airspeed1=*sensor_AirSpeed1,prev_sensor_Airspeed2=*sensor_AirSpeed2,prev_sensor_Airspeed3=*sensor_AirSpeed3;
    *sensor_AirSpeed1=echonet_getObject_sensorAirSpeed_reading("192.168.2.202",TID,0x01);
    *sensor_AirSpeed2=echonet_getObject_sensorAirSpeed_reading("192.168.2.202",(TID+1),0x02);
    *sensor_AirSpeed3=echonet_getObject_sensorAirSpeed_reading("192.168.2.202",(TID+2),0x03);
    if(sensor_AirSpeed1==0 || sensor_AirSpeed2==0 || sensor_AirSpeed3==0)
    {
        *UDP_network_fatal_airspeed_indoor=1;
        if(sensor_AirSpeed1==0)
            *sensor_AirSpeed1=prev_sensor_Airspeed1;
        if(sensor_AirSpeed2==0)
            *sensor_AirSpeed2=prev_sensor_Airspeed2;
        if(sensor_AirSpeed3==0)
            *sensor_AirSpeed3=prev_sensor_Airspeed3;
    }
    else
        *UDP_network_fatal_airspeed_indoor=0;
}

/*
 iHouse sensor node (IP: 192.168.2.145)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 1 airspeed sensor,1 temperature sensor,1 humidity sensor
 */
void echonet_getiHouse_2_145(uint16_t TID,float *sensor_AirSpeedOutdoor,float *sensor_TemperatureOutdoor,float *sensor_HumidityOutdoor,uint8_t *UDP_network_fatal_airspeed_outdoor,uint8_t *UDP_network_fatal_temperature_outdoor)
{
    float prev_sensor_AirSpeedOutdoor=*sensor_AirSpeedOutdoor,prev_sensor_TemperatureOutdoor=*sensor_TemperatureOutdoor,prev_sensor_HumidityOutdoor=*sensor_HumidityOutdoor;
    *sensor_AirSpeedOutdoor=echonet_getObject_sensorAirSpeed_reading("192.168.2.145",TID,0x01);
    *sensor_TemperatureOutdoor=echonet_getObject_sensorTemp_reading("192.168.2.145",(TID+1),0x01);
    *sensor_HumidityOutdoor=echonet_getObject_sensorHumid_reading("192.168.2.145",(TID+2),0x01);
    if(sensor_AirSpeedOutdoor==0)
    {
        *sensor_AirSpeedOutdoor=prev_sensor_AirSpeedOutdoor;
        *UDP_network_fatal_airspeed_outdoor=1;
    }
    else
        *UDP_network_fatal_airspeed_outdoor=0;
    if(sensor_TemperatureOutdoor==0)
    {
        *sensor_TemperatureOutdoor=prev_sensor_TemperatureOutdoor;
        *UDP_network_fatal_temperature_outdoor=1;
    }
    else
        *UDP_network_fatal_temperature_outdoor=0;
    if(sensor_HumidityOutdoor==0)
        *sensor_HumidityOutdoor=prev_sensor_HumidityOutdoor;
}

/*
 iHouse sensor node (IP: 192.168.2.108)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 1 temperature sensor,1 humidity sensor,1 illuminance sensor
 */
void echonet_getiHouse_2_108(uint16_t TID,float *sensor_TemperatureIndoor,float *sensor_HumidityIndoor,uint8_t *UDP_network_fatal_temperature_indoor)
{
    float prev_sensor_TemperatureIndoor=*sensor_TemperatureIndoor,prev_sensor_HumidityIndoor=*sensor_HumidityIndoor;
    *sensor_TemperatureIndoor=echonet_getObject_sensorTemp_reading("192.168.2.108",TID,0x01);
    *sensor_HumidityIndoor=echonet_getObject_sensorHumid_reading("192.168.2.108",(TID+1),0x01);
    if(sensor_TemperatureIndoor==0)
    {
        *UDP_network_fatal_temperature_indoor=1;
        *sensor_TemperatureIndoor=prev_sensor_TemperatureIndoor;
    }
    else
        *UDP_network_fatal_temperature_indoor=0;
    if(sensor_HumidityIndoor==0)
        *sensor_HumidityIndoor=prev_sensor_HumidityIndoor;
}

/*
 iHouse Air Conditioner node (IP: 192.168.2.174)
 Note: 1. Must be connected to iHouse through VPN
       2.
 */
void echonet_getiHouse_2_174(uint16_t TID,uint8_t *state_aircond,float *aircond_current_consumption,uint8_t *aircond_room_humidity,int8_t *aircond_room_temperature,int8_t *aircond_cooled_air_temperature,int8_t *aircond_outdoor_air_temperature,uint8_t *UDP_network_fatal_aircond)
{
    uint8_t prev_state_aircond=*state_aircond,prev_aircond_room_humidity=*aircond_room_humidity,prev_aircond_room_temperature=*aircond_room_temperature,prev_aircond_cooled_air_temperature=*aircond_cooled_air_temperature,prev_aircond_outdoor_air_temperature=*aircond_outdoor_air_temperature;
    float prev_aircond_current_consumption=*aircond_current_consumption;
    *state_aircond=echonet_getObject_operationalStatus("192.168.2.174",TID,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,1);
    *aircond_current_consumption=echonet_getObject_aircondCurrentConsumption("192.168.2.174",(TID+1),1);
    *aircond_room_humidity=echonet_getObject_aircondRoomHumidity("192.168.2.174",(TID+2),1);
    *aircond_room_temperature=echonet_getObject_aircondRoomTemperature("192.168.2.174",(TID+3),1);
    *aircond_cooled_air_temperature=echonet_getObject_aircondCooledAirTemperature("192.168.2.174",(TID+4),1);
    *aircond_outdoor_air_temperature=echonet_getObject_aircondOutdoorAirTemperature("192.168.2.174",(TID+5),1);
    if(state_aircond==0 || aircond_current_consumption<0 || (intptr_t)aircond_room_humidity==255 || (intptr_t)aircond_room_temperature==-128 || (intptr_t)aircond_cooled_air_temperature==-128 || (intptr_t)aircond_outdoor_air_temperature==-128)
    {
        *UDP_network_fatal_aircond=1;
        if(state_aircond==0)
            *state_aircond=prev_state_aircond;
        if(aircond_current_consumption<0)
            *aircond_current_consumption=prev_aircond_current_consumption;
        if((intptr_t)aircond_room_humidity==255)
            *aircond_room_humidity=prev_aircond_room_humidity;
        if((intptr_t)aircond_room_temperature==-128)
            *aircond_room_temperature=prev_aircond_room_temperature;
        if((intptr_t)aircond_cooled_air_temperature==-128)
            *aircond_cooled_air_temperature=prev_aircond_cooled_air_temperature;
        if((intptr_t)aircond_outdoor_air_temperature==-128)
            *aircond_outdoor_air_temperature=prev_aircond_outdoor_air_temperature;
    }
    else
        *UDP_network_fatal_aircond=0;
}

/*
 iHouse Window node (IP: 192.168.2.166)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 2 switch objects (open,close)
 */
void echonet_getiHouse_2_166(uint16_t TID,uint8_t *state_window1,uint8_t *UDP_network_fatal_window1)
{
    uint8_t prev_state_window1=*state_window1,current_state_open;
    current_state_open=echonet_getObject_operationalStatus("192.168.2.166",TID,CGC_MANAGEMENT_RELATED,CC_SWITCH,1);
    if(current_state_open==0)
    {
        *state_window1=prev_state_window1;
        *UDP_network_fatal_window1=1;
    }
    else
    {
        if(current_state_open==0x30)
            *state_window1=1;
        else
            *state_window1=0;
        *UDP_network_fatal_window1=0;
    }
}

/*
 iHouse Window node (IP: 192.168.2.167)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 2 switch objects (open,close)
 */
void echonet_getiHouse_2_167(uint16_t TID,uint8_t *state_window2,uint8_t *UDP_network_fatal_window2)
{
    uint8_t prev_state_window2=*state_window2,current_state_open;
    current_state_open=echonet_getObject_operationalStatus("192.168.2.167",TID,CGC_MANAGEMENT_RELATED,CC_SWITCH,1);
    if(current_state_open==0)
    {
        *state_window2=prev_state_window2;
        *UDP_network_fatal_window2=1;
    }
    else
    {
        if(current_state_open==0x30)
            *state_window2=1;
        else
            *state_window2=0;
        *UDP_network_fatal_window2=0;
    }
}

/*
 iHouse Curtain node (IP: 192.168.2.158)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 2 switch objects (open,close)
 */
void echonet_getiHouse_2_158(uint16_t TID,uint8_t *state_curtain,uint8_t *UDP_network_fatal_curtain)
{
    uint8_t prev_state_curtain=*state_curtain,current_state_open;
    current_state_open=echonet_getObject_operationalStatus("192.168.2.158",TID,CGC_MANAGEMENT_RELATED,CC_SWITCH,1);
    if(current_state_open==0)
    {
        *state_curtain=prev_state_curtain;
        *UDP_network_fatal_curtain=1;
    }
    else
    {
        if(current_state_open==0x30)
            *state_curtain=1;
        else
            *state_curtain=0;
        *UDP_network_fatal_curtain=0;
    }
}

/*
 (Multi-thread version) iHouse sensor node (IP: 192.168.2.202)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 3 airspeed sensor
 */
void echonetMT_getiHouse_2_202(uint16_t TID)
{
    echonetMT_getObject_sensorAirSpeed_reading("192.168.2.202",TID,0x01);
    echonetMT_getObject_sensorAirSpeed_reading("192.168.2.202",(TID+1),0x02);
    echonetMT_getObject_sensorAirSpeed_reading("192.168.2.202",(TID+2),0x03);
}

/*
 (Multi-thread version) iHouse sensor node (IP: 192.168.2.145)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 1 airspeed sensor,1 temperature sensor,1 humidity sensor
 */
void echonetMT_getiHouse_2_145(uint16_t TID)
{
    echonetMT_getObject_sensorAirSpeed_reading("192.168.2.145",TID,0x01);
    echonetMT_getObject_sensorTemp_reading("192.168.2.145",(TID+1),0x01);
    echonetMT_getObject_sensorHumid_reading("192.168.2.145",(TID+2),0x01);
}

/*
 (Multi-thread version) iHouse sensor node (IP: 192.168.2.108)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 1 temperature sensor,1 humidity sensor,1 illuminance sensor
 */
void echonetMT_getiHouse_2_108(uint16_t TID)
{
    echonetMT_getObject_sensorTemp_reading("192.168.2.108",TID,0x01);
    echonetMT_getObject_sensorHumid_reading("192.168.2.108",(TID+1),0x01);
}

/*
 (Multi-thread version) iHouse Air Conditioner node (IP: 192.168.2.174)
 Note: 1. Must be connected to iHouse through VPN
       2.
 */
void echonetMT_getiHouse_2_174(uint16_t TID)
{
    echonetMT_getObject_operationalStatus("192.168.2.174",TID,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,1);
    //echonetMT_getObject_aircondCurrentConsumption("192.168.2.174",(TID+1),1);
    //echonetMT_getObject_aircondRoomHumidity("192.168.2.174",(TID+2),1);
    //echonetMT_getObject_aircondRoomTemperature("192.168.2.174",(TID+3),1);
    //echonetMT_getObject_aircondCooledAirTemperature("192.168.2.174",(TID+4),1);
    //echonetMT_getObject_aircondOutdoorAirTemperature("192.168.2.174",(TID+5),1);
}

/*
 (Multi-thread version) iHouse Window node (IP: 192.168.2.166)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 2 switch objects (open,close)
 */
void echonetMT_getiHouse_2_166(uint16_t TID)
{
    echonetMT_getObject_operationalStatus("192.168.2.166",TID,CGC_MANAGEMENT_RELATED,CC_SWITCH,1);
}

/*
 (Multi-thread version) iHouse Window node (IP: 192.168.2.167)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 2 switch objects (open,close)
 */
void echonetMT_getiHouse_2_167(uint16_t TID)
{
    echonetMT_getObject_operationalStatus("192.168.2.167",TID,CGC_MANAGEMENT_RELATED,CC_SWITCH,1);
}

/*
 (Multi-thread version) iHouse Curtain node (IP: 192.168.2.158)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 2 switch objects (open,close)
 */
void echonetMT_getiHouse_2_158(uint16_t TID)
{
    echonetMT_getObject_operationalStatus("192.168.2.158",TID,CGC_MANAGEMENT_RELATED,CC_SWITCH,1);
}

/*
 (Multi-thread version) iHouse Air Conditioner node (IP: 192.168.2.174)
 Note: 1. Must be connected to iHouse through VPN
 2.
 */
void echonetMT_setiHouse_2_174(uint16_t TID,char season)
{
    echonetMT_setObject_operationalStatus("192.168.2.174",TID,CGC_AIR_CONDITIONER_RELATED,CC_HOME_AIR_CONDITIONER,1,OS_ON);
    
    //echonetMT_getObject_aircondCurrentConsumption("192.168.2.174",(TID+1),1);
    //echonetMT_getObject_aircondRoomHumidity("192.168.2.174",(TID+2),1);
    //echonetMT_getObject_aircondRoomTemperature("192.168.2.174",(TID+3),1);
    //echonetMT_getObject_aircondCooledAirTemperature("192.168.2.174",(TID+4),1);
    //echonetMT_getObject_aircondOutdoorAirTemperature("192.168.2.174",(TID+5),1);
}

/*
 (Multi-thread version) iHouse rain sensor node (IP: 192.168.2.146)
 Note: 1. Must be connected to iHouse through VPN
       2. Contain 1 rain sensor
 */
//void echonetMT_getiHouse_2_146(uint16_t TID)
//{
//
//    unsigned long echonet_length=14;
//    echonet_setPacket(EHD1_ECHONET, EHD2_FORMAT1, TID, CGC_PROFILE_CLASS,CC_NODE_PROFILE,IC_GENERAL_NODE,CGC_SENSOR_RELATED, 0x13,1, ESV_Get, 1);
//    echonet_setnEPC(1, 0xB1);
//    echonet_setnPDC(1,0);
//    send_udp(echonet_packet,&echonet_length,"192.168.2.146");
//}

/*
 Save to Previous Echonet Lite Packet Buffer
 Note: Save packet information except property payload
 */
void echonet_savePacket(void)
{
    int i;
    for(i=0;i<12;i++)
        prev_echonet_packet[i]=echonet_packet[i];
}

/*
 Print Related Echonet Lite Class Group Code
 Note:
 */
void echonet_printDeviceClassGroup(unsigned char class_group_code)
{
    printf("Class Group Code: ");
    switch (class_group_code)
    {
        case CGC_SENSOR_RELATED:
            printf("Sensor Related (0x%1X)",class_group_code);
            break;
        case CGC_AIR_CONDITIONER_RELATED:
            printf("Air Conditioner Related (0x%1X)",class_group_code);
            break;
        case CGC_HOUSING_RELATED:
            printf("Housing/Facility Related (0x%1X)",class_group_code);
            break;
        case CGC_COOKING_RELATED:
            printf("Cooking/Household Related (0x%1X)",class_group_code);
            break;
        case CGC_HEALTH_RELATED:
            printf("Health Related (0x%1X)",class_group_code);
            break;
        case CGC_MANAGEMENT_RELATED:
            printf("Management/Operation Related (0x%1X)",class_group_code);
            break;
        case CGC_AV_RELATED:
            printf("Audiovisual Related (0x%1X)",class_group_code);
            break;
        case CGC_PROFILE_CLASS:
            printf("Profile Class Related (0x%1X)",class_group_code);
            break;
        case CGC_USER_DEFINITION_CLASS:
            printf("User Definition Related (0x%1X)",class_group_code);
            break;
        default:
            printf("Input not defined in class group code (0x%1X)",class_group_code);
            break;
    }
    printf("\n");
}

/*
 Print Related Device Class Group for Echonet Lite Class Group Code
 Note:
 */
void echonet_printDeviceClass(unsigned char class_group_code,unsigned char class_code)
{
    switch (class_group_code)
    {
        case CGC_SENSOR_RELATED:
            printf("Class Code: ");
            switch (class_code)
        {
            case CC_GAS_LEAK_SENSOR:
                printf("Gas Leak Sensor (0x%1X)",class_code);
                break;
            case CC_CRIME_PREVENTION_SENSOR:
                printf("Crime Prevention Sensor (0x%1X)",class_code);
                break;
            case CC_EMERGENCY_BUTTON:
                printf("Emergency Button (0x%1X)",class_code);
                break;
            case CC_FIRST_AID_SENSOR:
                printf("First Aid Sensor (0x%1X)",class_code);
                break;
            case CC_EARTHQUAKE_SENSOR:
                printf("Earthquake Sensor (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_LEAK_SENSOR:
                printf("Electric Leak Sensor (0x%1X)",class_code);
                break;
            case CC_HUMAN_DETECTION_SENSOR:
                printf("Human Detection Sensor (0x%1X)",class_code);
                break;
            case CC_VISITOR_SENSOR:
                printf("Visitor Sensor (0x%1X)",class_code);
                break;
            case CC_CALL_SENSOR:
                printf("Call Sensor (0x%1X)",class_code);
                break;
            case CC_CONDENSATION_SENSOR:
                printf("Condensation Sensor (0x%1X)",class_code);
                break;
            case CC_AIR_POLLUTION_SENSOR:
                printf("Air Pollution Sensor (0x%1X)",class_code);
                break;
            case CC_OXYGEN_SENSOR:
                printf("Oxygen Sensor (0x%1X)",class_code);
                break;
            case CC_ILLUMINANCE_SENSOR:
                printf("Illuminance Sensor (0x%1X)",class_code);
                break;
            case CC_SOUND_SENSOR:
                printf("Sound Sensor (0x%1X)",class_code);
                break;
            case CC_MAILING_SENSOR:
                printf("Mailing Sensor (0x%1X)",class_code);
                break;
            case CC_WEIGHT_SENSOR:
                printf("Weight Sensor (0x%1X)",class_code);
                break;
            case CC_TEMPERTURE_SENSOR:
                printf("Temperature Sensor (0x%1X)",class_code);
                break;
            case CC_HUMIDITY_SENSOR:
                printf("Humidity Sensor (0x%1X)",class_code);
                break;
            case CC_RAIN_SENSOR:
                printf("Rain Sensor (0x%1X)",class_code);
                break;
            case CC_WATER_LEVEL_SENSOR:
                printf("Water Level Sensor (0x%1X)",class_code);
                break;
            case CC_BATH_WATER_LEVEL_SENSOR:
                printf("Bath Water Level Sensor (0x%1X)",class_code);
                break;
            case CC_BATH_HEATING_STATUS_SENSOR:
                printf("Bath Heating Status Sensor (0x%1X)",class_code);
                break;
            case CC_WATER_LEAK_SENSOR:
                printf("Water Leak Sensor (0x%1X)",class_code);
                break;
            case CC_WATER_OVERFLOW_SENSOR:
                printf("Water Overflow Sensor (0x%1X)",class_code);
                break;
            case CC_FIRE_SENSOR:
                printf("Fire Sensor (0x%1X)",class_code);
                break;
            case CC_CIGARETTE_SMOKE_SENSOR:
                printf("Cigarette Smoke Sensor (0x%1X)",class_code);
                break;
            case CC_CO2_SENSOR:
                printf("CO2 Sensor (0x%1X)",class_code);
                break;
            case CC_GAS_SENSOR:
                printf("Gas Sensor (0x%1X)",class_code);
                break;
            case CC_VOC_SENSOR:
                printf("VOC Sensor (0x%1X)",class_code);
                break;
            case CC_DIFFERENTIAL_PRESSURE_SENSOR:
                printf("Differential Pressure Sensor (0x%1X)",class_code);
                break;
            case CC_AIR_SPEED_SENSOR:
                printf("Air Speed Sensor (0x%1X)",class_code);
                break;
            case CC_ODOR_SENSOR:
                printf("Odor Sensor (0x%1X)",class_code);
                break;
            case CC_FLAME_SENSOR:
                printf("Flame Sensor (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_ENERGY_SENSOR:
                printf("Electric Energy Sensor (0x%1X)",class_code);
                break;
            case CC_CURRENT_VALUE_SENSOR:
                printf("Current Value Sensor (0x%1X)",class_code);
                break;
            case CC_DAYLIGHT_SENSOR:
                printf("Daylight Sensor (0x%1X)",class_code);
                break;
            case CC_WATER_FLOW_RATE_SENSOR:
                printf("Water Flow Rate Sensor (0x%1X)",class_code);
                break;
            case CC_MICROMOTION_SENSOR:
                printf("Micromotion Sensor (0x%1X)",class_code);
                break;
            case CC_PASSAGE_SENSOR:
                printf("Passage Sensor (0x%1X)",class_code);
                break;
            case CC_BED_PRESENCE_SENSOR:
                printf("Bed Presence Sensor (0x%1X)",class_code);
                break;
            case CC_OPEN_CLOSE_SENSOR:
                printf("Open/Close Sensor (0x%1X)",class_code);
                break;
            case CC_ACTIVITY_AMOUNT_SENSOR:
                printf("Activity Amount Sensor (0x%1X)",class_code);
                break;
            case CC_HUMAN_BODY_LOCATION_SENSOR:
                printf("Human Body Location Sensor (0x%1X)",class_code);
                break;
            case CC_SNOW_SENSOR:
                printf("Snow Sensor (0x%1X)",class_code);
                break;
            case CC_AIR_PRESSURE_SENSOR:
                printf("Air Pressure Sensor (0x%1X)",class_code);
                break;
            default:
                printf("Input not defined in Sensor-related class code (0x%1X)",class_code);
                break;
        }
            printf("\n");
            break;
        case CGC_AIR_CONDITIONER_RELATED:
            printf("Class Code: ");
            switch (class_code)
        {
            case CC_HOME_AIR_CONDITIONER:
                printf("Home Air Conditioner Device (0x%1X)",class_code);
                break;
            case CC_COLD_BLASTER:
                printf("Cold Blaster Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_FAN:
                printf("Electric Fan Device (0x%1X)",class_code);
                break;
            case CC_VENTILATION_FAN:
                printf("Ventilation Fan Device (0x%1X)",class_code);
                break;
            case CC_AIR_CONDITIONER_VENTILATION_FAN:
                printf("Air Conditioner Ventilation Fan Device (0x%1X)",class_code);
                break;
            case CC_AIR_CLEANER:
                printf("Air Cleaner Device (0x%1X)",class_code);
                break;
            case CC_COLD_BLAST_FAN:
                printf("Cold Blast Fan Device (0x%1X)",class_code);
                break;
            case CC_CIRCULATOR:
                printf("Circulator Device (0x%1X)",class_code);
                break;
            case CC_DEHUMIDIFIER:
                printf("Dehumidifier Device (0x%1X)",class_code);
                break;
            case CC_HUMIDIFIER:
                printf("Humidifier Device (0x%1X)",class_code);
                break;
            case CC_CEILING_FAN:
                printf("Ceiling Fan Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_KOTATSU:
                printf("Electric Kotatsu Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_HEATING_PAD:
                printf("Electric Heating Pad Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_BLANKET:
                printf("Electric Blanket Device (0x%1X)",class_code);
                break;
            case CC_SPACE_HEATER:
                printf("Space Heater Device (0x%1X)",class_code);
                break;
            case CC_PANEL_HEATER:
                printf("Panel Heater Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_CARPET:
                printf("Electric Carpet Device (0x%1X)",class_code);
                break;
            case CC_FLOOR_HEATER_0x01:
                printf("Floor Heater Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_HEATER:
                printf("Electric Heater Device (0x%1X)",class_code);
                break;
            case CC_FAN_HEATER:
                printf("Fan Heater Device (0x%1X)",class_code);
                break;
            case CC_BATTERY_CHARGER:
                printf("Battery Charger Device (0x%1X)",class_code);
                break;
            case CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_INDOOR:
                printf("Indoor Package Type Commercial Air Conditioner Device (0x%1X)",class_code);
                break;
            case CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_OUTDOOR:
                printf("Outdoor Package Type Commercial Air Conditioner Device (0x%1X)",class_code);
                break;
            case CC_PACKAGE_TYPE_COMMERCIAL_AIRCOND_THERMAL:
                printf("Thermal Package Type Commercial Air Conditioner Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_FAN_COIL_UNIT:
                printf("Commercial Fan Coil Unit Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_AIRCOND_COLD_SOURCE_CHILLER:
                printf("Commercial Air Conditioner Cold Source Chiller Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_AIRCOND_HOT_SOURCE_BOILER:
                printf("Commercial Air Conditioner Hot Source Boiler Device (0x%1X)",class_code);
                break;
            case CC_AIRCOND_VAV_FOR_COMMERCIAL_APPLICATIONS:
                printf("Air Conditioner VAV for Commercial Applications Device (0x%1X)",class_code);
                break;
            case CC_AIRCOND_FOR_COMMERCIAL_APPLICATIONS:
                printf("Air Conditioner for Commerial Applications Device (0x%1X)",class_code);
                break;
            case CC_UNIT_COOLER:
                printf("Unit Cooler Device (0x%1X)",class_code);
                break;
            case CC_CONDENSING_UNIT_FOR_COMMERCIAL_APP:
                printf("Condensing Unit for Commercial Applications Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_STORAGE_HEATER:
                printf("Electric Storage Heater Device (0x%1X)",class_code);
                break;
            default:
                printf("Input not defined in Air Conditioner-related class code (0x%1X)",class_code);
                break;
        }
            printf("\n");
            break;
        case CGC_HOUSING_RELATED:
            printf("Class Code: ");
            switch (class_code)
        {
            case CC_ELECTRICALLY_OPERATED_BLIND:
                printf("Electrically Operated Blind Device (0x%1X)",class_code);
                break;
            case CC_ELECTRICALLY_OPERATED_SHUTTER:
                printf("Electrically Operated Shutter Device (0x%1X)",class_code);
                break;
            case CC_ELECTRICALLY_OPERATED_CURTAIN:
                printf("Electrically Operated Curtain Device (0x%1X)",class_code);
                break;
            case CC_ELECTRICALLY_OPERATED_RAIN_SLIDING_DOOR:
                printf("Electrically Operated Rain Sliding Door Device (0x%1X)",class_code);
                break;
            case CC_ELECTRICALLY_OPERATED_GATE:
                printf("Electrically Operated Gate Device (0x%1X)",class_code);
                break;
            case CC_ELECTRICALLY_OPERATED_WINDOW:
                printf("Electrically Operated Window Device (0x%1X)",class_code);
                break;
            case CC_AUTOMATICALLY_OPERATED_ENTRANCE_DOOR:
                printf("Automatically Operated Entrance Door Device (0x%1X)",class_code);
                break;
            case CC_GARDEN_SPRINKLER:
                printf("Garden Sprinkler Device (0x%1X)",class_code);
                break;
            case CC_FIRE_SPRINKLER:
                printf("Fire Sprinkler Device (0x%1X)",class_code);
                break;
            case CC_FOUNTAIN:
                printf("Fountain Device (0x%1X)",class_code);
                break;
            case CC_INSTANTANEOUS_WATER_HEATER:
                printf("Instantaneous Water Heater Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_WATER_HEATER:
                printf("Electric Water Heater Device (0x%1X)",class_code);
                break;
            case CC_SOLAR_WATER_HEATER:
                printf("Solar Water Heater Device (0x%1X)",class_code);
                break;
            case CC_CIRCULATION_PUMP:
                printf("Circulation Pump Device (0x%1X)",class_code);
                break;
            case CC_BIDET_EQUIPPED_TOILET:
                printf("Bidet Equipped Toilet Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_LOCK:
                printf("Electric Lock Device (0x%1X)",class_code);
                break;
            case CC_GAS_LINE_VALVE:
                printf("Gas Line Valve Device (0x%1X)",class_code);
                break;
            case CC_HOME_SAUNA:
                printf("Home Sauna Device (0x%1X)",class_code);
                break;
            case CC_HOT_WATER_GENERATOR:
                printf("Hot Water Generator Device (0x%1X)",class_code);
                break;
            case CC_BATHROOM_DRYER:
                printf("Bathroom Dryer Device (0x%1X)",class_code);
                break;
            case CC_HOME_ELEVATOR:
                printf("Home Elevator Device (0x%1X)",class_code);
                break;
            case CC_ELECTRICALLY_OPERATED_ROOM_DIVIDER:
                printf("Electrically Operated Room Divider Device (0x%1X)",class_code);
                break;
            case CC_HORIZONTAL_TRANSFER:
                printf("Horizontal Transfer Device (0x%1X)",class_code);
                break;
            case CC_ELECTRICALLY_OPERATED_CLOTH_DRYING_POLE:
                printf("Electrically Operated Cloth Drying Pole Device (0x%1X)",class_code);
                break;
            case CC_SEPTIC_TANK:
                printf("Septic Tank Device (0x%1X)",class_code);
                break;
            case CC_HOME_SOLAR_POWER_GENERATION:
                printf("Home Solar Power Generation Device (0x%1X)",class_code);
                break;
            case CC_COLD_HOT_WATER_HEAT_SOURCE_EQUIPMENT:
                printf("Cold/Hot Water Source Equipment Device (0x%1X)",class_code);
                break;
            case CC_FLOOR_HEATER_0x02:
                printf("Floor Heater Device (0x%1X)",class_code);
                break;
            case CC_FUEL_CELL:
                printf("Fuel Cell Device (0x%1X)",class_code);
                break;
            case CC_STORAGE_BATTERY:
                printf("Storage Battery Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_VEHICLE_CHARGER_DISCHARGER:
                printf("Electric Vehicle Charger/Discharger Device (0x%1X)",class_code);
                break;
            case CC_ENGINE_COGENERATION:
                printf("Engine Cogeneration Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_ENERGY_METER:
                printf("Electric Energy Meter Device (0x%1X)",class_code);
                break;
            case CC_WATER_FLOW_METER:
                printf("Water Flow Meter Device (0x%1X)",class_code);
                break;
            case CC_GAS_METER:
                printf("Gas Meter Device (0x%1X)",class_code);
                break;
            case CC_LP_GAS_METER:
                printf("LP Gas Mater Device (0x%1X)",class_code);
                break;
            case CC_CLOCK:
                printf("Clock Device (0x%1X)",class_code);
                break;
            case CC_AUTOMATIC_DOOR:
                printf("Automatic Door Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_ELEVATOR:
                printf("Commercial Elevator Device (0x%1X)",class_code);
                break;
            case CC_DISTRIBUTION_PANEL_METERING:
                printf("Distribution Panel Metering Device (0x%1X)",class_code);
                break;
            case CC_LOW_VOLTAGE_SMART_ELECTRIC_ENERGY_METER:
                printf("Low Voltage Smart Electric Energy Meter Device (0x%1X)",class_code);
                break;
            case CC_SMART_GAS_METER:
                printf("Smart Gas Meter Device (0x%1X)",class_code);
                break;
            case CC_HIGH_VOLTAGE_SMART_ELECTRIC_ENERGY_METER:
                printf("High Voltage Smart Electric Energy Meter Device (0x%1X)",class_code);
                break;
            case CC_GENERAL_LIGHTING_CLASS:
                printf("General Lighting Class Device (0x%1X)",class_code);
                break;
            case CC_SINGLE_FUNCTION_LIGHTING:
                printf("Single Function Lighting Device (0x%1X)",class_code);
                break;
            case CC_EMERGENCY_LIGHTING:
                printf("Emergency Lighting Device (0x%1X)",class_code);
                break;
            case CC_EQUIPMENT_LIGHT:
                printf("Equipment Light Device (0x%1X)",class_code);
                break;
            case CC_BUZZER:
                printf("Buzzer Device (0x%1X)",class_code);
                break;
            default:
                printf("Input not defined in Housing/Facility-related class code (0x%1X)",class_code);
                break;
        }
            printf("\n");
            break;
        case CGC_COOKING_RELATED:
            printf("Class Code: ");
            switch (class_code)
        {
            case CC_COFFEE_MACHINE:
                printf("Coffee Machine Device (0x%1X)",class_code);
                break;
            case CC_COFFEE_MILL:
                printf("Coffee Mill Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_HOT_WATER_POT:
                printf("Electric Hot Water Pot Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_STOVE:
                printf("Electric Stove Device (0x%1X)",class_code);
                break;
            case CC_TOASTER:
                printf("Toaster Device (0x%1X)",class_code);
                break;
            case CC_JUICER_FOOD_MIXER:
                printf("Juicer Food Mixer Device (0x%1X)",class_code);
                break;
            case CC_FOOD_PROCESSOR:
                printf("Food Processor Device (0x%1X)",class_code);
                break;
            case CC_REFRIGERATOR:
                printf("Refrigerator Device (0x%1X)",class_code);
                break;
            case CC_COMBINATION_MICROWAVE_OVEN:
                printf("Combination Microwave Oven Device (0x%1X)",class_code);
                break;
            case CC_COOKING_HEATER:
                printf("Cooking Heater Device (0x%1X)",class_code);
                break;
            case CC_OVEN:
                printf("Oven Device (0x%1X)",class_code);
                break;
            case CC_RICE_COOKER:
                printf("Rice Cooker Device (0x%1X)",class_code);
                break;
            case CC_ELECTRONIC_JAR:
                printf("Electronic Jar Device (0x%1X)",class_code);
                break;
            case CC_DISH_WASHER:
                printf("Dish Washer Device (0x%1X)",class_code);
                break;
            case CC_DISH_DRYER:
                printf("Dish Dryer Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_RICE_CARD_COOKER:
                printf("Electric Rice Card Cooker Device (0x%1X)",class_code);
                break;
            case CC_KEEP_WARM_MACHINE:
                printf("Keep Warm Machine Device (0x%1X)",class_code);
                break;
            case CC_RICE_MILL:
                printf("Rice Mill Device (0x%1X)",class_code);
                break;
            case CC_AUTOMATIC_BREAD_COOKER:
                printf("Automatic Bread Cooker Device (0x%1X)",class_code);
                break;
            case CC_SLOW_COOKER:
                printf("Slow Cooker Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_PICKLES_COOKER:
                printf("Electric Pickles Cooker Device (0x%1X)",class_code);
                break;
            case CC_WASHING_MACHINE:
                printf("Washing Machine Device (0x%1X)",class_code);
                break;
            case CC_CLOTHES_DRYER:
                printf("Clothes Dryer Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_IRON:
                printf("Electric Iron Device (0x%1X)",class_code);
                break;
            case CC_TROUSER_PRESS:
                printf("Trouser Press Device (0x%1X)",class_code);
                break;
            case CC_FUTON_DRYER:
                printf("Futon Dryer Device (0x%1X)",class_code);
                break;
            case CC_SMALL_ARTICLE_SHOES_DRYER:
                printf("Small Article Shoes Dryer Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_VACUUM_CLEANER:
                printf("Electric Vacuum Cleaner Device (0x%1X)",class_code);
                break;
            case CC_DISPOSER:
                printf("Disposer Device (0x%1X)",class_code);
                break;
            case CC_ELECTRIC_MOSQUITO_CATCHER:
                printf("Electric Mosquito Catcher Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_SHOW_CASE:
                printf("Commercial Show Case Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_REFRIGERATOR:
                printf("Commercial Refrigerator Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_HOT_CASE:
                printf("Commercial Hot Case Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_FRYER:
                printf("Commercial Fryer Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_MICROWAVE_OVEN:
                printf("Microwave Oven Device (0x%1X)",class_code);
                break;
            case CC_WASHER_AND_DRYER:
                printf("Washer and Dryer Device (0x%1X)",class_code);
                break;
            case CC_COMMERCIAL_SHOW_CASE_OUTDOOR_UNIT:
                printf("Commercial Show Case Outdoor Unit Device (0x%1X)",class_code);
                break;
            default:
                printf("Input not defined in Cooking/Household-related class code (0x%1X)",class_code);
                break;
        }
            printf("\n");
            break;
        case CGC_HEALTH_RELATED:
            printf("Class Code: ");
            switch (class_code)
        {
            case CC_WEIGHTING_MACHINE:
                printf("Weighting Machine Device (0x%1X)",class_code);
                break;
            case CC_CLINICAL_THERMOMETER:
                printf("Clinical Thermometer Device (0x%1X)",class_code);
                break;
            case CC_BLOOD_PRESSURE_METER:
                printf("Blood Pressure Meter Device (0x%1X)",class_code);
                break;
            case CC_BLOOD_SUGAR_METER:
                printf("Blood Sugar Meter Device (0x%1X)",class_code);
                break;
            case CC_BODY_FAT_METER:
                printf("Body Fat Meter Device (0x%1X)",class_code);
                break;
            default:
                printf("Input not defined in Health-related class code (0x%1X)",class_code);
                break;
        }
            printf("\n");
            break;
        case CGC_MANAGEMENT_RELATED:
            printf("Class Code: ");
            switch (class_code)
        {
            case CC_SECURE_COMM_SHARED_KEY_SETUP_NODE:
                printf("Secure Communication Shared Key Setup Node Device (0x%1X)",class_code);
                break;
            case CC_SWITCH:
                printf("Switch Device (0x%1X)",class_code);
                break;
            case CC_PORTABLE_TERMINAL:
                printf("Portable Terminal Device (0x%1X)",class_code);
                break;
            case CC_CONTROLLER:
                printf("Controller Device (0x%1X)",class_code);
                break;
            default:
                printf("Input not defined in Management/Operation-related class code (0x%1X)",class_code);
                break;
        }
            printf("\n");
            break;
        case CGC_AV_RELATED:
            printf("Class Code: ");
            switch (class_code)
        {
            case CC_DISPLAY:
                printf("Display Device (0x%1X)",class_code);
                break;
            case CC_TELEVISION:
                printf("Television Device (0x%1X)",class_code);
                break;
            case CC_AUDIO:
                printf("Audio Device (0x%1X)",class_code);
                break;
            case CC_NETWORK_CAMERA:
                printf("Network Camera Device (0x%1X)",class_code);
                break;
            default:
                printf("Input not defined in Audiovisual-related class code (0x%1X)",class_code);
                break;
        }
            printf("\n");
            break;
        case CGC_PROFILE_CLASS:
            //
            printf("\n");
            break;
        case CGC_USER_DEFINITION_CLASS:
            //
            printf("\n");
            break;
        default:
            break;
    }
}

/*
 Print Echonet Lite Packet Buffer
 Note: printf suppressed to print only unsigned char
 */
void echonet_printPacket(uint16_t value)
{
    uint16_t i;
    if(value<=DEFAULT_BUFFER_SIZE)
    {
        for(i=0;i<value;i++)
            printf("0x%1X ",(unsigned char)echonet_packet[i]);
        printf("\n");
    }
    else
        printf("Input value larger than DEFAULT_BUFFER_SIZE! Input Length: %d\n",value);
}

/*
 Clear Echonet Lite Property Read/Write/Notify Data (only)
 Note: EHD1,EHD2,TID,EOJ,ESV,OPC are retained (9 bytes)
 */
void echonet_clearPacket_pdata(void)
{
    uint16_t i;
    for(i=9;i<DEFAULT_BUFFER_SIZE;i++)
        echonet_packet[i]=0;
}

/*
 Clear Echonet Lite EDATA (only)
 Note: EHD1,EHD2,TID are retained (4 bytes)
 */
void echonet_clearPacket_edata(void)
{
    uint16_t i;
    for(i=4;i<DEFAULT_BUFFER_SIZE;i++)
        echonet_packet[i]=0;
}

/*
 Clear Echonet Lite Packet Buffer (all)
 */
void echonet_clearPacket_all(void)
{
    memset(echonet_packet, 0, sizeof(echonet_packet));
    printf("Echonet packet cleared!\n");
}

