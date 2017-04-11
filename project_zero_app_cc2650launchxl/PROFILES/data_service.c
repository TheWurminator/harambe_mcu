/******************************************************************************
 * Filename:       Data_Service.c
 *
 * Description:    This file contains the implementation of the service.
 *
 *                 Generated by:
 *                 BDS version: 1.1.3135.0
 *                 Plugin:      Texas Instruments BLE SDK GATT Server plugin 1.0.8
 *                 Time:        Mon Apr 10 2017 16:54:01 GMT-05:00
 *

 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <string.h>

//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#ifdef ICALL_LITE
#  include "icall_api.h" // Only exists for 3.x stack version, so use ifdef.
#endif

#include "data_service.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// Data_Service Service UUID
CONST uint8_t DataServiceUUID[ATT_UUID_SIZE] =
{
  DATA_SERVICE_SERV_UUID_BASE128(DATA_SERVICE_SERV_UUID)
};

// speed UUID
CONST uint8_t ds_SpeedUUID[ATT_UUID_SIZE] =
{
  DS_SPEED_UUID_BASE128(DS_SPEED_UUID)
};

// maggyro UUID
CONST uint8_t ds_MaggyroUUID[ATT_UUID_SIZE] =
{
  DS_MAGGYRO_UUID_BASE128(DS_MAGGYRO_UUID)
};

// magaccel UUID
CONST uint8_t ds_MagaccelUUID[ATT_UUID_SIZE] =
{
  DS_MAGACCEL_UUID_BASE128(DS_MAGACCEL_UUID)
};

// calculation UUID
CONST uint8_t ds_CalculationUUID[ATT_UUID_SIZE] =
{
  DS_CALCULATION_UUID_BASE128(DS_CALCULATION_UUID)
};

// accely UUID
CONST uint8_t ds_AccelyUUID[ATT_UUID_SIZE] =
{
  DS_ACCELY_UUID_BASE128(DS_ACCELY_UUID)
};

// accelx UUID
CONST uint8_t ds_AccelxUUID[ATT_UUID_SIZE] =
{
  DS_ACCELX_UUID_BASE128(DS_ACCELX_UUID)
};


/*********************************************************************
 * LOCAL VARIABLES
 */

static DataServiceCBs_t *pAppCBs = NULL;
static uint8_t ds_icall_rsp_task_id = INVALID_TASK_ID;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t DataServiceDecl = { ATT_UUID_SIZE, DataServiceUUID };

// Characteristic "speed" Properties (for declaration)
static uint8_t ds_SpeedProps = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic "speed" Value variable
static uint8_t ds_SpeedVal[DS_SPEED_LEN] = {0};

// Length of data in characteristic "speed" Value variable, initialized to minimal size.
static uint16_t ds_SpeedValLen = DS_SPEED_LEN_MIN;

// Characteristic "speed" Client Characteristic Configuration Descriptor
static gattCharCfg_t *ds_SpeedConfig;



// Characteristic "maggyro" Properties (for declaration)
static uint8_t ds_MaggyroProps = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic "maggyro" Value variable
static uint8_t ds_MaggyroVal[DS_MAGGYRO_LEN] = {0};

// Length of data in characteristic "maggyro" Value variable, initialized to minimal size.
static uint16_t ds_MaggyroValLen = DS_MAGGYRO_LEN_MIN;

// Characteristic "maggyro" Client Characteristic Configuration Descriptor
static gattCharCfg_t *ds_MaggyroConfig;



// Characteristic "magaccel" Properties (for declaration)
static uint8_t ds_MagaccelProps = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic "magaccel" Value variable
static uint8_t ds_MagaccelVal[DS_MAGACCEL_LEN] = {0};

// Length of data in characteristic "magaccel" Value variable, initialized to minimal size.
static uint16_t ds_MagaccelValLen = DS_MAGACCEL_LEN_MIN;

// Characteristic "magaccel" Client Characteristic Configuration Descriptor
static gattCharCfg_t *ds_MagaccelConfig;



// Characteristic "calculation" Properties (for declaration)
static uint8_t ds_CalculationProps = GATT_PROP_NOTIFY | GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic "calculation" Value variable
static uint8_t ds_CalculationVal[DS_CALCULATION_LEN] = {0};

