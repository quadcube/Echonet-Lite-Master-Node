//
//  error.h
//  Echonet Lite Test
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#ifndef error_h
#define error_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include "mysql.h"

//Interface Error Definition
#define ERROR_INTERFACE_UDP_NETWORK                                 1
#define ERROR_INTERFACE_TCP_NETWORK                                 2

//Interface Error Type Definition
#define ERROR_TYPE_SEND_TIMEOUT                                     1
#define ERROR_TYPE_RECV_TIMEOUT                                     2
#define ERROR_TYPE_SEND_EXCEED_MAX_TIMEOUT_RETRY                    3
#define ERROR_TYPE_RECV_EXCEED_MAX_TIMEOUT_RETRY                    4

void exit_error(char *string);
int log_error(uint16_t interface,uint16_t error_type,char *error_msg);
//void exit_sql_error(MYSQL *con);

#endif /* error_h */
