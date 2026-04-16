/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "GLCD/GLCD.h"
#include "../timer/timer.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/


#define N_RIG 32
#define N_COL 24
volatile int down_0 = 0;
int dir=-1;
extern int pos_x;
extern int punteggio;
extern int pos_y;
extern int pos_pac_x;
extern int pos_pac_y;
int p;
int pausa=0;
int next_dir;

int i,j;
extern int matrice[N_RIG][N_COL];
extern void clear_pacman(int pos_x,int pos_y);
extern void draw_pacman(int pos_x,int pos_y);
void RIT_IRQHandler (void)
{						
	//ADC_start_conversion();
	int test;
	static int j_left=0, j_right=0, j_down=0, j_up=0;
	/*Gestione Joystick Polling*/
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){ //joystick down
		j_down++;
		switch(j_down){
			case 1:
				next_dir=0;
				break;			
			default:
				break;
		}
	}else
		j_down=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){ //joystick left
		j_left++;
		switch(j_left){
			case 1:
				next_dir=1;
				break;			
			default:
				break;
		}
	}else
		j_left=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){ //joystick right
		j_right++;
		switch(j_right){
			case 1:  //POS_X =18 POS_Y=12
				next_dir=2;
				break;			
			default:
				break;
		}
	}else
		j_right=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){ //joystick up
		j_up++;
		switch(j_up){
			case 1:
				next_dir=3;
				break;			
			default:
				break;
		}
	
	}else
		j_up=0;
	/*GESTIONE BUTTON*/
	if(down_0!=0){  
			down_0++;  
			if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){    //BOTTONE INT0
				//j_left=0, j_right=0, j_down=0, j_up=0;
				switch(down_0){
				case 2:
					if(pausa==0){
						pausa=1;
						for(i=0;i<50;i++){					
							for(j=0;j<70;j++){
								LCD_SetPoint(j+90,i+140,Black);
							}
						}
						GUI_Text(106,158,(uint8_t *)"PAUSE",White,Black);
						
					}
					else if(pausa==1){
						pausa=0;
						disegna_tutto(14,9,19,16);
						draw_pacman(pos_pac_x,pos_pac_y);
						enable_timer(1);
					}
					
					break;
				//per controllare pressioni prolungate: es. 5[sec]/50[msec](RIT timer) = 100
				//case 100:
				//	break;
				default:
					//se voglio gestire il button on X click
					//if(down_0 % X == 0){
					//}					
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	

	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