// Length of data in characteristic "calculation" Value variable, initialized to minimal size.
static uint16_t ds_CalculationValLen = DS_CALCULATION_LEN_MIN;

// Characteristic "calculation" Client Characteristic Configuration Descriptor
static gattCharCfg_t *ds_CalculationConfig;



// Characteristic "accely" Properties (for declaration)
static uint8_t ds_AccelyProps = GATT_PROP_NOTIFY | GATT_PROP_READ;

// Characteristic "accely" Value variable
static uint8_t ds_AccelyVal[DS_ACCELY_LEN] = {0};

// Length of data in characteristic "accely" Value variable, initialized to minimal size.
static uint16_t ds_AccelyValLen = DS_ACCELY_LEN_MIN;

// Characteristic "accely" Client Characteristic Configuration Descriptor
static gattCharCfg_t *ds_AccelyConfig;



// Characteristic "accelx" Properties (for declaration)
static uint8_t ds_AccelxProps = GATT_PROP_NOTIFY | GATT_PROP_READ;

// Characteristic "accelx" Value variable
static uint8_t ds_AccelxVal[DS_ACCELX_LEN] = {0};

// Length of data in characteristic "accelx" Value variable, initialized to minimal size.
static uint16_t ds_AccelxValLen = DS_ACCELX_LEN_MIN;

// Characteristic "accelx" Client Characteristic Configuration Descriptor
static gattCharCfg_t *ds_AccelxConfig;



