#include <Arduino.h>
#include "App.hpp"
#include "Appl_Timer.hpp"
#include "Appl_Lcd.hpp"
#include "nvm.hpp"
#include "Drv_Weigand.hpp"
#include <LiquidCrystal.h>

LiquidCrystal g_lcd(PIN_LCD_RS,
                    PIN_LCD_EN,
                    PIN_LCD_D4,
                    PIN_LCD_D5,
                    PIN_LCD_D6,
                    PIN_LCD_D7);

m_Button g_ButtonDown = {PIN_BUTTON_DOWN , 0 , 0 };
m_Button g_ButtonRight = {PIN_BUTTON_RIGHT , 0 , 0 };
m_Button g_ButtonCancel = {PIN_BUTTON_CANCEL , 0 , 0 };
m_Button g_ButtonOk = {PIN_BUTTON_OK , 0 , 0 };

const char CONST_SET[] = "Set";
const char CONST_BACK[] = "Back";
const char CONST_UP[] = "Up";
const char CONST_DOWN[] = "Down";
const char CONST_DATABASE[] = "Database";

static void DisplayMainMenuOptions(int row);
static void DisplayMainMenuOptions_2(int row);
static e_ButtonState GetButtonState_Button(m_Button *pButton);
static void DislayRec(uint32_t u32RecId, 
                uint32_t u32TotalRecSaved, 
                uint32_t u32CardId, String strCardName);
static void ScanNewrecordHandler(Database *pRecData);
static void DisplayRecordsMenu(void);
static void DisplayMainMenu(void);
static void DisplayMenu(const char *pItems , 
          uint32_t u32NoOfItems , uint8_t u8Selection);
static String GetKeyBoard_cardName(String VisData1);
static void DeleteRecordHandler(stcCardData *pRecData);
static void ResetAllHandler(void);
static void DeleteAllRecordsHandler(void);
static void GateDelayHandler(void);

void InitLcd(void)
{
  // set up the LCD's number of columns and rows:
  g_lcd.begin(LCD_MAX_COL , LCD_MAX_ROW);

  pinMode(g_ButtonCancel.u8PinNo, INPUT);
  pinMode(g_ButtonDown.u8PinNo, INPUT);
  pinMode(g_ButtonRight.u8PinNo, INPUT);
  pinMode(g_ButtonOk.u8PinNo, INPUT);
}


