#include "Drv_Weigand.hpp"

WIEGAND g_WgCh0;
WIEGAND g_WgCh1;

void InitilizeWeigand(WIEGAND *ptrWg ,
                        uint8_t pinD0 , 
                        uint8_t pinD1)
{
  // default Wiegand Pin 2 and Pin 3 see image on README.md
  // for non UNO board, use wg.begin(pinD0, pinD1) where pinD0 and pinD1 
  // are the pins connected to D0 and D1 of wiegand reader respectively.
  ptrWg->begin(pinD0 , pinD1);
}

bool WeigandProcess(WIEGAND *ptrWg , 
                            uint32_t *pVal)
{
  bool Status = false;
  if(ptrWg->available())
  {
    *pVal = (uint32_t)ptrWg->getCode();
    Status = true;
  }
  return Status;
}

WIEGAND* GetInstance_RfidCh0(void)
{
  return (&g_WgCh0);
}

WIEGAND* GetInstance_RfidCh1(void)
{
  return (&g_WgCh1);
}
