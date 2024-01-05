#ifndef APP_HPP
#define APP_HPP

#define DEBUG_SERIAL_BAUD_RATE (115200)

#define PIN_WG_CH0_D0 (0)
#define PIN_WG_CH0_D1 (1)
#define PIN_WG_CH1_D0 (6)
#define PIN_WG_CH1_D1 (7)

#define PIN_LCD_RS (19)
#define PIN_LCD_EN (22)
#define PIN_LCD_D4 (16)
#define PIN_LCD_D5 (17)
#define PIN_LCD_D6 (2)
#define PIN_LCD_D7 (3)

#define PIN_RELAY_1 (10)
#define PIN_RELAY_2 (11)

#define PIN_BUTTON_DOWN   (21)
#define PIN_BUTTON_RIGHT  (20)
#define PIN_BUTTON_CANCEL (24)
#define PIN_BUTTON_OK     (23)

void InitRfidApplication(void);
void ProcessRfidData(uint32_t u32Ch0 , 
                      uint32_t u32Ch1 ,
                      bool EnCh0 , bool EnCh1);
#endif
