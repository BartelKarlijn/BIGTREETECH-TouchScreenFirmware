#include "includes.h"
#include "xpt2046.h"
#include "GPIO_Init.h"
#include "Selectmode.h"


const GUI_RECT rect_of_mode[SELECTMODE]={
  //2 select icon
  {1*SPACE_SELEX+0*ICON_WIDTH, SPACE_SELEY, 1*SPACE_SELEX+1*ICON_WIDTH, SPACE_SELEY+ICON_HEIGHT},
  {3*SPACE_SELEX+1*ICON_WIDTH, SPACE_SELEY, 3*SPACE_SELEX+2*ICON_WIDTH, SPACE_SELEY+ICON_HEIGHT},
};

const uint8_t icon_mode [MODE_COUNT]={
  ICON_MARLIN,
  ICON_BIGTREETECH,
};

void show_selectICON(void)
{
  for(uint8_t i = 0;i < MODE_COUNT; i++)
  {
    ICON_ReadDisplay(rect_of_mode[i].x0, rect_of_mode[i].y0, icon_mode[i]);
  }

  const GUI_RECT mode_title_rect[MODE_COUNT] = {
    {0,           rect_of_mode[0].y1 + BYTE_HEIGHT/2,   text_startx,  rect_of_mode[0].y1 + BYTE_HEIGHT/2 + BYTE_HEIGHT},
    {text_startx, rect_of_mode[0].y1 + BYTE_HEIGHT/2,   LCD_WIDTH,    rect_of_mode[0].y1 + BYTE_HEIGHT/2 + BYTE_HEIGHT},
  };

  GUI_RestoreColorDefault();

  if(infoSettings.marlin_type == LCD12864)
    GUI_DispStringInPrect(&mode_title_rect[0],(uint8_t *)"LCD12864 Mode");
  else
    GUI_DispStringInPrect(&mode_title_rect[0],(uint8_t *)"LCD2004 Mode");

  GUI_DispStringInPrect(&mode_title_rect[1],(uint8_t *)"Touch Mode");
}

bool LCD_ReadPen(uint16_t intervals)
{
  static u32 TouchTime = 0;

  if(!XPT2046_Read_Pen())
  {
    if(OS_GetTimeMs() - TouchTime > intervals)
    {
      return true;
    }
  }
  else
  {
    TouchTime = OS_GetTimeMs();
  }
  return false;
}

bool LCD_BtnTouch(uint16_t intervals)
{
  const uint8_t border_off = 4;
  for (uint8_t i = 0; i < MODE_COUNT; i++)
  {
    GUI_SetColor(i == nowMode ? infoSettings.marlin_mode_font_color : BACKGROUND_COLOR);
    GUI_DrawRect(rect_of_mode[i].x0 - border_off, rect_of_mode[i].y0 - border_off,
                 rect_of_mode[i].x1 + border_off, rect_of_mode[i].y1 + border_off);
  }
  GUI_RestoreColorDefault();
}

void loopCheckMode(void)
{
//  #ifndef CLEAN_MODE_SWITCHING_SUPPORT
//  IDEALLY I would like to be able to swap even when the TFT is in printing mode
//  but before I can allow that I need a way to make sure that we swap back into the right mode (and correct screen)
//  and I really want a reliable way to DETECT that the TFT should be in printing mode even when the print was started externally.
  if(isPrinting() || skipMode)
    return;

  if(infoMenu.menu[infoMenu.cur] == menuMode)
    return;

//  #endif
  if(LCD_ReadPen(LCD_CHANGE_MODE_INTERVALS) || encoder_ReadBtn(LCD_CHANGE_MODE_INTERVALS))
  {
    BtnTime = OS_GetTimeMs();
  }
  return false;
}

