/******************************************************************************
*
* Module: Dio
*
* File Name: Dio.c
*
* Description: Source file for TM4C123GH6PM Microcontroller - Dio Driver
*
* Author: Abdelrhman Hesham
******************************************************************************/

#include "Dio.h"
#include "Dio_Regs.h"
#include "Port.h"


#if (DIO_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Dio Modules */
#if ((DET_AR_MAJOR_VERSION != DIO_AR_RELEASE_MAJOR_VERSION)\
|| (DET_AR_MINOR_VERSION != DIO_AR_RELEASE_MINOR_VERSION)\
|| (DET_AR_PATCH_VERSION != DIO_AR_RELEASE_PATCH_VERSION))
#error "The AR version of Det.h does not match the expected version"
#endif

#endif

STATIC const Dio_ConfigChannel * Dio_PortChannels = NULL_PTR;
STATIC uint8 Dio_Status = DIO_NOT_INITIALIZED;

extern const Port_ConfigChannel * Port_PortChannels;

/************************************************************************************
* Service Name: Dio_Init
* Service ID[hex]: 0x10
* Sync/Async: Synchronous
* Reentrancy: Non reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Initialize the Dio module.
************************************************************************************/
void Dio_Init(const Dio_ConfigType * ConfigPtr)
{
#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* check if the input configuration pointer is not a NULL_PTR */
      if (NULL_PTR == ConfigPtr)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_INIT_SID,
                   DIO_E_PARAM_CONFIG);
      }
      else
#endif
      {
              /*
               * Set the module state to initialized and point to the PB configuration structure using a global pointer.
               * This global pointer is global to be used by other functions to read the PB configuration structures
               */
              Dio_Status       = DIO_INITIALIZED;
              Dio_PortChannels = ConfigPtr->Channels; /* address of the first Channels structure --> Channels[0] */
      }
}

/************************************************************************************
* Service Name: Dio_WriteChannel
* Service ID[hex]: 0x01
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
*                  Level - Value to be written.
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to set a level of a channel.
************************************************************************************/
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
      volatile uint32 * Port_Ptr = NULL_PTR;
      boolean error = FALSE;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if the Driver is initialized before using this function */
      if (DIO_NOT_INITIALIZED == Dio_Status)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_WRITE_CHANNEL_SID, DIO_E_UNINIT);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      /* Check if the used channel is within the valid range */
      if (DIO_CONFIGURED_CHANNLES <= ChannelId)
      {

              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_WRITE_CHANNEL_SID, DIO_E_PARAM_INVALID_CHANNEL_ID);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      
      if (Dio_PortChannels[ChannelId].Port_Num > TOTAL_NO_OF_PORTS)
      {

              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_WRITE_CHANNEL_SID, DIO_E_PARAM_INVALID_PORT_ID);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
     
#endif

      /* In-case there are no errors */
      if(FALSE == error)
      {
              /* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
              switch(Dio_PortChannels[ChannelId].Port_Num)
              {
                  case 0:    Port_Ptr = &GPIO_PORTA_DATA_REG;
                             break;
                  case 1:    Port_Ptr = &GPIO_PORTB_DATA_REG;
                             break;
                  case 2:    Port_Ptr = &GPIO_PORTC_DATA_REG;
                             break;
                  case 3:    Port_Ptr = &GPIO_PORTD_DATA_REG;
                             break;
                  case 4:    Port_Ptr = &GPIO_PORTE_DATA_REG;
                             break;
                  case 5:    Port_Ptr = &GPIO_PORTF_DATA_REG;
                             break;
              }
              if(Level == STD_HIGH)
              {
                      /* Write Logic High */
                      if(GPIO_PORTF_DIR_REG & ( STD_ACTIVE << Dio_PortChannels[ChannelId].Ch_Num))
                      {
                      SET_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
                      }
                      else if(!(GPIO_PORTF_DIR_REG & ( STD_ACTIVE << Dio_PortChannels[ChannelId].Ch_Num)))
                      {
                       Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                                DIO_WRITE_CHANNEL_SID, DIO_E_FALSE_WRITE); 
                      }
                      
              }
              else if(Level == STD_LOW)
              {
                      /* Write Logic Low */
                 if(GPIO_PORTF_DIR_REG & ( STD_ACTIVE << Dio_PortChannels[ChannelId].Ch_Num))
                      {
                     CLEAR_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
                      }
                      else if(!(GPIO_PORTF_DIR_REG & ( STD_ACTIVE << Dio_PortChannels[ChannelId].Ch_Num)))
                      {
                       Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                                DIO_WRITE_CHANNEL_SID, DIO_E_FALSE_WRITE); 
                      }
                      
              }
      }
      else
      {
              /* No Action Required */
      }

}

