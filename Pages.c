#include "main.h"
#include <stdio.h>
#include <string.h>
/*
uint8_t *trainListname[30];
uint8_t *convertListname[30];
uint8_t trainList = 2, convertList = 2;
*/

#define AUDIO_IN_PCM_BUFFER_SIZE                   (4*2304) /* buffer size in half-word */
#define AUDIO_IN_PDM_BUFFER_SIZE                   INTERNAL_BUFF_SIZE
/* Audio file size and start address are defined here since the audio file is
   stored in Flash memory as a constant table of 16-bit data */
#define AUDIO_START_OFFSET_ADDRESS    (0)            /* Offset relative to audio file header size */

/* Private typedef -----------------------------------------------------------*/
typedef enum {
  REC_NONE_STATE = 0, 
  REC_RESET_STATE,     
  RECORD_ONGOING,   
  REC_ERROR_STATE,
}AUDIOREC_StateTypeDef;

typedef enum {
  BUFFER_EMPTY = 0,
  BUFFER_FULL,
}WR_BUFFER_StateTypeDef;

typedef enum {
  TRANSFER_NONE = 0,
  TRANSFER_HALF,
  TRANSFER_FULL
} IT_StateTypeDef;

typedef struct {
  int32_t offset;
  uint32_t rec_length;
  uint32_t pcm_ptr;
  WR_BUFFER_StateTypeDef wr_state;
  IT_StateTypeDef it_state;
  uint16_t pdm_buff[AUDIO_IN_PDM_BUFFER_SIZE];
  uint16_t pcm_buff[AUDIO_IN_PCM_BUFFER_SIZE];
}Audio_BufferTypeDef;

extern Audio_BufferTypeDef  buffer_ctl;
static uint8_t AudioRec_State;
static uint32_t AudioRecFullBuff_addr;

static __IO uint16_t CurrentPosition;

void HomePage(void)
{
    uint32_t setBgColor = 0xFFFC4646;
    uint32_t setTitleColor = 0xFF48f9d9;
    uint32_t setSubtitleColor = 0xFFf7e771;
    uint32_t setButton1Color = 0xFF000000, setButton2Color = 0xFF2c1a91;
    uint32_t setButtonTextColor = 0xFFFFFFFF;
    
    //lcd.DisplayStringAt(0, LINE(idx+1), (uint8_t *)&text, LEFT_MODE);
    BSP_LCD_Clear(setBgColor);
    BSP_LCD_SetBackColor(setBgColor);
    
    
    //display the title : Voice Convertion 
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetTextColor(setTitleColor);
    BSP_LCD_DisplayStringAt(0, LINE(3), (uint8_t *)"Voice Conversion", CENTER_MODE);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_SetTextColor(setSubtitleColor);
    BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)"Demo for ECL Project", CENTER_MODE);
    
    //display the two buttons
    BSP_LCD_SetTextColor(setButton1Color);
    BSP_LCD_FillCircle(180, 280, 100);
    BSP_LCD_SetTextColor(setButton2Color);
    BSP_LCD_FillCircle(600, 280, 100);
    
    //display the word on the buttons
    BSP_LCD_SetBackColor(setButton1Color);
    BSP_LCD_SetTextColor(setButtonTextColor );
    BSP_LCD_DisplayStringAt(580, 280, (uint8_t *)"Train Data", CENTER_MODE);
    BSP_LCD_SetBackColor(setButton2Color);
    BSP_LCD_DisplayStringAt(200, 280, (uint8_t *)"Start VC", CENTER_MODE);
}

