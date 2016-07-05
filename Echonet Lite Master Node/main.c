//
//  main.c
//  Echonet Lite Master Node
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#include <stdio.h>
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
    //int i;
    char buffer[DEFAULT_BUFFER_SIZE];
    unsigned long packet_length;
    init_network("127.0.0.1");
    echonet_setEHD2(1);
    echonet_setTID(0xABCD);
    echonet_printpacket(10);
    echonet_clearpacket_data();
    echonet_printpacket(10);
    while(1)
    {
        printf("Type a sentence to send to server:\n");
        fgets(buffer,DEFAULT_BUFFER_SIZE,stdin);
        packet_length=3;
        send_udp(buffer,&packet_length);
        memset(buffer, 0, sizeof(buffer));
        recv_udp(buffer,&packet_length);
        
        //printf("Received from server: %s\n",buffer);
        printf("Packet size: %lu\n",packet_length);
        printf("Received: %s",buffer);
        /*for(i=0;i<(packet_length);i++)
        {
            printf("%c",buffer[i]);
        }*/
        printf("!");
    }
    
    return 0;
}
