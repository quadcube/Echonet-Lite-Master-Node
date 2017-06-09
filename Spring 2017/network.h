//
//  network.h
//  Echonet Lite Test
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#ifndef network_h
#define network_h

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

//Echonet Lite setting
#define DEFAULT_BUFFER_SIZE             1500
#define DEFAULT_ECHONET_PORT            3610

//UDP network setting
#define RECV_TIMEOUT_SECONDS            1
#define NUMBER_OF_TIMEOUT_RETRY         1

//Function prototype
void init_network(void);
void initMT_network(void);
int send_udp(char *buffer,unsigned long *send_size,char *ip_addr);
int recv_udp(char *buffer,unsigned long *recv_size,char *ip_addr);
uint8_t udp_port_listener_echonet_lite(char *buffer,unsigned long *recv_size);
void return_incoming_ip(char *command,unsigned long *send_size); //temp usage,please clean up function prototype

#endif /* network_h */