/************************************************************************************
* Service Name: Dio_ReadChannel
* Service ID[hex]: 0x00
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_LevelType
* Description: Function to return the value of the specified DIO channel.
************************************************************************************/
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
 volatile uint32 * Port_Ptr = NULL_PTR;
      boolean error = FALSE;
      Dio_LevelType value;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if the Driver is initialized before using this function */
      if (DIO_NOT_INITIALIZED == Dio_Status)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_READ_CHANNEL_SID, DIO_E_UNINIT);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      /* Check if the used channel is within the valid range */
      if (DIO_CONFIGURED_CHANNLES <= ChannelId)
      {

              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_READ_CHANNEL_SID, DIO_E_PARAM_INVALID_CHANNEL_ID);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      if (Dio_PortChannels[ChannelId].Port_Num > TOTAL_NO_OF_PORTS)
      {

              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_READ_CHANNEL_SID, DIO_E_PARAM_INVALID_PORT_ID);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
#endif

      /* In-case there are no errors */
      if(FALSE == error)
      {
              /* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
              switch(Dio_PortChannels[ChannelId].Port_Num)
              {
                  case 0:    Port_Ptr = &GPIO_PORTA_DATA_REG;
                             break;
                  case 1:    Port_Ptr = &GPIO_PORTB_DATA_REG;
                             break;
                  case 2:    Port_Ptr = &GPIO_PORTC_DATA_REG;
                             break;
                  case 3:    Port_Ptr = &GPIO_PORTD_DATA_REG;
                             break;
                  case 4:    Port_Ptr = &GPIO_PORTE_DATA_REG;
                             break;
                  case 5:    Port_Ptr = &GPIO_PORTF_DATA_REG;
                             break;
              }
              value = GET_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
              
      }
      else
      {
        
      }
      return value;
}

/************************************************************************************
* Service Name: Dio_GetVersionInfo
* Service ID[hex]: 0x12
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): VersionInfo - Pointer to where to store the version information of this module.
* Return value: None
* Description: Function to get the version information of this module.
************************************************************************************/
#if (DIO_VERSION_INFO_API == STD_ON)
void Dio_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if input pointer is not Null pointer */
      if(NULL_PTR == versioninfo)
      {
              /* Report to DET  */
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_GET_VERSION_INFO_SID, DIO_E_PARAM_POINTER);
      }
      else
#endif /* (DIO_DEV_ERROR_DETECT == STD_ON) */
      {
              /* Copy the vendor Id */
              versioninfo->vendorID = (uint16)DIO_VENDOR_ID;
              /* Copy the module Id */
              versioninfo->moduleID = (uint16)DIO_MODULE_ID;
              /* Copy Software Major Version */
              versioninfo->sw_major_version = (uint8)DIO_SW_MAJOR_VERSION;
              /* Copy Software Minor Version */
              versioninfo->sw_minor_version = (uint8)DIO_SW_MINOR_VERSION;
              /* Copy Software Patch Version */
              versioninfo->sw_patch_version = (uint8)DIO_SW_PATCH_VERSION;
      }
}
#endif

/************************************************************************************
* Service Name: Dio_FlipChannel
* Service ID[hex]: 0x11
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelId - ID of DIO channel.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_LevelType
* Description: Function to flip the level of a channel and return the level of the channel after flip.
************************************************************************************/
#if (DIO_FLIP_CHANNEL_API == STD_ON)
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
  
 volatile uint32 * Port_Ptr = NULL_PTR;
      boolean error = FALSE;
      Dio_LevelType value;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if the Driver is initialized before using this function */
      if (DIO_NOT_INITIALIZED == Dio_Status)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_FLIP_CHANNEL_SID, DIO_E_UNINIT);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      /* Check if the used channel is within the valid range */
      if (DIO_CONFIGURED_CHANNLES <= ChannelId)
      {

              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_FLIP_CHANNEL_SID, DIO_E_PARAM_INVALID_CHANNEL_ID);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
