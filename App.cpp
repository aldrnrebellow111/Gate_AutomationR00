#include <Arduino.h>
#include "Appl_Timer.hpp"
#include "App.hpp"
#include "nvm.hpp"
#include "Drv_Relay.hpp"


void InitRfidApplication(void)
{
  ReadCardDetails();
}
void ProcessRfidData(uint32_t u32Ch0 , 
                      uint32_t u32Ch1 ,
                      bool EnCh0 , bool EnCh1)
{
  CardData *ptrCardData = NULL;
  Relay *ptrRelay1 = GetInstance_Relay1();
  Relay *ptrRelay2 = GetInstance_Relay2();
  /*Search logic*/
  if(NULL != ptrCardData)
  {
    for(uint32_t u32Idx = 0 ; 
                u32Idx < MAX_SIZE_CARD_SAVED ; ++u32Idx)
    {
        if(EnCh0)
        {
          if(u32Ch0 == ptrCardData->u32CardId)
          {
            /*Trigger Relay 1*/
            TriggerRelay(ptrRelay1);
          }
        }
        
        if(EnCh1)
        {
          if(u32Ch1 == ptrCardData->u32CardId)
          {
            /*Trigger Relay 2*/
            TriggerRelay(ptrRelay2);
          }
        }
    }
  }
}
