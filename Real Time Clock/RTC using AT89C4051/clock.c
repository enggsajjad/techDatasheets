/*------------------------------------------------------*
*         A real time clock using AT89C4051    			*
*    This c file is created using Keil C compiler		*
*														*
*		Designed By: Ajay Bhargav						*
*		E-mail:	bhargav.ajay@gmail.com					*
*				ajay_bhargav@hotmail.com				*
*		website: http://www.rickeyworld.tk				*
*------------------------------------------------------*/

#include <AT89X51.H>
#include <INTRINS.H>

/*----------------------------------------------------
****** port and bit assignment for LCD *******
----------------------------------------------------*/

typedef unsigned char bit_8;

sbit rs=P3^5;					// register select is port 3.5
sbit rw=P3^4;					// read/write is port 3.4
sbit en=P3^3;					// enable is port 3.3
sbit d7=P1^7;					// busy flag is port 1.7
sbit sel_param=P3^2;			// select switch to select the clock parameter
sbit chg_param_inc=P3^1;		// change switch to change the clock parameter
sbit chg_param_dec=P3^0;
bit_8 code *days[]={"*Sun*","*Mon*","*Tue*","*Wed*","*Thu*","*Fri*","*Sat*"};		// days array

bit_8 code *val[]={"0","1","2","3","4","5","6","7","8","9","10","11","12"};			// digit array

bit flag;

bit_8 month=1,date=1,hh,lmin,hmin,hsec,lsec,day=0,lyear=5,hyear=0,date_count;

/*-------------------------
*** Function Prototypes ***
-------------------------*/

void ini(void);
void command(bit_8);
void data_in(bit_8*);
void busy(void);
void sec1(void);
void month_select(bit_8);
void date_in(bit_8);
void set_clock();
void delay(void);

/*-----------------
*** some macros ***
-----------------*/

#define DISABLE_INT IE=0x0;
#define ENABLE_INT IE=0x81;

/*-------------------------------
*** Main programe starts here ***
-------------------------------*/

void main()
{
	bit_8 temp;
	ENABLE_INT
	ini();
	sel_param=1;
	chg_param_inc=1;
	chg_param_dec=1;
	month_select(month);
	while(1)
	{
		month_select(month);										// this will select month and dates
		while(date<=date_count)
		{
			command(0x88);
			date_in(date);
			command(0x8a);
			data_in("/");
			if(month<=9)
				data_in(val[0]);
			data_in(val[month]);
			command(0x8d);
			data_in("/");
			data_in(val[hyear]);
			data_in(val[lyear]);
			for(hh=1;hh<=24;hh++)
			{
				command(0x80);
				data_in(days[day]);
				command(0xc3);
				if(hh<=12)
				{
					if(hh<=9)
						data_in(val[0]);
					data_in(val[hh]);
				}
				else
				{
					temp=hh-12;
					if(temp<=9)
						data_in(val[0]);
					data_in(val[temp]);
				}
				if(hh<12)
				{
					command(0xcc);
					data_in("am");
				}
				else
				{
					command(0xcc);
					data_in("pm");
				}
				for(hmin=0;hmin<=5;hmin++)
				{
					command(0xc5);
					data_in(":");
					data_in(val[hmin]);
					for(lmin=0;lmin<=9;lmin++)
					{
						command(0xc7);
						data_in(val[lmin]);	
						for(hsec=0;hsec<=5;hsec++)
						{
							command(0xc8);
							data_in(":");
							data_in(val[hsec]);
							for(lsec=0;lsec<=9;lsec++)
							{
								command(0xca);
								data_in(val[lsec]);
								sec1();
								if(_testbit_(flag))
									set_clock();
							}
						}
					}
				}
				if(hh==23&&lmin==0&&hmin==0)
				{
					day=day+1;
					if(day==7)
						day=0;
				}
			}
			hh=1;
			date=date+1;
		}
		month=month+1;
		date=1;
		if(month==13)
		{
			month=1;
			lyear=lyear+1;
			if(lyear==10)
			{
				lyear=0;
				hyear=hyear+1;
			}
		}
	}
}


/*--------------------------
*** initialising the LCD ***
--------------------------*/

void ini()
{	
	command(56);
	command(12);
	command(1);
	command(6);
}

/*----------------------------
*** checking the busy flag ***
----------------------------*/

void busy()
{
	d7=1;
	rs=0;
	rw=1;
	while(d7!=0)
	{
		en=0;
		en=1;
	}
}

/*-------------------------
*** send command to LCD ***
-------------------------*/

void command(bit_8 val)
{
	busy();
	P1=val;
	rs=0;
	rw=0;
	en=1;
	en=0;
}

/*--------------------------
*** entering data to LCD ***
--------------------------*/