#endif

      /* In-case there are no errors */
      if(FALSE == error)
      {
              /* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
              switch(Dio_PortChannels[ChannelId].Port_Num)
              {
                  case 0:    Port_Ptr = &GPIO_PORTA_DATA_REG;
                             break;
                  case 1:    Port_Ptr = &GPIO_PORTB_DATA_REG;
                             break;
                  case 2:    Port_Ptr = &GPIO_PORTC_DATA_REG;
                             break;
                  case 3:    Port_Ptr = &GPIO_PORTD_DATA_REG;
                             break;
                  case 4:    Port_Ptr = &GPIO_PORTE_DATA_REG;
                             break;
                  case 5:    Port_Ptr = &GPIO_PORTF_DATA_REG;
                             break;
              }
              TOGGLE_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
              value = GET_BIT(*Port_Ptr,Dio_PortChannels[ChannelId].Ch_Num);
              
      }
      else
      {
        
      }
      return value;
}
#endif




/************************************************************************************
* Service Name: Dio_ReadPort
* Service ID[hex]: 0x02
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): PortId - ID of DIO Port.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_PortLevelType - Level of all channels of that port
* Description: Returns the level of all channels of that port.
************************************************************************************/
Dio_PortLevelType Dio_ReadPort( Dio_PortType PortId )
{
volatile uint32 * Port_Ptr = NULL_PTR;
boolean error = FALSE;
Dio_PortLevelType portValue = STD_LOW;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if the Driver is initialized before using this function */
      if (DIO_NOT_INITIALIZED == Dio_Status)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_READ_PORT_SID , DIO_E_UNINIT);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      
#endif

      /* In-case there are no errors */
      if(FALSE == error)
      {
              /* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
              switch(PortId)
              {
                  case 0:    Port_Ptr = &GPIO_PORTA_DATA_REG;
                             CLEAR_BIT(*Port_Ptr,PORTA_MISSING_PIN_1);
                             CLEAR_BIT(*Port_Ptr,PORTA_MISSING_PIN_2);
                             break;
                  case 1:    Port_Ptr = &GPIO_PORTB_DATA_REG;
                             break;
                  case 2:    Port_Ptr = &GPIO_PORTC_DATA_REG;
                             CLEAR_BIT(*Port_Ptr,PORTC_MISSING_PIN_1);
                             CLEAR_BIT(*Port_Ptr,PORTC_MISSING_PIN_2);
                             CLEAR_BIT(*Port_Ptr,PORTC_MISSING_PIN_3);
                             CLEAR_BIT(*Port_Ptr,PORTC_MISSING_PIN_4);                    
                             break;
                  case 3:    Port_Ptr = &GPIO_PORTD_DATA_REG;
                             CLEAR_BIT(*Port_Ptr,PORTD_MISSING_PIN_1);
                             CLEAR_BIT(*Port_Ptr,PORTD_MISSING_PIN_2);                    
                             break;
                  case 4:    Port_Ptr = &GPIO_PORTE_DATA_REG;
                             CLEAR_BIT(*Port_Ptr,PORTE_MISSING_PIN_1);
                             CLEAR_BIT(*Port_Ptr,PORTE_MISSING_PIN_2);
                             break;
                  case 5:    Port_Ptr = &GPIO_PORTF_DATA_REG;
                             CLEAR_BIT(*Port_Ptr,PORTF_MISSING_PIN_1);
                             CLEAR_BIT(*Port_Ptr,PORTF_MISSING_PIN_2);
                             CLEAR_BIT(*Port_Ptr,PORTF_MISSING_PIN_3);
                             break;
              }
              
              
       portValue = *Port_Ptr;
      }
      else
      {
      }
      return portValue;
}


/************************************************************************************
* Service Name: Dio_WritePort
* Service ID[hex]: 0x03
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): PortId - ID of DIO Port.
* Parameters (in): Level - Value to be written
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Service to set a value of the port.
************************************************************************************/                                
void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{
      volatile uint32 * Port_Ptr = NULL_PTR;
      boolean error = FALSE;
      uint8 portIterations;
#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if the Driver is initialized before using this function */
      if (DIO_NOT_INITIALIZED == Dio_Status)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_WRITE_PORT_SID, DIO_E_UNINIT);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      
#endif

      /* In-case there are no errors */
      if(FALSE == error)
      {
              /* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
              switch(PortId)
              {
                  case 0:    Port_Ptr = &GPIO_PORTA_DATA_REG;
                             break;
                  case 1:    Port_Ptr = &GPIO_PORTB_DATA_REG;
                             break;
                  case 2:    Port_Ptr = &GPIO_PORTC_DATA_REG;
                             break;
                  case 3:    Port_Ptr = &GPIO_PORTD_DATA_REG;
                             break;
                  case 4:    Port_Ptr = &GPIO_PORTE_DATA_REG;
                             break;
                  case 5:    Port_Ptr = &GPIO_PORTF_DATA_REG;
                             break;
              }
              for ( portIterations = 0; portIterations < PORT_CONFIGURED_CHANNLES ;portIterations++)
              {   
                if( Port_PortChannels[portIterations].port_number == PortId)
             {
                  if( Port_PortChannels[portIterations].Mode == PORT_PIN_MODE_DIO)
                 {   
                  if( Port_PortChannels[portIterations].Ch_Dir == PORT_PIN_OUT)
                  {
                    if( Level == STD_HIGH )
                    {
                    SET_BIT(*Port_Ptr,Port_PortChannels[portIterations].channel_number);
                    }
                    else if( Level == STD_LOW )
                    {
                      CLEAR_BIT(*Port_Ptr,Port_PortChannels[portIterations].channel_number);
                    }
                  }
                
                 else
                  {
                  /*Do Nothing*/
                  }
                 }
                 else
                 {
                   /*Do Nothing*/
                 }
              }
              else
              {
                /*Do Nothing*/
              }
            }
        }
}


