/*
 * sevenSeg.h
 *
 * Created: 31-3-2018 14:08:07
 *  Author: Rick
 */ 


#ifndef SEVENSEG_H_
#define SEVENSEG_H_
#define DisplaySetAddress 0xC0

void sevenSegInit(void);
void sevenSegCleanup(void);
void sevenSegStart(void);
void sevenSegStop(void);
void sevenSegWriteByte(uint8_t byteval);
void sevenSegCmd(uint8_t byteCmd);
void sevenSegConf(void);
void showDigit(int);
void sevenSegDisplaySegs(uint8_t segAddress, uint8_t value);
void wait(int us);



#endif /* SEVENSEG_H_ */