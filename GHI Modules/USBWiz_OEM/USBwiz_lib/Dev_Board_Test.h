// Dev_Board_Test.h

#ifndef GHI_DEV_BOARD_TEST
#define GHI_DEV_BOARD_TEST

#include "Types.h"
#include "GHI_inter.h"
#include "USBwiz_lib.h"
#include <usart.h>
#include <stdio.h>
#include <SPI.h>
#include <stdlib.h>
#include <string.h>

void BlinkEverySecond(void);
void SystemFail(void);

// Program interface functions
void TestDone(void);
void StartGateway(void);
void ReportError(int8 error);
void GHI_PrintDWORD(int32 dw);
int8 SetTimeStructure(int32 *time);
void ProcessCharFromUSART(int8 *character);
int8 UpdateFirmwareTest(void);
int32 HEXstrToDouble(int8 str[]);

#endif	// GHI_DEV_BOARD_TEST
