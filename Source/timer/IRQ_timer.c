/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "LPC17xx.h"
#include "timer.h"
#include "RIT/RIT.h"
#include "../GLCD/GLCD.h" 
#include <stdio.h> /*for sprintf*/

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint16_t SinTable[45] =
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};
// beat 1/4 = 1.65/4 seconds
#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1


//SHORTENING UNDERTALE: TOO MANY REPETITIONS
NOTE song[] = 
{
	// 1
	{d3, time_semicroma},
	{d3, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{pause, time_semicroma},
	{a3b, time_semicroma},
	{pause, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 2
	{c3, time_semicroma},
	{c3, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{pause, time_semicroma},
	{a3b, time_semicroma},
	{pause, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 3
	{c3b, time_semicroma},
	{c3b, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{pause, time_semicroma},
	{a3b, time_semicroma},
	{pause, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 4
	{a2b, time_semicroma},
	{a2b, time_semicroma},
	{d4, time_croma},
	{a3, time_croma},
	{pause, time_semicroma},
	{a3b, time_semicroma},
	{pause, time_semicroma},
	{g3, time_croma},
	{f3, time_semicroma*2},
	{d3, time_semicroma},
	{f3, time_semicroma},
	{g3, time_semicroma},
	// 5
	
};
NOTE pacman_intro[] = {
    {e2,time_semicroma}, {e3,time_semicroma}, {b2,time_semicroma}, {a2b,time_semicroma}, {e3,time_semicroma}, {b2,time_semicroma},{pause,time_biscroma},
		{f2,time_semicroma}, {f3,time_semicroma}, {c3,time_semicroma}, {a2,time_semicroma}, {f3,time_semicroma}, {c3,time_semicroma},{pause,time_biscroma},
		{e2,time_semicroma}, {e3,time_semicroma}, {b2,time_semicroma}, {a2b,time_semicroma}, {e3,time_semicroma}, {b2,time_semicroma},{pause,time_biscroma},
		{a2b,time_biscroma}, {a2,time_biscroma}, {bb2,time_biscroma},
		{bb2,time_biscroma}, {b2,time_biscroma}, {c3,time_biscroma},
		{c3,time_biscroma}, {db3,time_biscroma}, {d3,time_biscroma},
		{e3,time_biscroma}
};
NOTE fine[] ={
		{d2,time_semibreve}
};
NOTE win[] ={
		{b3,time_semibreve}
};
NOTE gioco[] ={
		{e2,time_semibiscroma}
};
NOTE standard_pills[] ={
		{e3,time_semibiscroma}
};
NOTE special_pillss[] ={
		{e4,time_semibiscroma}
};
extern int dir;
extern int palline_s_inserite;
extern int pos_x;
extern int pos_y;
extern int next_dir;
extern int pausa;
int flag=0;
extern int punteggio;
int flag_inseguimento=0;
int first_move=0;
#define N_RIG 32
#define N_COL 24
extern int matrice[N_RIG][N_COL];
extern int pos_blinky_x;
int intro_index;
volatile int conta;
volatile int condizione=1;
extern char n_vite_text[2];
extern int vite;
int non_puoi=0;
extern NOTE song[];
extern int punteggio_1;
int flag_fine_outro=0;
int flag_attesa_fantasma=0;
int tempo_attesa_fantasma=0;
int n_fantasmi_mangiati=0;
extern int pos_blinky_y;
void TIMER0_IRQHandler (void)
{
	static int outro_index=0;
	if(pausa==0){											//se non siamo in pausa
  			/* clear interrupt flag */
	
	switch (next_dir) {
        case 0: // DOWN
            if (matrice[pos_x + 1][pos_y] != 0) {
                dir = next_dir; // Cambia direzione
            }
            break;
        case 1: // LEFT
            if (matrice[pos_x][pos_y - 1] != 0) {
                dir = next_dir; // Cambia direzione
            }
            break;
        case 2: // RIGHT
            if (matrice[pos_x][pos_y + 1] != 0) {
                dir = next_dir; // Cambia direzione
            }
            break;
        case 3: // UP
            if (matrice[pos_x - 1][pos_y] != 0) {
                dir = next_dir; // Cambia direzione
            }
            break;
    }
	switch(dir){
		case 0:			//DOWN
			if(matrice[pos_x+1][pos_y]!=0){
							clear_pacman(pos_x,pos_y);
							if((pos_x == pos_blinky_x) && (pos_y == pos_blinky_y)&&condizione==1)
								non_puoi=1;
							pos_x=pos_x+1;
							
							draw_pacman(pos_x,pos_y);}
			first_move=1;
			break;
		case 1:			//LEFT
			if(matrice[pos_x][pos_y-1]!=0){
				if(matrice[pos_x][pos_y-1]!=7){  //gestisco il caso del corridoio portale
							clear_pacman(pos_x,pos_y);
							if((pos_x == pos_blinky_x) && (pos_y == pos_blinky_y)&&condizione==1)
								non_puoi=1;
							pos_y=pos_y-1;
						
							draw_pacman(pos_x,pos_y);
				}
				else{
							clear_pacman(pos_x,pos_y);
							pos_y=23;
							draw_pacman(pos_x,pos_y);
				}
			}
			first_move=1;			
			break;
		case 2:			//RIGHT
				if(matrice[pos_x][pos_y+1]!=0){
					if(matrice[pos_x][pos_y+1]!=7){ //gestisco il caso del corridoio portale
							clear_pacman(pos_x,pos_y);
							if((pos_x == pos_blinky_x) && (pos_y == pos_blinky_y)&&condizione==1)
								non_puoi=1;
							pos_y=pos_y+1;
						
							draw_pacman(pos_x,pos_y);
					}
						else{
							clear_pacman(pos_x,pos_y);
							pos_y=0;
							draw_pacman(pos_x,pos_y);
					}
				}
				first_move=1;
			break;
		case 3:			//UP
				if(matrice[pos_x-1][pos_y]!=0){
							clear_pacman(pos_x,pos_y);
							if((pos_x == pos_blinky_x) && (pos_y == pos_blinky_y)&&condizione==1)
								non_puoi=1;
							pos_x=pos_x-1;
							
							//RICHIAMA AI
							draw_pacman(pos_x,pos_y);}
				first_move=1;
			break;
						}
		if(first_move==0){  //musichetta iniziale
			if(!isNotePlaying()){
				if(intro_index<sizeof(pacman_intro)/sizeof(pacman_intro[0])){
					playNote(pacman_intro[intro_index]);
					intro_index++;
				}else{
				intro_index=0;
				}
			}
		}
		// QUI RICHIAMO LA FUNZIONE CHE MUOVE FANTASMA
		if(first_move!=0){
					
				int i,j;
			if(flag_inseguimento!=1)
				playNote(gioco[0]);
			if(pos_blinky_x==pos_x&&pos_blinky_y==pos_y && (condizione == 1)){
				if(vite==1){
					clear_pacman(pos_x,pos_y);
					delete_blinky(pos_blinky_x,pos_blinky_y);
					
					GUI_Text(200,297,(uint8_t *)"0",White,Black);
					pulisci_punteggio();
					for(i=0;i<50;i++){					
							for(j=0;j<70;j++){
								LCD_SetPoint(j+90,i+140,Black);
							}
						}
					GUI_Text(110,145,(uint8_t *)"GAME",White,Black);
					GUI_Text(110,172,(uint8_t *)"OVER",White,Black);
				
							playNote(fine[0]);
			
					disable_timer(0);
					disable_RIT();
					disable_timer(1);
					//disable_timer(2);
				}
				if(vite>1){
					vite--;
					sprintf(n_vite_text, "%d", vite);
					GUI_Text(200,297,(uint8_t *)n_vite_text,White,Black);
					delete_blinky(pos_blinky_x,pos_blinky_y);
					clear_pacman(pos_x,pos_y);
					pos_blinky_x=16,pos_blinky_y=12;
					pos_x=18,pos_y=12;
					draw_blinky(pos_blinky_x,pos_blinky_y,condizione);
					draw_pacman(pos_x,pos_y);
					flag=0;
					non_puoi=0;
				}
			}
			
			else if(pos_blinky_x==pos_x&&pos_blinky_y==pos_y&&condizione==0&&flag_attesa_fantasma==0){
				n_fantasmi_mangiati++;
				punteggio+=100;
				if(punteggio>=1000&&punteggio<2000)
					vite=2;
				else if(punteggio>=2000&&punteggio<3000)
					vite=3;
				else if(punteggio>=3000)
					vite=4;
				sprintf(n_vite_text, "%d", vite);
				GUI_Text(200,297,(uint8_t *)n_vite_text,White,Black);
				delete_blinky(pos_blinky_x,pos_blinky_y);
					pos_blinky_x=16,pos_blinky_y=12;
					condizione=1;
					flag=0;
					draw_blinky(pos_blinky_x,pos_blinky_y,condizione);
					flag_attesa_fantasma=1;
					non_puoi=0;
				
			}
			if(flag_attesa_fantasma==1)
				tempo_attesa_fantasma++;
			if(tempo_attesa_fantasma==8){
				flag_attesa_fantasma=0;
				tempo_attesa_fantasma=0;
				flag=0;
			}
			if(matrice[pos_x][pos_y]==10&&flag_inseguimento!=1){
						playNote(standard_pills[0]);
			}
			if(matrice[pos_x][pos_y]==11){
						playNote(special_pillss[0]);
								if(pos_blinky_x==16&&pos_blinky_y==12)
									flag=0;
								if(pos_blinky_x==15&&pos_blinky_y==12)
									flag=1;
								condizione=0;
								conta=0;
								flag_inseguimento=1;
							}
				if(flag_inseguimento==1){
					playNote(special_pillss[0]);
					if(conta==20){
									flag_inseguimento=0;
									condizione=1;
									conta=0;
								}
					conta++;
				}
				if(condizione==1&&flag_attesa_fantasma==0){
					Blinky();
				}
				else if(condizione==0)
					Blinky();
		
			flag++;
		}
		if((punteggio-n_fantasmi_mangiati*100)==2640){
			//STAMPA VICTORY
			int i,j;
			disable_timer(0);
			disable_timer(1);
			disable_RIT();
			for(i=0;i<50;i++){					
							for(j=0;j<70;j++){
								LCD_SetPoint(j+90,i+140,Black);
							}
						}
			GUI_Text(98,158,(uint8_t *)"VICTORY",White,Black);
			playNote(win[0]);
		}
	}
	else{
		disable_timer(1); //blocca countdown quando sei in pausa
	}
	
	LPC_TIM0->IR = 1;
  return;
}
	


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern int rand_time;
void TIMER1_IRQHandler (void)
{
	static int outro_index=0;
	
	int random_number;
	volatile int min=0,max=palline_s_inserite,contatore_posizione=0,p;	
	int i,j,n,k;
	char time_txt[3];
	char tmp[1];
	static int time=60;
	if(first_move==1){
		if(rand_time==time){
				for(p=0;p<6;p++){
					random_number =min + rand() % (max - min + 1);
					
					for(i=4;i<27;i++){
						for(j=1;j<22;j++){
							if(matrice[i][j]==10){
								if(contatore_posizione==random_number){
									matrice[i][j]=11;
								for(n=0;n<4;n++){
									for(k=0;k<4;k++){
										LCD_SetPoint(n+j*10+3,k+i*10+3,Red);
									}
								}
							}
								contatore_posizione++;
							}
						}
					}
					contatore_posizione=0;
				}
			}
		if(time>=0){
			if(time>9){
				sprintf(time_txt,"%d",time);
				
				GUI_Text(180,10,(uint8_t *)time_txt,White,Black);
			}
			else{
				sprintf(time_txt,"%ds",time);
				GUI_Text(180,10,(uint8_t *)time_txt,White,Black);
			}
			time--;
		}
		else{//partita finita
			//STAMPA GAME OVER
			
			
			for(i=0;i<50;i++){					
							for(j=0;j<70;j++){
								LCD_SetPoint(j+90,i+140,Black);
							}
						}
			GUI_Text(110,145,(uint8_t *)"GAME",White,Black);
			GUI_Text(110,172,(uint8_t *)"OVER",White,Black);
						playNote(fine[0]);
			
					disable_timer(0);
					disable_RIT();
					disable_timer(1);
		}
	}		
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}


void TIMER2_IRQHandler (void)
{
	if(LPC_TIM2->IR & 1){
		static int sineticks=0;
	/* DAC management */	
	static int currentValue; 
	currentValue = SinTable[sineticks];
	currentValue -= 410;
	currentValue /= 1;
	currentValue += 410;
	LPC_DAC->DACR = currentValue <<6;
	sineticks++;
	if(sineticks==45) sineticks=0;
		
		}
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void)
{
	disable_timer(2);
		LPC_TIM3->IR = 1;			/* clear interrupt flag */
	
	
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
