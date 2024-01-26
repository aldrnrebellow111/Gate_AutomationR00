#ifndef APP_LCD_HPP
#define APP_LCD_HPP

#define LCD_MAX_COL (20)
#define LCD_MAX_ROW (4)
#define LCD_DEFAULT_REFRESH_DELAY (500)/*Milliseconds*/

typedef struct
{
  uint8_t u8PinNo;
  uint8_t bCurrentState;
  uint8_t bPrevState;
}m_Button;

typedef enum
{
  enButton_off = 0,
  enButton_Released = 1,
  enButton_Pressed = 2,
  enButton_on = 3
}e_ButtonState;

void InitLcd(void);
void PrintLcd(String strMsg, 
              uint8_t u_n8Row , uint8_t u_n8Col , bool bRefresh);
void Display_ScanningInProgress(void);
#endif
