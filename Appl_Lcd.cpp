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
static void ScanNewrecordHandler(CardDatabase *pRecData);
static void DisplayRecordsMenu(void);
static void DisplayMainMenu(void);
static void DisplayMenu(const char *pItems , 
          uint32_t u32NoOfItems , uint8_t u8Selection);
          
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
  }
  /*Main menu button handler call - end*/
}
/*Scanning screen - end*/
/*main menu - start*/
void DisplayMainMenuOptions(int row)
{
  PrintLcd(String(CONST_DATABASE), 2 , 0, true);
  PrintLcd(("Export Records"), 2 , 1, false);
  PrintLcd(("Set Time"), 2 , 2, false);
  PrintLcd(("Set Gate Delay"), 2 , 3, false);
  PrintLcd("*", 0 , row, false);
}
void DisplayMainMenuOptions_2(int row)
{
  PrintLcd(("Reset All"), 2 , 0, true);
  PrintLcd(("Delete Logs"), 2 , 1, false);
  PrintLcd("*", 0 , row, false);
}
void DisplayMainMenu(void)
{
  int8_t n8SelectedRow = 0;
  int8_t n8MaxRow = 6;
  stcTimer m_Timer;
  InitilizeTimer(&m_Timer);
  StartTimer(&m_Timer , (LCD_DEFAULT_REFRESH_DELAY)/*100 Ms*/);
  while (true)
  { 
    if(true == Timer_IsRunning(&m_Timer))
    {
      if(true == Timer_IsTimeout(&m_Timer , enTimerNormalStop))
      {
        if(3 >= n8SelectedRow)
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
      /*NOP*/
    }
    
    if (enButton_Released == GetButtonState_Button(&g_ButtonCancel))
    {
      return;/*Return to main screen*/
    }
    if (enButton_Released == GetButtonState_Button(&g_ButtonDown))
    {
      n8SelectedRow++;
      if((n8MaxRow - 1) < n8SelectedRow)
      {
        n8SelectedRow = 0;
      }
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
        case 1:
        {
          Serial.println("Menu->Export record");
        }break;
        case 2:/*Set Time*/
        {
          Serial.println("Menu->Set time");
        }break;
        case 3:/*Set gate delay*/
        {
          Serial.println("Menu->Set delay time");
        }break;
        case 4:/*Reset All*/
        {
          Serial.println("Menu->Reset all");
        }break;
        case 5:/*Delete Logs*/
        {
          Serial.println("Menu->Delete logs");
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
  PrintLcd("Id : " + String(u32CardId) , 0 , 2, false);
}
void DisplayRecordsMenu(void)
{
  int8_t n8SelectedRecNo = 0;
  stcTimer m_Timer;
  ReadCardDetails();/*Read card record deatils*/
  CardDatabase *pRecData = GetAddressOfCardBuffer();
  InitilizeTimer(&m_Timer);
  StartTimer(&m_Timer , (LCD_DEFAULT_REFRESH_DELAY)/*200 Ms*/);
  while (true)
  {
    /*If no data saved - start*/
    if(0 == pRecData->u32NoOfCardDataSaved)/*if no data found*/
    {
      PrintLcd("No " + String(CONST_DATABASE) + " Found", 0 , 0 , true);
      PrintLcd("Add", 0 , 3 , false);
      while(true)
      {
          if (enButton_Released == 
            GetButtonState_Button(&g_ButtonCancel))
          {
            /*Call main menu*/
            return;
          }

          if (enButton_Released == 
            GetButtonState_Button(&g_ButtonOk))
          {
            /*Scan new record*/
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
    }

    if (enButton_Released == 
          GetButtonState_Button(&g_ButtonOk))
    {
      /*Scan new record*/
      ScanNewrecordHandler(pRecData);
    }
  } //outside while
}
/*Records menu - end*/
/*Scan new record menu - start*/
void ScanNewrecordHandler(CardDatabase *pRecData)
{
    bool StatusCh0 = false;
    uint32_t uRfidValCh0 = 0;
    stcTimer m_Timer;
    InitilizeTimer(&m_Timer);
    StartTimer(&m_Timer , (1000 * 10)/*10 Seconds*/);
    PrintLcd(String("Scanning new card") , 2 , 1 , true);
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
      StatusCh0 = WeigandProcess((GetInstance_RfidCh0()) , &uRfidValCh0);
      if(StatusCh0)
      {
        /*Save new record*/
//        String strName = GetKeyBoard_cardName(pRecData->g_DataBase);
//        SaveNewRecord(GetAddressOfCardBuffer() , uRfidValCh0);
        PrintLcd("Record saved", 0 , 0 , true);
        delay(1000);
        return;
      }
    }
}
String GetKeyBoard_cardName(char *arr_cCurrentName)
{
  char strCardName[MAX_LENTH_CARD_NAME] = {0};
  while(true)
  {
    
  }
}
/*Scan new record menu - end*/
