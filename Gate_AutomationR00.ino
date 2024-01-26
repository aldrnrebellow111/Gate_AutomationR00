#include <Arduino.h>
#include "Appl_Timer.hpp"
#include "Drv_Weigand.hpp"
#include "App.hpp"
#include "nvm.hpp"
#include "Appl_Lcd.hpp"
#include "Drv_Relay.hpp"
#include "JsonParser.hpp"

void setup()
{
  Serial.begin(DEBUG_SERIAL_BAUD_RATE);
  InitNVM();
  InitLcd();
  InitRelays();
  InitilizeWeigand_A(PIN_WG_CH0_D0 , PIN_WG_CH0_D1);
  InitilizeWeigand_B(PIN_WG_CH1_D0 , PIN_WG_CH1_D1);
  InitRfidApplication();
  Serial.println("Init - done");
}

void loop() 
{
 
  uint32_t uRfidValCh0 = 0;
  uint32_t uRfidValCh1 = 0;
  bool StatusCh0 = false;
  bool StatusCh1 = false;
  StatusCh0 = WeigandProcess_A(&uRfidValCh0);  
  StatusCh1 = WeigandProcess_B(&uRfidValCh1);
  ProcessRfidData(uRfidValCh0 , uRfidValCh1 , StatusCh0 , StatusCh1);
  ProcessRelayPulse(GetInstance_Relay1());
  ProcessRelayPulse(GetInstance_Relay2());
  Display_ScanningInProgress();
  parseJson();/*This part parse incomming message frame from PC , retruns true if a valid packet is obtained*/
}
