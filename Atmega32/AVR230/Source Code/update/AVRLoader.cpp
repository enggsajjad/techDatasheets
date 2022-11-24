#include "stdafx.h"
#include "serialport.h"
#include "AVRLoader.h"


void Errormsg(void)
{
	printf("\nAVRLoader file [-SWITCH] [-SWITCH]\n\n");
	printf("file            file contaning flash upgrade buffer\n");
	printf("SWITCH          S     SPI\n");
	printf("                U     UART\n\n");
	printf("                COM1  Communication port 1\n");
	printf("                COM2  Communication port 2\n");
	printf("                COM3  Communication port 3\n");
	printf("                COM4  Communication port 4\n\n");
	printf("Default: AVRupd myfile.bot -COM2 -U\n");
}


int main(int argc, char* argv[])
{

	FILE *InFile;

	char strComPort[5] = "com2";
	unsigned char Dump, Retrys=0;
	int x, TxSize, BufSize = 0, BytesSent = 0;
	unsigned char Buffer[0x20000];
	unsigned char TxRoute = 2;
	char *parameter;

	CSerialPort *ComPort;
	ComPort = new CSerialPort;

	argc--;
	argv++;

	// If no arguments
	// print Errormessage.
	if ((argc < 1) || (argv[0][0] == '-'))
	{
		Errormsg();
		return -1;
	}

	

	// Read the switches
	while(argc > 1)
	{
		argc--;
		parameter = _strupr(argv[argc]);
		
		if(!strncmp(parameter, "-COM1", 5))
			strcpy(strComPort, "com1");
		
		else if(!strncmp(parameter, "-COM2", 5))
			strcpy(strComPort, "com2");

		else if(!strncmp(parameter, "-COM3", 5))
			strcpy(strComPort, "com3");

		else if(!strncmp(parameter, "-COM4", 5))
			strcpy(strComPort, "com4");

		else if(!strncmp(parameter, "-S", 2))
			TxRoute = 1;

		else if(!strncmp(parameter, "-U", 2))
			TxRoute = 2;

		else
		{
			Errormsg();
			return -1;
		}
	}
	


	if (InFile = fopen(argv[0], "rb"))
	{
		if (!ComPort->Open(strComPort, CBR_9600, 2000))
		{
			printf("Comport %s in use or does not exist.\n", strComPort);
			return -1;
		}

		printf("%s opened for ", strComPort);
		switch(TxRoute)
		{
		case 1:
			printf("SPI");
			break;
		case 2:
			printf("UART");
			break;
		}
		printf(" communication with Target\n");


		while(!feof(InFile))
		{
			fscanf(InFile, "%2X", &Buffer[BufSize++]);
		}
		BufSize--;
		
		for(x = 0; x < BufSize; x += TxSize + 2)
		{
			TxSize = Buffer[x] << 8 | Buffer[x+1];

			// Ignore any received bytes.
			ComPort->SetTimeOut(1);
			while (ComPort->Read(&Dump, 1));
			ComPort->SetTimeOut(4000);
			
//			ComPort->Write(&TxRoute, 1);
			printf("\r                        \rTransfering.. %d%% done.", 100 * BytesSent / BufSize);
				ComPort->Write(&Buffer[x], TxSize + 2);
			
			
			if (ComPort->Read(&Dump, 1))
			{
				switch (Dump)
				{
					case Error_OK:
						BytesSent += TxSize + 2;
						Retrys = 0;
						break;

					case Error_CRC:
						Retrys++;
						if(Retrys < 4)
						{
							x -= TxSize - 2;
						}
						else 
						{
							printf("\nCRC error. File damaged.\n");
							ComPort->Close();
							fclose(InFile);
							return -1;
						}
						break;

					case Error_Line:
						Retrys++;
						if(Retrys < 4)
						{
							x -= TxSize - 2;
						}
						else 
						{
							printf("\nLine error. Check connection.\n");
							ComPort->Close();
							fclose(InFile);
							return -1;
						}
						break;

					case Error_Signature:
						printf("\nFailed.\n");
						printf("Not valid update file.");
						return -1;
						break;

					case Error_TopModule:
						printf("\nFailed.\n");
						printf("Top-Module is out of sync. Please reset the Top-Module and the Target.");
						return -1;
				}
			}
			else
			{
				printf("\nFailed.\n");
				printf("Target is not responsing.\n");
				ComPort->Close();
				fclose(InFile);
				return -1;
			}
		}

		printf("\rTransfering.. 100%% done.  \n");
		fclose(InFile);
	}
	else
	{
		printf("File not found. %s\n", argv[0]);
		ComPort->Close();
		return -1;
	}

	
	ComPort->Close();
	printf("Target updated successfully. Reset Target to start application\n");
	return 0;
}
