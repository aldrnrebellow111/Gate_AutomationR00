#include <Arduino.h>
#include "Appl_Timer.hpp"
#include "App.hpp"
#include "nvm.hpp"
#include "Drv_Relay.hpp"
#include "JsonParser.hpp"

void InitRfidApplication(void)
{
  ReadDatabseDetails();
}
void ProcessRfidData(uint32_t u32Ch0 , 
                      uint32_t u32Ch1 ,
                      bool EnCh0 , bool EnCh1)
{
  Database *ptrCardData = GetAddressOfDatabase();
  Relay *ptrRelay1 = GetInstance_Relay1();
  Relay *ptrRelay2 = GetInstance_Relay2();
  /*Search logic*/
  if(NULL != ptrCardData)
  {
    for(uint32_t u32Idx = 0 ; 
                u32Idx < MAX_SIZE_CARD_SAVED ; ++u32Idx)
    {
      if(true == ptrCardData->g_DataBase[u32Idx].bValidData)
      {
          if(EnCh0)
          {
            if(u32Ch0 == ptrCardData->g_DataBase[u32Idx].u32CardId)
            {
              if(true == ptrRelay1->RelayEnable)
              {
                /*Trigger Relay 1*/
                TriggerRelay(ptrRelay1);
                Serial.println("Trigger Relay 1");
                SendCardLogToPc(1 ,String(ptrCardData->g_DataBase[u32Idx].u32CardId) ,
                        String(ptrCardData->g_DataBase[u32Idx].arrName)); 
              }
            }
          }
          
          if(EnCh1)
          {
            if(u32Ch1 == ptrCardData->g_DataBase[u32Idx].u32CardId)
            {
              if(true == ptrRelay2->RelayEnable)
              {
                /*Trigger Relay 2*/
                TriggerRelay(ptrRelay2);
                Serial.println("Trigger Relay 2");
                SendCardLogToPc(2 , String(ptrCardData->g_DataBase[u32Idx].u32CardId) ,
                    String(ptrCardData->g_DataBase[u32Idx].arrName));
              }
            }
          }
      }
    }
  }
}
