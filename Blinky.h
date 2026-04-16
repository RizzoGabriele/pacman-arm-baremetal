#ifndef __Blinky_H
#define __Blinky_H
#include <stdbool.h>
#define N_RIG 32
#define N_COL 24
typedef struct {
    int x, y;       // Coordinate del nodo
    int g, h, f;    // g = costo per arrivare a questo nodo, h = euristica (distanza da Pacman), f = g + h
    int parentX, parentY;  // Coordinate del nodo genitore
} Node;
int gNew,hNew,fNew;
bool inClosed;
int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
int open_size = 0;
int closed_size = 0;
int def_x,def_y;
Node open_list[N_RIG * N_COL];
Node closed_list[N_RIG * N_COL];
Node current;
#endif