//=========================================================================
// cmdmgr.c
//
// commandline / configuration management routines for testMag utility.
//
// Author:      David Witten, KD0EAG
// Date:        June 19, 2020
// License:     GPL 3.0
//=========================================================================
#include "main.h"
#include "cmdmgr.h"

extern char version[];
//extern char outputPipeName[MAXPATHBUFLEN];
//extern char inputPipeName[MAXPATHBUFLEN];
extern char outFilePath[MAXPATHBUFLEN];
extern char workFilePath[MAXPATHBUFLEN];
extern char rollOverTime[UTCBUFLEN];
extern char sitePrefixString[SITEPREFIXLEN];

//------------------------------------------
// currentTimeMillis()
//------------------------------------------
long currentTimeMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}


//------------------------------------------
// getUTC()
//------------------------------------------
struct tm *getUTC()
{
    time_t now = time(&now);
    if(now == -1)
    {
        puts("The time() function failed");
    }
    struct tm *ptm = gmtime(&now);
    if(ptm == NULL)
    {
        puts("The gmtime() function failed");
    }
    return ptm;
}

//------------------------------------------
//  buildLogFilePath()
//------------------------------------------
int buildLogFilePath(pList *p)
{
    int rv = 0;
    char utcStr[UTCBUFLEN] = "";
    struct tm *utcTime = getUTC();

    p->outputFilePath = workFilePath;
    strcpy(p->outputFilePath, outFilePath);

    const char ch = '/';

    if(p->outputFilePath[strlen(p->outputFilePath) - 1] != ch)
    {
        printf("Adding '/' to: %s\n", p->outputFilePath);
        strcat(p->outputFilePath, "/");
    }
    strcat(p->outputFilePath, p->sitePrefix);
    strcat(p->outputFilePath, utcStr);
    strcat(p->outputFilePath, "-");
    utcTime = getUTC();
    strftime(utcStr, UTCBUFLEN, "%Y%m%d-runmag.log", utcTime);        // RFC 2822: "%a, %d %b %Y %T %z"      RFC 822: "%a, %d %b %y %T %z"
    //strftime(utcStr, UTCBUFLEN, "%Y%m%d-%H%M-runmag.log", utcTime);        // RFC 2822: "%a, %d %b %Y %T %z"      RFC 822: "%a, %d %b %y %T %z"
    strcat(p->outputFilePath, utcStr);
    //printf("p->outputFilePath: %s\n", p->outputFilePath);
    return rv;
}

