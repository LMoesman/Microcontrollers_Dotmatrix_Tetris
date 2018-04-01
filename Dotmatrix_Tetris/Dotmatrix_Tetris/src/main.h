/*
 * main.h
 *
 * Created: 30-3-2018 14:39:56
 *  Author: Lars Moesman
 */ 


#ifndef MAIN_H_
#define MAIN_H_

void setupDisplayArray(unsigned char* displayBuffer);
void startGame(void);
void resetGame(void);
void animateGame(void);
void checkForFullRows(void);
void shoveDown(int x);
void gameOver(void);

#endif /* MAIN_H_ */