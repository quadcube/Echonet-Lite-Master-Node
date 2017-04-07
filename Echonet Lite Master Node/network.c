//
//  network.c
//  Echonet Lite Test
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#include "network.h"

int masterSocket;
struct sockaddr_in serverAddr;
socklen_t addr_size;
struct timeval timeout_UDP={RECV_TIMEOUT_SECONDS,0}; //set timeout for 2 seconds
char network_buffer[DEFAULT_BUFFER_SIZE];
unsigned long network_send_size;
struct sockaddr_in tempAddr;    //temporary for passing IP address (please clean up function prototype)
/*
 Initialize UDP network setting
 Note:
 */
void init_network(void)
{
    if((masterSocket = socket(PF_INET, SOCK_DGRAM, 0))==-1)
        exit_error("Unable to create UDP socket");
    //Configure UDP address settings for server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_ECHONET_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Bind socket with address struct
    bind(masterSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //Initialize address size variable
    addr_size = sizeof serverAddr;
    setsockopt(masterSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout_UDP,sizeof(struct timeval));
}

/*
 Initialize UDP network setting (Multi-thread version)
 Note:
 */
void initMT_network(void)
{
    if((masterSocket = socket(PF_INET, SOCK_DGRAM, 0))==-1)
        exit_error("Unable to create UDP socket");
    //Configure UDP address settings for server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_ECHONET_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Bind socket with address struct
    bind(masterSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //Initialize address size variable
    addr_size = sizeof serverAddr;
}

/*
 Return node address size
 Note:
 */
socklen_t nodeaddr_size(void)
{
    return addr_size;
}

/*
 Send through UDP network
 Note:
 */
int send_udp(char *buffer,unsigned long *send_size,char *ip_addr)
{
    uint8_t i;
    struct sockaddr_in nodeAddr;
    //Configure UDP address settings for node
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_port = htons(DEFAULT_ECHONET_PORT);
    nodeAddr.sin_addr.s_addr = inet_addr(ip_addr);
    memset(nodeAddr.sin_zero, '\0', sizeof nodeAddr.sin_zero);
    network_send_size=*send_size;
    strncpy(network_buffer,buffer,network_send_size);
    for(i=0;i<NUMBER_OF_TIMEOUT_RETRY;i++)
    {
        if((sendto(masterSocket,buffer,*send_size,0,(struct sockaddr *)&nodeAddr,addr_size))==-1)
        {
            //do something
            printf("UDP send error! Retrying attempt: %d\n",(i+1));
            log_error(ERROR_INTERFACE_UDP_NETWORK,ERROR_TYPE_SEND_TIMEOUT,"");
        }
        else
            return 0;
    }
    //Reach maximum retry attempt
    log_error(ERROR_INTERFACE_UDP_NETWORK,ERROR_TYPE_SEND_EXCEED_MAX_TIMEOUT_RETRY,"");
    return -1;
}

/*
 Receive through UDP network
 Note: need cleaning up! 27/7/16
 */
int recv_udp(char *buffer,unsigned long *recv_size,char *ip_addr)
{
    char from_ip[1024];
    uint8_t i;
    struct sockaddr_in nodeAddr;
    //Configure UDP address settings for node
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_port = htons(DEFAULT_ECHONET_PORT);
    nodeAddr.sin_addr.s_addr = inet_addr(ip_addr);
    memset(nodeAddr.sin_zero, '\0', sizeof nodeAddr.sin_zero);
    //resend previous buffer
    for(i=0;i<NUMBER_OF_TIMEOUT_RETRY;i++)
    {
        *recv_size=recvfrom(masterSocket,buffer,DEFAULT_BUFFER_SIZE,0, (struct sockaddr *)&nodeAddr, &addr_size);
        if(*recv_size==-1)
        {
            printf("UDP receive timeout error! Resending previous payload attempt: %d\n",(i+1));
            //log_error(ERROR_INTERFACE_UDP_NETWORK,ERROR_TYPE_RECV_TIMEOUT,"");
            sendto(masterSocket,network_buffer,network_send_size,0,(struct sockaddr *)&nodeAddr,addr_size);
        }
        else
        {
            inet_ntop(AF_INET, &nodeAddr.sin_addr, from_ip, sizeof(from_ip));
            printf("Packet from: %s Size: %lu\n",from_ip,*recv_size);
            return 0;
        }
    }
    //Reach maximum retry attempt
    log_error(ERROR_INTERFACE_UDP_NETWORK,ERROR_TYPE_RECV_EXCEED_MAX_TIMEOUT_RETRY,"");
    return -1;
}

/*
 UDP Port Listener for Echonet Lite
 Note:
 */
uint8_t udp_port_listener_echonet_lite(char *buffer,unsigned long *recv_size)
{
    struct sockaddr_in nodeAddr;
    //Configure UDP address settings for node
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_port = htons(DEFAULT_ECHONET_PORT);
    //nodeAddr.sin_addr.s_addr = inet_addr(ip_addr);
    memset(nodeAddr.sin_zero, '\0', sizeof nodeAddr.sin_zero);
    //wait for UDP packets
    *recv_size=recvfrom(masterSocket,buffer,DEFAULT_BUFFER_SIZE,0, (struct sockaddr *)&nodeAddr, &addr_size);
    if(*recv_size==-1)
    {
        printf("UDP fatal error!\n");
        log_error(ERROR_INTERFACE_UDP_NETWORK,ERROR_TYPE_RECV_TIMEOUT,"");
        return 0;
    }
    else
    {
        //printf("Packet from: %s Size: %lu\n",inet_ntoa(nodeAddr.sin_addr),*recv_size);
        //tempAddr.sin_addr=nodeAddr.sin_addr;
        tempAddr=nodeAddr;
        return 1;
    }
}

/*
 Temporary function to return values to previous sender IP
 Note: temporary for passing IP address (please clean up function prototype)
 */
void return_incoming_ip(char *command,unsigned long *send_size)
{
    //tempAddr.sin_family = AF_INET;
    //tempAddr.sin_port = htons(DEFAULT_ECHONET_PORT);
    //memset(tempAddr.sin_zero, '\0', sizeof tempAddr.sin_zero);
    printf("%s\n",inet_ntoa(tempAddr.sin_addr));
    if(sendto(masterSocket,command,*send_size,0,(struct sockaddr *)&tempAddr,sizeof tempAddr)==-1)
    {
        printf("sent error!\n");
    }
}
 