void data_in(bit_8 *string)
{
	bit_8 i;
	busy();
	for(i=0;string[i]!='\0';i++)
		{
			P1=string[i];
			rs=1;
			rw=0;
			en=1;
			en=0;
		}
}

/*----------------------
*** one second delay ***
----------------------*/

void sec1()//can be fine tuned by changing the for loop values
{

	bit_8 i,j;
	TMOD=0x11;
	for(i=0;i<230;i++)
		_nop_();
	for(i=0;i<7;i++)
	{
		TH1=0;
		TH0=1;
		TL1=0;
		TL0=0;
		TR1=1;
		for(j=0;j<119;j++)
			_nop_();
		while(TF1!=1)
		{
			for(j=0;j<255;j++);
				_nop_();
		}
		TF1=0;
		TR0=1;
		TR1=0;
		while(TF0!=1)
		{
			for(j=0;j<120;j++)
				_nop_();
		}
		TF0=0;
		TR0=0;
	}
}

/*---------------------
*** selecting month ***
---------------------*/

void month_select(bit_8 mon)
{
	bit_8 year;
	switch(mon)
	{
		case 1:
			date_count=31;
			break;
		case 2:
			year=hyear*10+lyear;
			if(year%4==0)
				date_count=29;
			else
				date_count=28;
			break;
		case 3:
			date_count=31;
			break;
		case 4:
			date_count=30;
			break;
		case 5:
			date_count=31;
			break;
		case 6:
			date_count=30;
			break;
		case 7:
			date_count=31;
			break;
		case 8:
			date_count=31;
			break;
		case 9:
			date_count=30;
			break;
		case 10:
			date_count=31;
			break;
		case 11:
			date_count=30;
			break;
		case 12:
			date_count=31;
			break;
	}
}

/*-----------------------
-- Diaplaying the date --
-----------------------*/

void date_in(bit_8 a)
{
	bit_8 j;
	j=a/10;
	data_in(val[j]);
	j=a%10;
	data_in(val[j]);
}

/*---------------------------------
*** Setting parameters of clock ***
---------------------------------*/

void clock_flag(void) interrupt 0
{
	flag=1;
}

