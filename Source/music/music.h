#ifndef MUSIC_H
#define MUSIC_H


//Default: 1.65
#define SPEEDUP 2

#define TIMERSCALER 1
#define TIMERSCALER_1 0.3
#define SECOND 0x17D7840 * TIMERSCALER_1


typedef char BOOL;
#define TRUE 1
#define FALSE 0

typedef enum note_durations
{
	time_semibiscroma = (unsigned int)(SECOND * SPEEDUP / 64.0f + 0.5), // 1/128
	time_biscroma = (unsigned int)(SECOND * SPEEDUP / 32.0f + 0.5), // 1/64
	time_semicroma = (unsigned int)(SECOND * SPEEDUP / 16.0f + 0.5), // 1/32
	time_croma = (unsigned int)(SECOND * SPEEDUP / 8.0f + 0.5), // 1/16
	time_semiminima = (unsigned int)(SECOND * SPEEDUP / 4.0f + 0.5), // 1/4
	time_minima = (unsigned int)(SECOND * SPEEDUP / 2.0f + 0.5), // 1/2
	time_semibreve = (unsigned int)(SECOND * SPEEDUP + 0.5), // 1
} NOTE_DURATION;

typedef enum frequencies
{
	c2 = 8417, 			// 66Hz do1
	d2 = 7507,			//74        re1
	e2=  6693,      //83        mi1
	f2=6242,					//89				fa1
	g2=5669,					//98        sol1
	a2=5050,					//110       la1
	a2b = 5351,	// 103Hz	k=5351 a2b
	b2 = 4500,	// 123Hz	k=4500 b2
	c3b = 4370,	// 127Hz	k)4370 c3b
	c3 = 4240,	// 131Hz	k=4240 c3
	d3 = 3779,	// 147Hz	k=3779 d3
	e3 = 3367,	// 165Hz	k=3367 e3
	f3 = 3175,	// 175Hz	k=3175 f3
	g3 = 2834,	// 196Hz	k=2834 g3
	a3b = 2670, // 208Hz  k=2670 a4b
	a3 = 2525,	// 220Hz	k=2525 a3
	b3 = 2249,	// 247Hz	k=2249 b3
	c4 = 2120,	// 262Hz	k=2120 c4
	d4 = 1890,	// 294Hz	k=1890 d4
	e4 = 1684,	// 330Hz	k=1684 e4
	f4 = 1592,	// 349Hz	k=1592 f4
	g4 = 1417,	// 392Hz	k=1417 g4
	a4 = 1263,	// 440Hz	k=1263 a4
	b4 = 1125,	// 494Hz	k=1125 b4
	c5 = 1062, 	// 523Hz	k=1062 c5
	b5 = 562,  // 988Hz
	fs5 = 750, // 740
	ds5 = 893, // 622
	c6 = 531,  // 1046
	g6 = 354,  // 1568
	e6 = 421,  // 1319
	e5 = 843,   // 659
	f5 = 795,   // 698
	g5 = 708,   // 784
	gs5 = 668,  // 831
	a5 = 631,   // 880
	pause = 0,		// DO NOT SOUND
	bb2=4748,
	db3=3997
} FREQUENCY;


typedef struct 
{
	FREQUENCY freq;
	NOTE_DURATION duration;
} NOTE;

void playNote(NOTE note);
BOOL isNotePlaying(void);

#endif
/* EOF */
