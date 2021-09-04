//=========================================================================
// testMag.h
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// 
// Author:      David Witten, KD0EAG
// Date:        April 21, 2020
// License:     GPL 3.0
//=========================================================================
#ifndef SWX3100RUNMag_h
#define SWX3100RUNMag_h

#include "main.h"

//------------------------------------------
// Prototypes
//------------------------------------------
int openI2CBus(pList *p);
void closeI2CBus(int i2c_fd);
int getMagRev(pList *p);
int setup_mag(pList *p);
int runBIST(pList *p);

#endif // SWX3100RUNMag_h