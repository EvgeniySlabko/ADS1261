/* 
 * File:   ADS_Status.h
 * Author: Evgeniy
 *
 * Created on 4 ??????? 2021 ?., 9:53
 */

#ifndef ADS_STATUS_H
#define	ADS_STATUS_H

typedef enum ADS_OPERATION_STATUS
{
    ADS_BUFFER_OVERFLOW,
    ADS_INVALID_HANDLE,
    ADS_SPI_ERROR,
    ADS_COMPLETE, 
    ADS_BUSY , 
    ADS_INVALID_RESPONSE,
    ADS_CONFIGURATION_ERRROR,
    ADS_ALLOCATE_ERROR,
} ADS_OPERATION_STATUS;

#endif	/* ADS_STATUS_H */