void TrainingListPage(uint8_t *list[], uint8_t size)
{
    uint8_t i;
    uint32_t setBgColor  = 0xFFFC4646;
    uint32_t setTitleColor = 0xFF48f9d9;
    uint32_t setListColor = 0xFFf7e771;
    uint32_t setButtonColor = 0xFF000000;
    uint32_t setMenuTextColor = 0xFFFFFFFF;
   
    
    BSP_LCD_Clear(setBgColor);
    
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(setBgColor);
		BSP_LCD_SetTextColor(setTitleColor );
    BSP_LCD_DisplayStringAt(150, LINE(3), (uint8_t *)"Your Training List", LEFT_MODE);
    
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_SetBackColor(setBgColor);
    BSP_LCD_SetTextColor(setListColor );
    for(i = 0 ; i < size ; i++)
    {
				char* Str;
				sprintf((char*)Str, "%d. ", i + 1);
				BSP_LCD_DisplayStringAt(180, LINE(6 + i * 2), (uint8_t *)Str, LEFT_MODE);
        BSP_LCD_DisplayStringAt(260, LINE(6 + i * 2), list[i], LEFT_MODE);
				
    }
    
    
    BSP_LCD_SetTextColor(setButtonColor);
    BSP_LCD_FillRect(660, 350, 100, 50);
    
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(setButtonColor);
    BSP_LCD_SetTextColor(setMenuTextColor);
    BSP_LCD_DisplayStringAt(680, 360, (uint8_t *)"Back", LEFT_MODE);
    
    
}

void VCListPage(uint8_t *list[], uint8_t size)
{
    uint8_t i;
    uint32_t setBgColor  = 0xFFFC4646;
    uint32_t setTitleColor = 0xFF48f9d9;
    uint32_t setListColor = 0xFFf7e771;
    uint32_t setButtonColor = 0xFF000000;
    uint32_t setBackTextColor = 0xFFFFFFFF;
   
    BSP_LCD_Clear(setBgColor);
    
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(setBgColor);
    BSP_LCD_SetTextColor(setTitleColor );
    BSP_LCD_DisplayStringAt(150, LINE(3), (uint8_t *)"Your Voice Conversion List", LEFT_MODE);
    
    BSP_LCD_SetFont(&Font20);
    ///BSP_LCD_SetBackColor(setBgColor);
    ///BSP_LCD_SetTextColor(setListColor );
    for(i = 0 ; i < size ; i++)
    {
					
				BSP_LCD_SetBackColor(setBgColor);
				BSP_LCD_SetTextColor(setListColor );
				char* Str = ">>> ";
				BSP_LCD_DisplayStringAt(180, LINE(6 + i * 2), (uint8_t *)Str, LEFT_MODE);
        BSP_LCD_DisplayStringAt(230, LINE(6 + i * 2), list[i], LEFT_MODE);
				
				
				BSP_LCD_SetBackColor(setListColor);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				BSP_LCD_DisplayStringAt(420, LINE(6 + i * 2), (uint8_t *)" Convert! ", LEFT_MODE);
    }
    
    
    BSP_LCD_SetTextColor(setButtonColor);
    BSP_LCD_FillRect(660, 350, 100, 45);
    
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(setButtonColor);
    BSP_LCD_SetTextColor(setBackTextColor);
    BSP_LCD_DisplayStringAt(680, 360, (uint8_t *)"Back", LEFT_MODE);
    
    
}

