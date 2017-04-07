//
//  error.c
//  Echonet Lite Test
//
//  Created by QuadCube on 04/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#include "error.h"

/*
 Error Exit with Message
 */
void exit_error(char *string)
{
    perror(string);
    exit(EXIT_FAILURE);
}

/*
 Error Logging
 */
int log_error(uint16_t interface,uint16_t error_type,char *error_msg)
{
    return 0;
}

/*
 Error Exit for SQL
*/
void exit_sql_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}