#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "led//led.h"
#include "Blinky.h"
#include <stdio.h>
#include <limits.h>

#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // Per rand() e srand()
#include <time.h>   // Per time()
#define N_RIG 32
#define N_COL 24
#define INF 1000
#define SCALE 10
extern int matrice[N_RIG][N_COL];
extern int pos_x;
extern int non_puoi;
extern int pos_y;
extern int pos_blinky_x;
extern int conta;
extern int pos_blinky_y;
extern int punteggio;

extern int flag;
extern volatile int condizione;
void pulisci_punteggio(){
	int i,j;
	for(i=0;i<16;i++){
		for(j=0;j<35;j++){
				LCD_SetPoint(39+j,11+i,Black);
		}
	}
}
void delete_blinky(int x,int y){
	int i,j;
	if(matrice[x][y]==2||matrice[x][y]==1||matrice[x][y]==7)
	{
		for(i=0;i<SCALE;i++){  //Se il fantasma trova un posto percorribile senza pills
			for(j=0;j<SCALE;j++){
				LCD_SetPoint(y*SCALE+j,x*SCALE+i,White);
			}
		}
	}
	else if(matrice[x][y]==10){
		for(i=0;i<SCALE;i++){  //standard pills
			for(j=0;j<SCALE;j++){
				if(i>=4&&i<=5&&j>=4&&j<=5)
					LCD_SetPoint(y*SCALE+j,x*SCALE+i,Black);
				else
					LCD_SetPoint(y*SCALE+j,x*SCALE+i,White);
			}
		}
	}
	else if(matrice[x][y]==11){
		for(i=0;i<SCALE;i++){  //power pills
			for(j=0;j<SCALE;j++){
				if(i>=3&&i<=6&&j>=3&&j<=6)
					LCD_SetPoint(y*SCALE+j,x*SCALE+i,Red);
				else
					LCD_SetPoint(y*SCALE+j,x*SCALE+i,White);
			}
		}
	}
	return;
		
}


void draw_blinky(int pos_x,int pos_y,int condizione){
	int Blinky[SCALE][SCALE]={
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,1,0},
    {1,1,0,0,1,1,0,0,1,1},
    {1,1,0,2,1,1,0,2,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,1,0,1,1,0,1,0,1}
};

	int i,j;
	for(i=0;i<SCALE;i++){
		for(j=0;j<SCALE;j++){
			if(Blinky[i][j]==1){
				LCD_SetPoint(pos_y*SCALE+j,pos_x*SCALE+i,condizione==1?Red:Blue);
			}
			else if(Blinky[i][j]==2){
				LCD_SetPoint(pos_y*SCALE+j,pos_x*SCALE+i,Black);
			}
		}
	}
}


void inizializza_mappa_nodi(){
	int i,j;
	for ( i = 0; i < N_RIG; i++) {
        for ( j = 0; j < N_COL; j++) {
            // Nodo con valore infinito per ogni posizione
            open_list[i * N_COL + j].g = INF;
            open_list[i * N_COL + j].h = INF;
            open_list[i * N_COL + j].f = INF;
            open_list[i * N_COL + j].parentX = -1;
            open_list[i * N_COL + j].parentY = -1;
        }
    }
}
void allontanamento(){
	int i;
	int min=0,h;
	if(conta%2==0){
	for ( i = 0; i < 4; i++) {
            int newX = pos_blinky_x+ directions[i][0];
            int newY = pos_blinky_y+ directions[i][1];
            // Verifica se il nuovo nodo è dentro i limiti e percorribile
						int j=0;
            if (matrice[newX][newY] != 0) {
							h= abs(newX - pos_x) + abs(newY - pos_y);
							if(h>min){
								min=h;
								def_x=newX;
								def_y=newY;
							}
						}
					}
	delete_blinky(pos_blinky_x, pos_blinky_y);
	pos_blinky_x=def_x;
	pos_blinky_y=def_y;
  draw_blinky(def_x, def_y,condizione);}
}
void A_star() {
    // Direzioni possibili (su, giù, sinistra, destra)
	  int i,j;
    // Crea i nodi per il percorso A*
    int temp;
    open_size = 0;
    closed_size = 0;
    inizializza_mappa_nodi();
    // Nodo di partenza (Blinky)
    open_list[open_size++] = (Node){pos_blinky_x, pos_blinky_y, 0, abs(pos_blinky_x - pos_x) + abs(pos_blinky_y - pos_y), abs(pos_blinky_x - pos_x) + abs(pos_blinky_y - pos_y), -1, -1};
    // Nodo di destinazione (Pacman)
    while (open_size > 0) {
        // Trova il nodo con il valore f più basso
        int minF = INF;
        int currentIndex = -1;
				//TIENE CONTO DELLA DISTANZA
        for ( i = 0; i < open_size; i++) {
            if (open_list[i].f < minF) {
                minF = open_list[i].f;
                currentIndex = i;
            }
        }
        // Prendi il nodo corrente e rimuovilo dalla lista aperta
        current = open_list[currentIndex];
        open_list[currentIndex] = open_list[--open_size];
        // Se siamo arrivati al target, ricostruiamo il percorso
        if (current.x == pos_x && current.y == pos_y) {
            int tempX = current.x;
            int tempY = current.y;
            while (!(current.parentX == pos_blinky_x && current.parentY == pos_blinky_y)) {
                tempX = current.parentX;
                tempY = current.parentY;
								for(i=closed_size-1;i>=0;i--){
									if(closed_list[i].x==tempX&&closed_list[i].y==tempY){
										current = closed_list[i];
									}
								}
            }
            // Movimento successivo di Blinky
            pos_blinky_x = tempX;
            pos_blinky_y = tempY;
            // Disegna Blinky nel nuovo posto
            delete_blinky(current.parentX, current.parentY);
						if(pos_x==pos_blinky_x&&pos_y==pos_blinky_y)
							non_puoi=1;
						if(non_puoi==0)
							draw_blinky(pos_blinky_x, pos_blinky_y,condizione);
            return;
        }
        // Aggiungi il nodo corrente alla lista chiusa
        closed_list[closed_size++] = current;
        // Esplora i nodi vicini
        for ( i = 0; i < 4; i++) {
            int newX = current.x + directions[i][0];
            int newY = current.y + directions[i][1];
            // Verifica se il nuovo nodo è dentro i limiti e percorribile
            if (matrice[newX][newY] != 0) {
                // Calcola il costo g, h e f
                gNew = current.g + 1;
                hNew = abs(newX - pos_x) + abs(newY - pos_y);
                fNew = gNew + hNew;
                // Se il nodo non è in lista chiusa o ha un costo minore, aggiornalo
                inClosed = false;
                for ( j = 0; j < closed_size; j++) {
                    if (closed_list[j].x == newX && closed_list[j].y == newY) {
                        inClosed = true;
                        break;
                    }
                }
                if (!inClosed) {
                    open_list[open_size++] = (Node){newX, newY, gNew, hNew, fNew, current.x, current.y};
                }
            }
        }
    }
}

void Blinky(){
	 if(flag==1||flag==0){
		 matrice[15][12]=2;
		 delete_blinky(pos_blinky_x,pos_blinky_y);
		 pos_blinky_x-=1;
		 draw_blinky(pos_blinky_x,pos_blinky_y,condizione);
		 
		 
	 }
	 else{
		 matrice[15][12]=0;//non permetto l'accesso alla porta
		 if(condizione==1)
			A_star();
		 else{
				allontanamento();
		 }
		 
	 }
	 return;
}