//------------------------------------------
// showSettings()
//------------------------------------------
void showSettings(pList *p)
{
    char pathStr[128] = "";
    snprintf(pathStr, sizeof(pathStr), "/dev/i2c-%i", p->i2cBusNumber);

    fprintf(stdout, "\nVersion = %s\n", version);
    fprintf(stdout, "\nCurrent Parameters:\n\n");
    if(!p->magRevId)
    {
        getMagRev(p);
    }
    fprintf(stdout, "   Magnetometer revision ID detected:          %i (dec)\n",    p->magRevId);
    fprintf(stdout, "   Log output path:                            %s\n",          p->buildLogPath ? "TRUE" : "FALSE");
    fprintf(stdout, "   Log output:                                 %s\n",          p->logOutput ? "TRUE" : "FALSE");
    //fprintf(stdout, "   Log Rollover time:                          %s\n",          p->logOutputTime);
    fprintf(stdout, "   Log site prefix string:                     %s\n",          p->sitePrefix);
    fprintf(stdout, "   Output file path:                           %s\n",          p->outputFilePath);
#if (USE_PIPES)
    fprintf(stdout, "   Log output to pipes:                        %s\n",          p->useOutputPipe ? "TRUE" : "FALSE");
    fprintf(stdout, "   Input file path:                            %s\n",          p->pipeInPath);
    fprintf(stdout, "   Output file path:                           %s\n",          p->pipeOutPath);
#endif
    fprintf(stdout, "   I2C bus number as integer:                  %i (dec)\n",    p->i2cBusNumber);
    fprintf(stdout, "   I2C bus path as string:                     %s\n",          pathStr);
    fprintf(stdout, "   Built in self test (BIST) value:            %02X (hex)\n",  p->doBistMask);
    fprintf(stdout, "   NOS Register value:                         %02X (hex)\n",  p->NOSRegValue);
    fprintf(stdout, "   Post DRDY delay:                            %i (dec)\n",    p->DRDYdelay);
    fprintf(stdout, "   Device sampling mode:                       %s\n",          p->samplingMode     ? "CONTINUOUS" : "POLL");
    fprintf(stdout, "   Cycle counts by vector:                     X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->cc_x, p->cc_y, p->cc_z);
    fprintf(stdout, "   Gain by vector:                             X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->x_gain, p->y_gain, p->z_gain);
    fprintf(stdout, "   Read back CC Regs after set:                %s\n",          p->readBackCCRegs   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Software Loop Delay (uSec):                 %i (dec uSec)\n",    p->outDelay);
    fprintf(stdout, "   CMM sample rate:                            %2X (hex)\n",   p->CMMSampleRate);
    fprintf(stdout, "   TMRC reg value:                             %2X (hex)\n",   p->TMRCRate);
    fprintf(stdout, "   Format output as JSON:                      %s\n",          p->jsonFlag         ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read local temperature only:                %s\n",          p->localTempOnly    ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read remote temperature only:               %s\n",          p->remoteTempOnly   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Read magnetometer only:                     %s\n",          p->magnetometerOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   Local temperature address:                  %02X (hex)\n",  p->localTempAddr);
    fprintf(stdout, "   Remote temperature address:                 %02X (hex)\n",  p->remoteTempAddr);
    fprintf(stdout, "   Magnetometer address:                       %02X {hex)\n",  p->magnetometerAddr);
    fprintf(stdout, "   Show parameters:                            %s\n",          p->showParameters   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Quiet mode:                                 %s\n",          p->quietFlag        ? "TRUE" : "FALSE");
    fprintf(stdout, "   Hide raw measurements:                      %s\n",          p->hideRaw          ? "TRUE" : "FALSE");
    fprintf(stdout, "   Return single magnetometer reading:         %s\n",          p->singleRead       ? "TRUE" : "FALSE");
    fprintf(stdout, "   Magnetometer configuation:                  %s\n", (p->boardMode == LOCAL) ? "Local standalone" : "Extended with remote");
    fprintf(stdout, "   Timestamp format:                           %s\n",          p->tsMilliseconds   ? "RAW"  : "UTCSTRING");
    fprintf(stdout, "   Verbose output:                             %s\n",          p->verboseFlag      ? "TRUE" : "FALSE");
    fprintf(stdout, "   Show total field:                           %s\n",          p->showTotal        ? "TRUE" : "FALSE");
    fprintf(stdout, "\n\n");
}

//------------------------------------------
// getCommandLine()
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;
    int NOSval = 0;
    int magAddr = 0;
    int lTmpAddr = 0;
    int rTmpAddr = 0;

    if(p != NULL)
    {
        memset(p, 0, sizeof(pList));
    }

    p->SBCType          = eRASPI_I2C_BUS;
    p->boardType        = 0;
    p->boardMode        = LOCAL;
    p->doBistMask       = FALSE;
    p->NOSRegValue      = 10;           // 1
    p->DRDYdelay        = 0;
    p->buildLogPath     = FALSE;

    p->cc_x             = CC_400;       // CC_200;
    p->cc_y             = CC_400;       // CC_200;
    p->cc_z             = CC_400;       // CC_200;
    p->x_gain           = GAIN_150;     // GAIN_75;
    p->y_gain           = GAIN_150;     // GAIN_75;
    p->z_gain           = GAIN_150;     // GAIN_75;

    p->samplingMode     = POLL;
    p->readBackCCRegs   = FALSE;
    p->CMMSampleRate    = 200;
    p->hideRaw          = FALSE;
    //p->i2cBusNumber     = 1;
    p->i2cBusNumber     = busDevs[eRASPI_I2C_BUS].busNumber;
    p->i2c_fd           = 0;
    p->jsonFlag         = FALSE;

    p->localTempOnly    = FALSE;
    p->localTempAddr    = MCP9808_LCL_I2CADDR_DEFAULT;
    p->remoteTempOnly   = FALSE;
    p->remoteTempAddr   = MCP9808_RMT_I2CADDR_DEFAULT;
    p->magnetometerOnly = FALSE;
    p->magnetometerAddr = RM3100_I2C_ADDRESS;
    p->outDelay         = 1000000;
    p->quietFlag        = TRUE;
    p->showParameters   = FALSE;
    p->singleRead       = FALSE;
    p->tsMilliseconds   = FALSE;
    p->TMRCRate         = 0x96;
    p->verboseFlag      = FALSE;
    p->showTotal        = FALSE;
    p->outputFilePath   = outFilePath;
#if(USE_PIPES)
    p->useOutputPipe    = FALSE;
    p->pipeInPath       = inputPipeName;
    p->pipeOutPath      = outputPipeName;
#endif

    p->logOutput        = FALSE;
    p->Version          = version;
    while((c = getopt(argc, argv, "?b:B:M:PR:sS:Tt:YV")) != -1)
    {
        switch(c)
        {
            case 'b':
                p->i2cBusNumber = atoi(optarg);
                break;
            case 'B':
                p->doBistMask = atoi(optarg);
                break;
            case 'M':
                sscanf(optarg, "%x", &magAddr);
                p->magnetometerAddr = magAddr;
                break;
            case 'P':
                p->showParameters = TRUE;
                break;
#if (USE_PIPES)
            case 'Y':
                p->useOutputPipe = TRUE;
                break;
#endif
            case 'h':
            case '?':
                fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -B <reg mask>          :  Do built in self test (BIST).         [ Not implemented ]\n");
                fprintf(stdout, "   -b <bus as integer>    :  I2C bus number as integer.\n");
                fprintf(stdout, "   -M <addr as integer>   :  Magnetometer address.                 [ default 20 hex ]\n");
                fprintf(stdout, "   -m                     :  Read magnetometer only.\n");
                fprintf(stdout, "   -P                     :  Show Parameters.\n");
                fprintf(stdout, "   -R <addr as integer>   :  Remote temperature address.           [ default 18 hex ]\n");
                fprintf(stdout, "   -V                     :  Display software version and exit.\n");
                fprintf(stdout, "   -Y                     :  Use WebSockets.                       [ default False].\n");
                fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
                return 1;
                break;
            default:
                fprintf(stdout, "\n?? getopt returned character code 0x%2X ??\n", c);
                break;
        }
    }
    if(optind < argc)
    {
        printf("non-option ARGV-elements: ");
        while(optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }
    return 0;
}
