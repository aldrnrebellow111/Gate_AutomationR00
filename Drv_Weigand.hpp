#ifndef DRV_WEIGAND_HPP
#define DRV_WEIGAND_HPP
#include <Wiegand.h>
void InitilizeWeigand(WIEGAND *ptrWg ,
                        uint8_t pinD0 , 
                        uint8_t pinD1);
bool WeigandProcess(WIEGAND *ptrWg ,
                    uint32_t *pVal);
WIEGAND* GetInstance_RfidCh0(void);
WIEGAND* GetInstance_RfidCh1(void);
#endif
