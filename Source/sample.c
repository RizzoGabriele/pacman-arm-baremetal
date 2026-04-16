/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "led//led.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Per rand() e srand()
#include <time.h>   // Per time()
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#define INF 1000
#define N_COL 24
#define N_RIG 32
#define MAX_ROWS 32
#define MAX_COLS 24
#define SCALE 10
#define MAX_BALLS 240
#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
char punti_testo[20];
int coordinata_x_porta=15,coordinata_y_porta=12;
char x[2]="x";
char n_vite_text[2];
int rand_time;
int palline_s_inserite=0;
char base_pt[4]="Pt:";
int pos_x,pos_y,punteggio=0;
int vite=1;
int pos_pac_x=18,pos_pac_y=12;
int pos_blinky_x=16,pos_blinky_y=12;

int matrice[N_RIG][N_COL]= {  //1 spazio libero, 2 spazio libero ma dove non ci possono essere palline per regole di gioco, 0 muro, -1 background, 3 portale,10 palline normali,11 palline speciali
    /*0*/     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    /*1*/     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    /*2*/     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    /*3*/     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*4*/     {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    /*5*/     {0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0},
    /*6*/     {0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0},
    /*7*/     {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    /*8*/     {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0},
    /*9*/     {0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    /*10*/    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    /*11*/    {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
    /*12*/    {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    /*13*/    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    /*14*/    {0, 1, 0, 0, 1, 1, 1, 1, 0, 2, 2, 2, 2, 2, 2 ,2, 0, 0, 0, 1, 0, 0, 1, 0},
    /*15*/    {0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 1, 0},
    /*16*/    {7, 1, 0, 0, 0, 0, 0, 1, 0, 2, 0, 2, 2, 2, 0, 2, 0, 0, 0, 1, 0, 0, 1, 7},
    /*17*/    {0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 1, 0},
    /*18*/    {0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0},
    /*19*/    {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
    /*20*/    {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
    /*21*/    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
    /*22*/    {0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    /*23*/    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    /*24*/    {0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    /*25*/    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0},
    /*26*/    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
    /*27*/    {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    /*28*/    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*29*/    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    /*30*/    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    /*31*/    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
    };

void clear_pacman(int pos_x,int pos_y){
	int i,j;
	for(i=0;i<SCALE;i++){
		for(j=0;j<SCALE;j++){
			LCD_SetPoint(pos_y*SCALE+i,pos_x*SCALE+j,White);
		}
	}
		
	if(matrice[pos_x][pos_y]!=2&&matrice[pos_x][pos_y]!=7)
		matrice[pos_x][pos_y]=1;
}
void draw_heart(int pos_x,int pos_y){
	int heart[SCALE][SCALE]={
    {0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,1,1,0},
    {1,1,1,1,0,0,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,0},
    {0,0,1,1,1,1,1,1,0,0},
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0}
};

		int i,j;
	for(i=0;i<SCALE;i++){
		for(j=0;j<SCALE;j++){
			if(heart[i][j]==1){
				LCD_SetPoint(pos_y*SCALE+j,pos_x*SCALE+i,Red);
			}
		}
	}
	
}
void draw_pacman(int pos_x,int pos_y){
	if(matrice[pos_x][pos_y]==10)								//CALCOLO IL PUNTEGGIO IN REAL TIME
		punteggio=punteggio+10;
	else if(matrice[pos_x][pos_y]==11)
		punteggio=punteggio+50;
	sprintf(punti_testo, "%d", punteggio);    	//STAMPO IL PUNTEGGIO
	GUI_Text(40,11,(uint8_t *)punti_testo,White,Black);
	if(punteggio==1000){												//STAMPO IL NUMERO DI VITE
		vite=2;
		sprintf(n_vite_text, "%d", vite);
		GUI_Text(200,297,(uint8_t *)n_vite_text,White,Black);
	}
	else if(punteggio==2000){
		vite=3;
		sprintf(n_vite_text, "%d", vite);
		GUI_Text(200,297,(uint8_t *)n_vite_text,White,Black);
	}
	else if(punteggio==3000){
		vite=4;
		sprintf(n_vite_text, "%d", vite);
		GUI_Text(200,297,(uint8_t *)n_vite_text,White,Black);
	}
	int pacman[SCALE][SCALE]={
        {0, 0, 0, 2, 2, 2, 2, 0, 0, 0},
        {0, 0, 2, 1, 1, 1, 1, 2, 0, 0},
        {0, 2, 1, 1, 1, 1, 1, 1, 2, 0},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {2, 1, 1, 1, 1, 1, 1, 1, 0, 2},
        {0, 2, 1, 1, 1, 1, 1, 1, 2, 0},
        {0, 0, 2, 1, 1, 1, 1, 2, 0, 0},
        {0, 0, 0, 2, 2, 2, 2, 0, 0, 0}
    };
	pos_pac_x=pos_x;
	pos_pac_y=pos_y;
	int i,j;
	for(i=0;i<SCALE;i++){
		for(j=0;j<SCALE;j++){
			if(pacman[i][j]==1){
				LCD_SetPoint(pos_y*SCALE+i,pos_x*SCALE+j,Yellow);
			}
			else if(pacman[i][j]==2){
				LCD_SetPoint(pos_y*SCALE+i,pos_x*SCALE+j,Green);
			}
		}
	}
	
	return;
}
void disegna_tutto(int part_x,int part_y,int arr_x,int arr_y){ //5 7 9 14
	int i,j,n,k;
	for(i=part_x;i<arr_x;i++){					//disegna le mura di blu e il background in nero
			for(j=part_y;j<arr_y;j++){
				if(matrice[i][j]==0){
					if(i==15&&j==12)
						for(n=0;n<SCALE;n++){
							for(k=0;k<SCALE;k++){
								LCD_SetPoint(n+j*SCALE,k+i*SCALE,White);
							}
						}
					else{//EVITO DI FAR DISEGNARE IL MURO IN CORRISPONDENZA DELLA PORTICINA
						for(n=0;n<SCALE;n++){
							for(k=0;k<SCALE;k++){
								LCD_SetPoint(n+j*SCALE,k+i*SCALE,Blue);
							}
						}
					}
				}
				if(matrice[i][j]==2){
					for(n=0;n<SCALE;n++){
						for(k=0;k<SCALE;k++){
							LCD_SetPoint(n+j*SCALE,k+i*SCALE,White);
						}
					}
				}	
				if(matrice[i][j]==-1){
					for(n=0;n<SCALE;n++){
						for(k=0;k<SCALE;k++){
							LCD_SetPoint(n+j*SCALE,k+i*SCALE,Black);
						}
					}
				}
					
			}
		}
		for(i=0;i<SCALE;i++){						//PORTICINA
			for(j=0;j<SCALE;j++){
				if(i>2&&i<7)
					LCD_SetPoint(j+coordinata_y_porta*SCALE,i+coordinata_x_porta*SCALE,Magenta);
				else
					LCD_SetPoint(j+coordinata_y_porta*SCALE,i+coordinata_x_porta*SCALE,White);
			}
		}
    return;	
}

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	srand(time(NULL));		//genera un seed casuale ad ogni lancio
	LCD_Initialization();
	LCD_Clear(White);
  int i,j,n,k;
	disegna_tutto(0,0,N_RIG,N_COL);		
		//INSERIMENTO 240 PILLS BASE
		for(i=4;i<=27;i++){ //ESCLUDO BLOCCHI SUPERIORE E INFERIORE
			for(j=1;j<=22;j++){
				if(matrice[i][j]==1&&palline_s_inserite<MAX_BALLS){
					matrice[i][j]=10;
						for(n=0;n<2;n++){
							for(k=0;k<2;k++){
								LCD_SetPoint(n+j*SCALE+4,k+i*SCALE+4,Black);   
								
								//RAGIONAMENTO IN QUESTO MODO 
								// OGNI ELEMENTO 1,0,-1,2 corrisponde in realtà a 10x10 pixel
								//LE PALLINE SEMPLICI OCCUANO 2x2 pixel al centro invece quelle potenziate occupano 4x4 pixel dal centro, per questo motivo quando ho fatto i 2 for precedenti ho usato for(i=0;i<n...) 
							}
						}
					palline_s_inserite++;
				}
			}
		}
		rand_time=45 + rand() % (60 - 45 + 1);
		//INSERIMENTO 6 PILLS POTENZIATE
				//COme ragiono? Genero 6 numeri casuali uno per volta, dopo aver generato il valore compreso tra 0 e il numero di palline standard inserite (in questo caso 240)
																												//applico 2 cicli for annidati che quando intercettano una pallina (quindi un 1) aggiornano il contatore_posizione. Se il contatore posizone è uguale al random
																												//allora coloro i 4 pixel centrali di un determinato punto
		
/*
	PALLINA STANDARD					PALLINA POTENZIATA
X X X X X X X X X X					X X X X X X X X X X              
X X X X X X X X X X					X X X X X X X X X X 
X X X X X X X X X X					X X X X X X X X X X 
X X X X X X X X X X					X X X * * * * X X X 
X X X X * * X X X X					X X X * * * * X X X 
X X X X * * X X X X					X X X * * * * X X X 
X X X X X X X X X X					X X X * * * * X X X 
X X X X X X X X X X					X X X X X X X X X X 
X X X X X X X X X X					X X X X X X X X X X 
X X X X X X X X X X					X X X X X X X X X X 
*/		
	
	pos_x=18,pos_y=12;
	draw_pacman(pos_x,pos_y);   //SETTIAMO IL PACMAN IN POSIZIONE INIZIALE	
	/*
	PACMAN
X X X * * * * X X X            
X X * * * * * * X X 
X * * * * * *	* * X
* * * * * * * * * * 
* * * * * * * * * *
* * * * * * * * * *
* * * * * * * * * *
X * * * * * * * * X
X X * * * * * * X X
X X X * * * * X X X
*/	
	
	//DISEGNA CUORE
	draw_heart(30,18);
	GUI_Text(192,297,(uint8_t *)x,White,Black);	
	GUI_Text(200,297,(uint8_t *)"1",White,Black);
  //TP_Init();
	//TouchPanel_Calibrate();
	GUI_Text(15,11,(uint8_t *)base_pt,White,Black); 
	draw_blinky(pos_blinky_x,pos_blinky_y,1);
	//GUI_Text(0, 280, (uint8_t *) " touch here : 1 sec to clear  ", Red, White);
	joystick_init();
	BUTTON_init();
	matrice[15][12]=2;
	init_RIT(0xF4240);//10ms
	enable_RIT();
	init_timer(0, 0X2DC6C0);// 30ms
	enable_timer(0);
	init_timer(1, 0x7F2815);// 0.34
	enable_timer(1);
	
	ADC_init();
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);			
	/* TIMERS (2-3) POWER UP*/
	LPC_SC -> PCONP |= (1<<20); 
  LPC_SC -> PCONP |= (1<<21);
  LPC_SC -> PCONP |= (1<<22);
  LPC_SC -> PCONP |= (1<<23);	
		
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);

	//init_timer(0, 0x1312D0 ); 						/* 50ms * 25MHz = 1.25*10^6 = 0x1312D0 */
	//init_timer(0, 0x6108 ); 						  /* 1ms * 25MHz = 25*10^3 = 0x6108 */
	//init_timer(0, 0x4E2 ); 						    /* 500us * 25MHz = 1.25*10^3 = 0x4E2 */
	//init_timer(0, 0xC8); 						    /* 8us * 25MHz = 200 ~= 0xC8 */
	//init_timer(0,0x00989680);
	//enable_timer(0);
	
	//LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	//LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
