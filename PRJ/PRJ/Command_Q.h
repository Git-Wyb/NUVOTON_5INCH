#ifndef COMMAND_Q_H
#define COMMAND_Q_H

#include <stdint.h>

#define N2S_Q0			0
#define N2S_Q1			1


int CpyNandToSdramCMd(char cmd, uint16_t *fileName,int cnt);

#endif

