#include "WiegandA.h"
#include "WiegandB.h"
#include "Drv_Weigand.hpp"

WIEGAND_A g_WgCh0;
WIEGAND_B g_WgCh1;

void InitilizeWeigand_A(uint8_t pinD0 , 
                        uint8_t pinD1)
{
  g_WgCh0.begin(pinD0,pinD1);
}
void InitilizeWeigand_B(uint8_t pinD0 , 
                        uint8_t pinD1)
{
  g_WgCh1.begin(pinD0,pinD1);
}

bool WeigandProcess_A(uint32_t *pVal)
{
  bool Status = false;
  if(g_WgCh0.available())
  {
    *pVal = (uint32_t)g_WgCh0.getCode();
    Status = true;
  }
  return Status;
}

bool WeigandProcess_B(uint32_t *pVal)
{
  bool Status = false;
  if(g_WgCh1.available())
  {
    *pVal = (uint32_t)g_WgCh1.getCode();
    Status = true;
  }
  return Status;
}
