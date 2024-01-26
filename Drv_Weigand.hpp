#ifndef DRV_WEIGAND_HPP
#define DRV_WEIGAND_HPP
void InitilizeWeigand_A(uint8_t pinD0 , 
                        uint8_t pinD1);
void InitilizeWeigand_B(uint8_t pinD0 , 
                        uint8_t pinD1);



bool WeigandProcess_A(uint32_t *pVal);
bool WeigandProcess_B(uint32_t *pVal);

#endif