#if 1
 uint8_t LCD_ReadTouch(void)
{
	u16 ex=0,ey=0;
  static u32 CTime = 0;
  static u16 sy;
	static bool MOVE = false;

	if(!XPT2046_Read_Pen() && CTime < OS_GetTimeMs())
  {
		TS_Get_Coordinates(&ex,&ey);
		if(!MOVE)
		sy = ey;

		MOVE = true;

    if(key_num == MKEY_0 || key_num == MKEY_1)
    {
      nowMode = key_num;
      break;
    }

    #if LCD_ENCODER_SUPPORT
      if(encoderPosition)
      {
        nowMode = NOBEYOND(0, nowMode + encoderPosition, MODE_COUNT - 1);
        drawSelectedMode(nowMode);
        encoderPosition = 0;
      }

      if(encoder_ReadBtn(LCD_BUTTON_INTERVALS))
      {
        break;
      }
      loopCheckEncoderSteps();
    #endif

    #ifdef LCD_LED_PWM_CHANNEL
      loopDimTimer();
    #endif

    if (infoSettings.serial_alwaysOn == 1)
    {
      loopBackEnd();
    }
  }
	switch(num)
	{
		case 0:
			break;
		case 1:
			GPIO_SetLevel(LCD_BTN_PIN, 0);
			GPIO_SetLevel(LCD_BTN_PIN, 1);
			break;
		case 2:
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			break;
		case 3:
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 0);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 0);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			Delay_us(8);
			GPIO_SetLevel(LCD_ENCA_PIN, 1);
			GPIO_SetLevel(LCD_ENCB_PIN, 1);
			break;
	}

  LCD_EncoderInit();
}

MKEY_VALUES MKeyGetValue(void)
{
  return (MKEY_VALUES)KEY_GetValue(sizeof(rect_of_mode)/sizeof(rect_of_mode[0]), rect_of_mode);
}

void selectmode(int8_t  nowMode)
{
	GUI_SetBkColor(BLACK);
  GUI_ClearRect(text_startx, rect_of_mode[1].y0-BYTE_HEIGHT+ICON_WIDTH+BYTE_WIDTH, LCD_WIDTH, rect_of_mode[1].y0+ICON_WIDTH+BYTE_WIDTH);
	GUI_ClearRect(0, rect_of_mode[1].y0-BYTE_HEIGHT+ICON_WIDTH+BYTE_WIDTH, text_startx, rect_of_mode[1].y0+ICON_WIDTH+BYTE_WIDTH);

  if(nowMode==SERIAL_TSC)
  {
    GUI_SetColor(ST7920_FNCOLOR);
    GUI_DispStringInRect(text_startx, rect_of_mode[1].y0-BYTE_HEIGHT+ICON_WIDTH+BYTE_WIDTH, LCD_WIDTH, rect_of_mode[1].y0+ICON_WIDTH+BYTE_WIDTH,(uint8_t *)"Touch Mode");
    GUI_SetColor(FONT_COLOR);
    GUI_DispStringInRect(0, rect_of_mode[1].y0-BYTE_HEIGHT+ICON_WIDTH+BYTE_WIDTH, text_startx, rect_of_mode[1].y0+ICON_WIDTH+BYTE_WIDTH,(uint8_t *)"Marlin Mode");
  }
  else
  {
    GUI_SetColor(ST7920_FNCOLOR);
    GUI_DispStringInRect(0, rect_of_mode[1].y0-BYTE_HEIGHT+ICON_WIDTH+BYTE_WIDTH, text_startx,rect_of_mode[1].y0+ICON_WIDTH+BYTE_WIDTH,(uint8_t *)"Marlin Mode");
    GUI_SetColor(FONT_COLOR);
    GUI_DispStringInRect(text_startx,rect_of_mode[1].y0-BYTE_HEIGHT+ICON_WIDTH+BYTE_WIDTH,LCD_WIDTH,rect_of_mode[1].y0+ICON_WIDTH+BYTE_WIDTH,(uint8_t *)"Touch Mode");
  }
}

#endif
