#include <Arduino.h>
#include <LittleFS.h>
#include "nvm.hpp"

CardDatabase m_CardDatabase;
#if false/*Log not required to save in device*/
LogDatabase  m_LogDatabase;
#endif

void InitNVM(void)
{
    LittleFS.begin();
}
void SaveCardDetails(void)
{
    File fp = LittleFS.open("card.bin", "w");
    if (fp)
    {
      fp.write((uint8_t *)&m_CardDatabase, sizeof(CardDatabase));
    }
    fp.close();
}

CardDatabase* ReadCardDetails(void)
{
    File fp = LittleFS.open("card.bin", "r");
    if (fp)
    {
      fp.read((uint8_t *)&m_CardDatabase, sizeof(CardDatabase));
    }
    fp.close();
    return &m_CardDatabase;
}
CardDatabase* GetAddressOfCardBuffer(void)
{
  return &m_CardDatabase;
}
void SaveNewRecord(String strName , uint32_t u32CardId)
{
  
}
