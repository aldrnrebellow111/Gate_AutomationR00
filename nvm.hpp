#ifndef NVM_HPP
#define NVM_HPP

#define MAX_LENTH_CARD_NAME (16)
#define MAX_SIZE_CARD_SAVED (500)
#define MAX_SIZE_LOG_SAVED (500)
#define INIT_SIGNATURE (0xAA)
#define DEFAULT_RELAY_LAG_TIME (15)/*seconds*/
#define DEFAULT_RELAY_PULSE_TIME (1)/*seconds*/

typedef struct
{
  bool bValidData;
  uint32_t u32CardId;
  char arrName[MAX_LENTH_CARD_NAME];
} stcCardData;

typedef struct
{
  uint32_t RelayLagTime;
  uint32_t RelayPulseTime;
} Settings;

typedef struct
{
  uint32_t u32InitFlag;
  uint32_t u32NoOfCardDataSaved;
  Settings m_Settings;
  stcCardData g_DataBase[MAX_SIZE_CARD_SAVED];
} Database;

void InitNVM(void);
void SaveDatabaseDetails(void);
Database* ReadDatabseDetails(void);
Database* GetAddressOfDatabase(void);
void ResetAll(void);
void DeleteAllRecords(void);
uint32_t GetNumOfCardDetailsSaved(void);
uint32_t GetRelaylagTime(void);
bool SaveNewRecord(String strName , uint32_t u32CardId);
bool checkForDuplicateCardData(uint32_t u32CardId , uint32_t *pIndex);
#endif