void VCPage(char * targetName)
{
		uint32_t setBgColor  = 0xFFFC4646;
		uint32_t setTitleColor = 0xFF48f9d9;
    uint32_t setButtonColor = 0xFF000000;
    uint32_t setBackTextColor = 0xFFFFFFFF;
		
		BSP_LCD_Clear(setBgColor);
		
		///char* nameStr;
		///sprintf((char*)nameStr, "%s Voice Conversion", targetName);
		
		BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(setBgColor);
    BSP_LCD_SetTextColor(setTitleColor);
    BSP_LCD_DisplayStringAt(0, LINE(7), (uint8_t *)"Voice Conversion", CENTER_MODE);
		BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *) targetName, CENTER_MODE);
		
		BSP_LCD_SetFont(&Font20);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0, LINE(10), (uint8_t *)"Press the Button to record", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, LINE(11), (uint8_t *)"and", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, LINE(12), (uint8_t *)"release to finish.", CENTER_MODE);
		
	
		
    BSP_LCD_SetTextColor(setButtonColor);
    BSP_LCD_FillRect(660, 350, 100, 45);
    
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetBackColor(setButtonColor);
    BSP_LCD_SetTextColor(setBackTextColor);
    BSP_LCD_DisplayStringAt(680, 360, (uint8_t *)"Back", LEFT_MODE);
		
		
		
		//////Start Record //////		
		
		uint16_t NbRecord_done = 0;
		uint8_t FreqStr[256] = {0};
		AudioRec_State = REC_NONE_STATE;
		buffer_ctl.rec_length = 0;
		/* Initialize AudioRecFullBuff at the address of the audio_play reader */

		AudioRecFullBuff_addr = AUDIO_REC_START_ADDR;

		///AudioRec_SetHint();

		/* Initialize Audio Recorder */
		if (BSP_AUDIO_IN_Init(DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR) == AUDIO_OK)
		{
			AudioRec_State = REC_RESET_STATE;
			///BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			///BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
			///BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"  AUDIO RECORD INIT OK  ", CENTER_MODE);
		}
		else
		{
			AudioRec_State = REC_ERROR_STATE;
			BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"  AUDIO RECORD INIT FAIL", CENTER_MODE);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 80, (uint8_t *)"  Try to reset board ", CENTER_MODE);
		}
		uint8_t is_record = 0;
		/* Infinite loop */
		while(!is_record)
		{
			if(BSP_PB_GetState(BUTTON_USER) == PB_RESET)
			{
				HAL_Delay(10);
				while (buffer_ctl.rec_length < AUDIO_REC_TOTAL_SIZE * 4 && BSP_PB_GetState(BUTTON_USER) == PB_RESET)
				{
					is_record = 1;
					BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
					BSP_LCD_SetTextColor(LCD_COLOR_BLUE);

					if (buffer_ctl.it_state == TRANSFER_HALF)
					{
						/* PDM to PCM data convert */
						BSP_AUDIO_IN_PDMToPCM((uint16_t*)&buffer_ctl.pdm_buff[0],
																	&buffer_ctl.pcm_buff[buffer_ctl.pcm_ptr]);

						buffer_ctl.pcm_ptr+= AUDIO_IN_PDM_BUFFER_SIZE/4/2;

						if(buffer_ctl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE/2)
						{
							buffer_ctl.wr_state   =  BUFFER_FULL;
							buffer_ctl.offset  = 0;
						}

						if(buffer_ctl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
						{
							buffer_ctl.wr_state   =  BUFFER_FULL;
							buffer_ctl.offset  = AUDIO_IN_PCM_BUFFER_SIZE/2;
							buffer_ctl.pcm_ptr = 0;
						}
						buffer_ctl.it_state = TRANSFER_NONE;
					}

					if (buffer_ctl.it_state == TRANSFER_FULL)
					{
						/* PDM to PCM data convert */
						if (BSP_AUDIO_IN_PDMToPCM((uint16_t*)&buffer_ctl.pdm_buff[INTERNAL_BUFF_SIZE/2], &buffer_ctl.pcm_buff[buffer_ctl.pcm_ptr]) == 0)
						{
							buffer_ctl.pcm_ptr+= AUDIO_IN_PDM_BUFFER_SIZE/4/2;

							if(buffer_ctl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE/2)
							{
								buffer_ctl.wr_state =  BUFFER_FULL;
								buffer_ctl.offset = 0;
							}

							if(buffer_ctl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
							{
								buffer_ctl.wr_state =  BUFFER_FULL;
								buffer_ctl.offset = AUDIO_IN_PCM_BUFFER_SIZE/2;
								buffer_ctl.pcm_ptr = 0;
							 }
						}
						buffer_ctl.it_state = TRANSFER_NONE;
						NbRecord_done++;

					}

					/* PCM buffer is full, copy it into the record_file */
					if (buffer_ctl.wr_state == BUFFER_FULL)
					{
						memcpy((uint32_t *)AudioRecFullBuff_addr, (uint8_t*)(buffer_ctl.pcm_buff + buffer_ctl.offset), AUDIO_IN_PCM_BUFFER_SIZE);
						AudioRecFullBuff_addr += AUDIO_IN_PCM_BUFFER_SIZE;
						buffer_ctl.rec_length += AUDIO_IN_PCM_BUFFER_SIZE;
						buffer_ctl.wr_state =  BUFFER_EMPTY;
					}
					if (AudioRec_State == REC_RESET_STATE)
					{
						/* Display the state on the screen */
						BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
						BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
						BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 80, (uint8_t *)"       RECORDING...     ", CENTER_MODE);
						/* Start Recording */
						buffer_ctl.rec_length = 0;
						buffer_ctl.wr_state = BUFFER_EMPTY;
						buffer_ctl.it_state = TRANSFER_NONE;
						BSP_AUDIO_IN_Record((uint16_t*)&buffer_ctl.pdm_buff[0], INTERNAL_BUFF_SIZE);
						AudioRec_State = RECORD_ONGOING;
					}

					//if (CheckForUserInput() > 0)
					//{
						//BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 80, (uint8_t *)"Hello World", CENTER_MODE);
						
						//HAL_Delay(1000);
						/* Stop recorder and exit the test (go to next test) */
						//BSP_AUDIO_IN_Stop();
						//return;
					//}
				}
			}
		}

		/* Stop recorder once buffer_ctl.rec_length < AUDIO_REC_TOTAL_SIZE*/
		BSP_AUDIO_IN_Stop();
		
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_SetBackColor(setBgColor);
		///BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
		BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
		///sprintf((char*)FreqStr, "RECORDING DONE, %d frames, START PLAYBACK", NbRecord_done);
		///BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 65, (uint8_t *)FreqStr, CENTER_MODE);
		NbRecord_done = buffer_ctl.rec_length / (DEFAULT_AUDIO_IN_FREQ * DEFAULT_AUDIO_IN_CHANNEL_NBR * 2);
		sprintf((char*)FreqStr, "Elapsed_time: %d s", NbRecord_done);
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 65, (uint8_t *)FreqStr, CENTER_MODE);



		/* -----------Start Playback -------------- */
		
		HAL_Delay(500);
		/* Initialize audio IN at REC_FREQ*/
		if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, 60/*Volume*/, DEFAULT_AUDIO_IN_FREQ/2) == 0)
		{
			///BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			///BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"  AUDIO CODEC   OK  ", CENTER_MODE);
		}
		else
		{
			///BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			///BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"  AUDIO CODEC  FAIL ", CENTER_MODE);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 80, (uint8_t *)" Try to reset board ", CENTER_MODE);
		}

		/* Play the recorded buffer*/
		uint32_t maxLength;
		if(buffer_ctl.rec_length > AUDIO_REC_TOTAL_SIZE * 4)
			maxLength = AUDIO_REC_TOTAL_SIZE * 4;
		else maxLength = buffer_ctl.rec_length;
		
		if(AUDIO_Play_Start((uint32_t *)AUDIO_REC_START_ADDR, (uint32_t)maxLength) == 0)
		{
			///BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			///BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"  AUDIO PLAYBACK   OK  ", CENTER_MODE);
		} else
		{
			BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"  AUDIO PLAYBACK  FAIL ", CENTER_MODE);
		}
		
		BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 45, (uint8_t *)"PLAYBACK DONE", CENTER_MODE);

		while (1)
		{
			/* Toggle LED4 */
			BSP_LED_Toggle(LED4);

			/* Insert 100 ms delay */
			HAL_Delay(100);
			if (CheckForUserInput() > 0)
			{
				/* Set LED4 */
				BSP_LED_On(LED4);
				/* Stop recorder */
				BSP_AUDIO_IN_Stop();
				/* Stop Player before close Test */
				BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
				break;
				//return;
			}
		}
		
}

