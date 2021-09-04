//=========================================================================
// testMag.c
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// Derived in part from several sources:
//      https://github.com/miguelrasteiro/RM3100.X
//      Jeremiah Mattison / Rm3100: https://os.mbed.com/users/fwrawx/code/Rm3100/
//      https://github.com/shannon-jia/rm3100
//      Song Qiang <songqiang1304521@gmail.com (Linux driver):
//          Linux kernel driver: https://github.com/torvalds/linux/tree/v5.3/drivers/iio/magnetometer
// 
// Author:      David Witten, KD0EAG
// Date:        April 21, 2020
// License:     GPL 3.0
//=========================================================================
#include "i2c.h"
#include "main.h"
#include "testMag.h"

//------------------------------------------
// openI2CBus()
//------------------------------------------
int openI2CBus(pList *p)
{
    p->i2c_fd = -1;
    
    char pathStr[64] = "";
    snprintf(pathStr, sizeof(pathStr), "/dev/i2c-%i", p->i2cBusNumber);
    
    if((p->i2c_fd = open(pathStr, O_RDWR)) < 0)
    {
        perror("Bus open failed\n");
        return -1;
    }
    else
    {
        if(p->verboseFlag)
        {
            fprintf(stdout, "Device handle p->i2c_fd:  %d\n", p->i2c_fd);
            fprintf(stdout, "i2c_init OK!\n");
            fflush(stdout);
        }
    }
    return p->i2c_fd;
}

//--------------------------------------------------------------------
// closeI2CBus()
//
// Close I2C bus
//--------------------------------------------------------------------
void closeI2CBus(int i2c_fd)
{
    close(i2c_fd);
}

//------------------------------------------
// getMagSampleRate();
// The actual sample rate of the sensor.
//------------------------------------------
unsigned short getMagSampleRate(pList *p)
{
    return p->CMMSampleRate;
}

//------------------------------------------
// getMagRev(pList *p)
//------------------------------------------
int getMagRev(pList *p)
{
    // Set address of the RM3100
    i2c_setAddress(p->i2c_fd,  p->magnetometerAddr);

    // Check Version
    if((p->magRevId = i2c_read(p->i2c_fd, RM3100I2C_REVID)) != (uint8_t)RM3100_VER_EXPECTED)
    {
        // Fail, exit...
        fprintf(stderr, "\nRM3100 REVID NOT CORRECT: ");
        fprintf(stderr, "RM3100 REVID: 0x%X <> EXPECTED: 0x%X.\n\n", p->magRevId, RM3100_VER_EXPECTED);
        fflush(stdout);
        return 0;
    }
    else
    {
        if(p->verboseFlag)
        {
             fprintf(stdout,"RM3100 Detected Properly: ");
             fprintf(stdout,"REVID: %x.\n", p->magRevId);
        }
    }
    return p->magRevId;
}

//------------------------------------------
// setup_mag()
//------------------------------------------
int setup_mag(pList *p)
{
    int rv = SensorOK;

    // Set address of the RM3100
    i2c_setAddress(p->i2c_fd,  p->magnetometerAddr);
    // Check Version
    if(!getMagRev(p))
    {
        exit (1);
    }
    // Setup the NOS register
    // setNOSReg(p);
    // Clear out these registers
    i2c_write(p->i2c_fd, RM3100_MAG_POLL, 0);
    i2c_write(p->i2c_fd, RM3100I2C_CMM,  0);
    // Initialize CC settings
    setCycleCountRegs(p);
    // Sleep for 1 second
    usleep(100000);                           // delay to help monitor DRDY pin on eval board
    return rv;
}

//------------------------------------------
// runBIST()
// Runs the Built In Self Test.
//------------------------------------------
int runBIST(pList *p)
{
    return 0;
    //return i2c_read(p->i2c_fd, RM3100I2C_TMRC);
}

