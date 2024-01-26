#ifndef NVM_HPP
#define NVM_HPP

#define MAX_LENTH_CARD_NAME (16)
#define MAX_SIZE_CARD_SAVED (500)
#define MAX_SIZE_LOG_SAVED (500)

typedef struct
{
  uint32_t u32CardId;
  char arrName[MAX_LENTH_CARD_NAME];
}CardData;

typedef struct
{
  uint32_t u32CardId;
  uint32_t Date;
  uint32_t Time;
  char arrName[MAX_LENTH_CARD_NAME];
}LogData;

typedef struct
{
  uint32_t u32NoOfCardDataSaved;
  CardData g_DataBase[MAX_SIZE_CARD_SAVED];
}CardDatabase;

/*Log not required to save in device*/
#if false
typedef struct
{
  uint32_t u32NoOfLogSaved;
  LogData  g_LogData[MAX_SIZE_LOG_SAVED];
}LogDatabase;
#endif

void InitNVM(void);
void SaveCardDetails(void);
CardDatabase* ReadCardDetails(void);
CardDatabase* GetAddressOfCardBuffer(void);
void SaveNewRecord(String strName , uint32_t u32CardId);
#endif