/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t Data_ServiceAttrTbl[] =
{
  // Data_Service Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&DataServiceDecl
  },
    // speed Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ds_SpeedProps
    },
      // speed Characteristic Value
      {
        { ATT_UUID_SIZE, ds_SpeedUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        ds_SpeedVal
      },
      // speed CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&ds_SpeedConfig
      },
    // maggyro Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ds_MaggyroProps
    },
      // maggyro Characteristic Value
      {
        { ATT_UUID_SIZE, ds_MaggyroUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        ds_MaggyroVal
      },
      // maggyro CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&ds_MaggyroConfig
      },
    // magaccel Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ds_MagaccelProps
    },
      // magaccel Characteristic Value
      {
        { ATT_UUID_SIZE, ds_MagaccelUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        ds_MagaccelVal
      },
      // magaccel CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&ds_MagaccelConfig
      },
    // calculation Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ds_CalculationProps
    },
      // calculation Characteristic Value
      {
        { ATT_UUID_SIZE, ds_CalculationUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        ds_CalculationVal
      },
      // calculation CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&ds_CalculationConfig
      },
    // accely Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ds_AccelyProps
    },
      // accely Characteristic Value
      {
        { ATT_UUID_SIZE, ds_AccelyUUID },
        GATT_PERMIT_READ,
        0,
        ds_AccelyVal
      },
      // accely CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&ds_AccelyConfig
      },
    // accelx Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ds_AccelxProps
    },
      // accelx Characteristic Value
      {
        { ATT_UUID_SIZE, ds_AccelxUUID },
        GATT_PERMIT_READ,
        0,
        ds_AccelxVal
      },
      // accelx CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&ds_AccelxConfig
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t Data_Service_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t Data_Service_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t Data_ServiceCBs =
{
  Data_Service_ReadAttrCB,  // Read callback function pointer
  Data_Service_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * DataService_AddService- Initializes the DataService service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t DataService_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  ds_SpeedConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( ds_SpeedConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, ds_SpeedConfig );
  // Allocate Client Characteristic Configuration table
  ds_MaggyroConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( ds_MaggyroConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, ds_MaggyroConfig );
  // Allocate Client Characteristic Configuration table
  ds_MagaccelConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( ds_MagaccelConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, ds_MagaccelConfig );
  // Allocate Client Characteristic Configuration table
  ds_CalculationConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( ds_CalculationConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, ds_CalculationConfig );
  // Allocate Client Characteristic Configuration table
  ds_AccelyConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( ds_AccelyConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, ds_AccelyConfig );
  // Allocate Client Characteristic Configuration table
  ds_AccelxConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( ds_AccelxConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, ds_AccelxConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( Data_ServiceAttrTbl,
                                        GATT_NUM_ATTRS( Data_ServiceAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &Data_ServiceCBs );
  Log_info1("Registered service, %d attributes", (IArg)GATT_NUM_ATTRS( Data_ServiceAttrTbl ));
  ds_icall_rsp_task_id = rspTaskId;

  return ( status );
}

/*
 * DataService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t DataService_RegisterAppCBs( DataServiceCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;
    Log_info1("Registered callbacks to application. Struct %p", (IArg)appCallbacks);
    return ( SUCCESS );
  }
  else
  {
    Log_warning0("Null pointer given for app callbacks.");
    return ( FAILURE );
  }
}

/*
 * DataService_SetParameter - Set a DataService parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t DataService_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  uint8_t  *pAttrVal;
  uint16_t *pValLen;
  uint16_t valMinLen;
  uint16_t valMaxLen;
  uint8_t sendNotiInd = FALSE;
  gattCharCfg_t *attrConfig;
  uint8_t needAuth;

  switch ( param )
  {
    case DS_SPEED_ID:
      pAttrVal  =  ds_SpeedVal;
      pValLen   = &ds_SpeedValLen;
      valMinLen =  DS_SPEED_LEN_MIN;
      valMaxLen =  DS_SPEED_LEN;
      sendNotiInd = TRUE;
      attrConfig  = ds_SpeedConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Speed", (IArg)len);
      break;

    case DS_MAGGYRO_ID:
      pAttrVal  =  ds_MaggyroVal;
      pValLen   = &ds_MaggyroValLen;
      valMinLen =  DS_MAGGYRO_LEN_MIN;
      valMaxLen =  DS_MAGGYRO_LEN;
      sendNotiInd = TRUE;
      attrConfig  = ds_MaggyroConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Maggyro", (IArg)len);
      break;

    case DS_MAGACCEL_ID:
      pAttrVal  =  ds_MagaccelVal;
      pValLen   = &ds_MagaccelValLen;
      valMinLen =  DS_MAGACCEL_LEN_MIN;
      valMaxLen =  DS_MAGACCEL_LEN;
      sendNotiInd = TRUE;
      attrConfig  = ds_MagaccelConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Magaccel", (IArg)len);
      break;

    case DS_CALCULATION_ID:
      pAttrVal  =  ds_CalculationVal;
      pValLen   = &ds_CalculationValLen;
      valMinLen =  DS_CALCULATION_LEN_MIN;
      valMaxLen =  DS_CALCULATION_LEN;
      sendNotiInd = TRUE;
      attrConfig  = ds_CalculationConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Calculation", (IArg)len);
      break;

    case DS_ACCELY_ID:
      pAttrVal  =  ds_AccelyVal;
      pValLen   = &ds_AccelyValLen;
      valMinLen =  DS_ACCELY_LEN_MIN;
      valMaxLen =  DS_ACCELY_LEN;
      sendNotiInd = TRUE;
      attrConfig  = ds_AccelyConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Accely", (IArg)len);
      break;

    case DS_ACCELX_ID:
      pAttrVal  =  ds_AccelxVal;
      pValLen   = &ds_AccelxValLen;
      valMinLen =  DS_ACCELX_LEN_MIN;
      valMaxLen =  DS_ACCELX_LEN;
      sendNotiInd = TRUE;
      attrConfig  = ds_AccelxConfig;
      needAuth    = FALSE; // Change if authenticated link is required for sending.
      Log_info2("SetParameter : %s len: %d", (IArg)"Accelx", (IArg)len);
      break;

    default:
      Log_error1("SetParameter: Parameter #%d not valid.", (IArg)param);
      return INVALIDPARAMETER;
  }

  // Check bounds, update value and send notification or indication if possible.
  if ( len <= valMaxLen && len >= valMinLen )
  {
    memcpy(pAttrVal, value, len);
    *pValLen = len; // Update length for read and get.

    if (sendNotiInd)
    {
      Log_info2("Transmitting noti/ind- connHandle %d, %s", (IArg)attrConfig[0].connHandle,
                                                    (IArg)( (attrConfig[0].value==0)?"Noti/ind disabled":
                                                    (attrConfig[0].value==1)?"Notification enabled":
                                                     "Indication enabled" ) );
      // Try to send notification.
      GATTServApp_ProcessCharCfg( attrConfig, pAttrVal, needAuth,
                                  Data_ServiceAttrTbl, GATT_NUM_ATTRS( Data_ServiceAttrTbl ),
                                  ds_icall_rsp_task_id,  Data_Service_ReadAttrCB);
    }
  }
  else
  {
    Log_error3("Length outside bounds: Len: %d MinLen: %d MaxLen: %d.", (IArg)len, (IArg)valMinLen, (IArg)valMaxLen);
    ret = bleInvalidRange;
  }

  return ret;
}


/*
 * DataService_GetParameter - Get a DataService parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t DataService_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case DS_SPEED_ID:
      *len = MIN(*len, ds_SpeedValLen);
      memcpy(value, ds_SpeedVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"Speed", (IArg)*len);
      break;

    case DS_MAGGYRO_ID:
      *len = MIN(*len, ds_MaggyroValLen);
      memcpy(value, ds_MaggyroVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"Maggyro", (IArg)*len);
      break;

    case DS_MAGACCEL_ID:
      *len = MIN(*len, ds_MagaccelValLen);
      memcpy(value, ds_MagaccelVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"Magaccel", (IArg)*len);
      break;

    case DS_CALCULATION_ID:
      *len = MIN(*len, ds_CalculationValLen);
      memcpy(value, ds_CalculationVal, *len);
      Log_info2("GetParameter : %s returning %d bytes", (IArg)"Calculation", (IArg)*len);
      break;

    default:
      Log_error1("GetParameter: Parameter #%d not valid.", (IArg)param);
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}

/*********************************************************************
 * @internal
 * @fn          Data_Service_findCharParamId
 *
 * @brief       Find the logical param id of an attribute in the service's attr table.
 *
 *              Works only for Characteristic Value attributes and
 *              Client Characteristic Configuration Descriptor attributes.
 *
 * @param       pAttr - pointer to attribute
 *
 * @return      uint8_t paramID (ref Data_Service.h) or 0xFF if not found.
 */
static uint8_t Data_Service_findCharParamId(gattAttribute_t *pAttr)
{
  // Is this a Client Characteristic Configuration Descriptor?
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
    return Data_Service_findCharParamId(pAttr - 1); // Assume the value attribute precedes CCCD and recurse

  // Is this attribute in "speed"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, ds_SpeedUUID, pAttr->type.len))
    return DS_SPEED_ID;

  // Is this attribute in "maggyro"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, ds_MaggyroUUID, pAttr->type.len))
    return DS_MAGGYRO_ID;

  // Is this attribute in "magaccel"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, ds_MagaccelUUID, pAttr->type.len))
    return DS_MAGACCEL_ID;

  // Is this attribute in "calculation"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, ds_CalculationUUID, pAttr->type.len))
    return DS_CALCULATION_ID;

  // Is this attribute in "accely"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, ds_AccelyUUID, pAttr->type.len))
    return DS_ACCELY_ID;

  // Is this attribute in "accelx"?
  else if ( ATT_UUID_SIZE == pAttr->type.len && !memcmp(pAttr->type.uuid, ds_AccelxUUID, pAttr->type.len))
    return DS_ACCELX_ID;

  else
    return 0xFF; // Not found. Return invalid.
}

/*********************************************************************
 * @fn          Data_Service_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t Data_Service_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;
  uint16_t valueLen;
  uint8_t paramID = 0xFF;

  // Find settings for the characteristic to be read.
  paramID = Data_Service_findCharParamId( pAttr );
  switch ( paramID )
  {
    case DS_SPEED_ID:
      valueLen = ds_SpeedValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Speed",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for speed can be inserted here */
      break;

    case DS_MAGGYRO_ID:
      valueLen = ds_MaggyroValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Maggyro",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for maggyro can be inserted here */
      break;

    case DS_MAGACCEL_ID:
      valueLen = ds_MagaccelValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Magaccel",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for magaccel can be inserted here */
      break;

    case DS_CALCULATION_ID:
      valueLen = ds_CalculationValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Calculation",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for calculation can be inserted here */
      break;

    case DS_ACCELY_ID:
      valueLen = ds_AccelyValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Accely",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for accely can be inserted here */
      break;

    case DS_ACCELX_ID:
      valueLen = ds_AccelxValLen;

      Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                 (IArg)"Accelx",
                 (IArg)connHandle,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for accelx can be inserted here */
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check bounds and return the value
  if ( offset > valueLen )  // Prevent malicious ATT ReadBlob offsets.
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else
  {
    *pLen = MIN(maxLen, valueLen - offset);  // Transmit as much as possible
    memcpy(pValue, pAttr->pValue + offset, *pLen);
  }

  return status;
}

/*********************************************************************
 * @fn      Data_Service_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t Data_Service_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;
  uint8_t   changeParamID = 0xFF;
  uint16_t writeLenMin;
  uint16_t writeLenMax;
  uint16_t *pValueLenVar;

  // See if request is regarding a Client Characterisic Configuration
  if (ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID == *(uint16_t *)pAttr->type.uuid)
  {
    // Allow notification and indication, but do not check if really allowed per CCCD.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY |
                                                     GATT_CLIENT_CFG_INDICATE );
    if (SUCCESS == status && pAppCBs && pAppCBs->pfnCfgChangeCb)
       pAppCBs->pfnCfgChangeCb( connHandle, DATA_SERVICE_SERV_UUID,
                                Data_Service_findCharParamId(pAttr), pValue, len );

     return status;
  }

  // Find settings for the characteristic to be written.
  paramID = Data_Service_findCharParamId( pAttr );
  switch ( paramID )
  {
    case DS_SPEED_ID:
      writeLenMin  = DS_SPEED_LEN_MIN;
      writeLenMax  = DS_SPEED_LEN;
      pValueLenVar = &ds_SpeedValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"Speed",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for speed can be inserted here */
      break;

    case DS_MAGGYRO_ID:
      writeLenMin  = DS_MAGGYRO_LEN_MIN;
      writeLenMax  = DS_MAGGYRO_LEN;
      pValueLenVar = &ds_MaggyroValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"Maggyro",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for maggyro can be inserted here */
      break;

    case DS_MAGACCEL_ID:
      writeLenMin  = DS_MAGACCEL_LEN_MIN;
      writeLenMax  = DS_MAGACCEL_LEN;
      pValueLenVar = &ds_MagaccelValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"Magaccel",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for magaccel can be inserted here */
      break;

    case DS_CALCULATION_ID:
      writeLenMin  = DS_CALCULATION_LEN_MIN;
      writeLenMax  = DS_CALCULATION_LEN;
      pValueLenVar = &ds_CalculationValLen;

      Log_info5("WriteAttrCB : %s connHandle(%d) len(%d) offset(%d) method(0x%02x)",
                 (IArg)"Calculation",
                 (IArg)connHandle,
                 (IArg)len,
                 (IArg)offset,
                 (IArg)method);
      /* Other considerations for calculation can be inserted here */
      break;

    default:
      Log_error0("Attribute was not found.");
      return ATT_ERR_ATTR_NOT_FOUND;
  }
  // Check whether the length is within bounds.
  if ( offset >= writeLenMax )
  {
    Log_error0("An invalid offset was requested.");
    status = ATT_ERR_INVALID_OFFSET;
  }
  else if ( offset + len > writeLenMax )
  {
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else if ( offset + len < writeLenMin && ( method == ATT_EXECUTE_WRITE_REQ || method == ATT_WRITE_REQ ) )
  {
    // Refuse writes that are lower than minimum.
    // Note: Cannot determine if a Reliable Write (to several chars) is finished, so those will
    //       only be refused if this attribute is the last in the queue (method is execute).
    //       Otherwise, reliable writes are accepted and parsed piecemeal.
    Log_error0("Invalid value length was received.");
    status = ATT_ERR_INVALID_VALUE_SIZE;
  }
  else
  {
    // Copy pValue into the variable we point to from the attribute table.
    memcpy(pAttr->pValue + offset, pValue, len);

    // Only notify application and update length if enough data is written.
    //
    // Note: If reliable writes are used (meaning several attributes are written to using ATT PrepareWrite),
    //       the application will get a callback for every write with an offset + len larger than _LEN_MIN.
    // Note: For Long Writes (ATT Prepare + Execute towards only one attribute) only one callback will be issued,
    //       because the write fragments are concatenated before being sent here.
    if ( offset + len >= writeLenMin )
    {
      changeParamID = paramID;
      *pValueLenVar = offset + len; // Update data length.
    }
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (changeParamID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
      pAppCBs->pfnChangeCb( connHandle, DATA_SERVICE_SERV_UUID, paramID, pValue, len+offset ); // Call app function from stack task context.

  return status;
}