void PrintLcd(String strMsg, 
              uint8_t u_n8Row , uint8_t u_n8Col , bool bRefresh)
{
  if (bRefresh)
  {
    g_lcd.clear();
  }
  g_lcd.setCursor( u_n8Row , u_n8Col);
  g_lcd.print(strMsg);
}
e_ButtonState GetButtonState_Button(m_Button *pButton)
{
  static stcTimer m_ButtonTimer;
  e_ButtonState bState = (enButton_off);
  if(true == Timer_IsRunning(&m_ButtonTimer))
  {
    if(true == Timer_IsTimeout(&m_ButtonTimer , enTimerNormalStop))
    {
      e_ButtonState bState = enButton_off;
      pButton->bCurrentState = digitalRead(pButton->u8PinNo);
      if(HIGH == pButton->bCurrentState && 
                    HIGH == pButton->bPrevState)
      {
       /*ON*/
       bState = (enButton_on);
      }
      else if(HIGH == pButton->bCurrentState && 
                    LOW == pButton->bPrevState)
      {
        /*Rising edge*/
        bState = (enButton_Released);
      }
      else if(LOW == pButton->bCurrentState && 
                    HIGH == pButton->bPrevState)
      {
        /*Falling edge*/
        bState = (enButton_Pressed);
      }
      else if(LOW == pButton->bCurrentState && 
                    LOW == pButton->bPrevState)
      {
        /*OFF*/
        bState = (enButton_off);
      }
      pButton->bPrevState = pButton->bCurrentState;
      return bState;
    }
  }
  else
  {
      InitilizeTimer(&m_ButtonTimer);
      StartTimer(&m_ButtonTimer , (50)/*100 Ms*/);
  }
  return bState;
}
/*Scanning screen - start*/
void Display_ScanningInProgress(void)
{
  static stcTimer m_ScanningTimer;
  static bool bState = false;
  if(true == Timer_IsRunning(&m_ScanningTimer))
  {
    if(true == Timer_IsTimeout(&m_ScanningTimer , enTimerNormalStop))
    { 
        PrintLcd(("Ready") , 7 , 0 , true);
        PrintLcd(("Menu") , 0 , 3 , false);
        if(bState)
        {
          PrintLcd(String("***Scanning***") , 3 , 1 , false);
          bState = false;
        }
        else
        {
          PrintLcd(String("   Scanning   ") , 3 , 1 , false);
          bState = true;
        }
    }
  }
  else
  {
    InitilizeTimer(&m_ScanningTimer);
    StartTimer(&m_ScanningTimer , (500)/*(0.5) Seconds*/);
  }

  /*Main menu button handler call - start*/
  if (enButton_Released == GetButtonState_Button(&g_ButtonOk))
  {
    /*Call main menu*/
    DisplayMainMenu();
    Serial.println("SCAN MENU -> MAIN MENU");
  }
  /*Main menu button handler call - end*/
}
/*Scanning screen - end*/
/*main menu - start*/
void DisplayMainMenuOptions(int row)
{
  PrintLcd(String(CONST_DATABASE), 2 , 0, true);
  PrintLcd(("Set Gate Delay"), 2 , 1, false);
  PrintLcd(("Delete database"), 2 , 2, false);
  PrintLcd(("Reset All"), 2 , 3, false);
  PrintLcd("*", 0 , row, false);
}
void DisplayMainMenuOptions_2(int row)
{
  PrintLcd(("Reset All"), 2 , 0, true);
  PrintLcd("*", 0 , row, false);
}
void DisplayMainMenu(void)
{
  int8_t n8SelectedRow = 0;
  int8_t n8MaxRow = 4;
  stcTimer m_Timer;
  InitilizeTimer(&m_Timer);
  StartTimer(&m_Timer , (LCD_DEFAULT_REFRESH_DELAY)/*100 Ms*/);
  while (true)
  {
    if(true == Timer_IsRunning(&m_Timer))
    {
      if(true == Timer_IsTimeout(&m_Timer , enTimerNormalStop))
      {
          if(n8MaxRow >= n8SelectedRow)
          {
            DisplayMainMenuOptions(n8SelectedRow);
          }
          else
          {
            DisplayMainMenuOptions_2(n8SelectedRow - LCD_MAX_ROW);
          }
      }
    }
    else
    {
        InitilizeTimer(&m_Timer);
        StartTimer(&m_Timer , (LCD_DEFAULT_REFRESH_DELAY)/*100 Ms*/);
    }


    if (enButton_Released == GetButtonState_Button(&g_ButtonRight))
    {
      Serial.println(String("Menu->RIGHT BUTTON (NOP)"));
      /*NOP*/
    }
    
    if (enButton_Released == GetButtonState_Button(&g_ButtonCancel))
    {
      Serial.println(String("Menu->Return"));
      return;/*Return to main screen*/
    }
    if (enButton_Released == GetButtonState_Button(&g_ButtonDown))
    {
      n8SelectedRow++;
      if((n8MaxRow - 1) < n8SelectedRow)
      {
        n8SelectedRow = 0;
      }
      Serial.println(String("Menu->ROW : ") + String(n8SelectedRow));
    }
    if (enButton_Released == GetButtonState_Button(&g_ButtonOk))
    {
      switch(n8SelectedRow)
      {
        case 0:/*Database*/
        {
          Serial.println("Menu->Database");
          DisplayRecordsMenu();
        }break;/*Export Records*/
        case 1:/*Set gate delay*/
        {
          GateDelayHandler();
          Serial.println("Menu->Set delay time");
        }break;
        case 2:/*Delete database*/
        {
          DeleteAllRecordsHandler();
          Serial.println("Menu->Delete databse");
        }break;
        case 3:/*Reset All*/
        {
          ResetAllHandler();
          Serial.println("Menu->Reset all");
        }break;
      }
    }
  }
}
/*main menu - end*/