void set_clock(void)
{
	bit_8 temp;
	DISABLE_INT
	command(1);
	command(0x80);
	data_in("Set");
	command(0x8c);
	data_in("Date");
	command(0x0f);
	command(0xc4);
	date_in(date);
	command(0xc6);
	data_in("/");
	if(month<=9)
		data_in(val[0]);
	data_in(val[month]);
	command(0xc9);
	data_in("/");
	data_in(val[hyear]);
	data_in(val[lyear]);
	command(0xcb);
up2:while(chg_param_inc!=0&&chg_param_dec!=0&&sel_param!=0);
		if(chg_param_inc==0)
		{
			lyear++;
			if(lyear==10)
			{
				lyear=0;
				hyear++;
			}
			command(0xca);
			data_in(val[hyear]);
			data_in(val[lyear]);
			month_select(month);
			command(0x10);
			delay();
			goto up2;
		}
		if(chg_param_dec==0)
		{
			lyear--;
			if(lyear==-1)
			{
				lyear=9;
				hyear--;
				if(hyear==-1)
					hyear=9;
			}
			command(0xca);
			data_in(val[hyear]);
			data_in(val[lyear]);
			month_select(month);
			command(0x10);
			delay();
			goto up2;
		}
	command(0xc8);
	delay();
up1:while(chg_param_inc!=0&&chg_param_dec!=0&&sel_param!=0);
		if(chg_param_inc==0)
		{
			month++;
			if(month==13)
				month=1;
			command(0xc7);
			if(month<=9)
				data_in(val[0]);
			data_in(val[month]);
			month_select(month);
			command(0x10);
			delay();
			goto up1;
		}
		if(chg_param_dec==0)
		{
			month--;
			if(month==0)
				month=12;
			command(0xc7);
			if(month<=9)
				data_in(val[0]);
			data_in(val[month]);
			month_select(month);
			command(0x10);
			delay();
			goto up1;
		}
		
	command(0xc5);
	delay();
up:	while(chg_param_inc!=0&&chg_param_dec!=0&&sel_param!=0);
		if(chg_param_inc==0)
		{	
			date++;
			if(date>date_count)
				date=1;
			command(0xc4);
			date_in(date);
			command(0x10);
			delay();
			goto up;
		}
		if(chg_param_dec==0)
		{
			date--;
			if(date==0)
				date=date_count;
			command(0xc4);
			date_in(date);
			command(0x10);
			delay();
			goto up;
		}
		command(0x01);
		command(0x80);
		data_in("Set");
		command(0x8c);
		data_in("Time");
		command(0xc4);
		if(hh<=12)
		{
			if(hh<=9)
				data_in(val[0]);
			data_in(val[hh]);
		}
		else
		{
			temp=hh-12;
			if(temp<=9)
				data_in(val[0]);
			data_in(val[temp]);
		}
		data_in(":");
		data_in(val[hmin]);
		data_in(val[lmin]);
		command(0xca);
		if(hh<12)
		{	
			data_in("am");
		}
		else
		{
			data_in("pm");
		}
		command(0xc5);
		delay();
up3:	while(chg_param_inc!=0&&chg_param_dec!=0&&sel_param!=0);
		if(chg_param_inc==0)
		{
			hh++;
			if(hh==24)
				hh=1;
			command(0xc4);
			if(hh<=12)
			{
				if(hh<=9)
					data_in(val[0]);
				data_in(val[hh]);
			}
			else
			{
				temp=hh-12;
				if(temp<=9)
					data_in(val[0]);
				data_in(val[temp]);
			}
			command(0xca);
			if(hh<12)
			{	
				data_in("am");
			}
			else
			{
				data_in("pm");
			}
			command(0xc5);
			delay();
			goto up3;
		}
		if(chg_param_dec==0)
		{
			hh--;
			if(hh==0)
				hh=24;
			command(0xc4);
			if(hh<=12)
			{
				if(hh<=9)
					data_in(val[0]);
				data_in(val[hh]);
			}
			else
			{
				temp=hh-12;
				if(temp<=9)
					data_in(val[0]);
				data_in(val[temp]);
			}
			command(0xca);
			if(hh<12)
			{	
				data_in("am");
			}
			else
			{
				data_in("pm");
			}
			command(0xc5);
			delay();
			goto up3;
		}
		command(0xc8);
		delay();
up4:	while(chg_param_inc!=0&&chg_param_dec!=0&&sel_param!=0);
		if(chg_param_inc==0)
		{
			lmin++;
			if(lmin==10)
			{
				lmin=0;
				hmin++;
				if(hmin==6)
					hmin=0;
			}
			command(0xc7);
			data_in(val[hmin]);
			data_in(val[lmin]);
			command(0x10);
			delay();
			goto up4;
		}
		if(chg_param_dec==0)
		{
			lmin--;
			if(lmin==-1)
			{
				lmin=9;
				hmin--;
				if(hmin==-1)
					hmin=5;
			}
			command(0xc7);
			data_in(val[hmin]);
			data_in(val[lmin]);
			command(0x10);
			delay();
			goto up4;
		}
		command(0xcc);
		delay();
		command(0x01);
		command(0x80);
		data_in("Set");
		command(0x8c);
		data_in("Day");
		command(0xc4);
		data_in(days[day]);
		delay();
up6:	while(chg_param_inc!=0&&chg_param_dec!=0&&sel_param!=0);
		if(chg_param_inc==0)
		{
			day++;
			if(day==7)
				day=0;
			command(0xc4);
			data_in(days[day]);
			delay();
			goto up6;
		}
		if(chg_param_dec==0)
		{
			day--;
			if(day==-1)
				day=6;
			command(0xc4);
			data_in(days[day]);
			delay();
			goto up6;
		}
		delay();
		ini();
		command(0x80);
		data_in(days[day]);
		command(0x88);
		date_in(date);
		command(0x8a);
		data_in("/");
		if(month<=9)
			data_in(val[0]);
		data_in(val[month]);
		command(0x8d);
		data_in("/");
		data_in(val[hyear]);
		data_in(val[lyear]);
		command(0xcc);
		if(hh<12)
		{
			data_in("am");
		}
		else
		{
			data_in("pm");
		}
		command(0xc3);
		if(hh<=12)
		{
			if(hh<=9)
				data_in(val[0]);
			data_in(val[hh]);
		}
		else
		{
			temp=hh-12;
			if(temp<=9)
				data_in(val[0]);
			data_in(val[temp]);
		}
		command(0xc5);
		data_in(":");
		data_in(val[hmin]);
		command(0xc7);
		data_in(val[lmin]);	
		command(0xc8);
		data_in(":");
		data_in(val[hsec]);
		data_in(val[lsec]);
		delay();
		ENABLE_INT
}


/*--------------------
*** Delay function ***
--------------------*/

void delay(void)//this delay is can be changed according to the
{				//speed requirement for the parameter values while changing.
	bit_8 i,j;
	TMOD=0x11;
	for(i=0;i<2;i++)
	{
		TH1=0;
		TH0=1;
		TL1=0;
		TL0=0;
		TR1=1;
		while(TF1!=1)
		{
			for(j=0;j<20;j++);
		}
		TF1=0;
		TR0=1;
		TR1=0;
		while(TF0!=1)
		{}
		TF0=0;
		TR0=0;
	}
}
