#include <Arduino.h>
#include <LittleFS.h>
#include "nvm.hpp"

Database m_NvmDatabase;

void InitNVM(void)
{
    LittleFS.begin();
    ReadDatabseDetails();
    if(INIT_SIGNATURE != m_NvmDatabase.u32InitFlag)/*Reset memory*/
    {
      ResetAll();
    }
}
void SaveDatabaseDetails(void)
{
    File fp = LittleFS.open("database.bin", "w");
    if (fp)
    {
      fp.write((uint8_t *)&m_NvmDatabase, sizeof(Database));
    }
    fp.close();
}
Database* ReadDatabseDetails(void)
{
    File fp = LittleFS.open("database.bin", "r");
    if (fp)
    {
      fp.read((uint8_t *)&m_NvmDatabase, sizeof(Database));
    }
    fp.close();
    return &m_NvmDatabase;
}
Database* GetAddressOfDatabase(void)
{
  return &m_NvmDatabase;
}
uint32_t GetRelaylagTime(void)
{  
  return m_NvmDatabase.m_Settings.RelayLagTime;
}
uint32_t GetNumOfCardDetailsSaved(void)
{
  uint32_t NumOfCardData = 0;
  for(uint32_t u32Idx = 0 ; 
          u32Idx < MAX_SIZE_CARD_SAVED ; ++u32Idx)
  {
    if(true == m_NvmDatabase.g_DataBase[u32Idx].bValidData)/*If a vacant space is found*/
    {
      NumOfCardData++;
    }
  }
  return NumOfCardData;
}
void DeleteAllRecords(void)
{
  for(uint32_t u32Idx = 0 ; 
          u32Idx < MAX_SIZE_CARD_SAVED ; ++u32Idx)
  {
    m_NvmDatabase.g_DataBase[u32Idx].u32CardId = 0;
    m_NvmDatabase.g_DataBase[u32Idx].bValidData = false;
  }
  SaveDatabaseDetails();
  Serial.println("delete all records - completed");
}
void ResetAll(void)
{
  for(uint32_t u32Idx = 0 ; 
          u32Idx < MAX_SIZE_CARD_SAVED ; ++u32Idx)
  {
    m_NvmDatabase.g_DataBase[u32Idx].bValidData = false;
  }
  m_NvmDatabase.m_Settings.RelayLagTime = DEFAULT_RELAY_LAG_TIME;
  m_NvmDatabase.m_Settings.RelayPulseTime = DEFAULT_RELAY_PULSE_TIME;
  m_NvmDatabase.u32InitFlag = INIT_SIGNATURE;
  SaveDatabaseDetails();
  Serial.println("Reset all - completed");
}
bool checkForDuplicateCardData(uint32_t u32CardId , uint32_t *pIndex)
{
  bool bStatus = false;
  for(uint32_t u32Idx = 0 ; 
          u32Idx < MAX_SIZE_CARD_SAVED ; ++u32Idx)
  {
    if(true == m_NvmDatabase.g_DataBase[u32Idx].bValidData)/*If a vacant space is found*/
    {
      if(u32CardId == m_NvmDatabase.g_DataBase[u32Idx].u32CardId)
      {
        *pIndex = u32Idx;
        bStatus = true;
        break;
      }
    }
  }
  return bStatus;
}
bool SaveNewRecord(String strName , uint32_t u32CardId)
{
  bool bStatus = false;
  for(uint32_t u32Idx = 0 ; 
          u32Idx < MAX_SIZE_CARD_SAVED ; ++u32Idx)
  {
    if(false == m_NvmDatabase.g_DataBase[u32Idx].bValidData)/*If a vacant space is found*/
    {
      /*Save to that vacant space in database*/
      m_NvmDatabase.g_DataBase[u32Idx].u32CardId = u32CardId;
      strncpy(m_NvmDatabase.g_DataBase[u32Idx].arrName , strName.c_str() , sizeof(m_NvmDatabase.g_DataBase[u32Idx].arrName));
      m_NvmDatabase.g_DataBase[u32Idx].bValidData = true;
      bStatus = true;
      SaveDatabaseDetails();
      break;
    }
  }
  return bStatus;
}
