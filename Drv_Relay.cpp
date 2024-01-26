#include <Arduino.h>
#include "Appl_Timer.hpp"
#include "Drv_Relay.hpp"
#include "App.hpp"
#include "nvm.hpp"

Relay g_Relay1;
Relay g_Relay2;

void InitRelays(void)
{
  g_Relay1.u8Pin = PIN_RELAY_1;
  g_Relay2.u8Pin = PIN_RELAY_2;
  g_Relay1.RelayState = false;
  g_Relay2.RelayState = false;
  g_Relay1.RelayEnable = true;
  g_Relay2.RelayEnable = true;
  pinMode(g_Relay1.u8Pin , OUTPUT);
  pinMode(g_Relay2.u8Pin , OUTPUT);
  digitalWrite(g_Relay1.u8Pin , g_Relay1.RelayState);
  digitalWrite(g_Relay2.u8Pin , g_Relay2.RelayState);
}

void TriggerRelay(Relay *ptrRelay)
{
  if(true == ptrRelay->RelayEnable)
  {
    ptrRelay->RelayState = true;
    /*Disable Relay*/
    ptrRelay->RelayEnable = false;
    digitalWrite(ptrRelay->u8Pin , ptrRelay->RelayState);
    StartTimer(&ptrRelay->m_Pulsetimer , 1000/*Milli second*/);
  }
}

void ProcessRelayPulse(Relay *ptrRelay)
{
  /*Handler for pulse width timer - start*/
  if(true == Timer_IsTimeout(&ptrRelay->m_Pulsetimer , enTimerNormalStop))
  {
    /*pulse -> LOW*/
    ptrRelay->RelayState = false;
    digitalWrite(ptrRelay->u8Pin , ptrRelay->RelayState);
    /*Start lag timer*/
    StartTimer(&ptrRelay->m_Lagtimer , (1000 * GetRelaylagTime())/*Lag time Milli second*/);
  }
  /*Handler for pulse width timer - end*/

  /*Handler for lag timer - start*/
  if(true == Timer_IsTimeout(&ptrRelay->m_Lagtimer , enTimerNormalStop))
  {
    /*Relay enable*/
    ptrRelay->RelayEnable = true;
  }
  /*Handler for lag timer - end*/
}

Relay* GetInstance_Relay1(void)
{
  return &g_Relay1;
}

Relay* GetInstance_Relay2(void)
{
  return &g_Relay1;
}