/************************************************************************************
* Service Name: Dio_ReadChannelGroup
* Service ID[hex]: 0x04
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): ChannelGroupIdPtr - Pointer to ChannelGroup.
* Parameters (inout): None
* Parameters (out): None
* Return value: Dio_PortLevelType - Level of a subset of the adjoining bits of a port
* Description: This Service reads a subset of the adjoining bits of a port.
************************************************************************************/                                
Dio_PortLevelType Dio_ReadChannelGroup( const Dio_ChannelGroupType* ChannelGroupIdPtr )
{
  volatile uint32 * Port_Ptr = NULL_PTR;
  boolean error = FALSE;
  Dio_PortLevelType portValue = STD_LOW;

#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if the Driver is initialized before using this function */
      if (DIO_NOT_INITIALIZED == Dio_Status)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_READ_CHANNEL_GROUP_SID , DIO_E_UNINIT);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
      
#endif

      /* In-case there are no errors */
      if(FALSE == error)
      {
              /* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
              switch(ChannelGroupIdPtr->PortIndex)
              {
                  case 0:    Port_Ptr = &GPIO_PORTA_DATA_REG;
                             break;
                  case 1:    Port_Ptr = &GPIO_PORTB_DATA_REG;
                             break;
                  case 2:    Port_Ptr = &GPIO_PORTC_DATA_REG;
                             break;
                  case 3:    Port_Ptr = &GPIO_PORTD_DATA_REG;
                             break;
                  case 4:    Port_Ptr = &GPIO_PORTE_DATA_REG;
                             break;
                  case 5:    Port_Ptr = &GPIO_PORTF_DATA_REG;
                             break;
              }
        uint8 maskedReg;      
        maskedReg = (uint8)(*Port_Ptr) & (ChannelGroupIdPtr->mask);
        portValue = (maskedReg >> ChannelGroupIdPtr->offset); 
      }
      return portValue;
}

void Dio_WriteChannelGroup( const Dio_ChannelGroupType* ChannelGroupIdPtr, Dio_PortLevelType Level )
{
  volatile uint32 * Port_Ptr = NULL_PTR;
  boolean error = FALSE;
  uint8 portIterations;
  uint8 pinsDir[TOTAL_NO_OF_PINS];
  uint8 i = STD_LOW;
#if (DIO_DEV_ERROR_DETECT == STD_ON)
      /* Check if the Driver is initialized before using this function */
      if (DIO_NOT_INITIALIZED == Dio_Status)
      {
              Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID,
                              DIO_WRITE_CHANNEL_GROUP_SID , DIO_E_UNINIT);
              error = TRUE;
      }
      else
      {
              /* No Action Required */
      }
     
#endif

      /* In-case there are no errors */
      if(FALSE == error)
      {
              /* Point to the correct PORT register according to the Port Id stored in the Port_Num member */
              switch(ChannelGroupIdPtr->PortIndex)
              {
                  case 0:    Port_Ptr = &GPIO_PORTA_DATA_REG;
                             break;
                  case 1:    Port_Ptr = &GPIO_PORTB_DATA_REG;
                             break;
                  case 2:    Port_Ptr = &GPIO_PORTC_DATA_REG;
                             break;
                  case 3:    Port_Ptr = &GPIO_PORTD_DATA_REG;
                             break;
                  case 4:    Port_Ptr = &GPIO_PORTE_DATA_REG;
                             break;
                  case 5:    Port_Ptr = &GPIO_PORTF_DATA_REG;
                             break;
              }
        for(portIterations = 0; portIterations < PORT_CONFIGURED_CHANNLES; portIterations++)
        {
          if( Port_PortChannels[portIterations].port_number == ChannelGroupIdPtr->PortIndex)
                {
                  if( Port_PortChannels[portIterations].Mode == PORT_PIN_MODE_DIO)
                 {   
                  if( Port_PortChannels[portIterations].Ch_Dir == PORT_PIN_OUT)
                  {
                    pinsDir[i] = STD_ACTIVE;
                  }
                
                 else
                  {
                   pinsDir[i] = STD_IDLE;
                  }
                 }
                 else
                 {
                   /*Do Nothing*/
                 }
              }
             
            
           else
              {
                /*Do Nothing*/
              }
        }
        *Port_Ptr = (*Port_Ptr & (ChannelGroupIdPtr->mask));
        for (i = 0; i< TOTAL_NO_OF_PINS;i++)
        {
          if(pinsDir[i] == STD_ACTIVE)
          {
           *Port_Ptr &= Level;
          }
        }
      }
  
}