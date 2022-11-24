/*
 * Copyright (c) 2006, GHI Electronics, LLC
 * All rights reserved. 
 *
 * You can use this file if you agree to the following
 *
 * 1. This header can't be changed under any condition
 *    
 * 2. This is free software and therefore is providede with NO warranty 
 * 
 * 3. Feel free to modify the codes but we ask you to provide us with 
 *	  any bugs reports so we can keep the code up to date
 *
 * 4. This code may ONLY be used with GHI Electronics, LLC products
 *
 * THIS SOFTWARE IS PROVIDED BY GHI ELECTRONICS, LLC ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL 
 * GHI ELECTRONICS, LLC BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR ORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 *	Specs are subject to change without any notice
 */

#include "USBwiz_lib.h"
#include "GHI_inter.h"
#include <string.h>

//---------------------------------------------
int8 ASCIItoNibble(int8 digit)
{
	if ( digit<='9' )
    {
        return(digit-'0');
    }else if (digit>='A'&& digit<='F')
    {
        return((digit-'A')+10);
    }else
    {
        if(digit>='a' && digit<='f')
        {
            return((digit-'a')+10);
        }
    }
    return 0;
}
//---------------------------------------------
int32 HEXstrToDouble(int8 str[])
{
    int32 result;
    int8 i;

    result=0;
    i=0;
    while(str[i])
    {
        result=result<<4;
        if( ((str[i]>='0')&&(str[i]<='9')) )
          result=result | (str[i]-'0');
        if( ((str[i]>='A')&&(str[i]<='F'))  )
          result=result | ((str[i]-'A')+10);
        i++;
    }
    return result;
}
//---------------------------------------------
void GHI_PutNibble(int8 n)
{
	n&=0x0f;
	if(n<10)
		GHI_PutC((n+'0'));
	else
		GHI_PutC((n-10)+'A');
}
//---------------------------------------------
void GHI_PutDWORD(int32 dw)
{
	// send the high nibble first
	GHI_PutNibble(dw>>28);
	GHI_PutNibble(dw>>24);
	GHI_PutNibble(dw>>20);
	GHI_PutNibble(dw>>16);
	GHI_PutNibble(dw>>12);
	GHI_PutNibble(dw>>8);
	GHI_PutNibble(dw>>4);
	GHI_PutNibble(dw);
}
//---------------------------------------------
void GHI_PutWORD(int16 w)
{
	// send the high nibble first
	GHI_PutNibble(w>>12);
	GHI_PutNibble(w>>8);
	GHI_PutNibble(w>>4);
	GHI_PutNibble(w);
}
//---------------------------------------------
void GHI_PutByte(int8 b)
{
	// send the high nibble first
	GHI_PutNibble(b>>4);
	GHI_PutNibble(b);
}
//---------------------------------------------
// the return data is always in this format
// !zz<CR>
int8 GHI_GetResult(void)
{
	int8 result;
	int8 c;

	c=GHI_GetC();
	if(c=='!')// the command has returned the results
	{
		result=ASCIItoNibble(GHI_GetC())<<4;
		result|=ASCIItoNibble(GHI_GetC());
		if(GHI_GetC()!='\r') // data MUST end with <CR>
			return ERROR_UNEXPECTED_VALUE;
		// everything went well
		return result;
	}
	
	return ERROR_UNEXPECTED_VALUE;	
}
//----------------------------------------------
int16 GHI_GetLibraryVersion()
{
	return GHI_USBWIZ_LIBRARY_VERSION;
}
//---------------------------------------------
int8 GHI_GetVersion(int8 * major, int8 * BCDminor)
{
	int8 c;
	
	int8 cmd[]="VR\r";
	
	GHI_PutS(cmd);

	// lets double check 2 of the characters (USBwiz x.xx)
	if(GHI_GetC()!='U')
		return ERROR_UNEXPECTED_VALUE;
	if(GHI_GetC()!='S')
		return ERROR_UNEXPECTED_VALUE;

	// dump the rest of the string USBizi
	c=GHI_GetC();//B
	c=GHI_GetC();//i or w
	c=GHI_GetC();//z or i
	c=GHI_GetC();//i or z
	c=GHI_GetC();// space

	*major = ASCIItoNibble(GHI_GetC());
	c=GHI_GetC();//dot
	*BCDminor = ASCIItoNibble(GHI_GetC());
	*BCDminor <<=4;
	*BCDminor |= ASCIItoNibble(GHI_GetC());
	c=GHI_GetC();//cariage return
	
	//GHI_Sleep(1);	
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_SetUSBwizBaudRate(int16 bauderate)
{
	int8 cmd[]="BR ";
	
	GHI_PutS(cmd);
	GHI_PutWORD(bauderate);
	GHI_PutC('\r');
	
	// return no error
	return  0;
	
}
//-----------------------------------------------------------------------------------------
#ifdef	_INCLUDE_EXTRA_COMMANDS_SUPPORT_
int8 GHI_ReadDeviceInfo(int8 device, DEVICE_INFO *info)
{
	int8 cmd[] = "II ", c, temp[5];
	
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');
	
	if( c = GHI_GetResult() )	// if error
		return c;
	
	temp[4] = 0;	// terminate with 0	
	
	// read vendor id
	if(GHI_GetC() != '$')
		return ERROR_UNEXPECTED_VALUE;
	for(c = 0; c < 4; c++)
		temp[c] = GHI_GetC();
	
	info->vendor_id = HEXstrToDouble(temp);	
	if(GHI_GetC() != '>')
		return ERROR_UNEXPECTED_VALUE;
		
	// read product id
	if(GHI_GetC() != '$')
		return ERROR_UNEXPECTED_VALUE;
	for(c = 0; c < 4; c++)
		temp[c] = GHI_GetC();
	
	info->product_id = HEXstrToDouble(temp);	
	if(GHI_GetC() != '>')
		return ERROR_UNEXPECTED_VALUE;
		
	temp[2] = 0;	// terminate with 0
	// read number of interfaces
	if(GHI_GetC() != '$')
		return ERROR_UNEXPECTED_VALUE;
	for(c = 0; c < 2; c++)
		temp[c] = GHI_GetC();
	
	info->number_of_interfaces = HEXstrToDouble(temp);	
	if(GHI_GetC() != '\r')
		return ERROR_UNEXPECTED_VALUE;
		
	// read inter1 class
	if(GHI_GetC() != '$')
		return ERROR_UNEXPECTED_VALUE;
	for(c = 0; c < 2; c++)
		temp[c] = GHI_GetC();
	
	info->interface1_class = HEXstrToDouble(temp);	
	if(GHI_GetC() != '>')
		return ERROR_UNEXPECTED_VALUE;
		
	// read inter1 sub-class
	if(GHI_GetC() != '$')
		return ERROR_UNEXPECTED_VALUE;
	for(c = 0; c < 2; c++)
		temp[c] = GHI_GetC();
	
	info->interface1_subclass = HEXstrToDouble(temp);	
	if(GHI_GetC() != '>')
		return ERROR_UNEXPECTED_VALUE;
	
	// read inter1 protocol
	if(GHI_GetC() != '$')
		return ERROR_UNEXPECTED_VALUE;
	for(c = 0; c < 2; c++)
		temp[c] = GHI_GetC();
	
	info->interface1_protocol = HEXstrToDouble(temp);	
	if(GHI_GetC() != '\r')
		return ERROR_UNEXPECTED_VALUE;
		
	// Ignore all other interfaces
	while(GHI_GetC() != '!');
	
	// read the error
	temp[0] = GHI_GetC();
	temp[1] = GHI_GetC();
	if(GHI_GetC() != '\r')
		return ERROR_UNEXPECTED_VALUE;
	
	return HEXstrToDouble(temp);
	
}
//----------------------------------------------------------------------------
int8 GHI_UpdateFirmware(int8 device)
{
	int8 cmd[] = "BL ", c, result;
	
	GHI_PutS(cmd);
	
	// check for error
	if(device != USB_DEVICE_PORT_0 &&
	   device != USB_DEVICE_PORT_1 &&
	   device != SD_DEVICE)
		return ERROR_COMMANDER_INCORRECT_CMD_PARAMETER;
		
	GHI_PutC(device);
	GHI_PutC('\r');
	
	// get results after 100ms
	GHI_Sleep(100);
	
	c = GHI_GetC();
	result = ERROR_NO_ERROR;
	
	if(c=='!')	// we got error, save it
	{
		result=ASCIItoNibble(GHI_GetC())<<4;
		result|=ASCIItoNibble(GHI_GetC());
	}
	else if(c != 'W')	// if we didn't get W00 that is an error
			return ERROR_UNEXPECTED_VALUE;
			
	// dump the data untill the end
	if(result == ERROR_NO_ERROR)	// if no error,
		while(GHI_GetC() != '!'); // we reached the !00 after writing data
	// get the banner
	while(GHI_GetC() != '!'); // we reached !00
	GHI_GetC(); // dump '0'
	GHI_GetC(); // dump '0'
	GHI_GetC(); // dump '\r'
	
	return result;
	
}
//----------------------------------------------------------
int8 GHI_SoftwareReset(void)
{
	int8 cmd[] = "RT OK\r", c;
	
	GHI_PutS(cmd);
	
	// use cmd to store data
	
	// ignore untill !xx
	while(GHI_GetC() != '!');
	
	cmd[0] = GHI_GetC();
	cmd[1] = GHI_GetC();
	cmd[2] = 0;
	
	if(GHI_GetC() != '\r')
		return ERROR_UNEXPECTED_VALUE;
	
	c = HEXstrToDouble(cmd);
	
	return c;
}
#endif	// _INCLUDE_EXTRA_COMMANDS_SUPPORT_
//--------------------------------------------------------
#ifdef	_INCLUDE_FAT_SYSTEM_SUPPORT_
int8 GHI_MountFATFileSystem(int8 device)
{
	int8 cmd[] = "FM ", c;
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');
	
	c = GHI_GetResult();
	if(c)		// if error
	{
		return c;
	}
	
	// ignore LUNs
	if(device == USB_DEVICE_PORT_0 || device == USB_DEVICE_PORT_1)
	{
		if(GHI_GetC() != '$')	// read data
			return ERROR_UNEXPECTED_VALUE;
			
		GHI_GetC();	// ignore
		GHI_GetC();	// ignore
		
		if(GHI_GetC() != '\r')
			return ERROR_UNEXPECTED_VALUE;

		c = GHI_GetResult();
	}
	
	return c;
}
//-----------------------------------------------------------------------------------------
int8 GHI_SwitchDevice(int8 device)
{	
	int8 cmd[] = "DS ";

	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//----------------------------------------------
int8 GHI_ChangeDirectory(int8 * filename)
{
	int8 cmd[]="CD ";
	
	GHI_PutS(cmd);
	GHI_PutS(filename);
	GHI_PutC('\r');
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_MakeDirectory(int8* filename)
{
	int8 cmd[]="MD ";
	
	GHI_PutS(cmd);
	GHI_PutS(filename);
	GHI_PutC('\r');
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_OpenFile(int8 filehandle, int8 *filename, int8 openmode)
{
	int8 cmd[]="OF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC(openmode);
	GHI_PutC('>');
	GHI_PutS(filename);
	GHI_PutC('\r');
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_SendReadCommand(int8 filehandle, int32 desireddatasize, int8 filler)
{
	int8 cmd[]="RF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC(filler);
	GHI_PutC('>');
	GHI_PutDWORD(desireddatasize);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_GetReadAndWriteResult(int32 * actualdatasize)
{
	int8 datastring[9];
	int8 c;

	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		datastring[c]=GHI_GetC();
	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;
	datastring[8]=0;//terminate with null

	*actualdatasize=HEXstrToDouble(datastring);

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_SendWriteCommand(int8 filehandle, int32 desireddatasize)
{
	int8 cmd[]="WF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC('>');
	GHI_PutDWORD(desireddatasize);
	GHI_PutC('\r');

	return GHI_GetResult();
	
}
//-----------------------------------------------------------------------------------------
int8 GHI_CloseFile(int8 filehandle)
{
	int8 cmd[]="CF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_DeleteFile(int8 *filename)
{
	int8 cmd[]="DF ";
	
	GHI_PutS(cmd);
	GHI_PutS(filename);
	GHI_PutC('\r');

	return GHI_GetResult();
}
#endif	// _INCLUDE_FAT_SYSTEM_SUPPORT_
//--------------------------------------------------------
#ifdef	_INCLUDE_STORAGE_SECTOR_SUPPORT_
int8 GHI_RegisterSD(void)
{
	int8 cmd[] = "SD\r";
	
	GHI_PutS(cmd);
	
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_RegisterUSBMassStorage(int8 device)
{
	int8 cmd[] = "UM ", c;
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');
	
	if(c = GHI_GetResult())		// if(error)
		return c;
	if(GHI_GetC() != '$')		// must read $...
		return ERROR_UNEXPECTED_VALUE;
		
	// ignore the LUN
	GHI_GetC();	// ignore
	GHI_GetC(); // ignore 
	
	if(GHI_GetC() != '\r')
		return ERROR_UNEXPECTED_VALUE;
	
	return GHI_GetResult();
	
}
//---------------------------------------------------------------
int8 GHI_ReadSector(int32 sectornum)
{
	int8 cmd[]="RS H";
	
	GHI_PutS(cmd);
	GHI_PutDWORD(sectornum);
	GHI_PutC('\r');
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_WriteSector(int32 sectornum)
{
	int8 cmd[]="WS ";
	
	GHI_PutS(cmd);
	GHI_PutDWORD(sectornum);
	GHI_PutC('\r');
	return GHI_GetResult();
}
//--------------------------------------------
#ifndef	_INCLUDE_FAT_SYSTEM_SUPPORT_
	int8 GHI_SwitchDevice(int8 device)
	{	
		int8 cmd[] = "DS ";
	
		GHI_PutS(cmd);
		GHI_PutC(device);
		GHI_PutC('\r');
	
		return GHI_GetResult();
	}
#endif
#endif	// _INCLUDE_STORAGE_SECTOR_SUPPORT_
//-------------------------------------------------------------
#ifdef	_INCLUDE_EXTENDED_FAT_SYSTEM_SUPPORT_
void GHI_StartMediaStatistics (void)
{
	int8 cmd[]="MS\r";
	
	GHI_PutS(cmd);
}
//-----------------------------------------------------------------------------------------
int8 GHI_GetResultMediaStatistics(int32 * size, int32 * free)
{
	int8 c;
	int8 temp[9];
	
	c=GHI_GetResult();
	if(c)
	{
		// the command failed
		return c;
	}
	// the command passed
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<8;c++)
		temp[c]=GHI_GetC();
	temp[8]=0;//terminate
	*size=HEXstrToDouble(temp);
	
	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;
	
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<8;c++)
		temp[c]=GHI_GetC();
	temp[8]=0;//terminate
	*free=HEXstrToDouble(temp);
	
	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;
	
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
void GHI_StartQformat(void)
{
	int8 confirm[]="CONFIRM FORMAT";
	int8 cmd[]="QF ";
	
	GHI_PutS(cmd);
	GHI_PutS(confirm);
	GHI_PutC('\r');
}
//-----------------------------------------------------------------------------------------
int8 GHI_InitGetFile(void)
{
	int8 cmd[]="IL\r";
	
	GHI_PutS(cmd);
	
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_GetNextFile(int8 * filename, int8 * fileext, int8 * attributes, int32 * size)
{
	int8 c;
	int8 temp[9];
	
	int8 cmd[]="NF\r";
	
	GHI_PutS(cmd);
	c=GHI_GetResult();
	if(c)
	{
		// error code
		return c;
	}
	// the command passed
	for(c=0;c<8;c++)
		filename[c]=GHI_GetC();
	if(GHI_GetC()!='.')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<3;c++)
		fileext[c]=GHI_GetC();
	// read the attributes	
	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<2;c++)
		temp[c]=GHI_GetC();
	temp[2]=0;//terminate
	*attributes=HEXstrToDouble(temp);// our function is smart enough to get just a byte back
	// read the file size
	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<8;c++)
		temp[c]=GHI_GetC();
	temp[8]=0;//terminate
	*size=HEXstrToDouble(temp);
	
	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;
	
	return GHI_GetResult();
	
}
//-----------------------------------------------------------------------------------------
int8 GHI_RemoveDirectory(int8* filename)
{
	int8 cmd[]="RD ";
	
	GHI_PutS(cmd);
	GHI_PutS(filename);
	GHI_PutC('\r');
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_SendReadWriteFileCommand(int8 readhandle, int8 writehandle,int32 size)
{
	int8 c;
	int8 temp[9];


	int8 cmd[]="RW ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(readhandle);
	GHI_PutC(' ');
	GHI_PutNibble(writehandle);
	GHI_PutC('>');
	GHI_PutDWORD(size);
	GHI_PutC('\r');

	return GHI_GetResult();
	
}
//-----------------------------------------------------------------------------------------
int8 GHI_SendShadowWriteTwoFiles(int8 firstfilehandle, int8 secondfilehandle, int32 desireddatasize)
{
	int8 cmd[]="SH ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(firstfilehandle);
	GHI_PutC(' ');
	GHI_PutNibble(secondfilehandle);
	GHI_PutC('>');
	GHI_PutDWORD(desireddatasize);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_GetShadowWriteTwoFileResults(int8 *fresult1, int8 *fresult2, int32 *writtendata1, int32 *writtendata2)
{
	int8 datastring[9];
	int8 c;

	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		datastring[c]=GHI_GetC();

	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;
	datastring[8]=0;//terminate with null

	*writtendata1 = HEXstrToDouble(datastring);

	*fresult1 = GHI_GetResult();
	
   // Second file

	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		datastring[c]=GHI_GetC();

	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;

	datastring[8]=0;//terminate with null

	*writtendata2=HEXstrToDouble(datastring);

	*fresult2 = GHI_GetResult();
   
	return ERROR_NO_ERROR;
	
}
//-----------------------------------------------------------------------------------------
int8 GHI_SendShadowWriteThreeFiles(int8 firstfilehandle, int8 secondfilehandle, int8 thirdfilehandle, int32 desireddatasize)
{
	int8 cmd[]="SH ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(firstfilehandle);
	GHI_PutC(' ');
	GHI_PutNibble(secondfilehandle);
	GHI_PutC(' ');
	GHI_PutNibble(thirdfilehandle);
	GHI_PutC('>');
	GHI_PutDWORD(desireddatasize);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------		 
int8 GHI_GetShadowWriteThreeFileResults(int8 *fresult1, int8 *fresult2, int8 *fresult3, int32 *writtendata1, int32 *writtendata2, int32 *writtendata3)
{
	int8 datastring[9];
	int8 c;

	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		datastring[c]=GHI_GetC();

	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;
	datastring[8]=0;//terminate with null

	*writtendata1 = HEXstrToDouble(datastring);

	*fresult1 = GHI_GetResult();
   //------------------------------------------

	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		datastring[c]=GHI_GetC();

	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;

	datastring[8]=0;//terminate with null

	*writtendata2=HEXstrToDouble(datastring);

	*fresult2 = GHI_GetResult();
   //------------------------------------------
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		datastring[c]=GHI_GetC();

	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;

	datastring[8]=0;//terminate with null

	*writtendata3=HEXstrToDouble(datastring);

	*fresult3 = GHI_GetResult();
   //------------------------------------------
	return NO_ERROR;
}
//-----------------------------------------------------------------------------------------
int8 GHI_SeekFile(int8 filehandle, int32 newposition)
{
	int8 cmd[]="PF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC('>');
	GHI_PutDWORD(newposition);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_GetPointerPosition(int8 filehandle, int32 *sector, int16 *positioninsector)
{
	int8 cmd[]="FP ";
	int8 datastring[8];
	int8 c;

	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC('\r');
	c = GHI_GetResult();
	if(c) return c;
	
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		datastring[c]=GHI_GetC();

	if(GHI_GetC()!=' ')
		return ERROR_UNEXPECTED_VALUE;

	datastring[8]=0;//terminate with null
	*sector=HEXstrToDouble(datastring);

	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<4;c++)
		datastring[c]=GHI_GetC();
	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;
	datastring[4]=0;//terminate with null
	*positioninsector=HEXstrToDouble(datastring);

	

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_FlushFile(int8 filehandle)
{
	int8 cmd[]="FF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_SplitFile(int8 sourcehandle, int8 desthandle1,  int8 desthandle2, int32 splitposition, int32 * actualdestsize1, int32 * actualdestsize2)
{
	int8 c;
	int8 temp[9];

	int8 cmd[]="SF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(sourcehandle);
	GHI_PutC('>');
	GHI_PutNibble(desthandle1);
	GHI_PutC('+');
	GHI_PutNibble(desthandle2);
	GHI_PutC('@');

	GHI_PutDWORD(splitposition);
	GHI_PutC('\r');

	// get the first write result
	c=GHI_GetResult();
	if(c)
	{
		*actualdestsize1=0;
		*actualdestsize2=0;
		// error code
		return c;
	}
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		temp[c]=GHI_GetC();

	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;

	temp[8]=0;//terminate with null

	*actualdestsize1=HEXstrToDouble(temp);

	// get the second write result
	c=GHI_GetResult();
	if(c)
	{
		*actualdestsize2=0;
		// error code
		return c;
	}
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;

	for(c=0;c<8;c++)
		temp[c]=GHI_GetC();
	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;
	temp[8]=0;//terminate with null

	*actualdestsize2=HEXstrToDouble(temp);
	
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_GetFileInfo(int8 * filename, int32 * size, int8 * attributes, int32 * DateTime)
{
	int8 temp[9];
	int8 c;
	
	int8 cmd[]="IF ";
	
	GHI_PutS(cmd);
	GHI_PutS(filename);
	GHI_PutC('\r');
	c=GHI_GetResult();
	if(c)
	{
		// error code
		return c;
	}
	// the command passed
	if(GHI_GetC()!='$') return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<8;c++)    
		temp[c]=GHI_GetC();
	temp[8]=0;//terminate
	*size=HEXstrToDouble(temp);
	
	if(GHI_GetC()!=' ')	return ERROR_UNEXPECTED_VALUE;
	if(GHI_GetC()!='$')	return ERROR_UNEXPECTED_VALUE;
	
	for(c=0;c<2;c++)// we got 2 bytes only
		temp[c]=GHI_GetC();
	temp[2]=0;//terminate
	
	*attributes=HEXstrToDouble(temp);// our function is smart enough to get just a byte back
	
	if(GHI_GetC()!=' ')	return ERROR_UNEXPECTED_VALUE;
	if(GHI_GetC()!='$')	return ERROR_UNEXPECTED_VALUE;
	
	for(c=0;c<8;c++)// we got 8 bytes only
		temp[c]=GHI_GetC();
	temp[8]=0;//terminate
	
	*DateTime=HEXstrToDouble(temp);// our function is smart enough to get just a byte back
	
	if(GHI_GetC()!='\r') return ERROR_UNEXPECTED_VALUE;
	
	return GHI_GetResult();

}
//-----------------------------------------------------------------------------------------
int8 GHI_RenameFile(int8 *filename,int8 *newname)
{
	int8 cmd[]="ND ";
	
	GHI_PutS(cmd);
	GHI_PutS(filename);
	GHI_PutC(' ');
	GHI_PutS(newname);
	GHI_PutC('\r');

	return GHI_GetResult();

}
//-----------------------------------------------------------------------------------------
int8 GHI_SetFileSize(int8 filehandle,int32 newsize)
{
	int8 cmd[]="ZF ";
	
	GHI_PutS(cmd);
	GHI_PutNibble(filehandle);
	GHI_PutC('>');
	GHI_PutDWORD(newsize);
	GHI_PutC('\r');

	return GHI_GetResult();
}
#endif	// _INCLUDE_EXTENDED_FAT_SYSTEM_SUPPORT_
//---------------------------------------------
#ifdef _INCLUDE_READWRITE_WRAPPERS_SUPPORT_
int8 GHI_ReadFile(int8 handle, int8 *buffer, int32 size, int8 filler, int32 *actualdatasize)
{
	int32 var32;
	if(var32 = GHI_SendReadCommand(handle, size, filler))
		return var32;
	
	for(var32 = 0; var32 < size; var32++)
	{
		*buffer++ = GHI_GetC();
	}
	
	return GHI_GetReadAndWriteResult(actualdatasize);
}
//---------------------------------------------------
int8 GHI_WriteFile(int8 handle, int8 *buffer, int32 size, int32 *actualdatasize)
{
	int32 var;
	if(var = GHI_SendWriteCommand(handle, size))
		return var;
	
	for(var = 0; var < size; var++)
	{
		GHI_PutC(buffer[var]);
	}
	
	
	return GHI_GetReadAndWriteResult(actualdatasize);
}
#endif	// _INCLUDE_READWRITE_WRAPPERS_SUPPORT_
//-----------------------------------------------------------------------------------------
#ifdef	_INCLUDE_HID_SUPPORT_
int8 GHI_RegisterHID(int8 device)
{
	int8 cmd[]="UH ";
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_PrepareHIDReport(int8 device, int8 requested_size, int8 * actual_report_size)
{
	int8 cmd[]="HR ";
	int8 c;
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	if(requested_size)
	{
		GHI_PutC('>');
		GHI_PutByte(requested_size);
	}
	GHI_PutC('\r');

	c = GHI_GetResult();
	if(c)
	{
		// error code
		return c;
	}
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;
	// use cmd[] for reading report size
	cmd[0] = GHI_GetC();
	cmd[1] = GHI_GetC();
	
	if(GHI_GetC() != '\r')	
		return ERROR_UNEXPECTED_VALUE;
		
	cmd[2]=0;//terminate with null

	*actual_report_size=HEXstrToDouble(cmd);
		
	return ERROR_NO_ERROR;
}
#endif	// _INCLUDE_HID_SUPPORT_
//----------------------------------------------
#ifdef	_INCLUDE_PRINTER_SUPPORT_
int8 GHI_RegisterPrinter(int8 device)
{
	int8 cmd[]="UP ";
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_ResetPrinter(int8 device)
{
	int8 cmd[]="PR ";
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');
	
	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_GetPrinterStatus(int8 device, int8 * status)
{
	int8 c;
	int8 temp[3];
	int8 cmd[]="PS ";
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');

	c=GHI_GetResult();
	if(c)
	{
		// error code
		return c;
	}
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<2;c++)
		temp[c]=GHI_GetC();
	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;
	temp[2]=0;//terminate with null

	*status=HEXstrToDouble(temp);

	return GHI_GetResult();
}
//-----------------------------------------------------------------------------------------
int8 GHI_PrinterPrint(int8 device, int8 size)
{
	int8 cmd[]="PP ";
	int8 c;
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('>');
	GHI_PutByte(size);
	GHI_PutC('\r');

	return GHI_GetResult();
}
#endif	// _INCLUDE_PRINTER_SUPPORT_
//------------------------------------------------------------------
#ifdef	_INCLUDE_SERIAL_DEVICES_SUPPORT_
int8 GHI_RegisterSerialDevice(int8 device, int8 type, int32 baudrate)
{
	int8 cmd[]="US ";
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	
	GHI_PutC('>');
	GHI_PutC(type);
	
	GHI_PutC('>');
	GHI_PutDWORD(baudrate);
	
	GHI_PutC('\r');

	return GHI_GetResult();
}
//---------------------------------------------------------
int8 GHI_SerialWrite(int8 device, int8 size)
{
	int8 cmd[]="SW ";
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('>');
	GHI_PutByte(size);
	GHI_PutC('\r');

	return GHI_GetResult();
}
//---------------------------------------------------------
int8 GHI_SerialRead(int8 device, int8 *sent_data_size)
{
	int8 cmd[]="SR ", c;
	
	GHI_PutS(cmd);
	GHI_PutC(device);
	GHI_PutC('\r');

	if(c = GHI_GetResult())		// error
	{
		return c;
	}
	if(GHI_GetC() != '$')
		return ERROR_UNEXPECTED_VALUE;
	
	// use cmd[] to store data
	cmd[0] = GHI_GetC();
	cmd[1] = GHI_GetC();
	cmd[2] = 0;		// make it null-terminated
	if(GHI_GetC() != '\r')
		return ERROR_UNEXPECTED_VALUE;

	*sent_data_size = HEXstrToDouble(cmd);

	return ERROR_NO_ERROR;
}
#endif	// _INCLUDE_SERIAL_DEVICES_SUPPORT_
//--------------------------------------------------------------
#ifdef	_INCLUDE_TIME_WORK_SUPPORT_
int8 GHI_InitializeTime(int8 backup)
{
	int8 cmd[]="IT ";
	
	GHI_PutS(cmd);
	if(backup)
		GHI_PutC('B');
	else
		GHI_PutC('S');
	GHI_PutC('\r');
	
	return GHI_GetResult();
}
//---------------------------------------------
int8 GHI_GetTime(int32 * time)
{
	int8 c;
	int8 temp[9];
	int8 cmd[]="GT X\r";
	
	GHI_PutS(cmd);
		
	c=GHI_GetResult();
	if(c)
	{
		// error code
		return c;
	}
	// the command passed
	if(GHI_GetC()!='$')
		return ERROR_UNEXPECTED_VALUE;
	for(c=0;c<8;c++)
		temp[c]=GHI_GetC();
	temp[8]=0;//terminate
	*time=HEXstrToDouble(temp);
	if(GHI_GetC()!='\r')
		return ERROR_UNEXPECTED_VALUE;
	
	return GHI_GetResult();
}
//---------------------------------------------
int8 GHI_GetFormattedTime(int8 *buffer)
{
	int8 index;
	
	int8 cmd[]="GT F\r";
	
	GHI_PutS(cmd);
	
	index = GHI_GetResult();		// check for error
	if(index)		// error
	{
		return index;
	}
	
	// check if the command passed
	buffer[2] = 0;		// mm'/'
	buffer[0] = GHI_GetC();
	buffer[1] = GHI_GetC();
	buffer[2] = GHI_GetC();
	if(buffer[2] != '/')
		return ERROR_UNEXPECTED_VALUE;
	
	// get the rest
	index = 3;
	while(1)
	{
		buffer[index] = GHI_GetC();
		if(buffer[index] == '\r')
		{
			buffer[index] = 0;	// make it null terminated
			return GHI_GetResult();
		}
		index++;
	}
}
//---------------------------------------------
int8 GHI_SetTime(int32 time)
{
	int8 cmd[]="ST ";
	
	GHI_PutS(cmd);
	
	GHI_PutDWORD(time);
	GHI_PutC('\r');
	
	return GHI_GetResult();
}
//-------------------------------------------------
int32 GHI_GetFATTimeStructure(int32 year, int32 month, int32 day, int32 hours, int32 minutes, int32 seconds)
{
	int32 time = 0;
	
	time |= (seconds / 2) & 0x1F;		// for FAT Time Structure this field is seconds/2
	time |= (minutes & 0x3F) << 5;
	time |= (hours & 0x1F) << 11;
	time |= (day & 0x1F) << 16;
	time |= (month & 0x0F) << 21;
	time |= ( (year - 1980) & 0x7F) << 25;  // for FAT Time Structure this field is years since 1980
	
	return time;
}
#endif	// _INCLUDE_TIME_WORK_SUPPORT_
//-----------------------------------------------------------------
#ifdef _INCLUDE_OLD_STORAGE_FUNCTIONS_WRAPPERS_SUPPORT_
int8 file_system[3];
// this is to give a proper value
void UpdateDrive(int8 *drive)
{
	switch(*drive)
	{
		case 'A':	case 'a':
			*drive = SD_DEVICE;
		break;
		case 'B':	case 'b':
			*drive = USB_DEVICE_PORT_0;
		break;
		case 'C':	case 'c':
			*drive = USB_DEVICE_PORT_1;
		break;
	
	}
}
//--------------------------------------------
// returns a correct device
int8 UpdateDevice(int8 device, int8 deviceorder)
{
	if(device == 'C' || device == 'S')
		return SD_DEVICE;
	else if(device == 'U')
	{
		if(deviceorder == 0)
			return USB_DEVICE_PORT_0;
		else if(deviceorder == 1)
			return USB_DEVICE_PORT_1;
	}
	
	// return anything (error device or deviceorder)
	return device;
}
//--------------------------------------------
int8 GHI_BL_LoadFirmware(int8 drive)
{
	UpdateDrive(&drive);
	return GHI_UpdateFirmware(drive);
		
}
//---------------------------------------------
int8 GHI_GetResults(void)
{
	return GHI_GetResult();	
}
//--------------------------------------------------
int8 GHI_SetARMBaudRate(int32 bauderate)
{
	return GHI_SetUSBwizBaudRate(bauderate);
}
//---------------------------------------------
int8 GHI_AttachStorageMedia(int8 device, int8 deviceorder, int8 LUN)
{
	int8 new_device = UpdateDevice(device, deviceorder);
	
	if(new_device == SD_DEVICE)
		return GHI_RegisterSD();
	else
		return GHI_RegisterUSBMassStorage(new_device);
}
//----------------------------------------------------
int8 GHI_ReadSectorFromCurrentFileSystem(int32 sectornum)
{
	return GHI_ReadSector(sectornum);
}
//----------------------------------------------------
int8 GHI_GHI_WriteSectorFromCurrnetFileDydtem(int32 sectornum)
{
	return GHI_WriteSector(sectornum);
}
//-----------------------------------------------------------
int8 GHI_MountFileSystem(int8 filesystemorder, int8 device, int8 deviceorder)
{
	int8 new_device = UpdateDevice(device, deviceorder);
	
	if(filesystemorder < 3)
		file_system[filesystemorder] = new_device;
	else
		return ERROR_UNEXPECTED_VALUE;
	
	return GHI_MountFATFileSystem(new_device);
}
//-------------------------------------------------------
int8 GHI_SwitchFileSystem(int8 filesystemorder)
{
	if(filesystemorder < 3)
		return GHI_SwitchDevice(file_system[filesystemorder]);
	else
		return ERROR_UNEXPECTED_VALUE;
}
//--------------------------------------------------
int8 GHI_EnumurateUSBDevicetoRootHub(int8 usbport, int8 usbdevicehandle)
{
	return ERROR_NO_ERROR;
}
//--------------------------------------------------
int8 GHI_ReleaseUSBDeviceHandle(int8 usbdevicehandle)
{
	return ERROR_NO_ERROR;
}
//-------------------------------------------------
int8 GHI_RegisterMassStorageDevice(int8 usbdevicehandle, int8 massstoragedevicehandle, int8 *lastLUN)
{
	return ERROR_NO_ERROR;
}
//----------------------------------------------------
int8 InitializeFATMedia(int8 DriveLetter)
{
	UpdateDrive(&DriveLetter);
	
	return GHI_MountFATFileSystem(DriveLetter);
}
//---------------------------------------------------
int8 SwitchToFATMedia(int8 DriveLetter)
{
	UpdateDrive(&DriveLetter);
	
	return GHI_SwitchDevice(DriveLetter);
}
//------------------------------------------------------
int8 GHI_GetResultsQformat(void)
{
	return GHI_GetResult();
}
#endif
//-------------------------------------------------------------