/*Records menu - start*/
void DislayRec(uint32_t u32RecId, 
                uint32_t u32TotalRecSaved, 
                uint32_t u32CardId, String strCardName)
{
  PrintLcd(String(u32RecId) + "/" + String(u32TotalRecSaved), 0 , 0, true);
  PrintLcd(strCardName, 0 , 1, false);
  PrintLcd("Add", 0 , 3 , false);
  PrintLcd("Back", 5 , 3 , false);
  PrintLcd("Del", 11 , 3 , false);
  PrintLcd("^", 16 , 3 , false);
  PrintLcd("Id : " + String(u32CardId) , 0 , 2, false);
}
void DisplayRecordsMenu(void)
{
  int8_t n8SelectedRecNo = 0;
  stcTimer m_Timer;
  ReadDatabseDetails();/*Read card record deatils*/
  Database *pRecData = GetAddressOfDatabase();
  InitilizeTimer(&m_Timer);
  StartTimer(&m_Timer , (LCD_DEFAULT_REFRESH_DELAY)/*200 Ms*/);
  pRecData->u32NoOfCardDataSaved = GetNumOfCardDetailsSaved();
  while (true)
  {
    /*If no data saved - start*/
    if(0 == pRecData->u32NoOfCardDataSaved)/*if no data found*/
    {
      PrintLcd("No " + String(CONST_DATABASE) + " Found", 0 , 0 , true);
      PrintLcd("Add", 0 , 3 , false);
      PrintLcd("Back", 5 , 3 , false);
      while(true)
      {
          if (enButton_Released == 
            GetButtonState_Button(&g_ButtonCancel))
          {
            /*Call main menu*/
            Serial.println("Window - Return");
            return;
          }

          if (enButton_Released == 
            GetButtonState_Button(&g_ButtonOk))
          {
            /*Scan new record*/
            Serial.println("Window - ADD new record");
            ScanNewrecordHandler(pRecData);
            return;
          }
      }
    }
    /*If no data saved - end*/
    if(true == Timer_IsRunning(&m_Timer))
    {
      if(true == Timer_IsTimeout(&m_Timer , enTimerNormalStop))
      {
        DislayRec(n8SelectedRecNo , pRecData->u32NoOfCardDataSaved ,
              pRecData->g_DataBase[n8SelectedRecNo].u32CardId ,
              pRecData->g_DataBase[n8SelectedRecNo].arrName);
        
      }
    }
    else
    {
        InitilizeTimer(&m_Timer);
        StartTimer(&m_Timer , (LCD_DEFAULT_REFRESH_DELAY)/*100 Ms*/);
    }
    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonDown))
    {
      n8SelectedRecNo++;
      if((pRecData->u32NoOfCardDataSaved - 1) < n8SelectedRecNo)
      {
        n8SelectedRecNo = 0;
      }
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonRight))
    {
      /*Delete current record*/
      DeleteRecordHandler(&pRecData->g_DataBase[n8SelectedRecNo]);
      pRecData->u32NoOfCardDataSaved = GetNumOfCardDetailsSaved();
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonOk))
    {
      /*Scan new record*/
      ScanNewrecordHandler(pRecData);
      pRecData->u32NoOfCardDataSaved = GetNumOfCardDetailsSaved();
    }
    if (enButton_Released == 
      GetButtonState_Button(&g_ButtonCancel))
    {
      /*Call main menu*/
      Serial.println("Window - Return");
      return;
    }
  } //outside while
}
void DeleteRecordHandler(stcCardData *pRecData)
{
  PrintLcd(String("Delete Record?") , 0 , 0 , true);
  PrintLcd(String(pRecData->arrName) , 0 , 1 , false);
  PrintLcd(String(pRecData->u32CardId) , 0 , 2 , false);
  PrintLcd(String("YES NO") , 0 , 3 , false);
  while(true)
  {
    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonOk))
    {
      pRecData->u32CardId = 0;
      pRecData->bValidData = false;/*Delete record from database*/
      Serial.println("Delete record - YES");
      SaveDatabaseDetails();
      return;
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonCancel))
    {
      Serial.println("Delete record - NO");
      return;
    }
  }
}
/*Records menu - end*/
/*Scan new record menu - start*/
void ScanNewrecordHandler(Database *pRecData)
{
    bool StatusCh0 = false;
    uint32_t uRfidValCh0 = 0;
    stcTimer m_Timer;
    InitilizeTimer(&m_Timer);
    StartTimer(&m_Timer , (1000 * 10)/*10 Seconds*/);
    PrintLcd(String("Scanning new card") , 2 , 1 , true);
    PrintLcd(String("Back") , 4 , 3 , false);
    while(true)
    {
      if(true == Timer_IsTimeout(&m_Timer , enTimerNormalStop))
      {
        /*Timeout occured*/
        PrintLcd("Timeout", 0 , 0 , true);
        delay(1000);
        return;
        /*Return to record menu*/
      }
      if (enButton_Released == 
          GetButtonState_Button(&g_ButtonCancel))
      {
        return;
        /*Return to record menu*/
      }

      /*Saving new record only done in channel 0*/
      StatusCh0 = WeigandProcess_A(&uRfidValCh0);
      if(StatusCh0)
      {
        /*Check for existing record - start*/
        uint32_t u32RecIdx = 0;
        if(true == checkForDuplicateCardData(uRfidValCh0 , &u32RecIdx))
        {
            PrintLcd(String("Duplicate Record"), 0 , 1 , true);
            PrintLcd(String("Name :") + pRecData->g_DataBase[u32RecIdx].arrName, 0 , 2 , false);
            PrintLcd(String("ID :") + String(uRfidValCh0), 0 , 3 , false);
            Serial.println(String("Duplicate Record , Name : ")
                    + pRecData->g_DataBase[u32RecIdx].arrName + String(" , ID : ") +
                    String(uRfidValCh0));
                    
            delay(1000);
            return;
        }
        /*Check for existing record - end*/
        
        /*If new record then Save new record - start*/
        String strName = GetKeyBoard_cardName(String(uRfidValCh0));
        if(true == SaveNewRecord(strName , uRfidValCh0))/*New record - saved*/
        {
          PrintLcd(String("Saved Record"), 0 , 1 , true);
          PrintLcd(String("Name :") + strName, 0 , 2 , false);
          PrintLcd(String("ID :") + String(uRfidValCh0), 0 , 3 , false);
          Serial.println(String("Record saved , Name : ")
                    + strName + String(" , ID : ") +
                    String(uRfidValCh0));
        }
        else/*Not saved - memory full*/
        {
          PrintLcd(String("ERROR - Memory full"), 0 , 1 , true);
          Serial.println(String("Record saving failed (memroy full) , Name : ")
                    + strName + String(" , ID : ") +
                    String(uRfidValCh0));
        }
        delay(1000);
        return;
        /*If new record then Save new record - end*/
      }
    }
}
String GetKeyBoard_cardName(String VisData1)
{
  String StrBuffdata;
  uint8_t u8Idx = 0;
  char cAplhabet = '0';
  char strCardName[MAX_LENTH_CARD_NAME] = {0};
  stcTimer m_KeyBoardTimer;
  memset(strCardName , '_' , MAX_LENTH_CARD_NAME - 1);
  PrintLcd(VisData1, 0 , 0 , true);
  PrintLcd(String("Enter Name"), 0 , 1 , false);
  while(true)
  {
    if(true == Timer_IsRunning(&m_KeyBoardTimer))
    {
      if(true == Timer_IsTimeout(&m_KeyBoardTimer , enTimerNormalStop))
      {
        PrintLcd(VisData1, 0 , 0 , true);
        PrintLcd(String("Enter Name"), 0 , 1 , false);
        PrintLcd(String(strCardName), 0 , 2 , false);
        PrintLcd(String("^"), u8Idx , 3 , false);
      }
    }
    else
    {
        InitilizeTimer(&m_KeyBoardTimer);
        StartTimer(&m_KeyBoardTimer , (100)/*100 Ms*/);
    }

    if (enButton_Released == 
      GetButtonState_Button(&g_ButtonCancel))
    {
      /*Call main menu*/
      StrBuffdata = String("NO NAME");
      Serial.println("Window - Return");
      return StrBuffdata;
    }
    
    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonDown))
    {
      cAplhabet++;
      if('z' < cAplhabet)
      {
        cAplhabet = '0';/*Reset aphabet to zero*/
      }
      strCardName[u8Idx] = cAplhabet;
      /*Increment alphabet*/
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonRight))
    {
      u8Idx++;
      if((MAX_LENTH_CARD_NAME - 1) <= u8Idx)
      {
        u8Idx = 0;/*Reset index to zero*/
      }
      /*Next char*/
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonOk))
    {
       /*Save new record*/
       for(uint8_t nI = 0 ; nI < MAX_LENTH_CARD_NAME; ++nI)
       {
          if('_' == strCardName[nI])
          {
            strCardName[nI] = '\0';/*Remove '-'*/
          }
       }
       strCardName[MAX_LENTH_CARD_NAME - 1] = '\0';
       StrBuffdata = String(strCardName);
       return StrBuffdata;
    }
  }
}
/*Scan new record menu - end*/
void ResetAllHandler(void)
{
  PrintLcd(String("Reset all?") , 0 , 0 , true);
  PrintLcd(String("YES NO") , 0 , 3 , false);
  while(true)
  {
    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonOk))
    {
      ResetAll();
      Serial.println("Reset all - YES");
      SaveDatabaseDetails();
      return;
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonCancel))
    {
      Serial.println("Reset all - NO");
      return;
    }
  }
}
void DeleteAllRecordsHandler(void)
{
  PrintLcd(String("Delete all records?") , 0 , 0 , true);
  PrintLcd(String("YES NO") , 0 , 3 , false);
  while(true)
  {
    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonOk))
    {
      DeleteAllRecords();
      Serial.println("Delete all records - YES");
      SaveDatabaseDetails();
      return;
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonCancel))
    {
      Serial.println("Delete all records - NO");
      return;
    }
  }
}
void GateDelayHandler(void)
{
  stcTimer m_GateTimer;
  Database *pRecData = GetAddressOfDatabase();
  PrintLcd(String("Gate lag time(S) : ") , 0 , 0 , true);
  PrintLcd(String(pRecData->m_Settings.RelayLagTime) , 0 , 1 , false);
  PrintLcd("OK", 0 , 3 , false);
  PrintLcd("Back", 5 , 3 , false);
  PrintLcd("^", 16 , 3 , false);
  while(true)
  {
    if(true == Timer_IsRunning(&m_GateTimer))
    {
      if(true == Timer_IsTimeout(&m_GateTimer , enTimerNormalStop))
      {
          PrintLcd(String("Gate lag time(S) : ") , 0 , 0 , true);
          PrintLcd(String(pRecData->m_Settings.RelayLagTime) , 0 , 1 , false);
          PrintLcd("OK", 0 , 3 , false);
          PrintLcd("Back", 5 , 3 , false);
          PrintLcd("^", 16 , 3 , false);
      }
    }
    else
    {
        InitilizeTimer(&m_GateTimer);
        StartTimer(&m_GateTimer , (100)/*100 Ms*/);
    }

      if (enButton_Released == 
          GetButtonState_Button(&g_ButtonOk))
      {
        SaveDatabaseDetails();
        Serial.println("OK");
        return;
      }
      if (enButton_Released == 
          GetButtonState_Button(&g_ButtonCancel))
      {
        Serial.println("Back");
        return;
      }
      if (enButton_Released == 
          GetButtonState_Button(&g_ButtonDown))
      {
        pRecData->m_Settings.RelayLagTime++;
        if(MAX_RELAY_LAG_TIME <= pRecData->m_Settings.RelayLagTime)
        {
          pRecData->m_Settings.RelayLagTime = MIN_RELAY_LAG_TIME;
        }
        SaveDatabaseDetails();
        Serial.println("Saved relay lag time");
        /*Return to record menu*/
      }
  }
}
