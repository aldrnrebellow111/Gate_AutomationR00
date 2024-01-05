#ifndef DRV_RELAY_HPP
#define DRV_RELAY_HPP

typedef struct
{
  stcTimer m_Pulsetimer;/*Timer to control signal pulse width*/
  stcTimer m_Lagtimer;/*Signal to control signal lag*/
  uint8_t u8Pin;
  bool RelayState;
  bool RelayEnable;
}Relay;



void InitRelays(void);
void TriggerRelay(Relay *ptrRelay);
Relay* GetInstance_Relay1(void);
Relay* GetInstance_Relay2(void);
void ProcessRelayPulse(Relay *ptrRelay);
#endif
