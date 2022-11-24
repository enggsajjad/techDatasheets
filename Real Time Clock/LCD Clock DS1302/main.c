#include<reg51.h>

sbit RS=P2^0;
sbit E=P2^1;
sbit SCLK=P2^2; //DS1302 SCLK
sbit IO=P2^3;   //DS1302 IO
sbit RST=P2^4;  //DS1302 CE
sbit bt1=P2^5;
sbit bt2=P2^6;
sbit alm=P3^0;
sbit PWR=P2^7;
sbit reset=P3^1;

bit blag,rstflag,almflag;
bit pm_flag;
bit odd;

unsigned char i,j,pt,blind,bcount;
unsigned char time[8];
unsigned char nbr[3];
unsigned char AlmCount,counter;

void init(void);
void initLCD(void);
void delay(unsigned int d);
void gotoxy(unsigned char x,unsigned char y);
void putch_lcd(unsigned char ch,bit rs);
void PrintLCD(unsigned char *ch);
void ClearLCD(void);
void WriteClk(unsigned char cmd,unsigned char ch);
void BrstReadClk(unsigned char *ch);
void num2char(unsigned char num,unsigned char *ch);
void UpdateTime(unsigned char *tm);
unsigned char increment(unsigned char n);
void ldelay(void);
void Cal(unsigned char date,unsigned char month,unsigned char year);
unsigned char hex2dec(unsigned char num);

void main(void)
{
	RST=0;
	init();
	
	initLCD();

	while(1)
	{
		if(!reset)
		{
			init();
			while(!reset);
			delay(0xFFF0);
			ClearLCD();	
			WriteClk(0x8E,0x00); //Disable Write Protect
			WriteClk(0x90,0xA5); //Initialize Trickle Charger
			
			WriteClk(0x80,0x00); //Clear Seconds
			WriteClk(0x82,0x00); //Clear Minutes
			WriteClk(0x84,0x00); //Clear Hours
			WriteClk(0x86,0x01); //Clear Date
			WriteClk(0x88,0x01); //Clear Month
			WriteClk(0x8A,0x01); //Clear Day
			WriteClk(0x8C,0x00); //Clear Year

			WriteClk(0x8E,0x80); //Enalble Write Protect
		}

		BrstReadClk(time);
		if(time[0]!=pt)
		{
			pt=time[0];
			UpdateTime(time);
		}
		
		if(!bt1)
		{
			while(!bt1);
			ldelay();
			ldelay();
			if(!blind)
			{
				blag=1;
				putch_lcd(0x0E,0);	//Enable cursor
				WriteClk(0x8E,0x00); //Disable Write Protect
			}
			blind++;
			if(blind==7)
			{
				blag=0;
				blind=0;
				switch(time[4])
				{
					case 0x04:
					case 0x06:
					case 0x09:
					case 0x11:
						if(time[3]==0x31)
							time[3]=0x01;
						break;
					
					case 0x02:
						i=time[6] & 0xF0;
						j=time[6] & 0x0F;
						i=i>>4;
						i=i*10;
						i=i+j;
						j=i & 0x03;
						if(j!=0 && time[3]>0x28)	//Not Leap year
							time[3]=0x01;

						if(j==0 && time[3]>0x29)
							time[3]=0x01;
				}
				WriteClk(0x86,time[3]);
				Cal(time[3],time[4],time[6]);
				UpdateTime(time);
				WriteClk(0x8E,0x80); //Enalble Write Protect
				putch_lcd(0x0C,0);	//Disable Cursor
			}
		}
		
		if(!blag && !bt2)
		{
			ClearLCD();
			gotoxy(0,0);
			PrintLCD("Real Time Clock");
			gotoxy(0,1);
			PrintLCD("By AAMER MUMTAZ");
			for(i=0;i<100;i++)
				delay(0xFF00);
			while(!bt2);
			ClearLCD();
		}

		if(blag)
		{
			switch(blind)
			{
				case 1:
					gotoxy(11,1);	
					if(!bt2)
					{
						ldelay();
						time[0]=increment(time[0]);
						if(time[0]==0x60)
							time[0]=0;
						WriteClk(0x80,time[0]);
					}
					break;

				case 2:
					gotoxy(8,1);
					if(!bt2)
					{
						ldelay();
						time[1]=increment(time[1]);
						if(time[1]==0x60)
							time[1]=0;
						WriteClk(0x82,time[1]);
						UpdateTime(time);
					}
					break;

				case 3:
					gotoxy(6,1);
					if(!bt2)
					{
						ldelay();
						time[2]=increment(time[2]);
						if(time[2]==0x24)
							time[2]=0x00;
						WriteClk(0x84,time[2]);
						UpdateTime(time);
					}
					break;

				case 4:
					gotoxy(12,0);
					if(!bt2)
					{
						ldelay();
						time[6]=increment(time[6]);
						if(time[6]==0x99)
							time[6]=0x00;
						WriteClk(0x8C,time[6]);
						UpdateTime(time);
					 }
					 break;

				case 5:
					gotoxy(8,0);
					if(!bt2)
					{
						ldelay();
						time[4]=increment(time[4]);
						if(time[4]==0x13)
							time[4]=0x01;
						WriteClk(0x88,time[4]);
						UpdateTime(time);
					}
					break;
				
				case 6:
					gotoxy(5,0);
					if(!bt2)
					{
						ldelay();
						time[3]=increment(time[3]);
						switch(time[4])
						{
							case 0x01: 
							case 0x03: 
							case 0x05: 
							case 0x07: 
							case 0x08: 
							case 0x10: 
							case 0x12:
 						    	if(time[3]==0x32)
									time[3]=0x01;
								break;

							case 0x04:
							case 0x06:
							case 0x09:
							case 0x11:
								if(time[3]==0x31)
									time[3]=0x01;
								break;

							case 0x02:
								i=time[6] & 0xF0;
								j=time[6] & 0x0F;
								i=i>>4;
								i=i*10;
								i=i+j;
								j=i & 0x03;
								if(j!=0 && time[3]==0x29)	//Not Leap year
									time[3]=0x01;

								if(j==0 && time[3]==0x30)
									time[3]=0x01;
						}
						WriteClk(0x86,time[3]);
						UpdateTime(time);
					}
				}

		}
	}
}

////////////////////////////--Initialization Function--//////////////////////////////
void init(void)
{
	TMOD=0x11;
	TF0=0;
	TR0=0;
	pt=0xFF;
	blind=0;
	bcount=0;
	blag=0;
	EA=1;
	ET1=1;
	EX0=1;
	almflag=0;
}

void delay(unsigned int d)
{
	d=~d;
	d--;
	TL0=d;
	TH0=d>>8;
	TR0=1;
	while(!TF0);
	TR0=0;
	TF0=0;
}

///////////////////////////--Initialize LCD in 8-bit mode--///////////////////////////////
void initLCD(void)
{
	delay(0xFFFE);
	putch_lcd(0x30,0);
	delay(0x3000);
	putch_lcd(0x30,0);
	delay(300);
	putch_lcd(0x30,0);
	delay(300);

	putch_lcd(0x38,0);	//Set interface length
	putch_lcd(0x10,0);	//Turn off display
	putch_lcd(0x01,0);	//Clear the display
	delay(0x3000);
	putch_lcd(0x06,0);	//Set cursor move direction
	putch_lcd(0x0C,0);	//Enable display cursor
}

////////////////////--Write one Character/Command (rs=0 comamnd, rs=1 character) on LCD--//////////
void putch_lcd(unsigned char ch,bit rs)
{
	E=1;
	P0=ch;
	RS=rs;
	delay(400);
	E=0;
	delay(400);
}

///////////////////////--Move Cursor on LCD--/////////////////////////////////
void gotoxy(unsigned char x,unsigned char y)
{
	unsigned char ch;

	if(x==0 && y==0)
		ch=0x02;
	else
	{
		ch=0x80;
		ch=ch | x;
		if(y==1)
			ch=ch | 0x40;
	}
	putch_lcd(ch,0);
	if(x==0 && y==0)
		delay(0x3000);
}

///////////////////--Write String on LCD--////////////////////////////////
void PrintLCD(unsigned char *ch)
{
	while(*ch)
	{
		putch_lcd(*ch,1);
		ch++;
	}
}

void ClearLCD(void)
{
	putch_lcd(0x01,0);	//Clear the display
	delay(0x3000);
}

//////////////////// Write a character in DS1302 ////////////////////
void WriteClk(unsigned char cmd,unsigned char ch)
{
	unsigned char count,mask,temp,wrd;
	SCLK=0;
	RST=1;
	mask=0x01;
	wrd=cmd;
	
	for(count=0;count<16;count++)
	{
		IO=0;
		temp=wrd & mask;
		if(temp)
			IO=1;
		SCLK=1;
		mask=mask<<1;

		if(count==7)
		{
			wrd=ch;
			mask=0x01;
		}

		SCLK=0;
		//_nop_();
	}

	RST=0;
	SCLK=1;
	IO=1;	
}

/////////////////////// Burst Clock Read /////////////////////////////
void BrstReadClk(unsigned char *ch)
{
	unsigned char count,wrd,mask,temp,i;
	//WriteClk(0x8E,0x00); //Disable Write Protect
	RST=1;
	
	wrd=0xBF;
	mask=0x01;
	for(count=0;count<8;count++)
	{
		IO=0;
		temp=wrd & mask;

		if(temp)
			IO=1;
		SCLK=1;
		mask=mask <<1;
		SCLK=0;
	}

	for(count=0;count<8;count++)
	{
		wrd=0x00;
		for(i=0;i<8;i++)
		{
			if(IO)
				temp=0x01;
			else
				temp=0x00;
			
			SCLK=1;
			temp=temp<<i;
			wrd=wrd | temp;
			SCLK=0;
		}

		*ch++=wrd;			
	}
	WriteClk(0x8E,0x80); //Enable Write Protect
	RST=0;
	SCLK=1;
	IO=1;
}
//////////////////////////--Update Time--//////////////////////////////////////
void UpdateTime(unsigned char *tm)
{	
	unsigned char temp;
	bit h_flag;
	h_flag=0;
	gotoxy(0,0);
	tm+=5;
	temp=*tm;

	switch(temp)
	{
		case 0x01:
			PrintLCD("SAT");	break;
		case 0x02:
			PrintLCD("SUN");	break;
		case 0x03:
			PrintLCD("MON");	break;
		case 0x04:
			PrintLCD("TUE");	break;
		case 0x05:
			PrintLCD("WED");	break;
		case 0x06:
			PrintLCD("THU");	break;
		case 0x07:
			PrintLCD("FRI");	break;
	}
	PrintLCD("  ");

	tm-=2;
	temp=*tm;

	num2char(temp,nbr);
	PrintLCD(nbr);	//Date

	tm++;
	temp=*tm;
	switch(temp)
	{
		case 0x01:
			PrintLCD(" JAN "); 	break;
		case 0x02:
			PrintLCD(" FEB ");	break;
		case 0x03:
			PrintLCD(" MAR ");	break;
		case 0x04:
			PrintLCD(" APR "); 	break;
		case 0x05:
			PrintLCD(" MAY ");	break;
		case 0x06:
			PrintLCD(" JUN ");	break;
		case 0x07:
			PrintLCD(" JUL ");	break;
		case 0x08:
			PrintLCD(" AUG ");	break;
		case 0x09:
			PrintLCD(" SEP ");	break;
		case 0x10:
			PrintLCD(" OCT "); 	break;
		case 0x11:
			PrintLCD(" NOV "); 	break;
		case 0x12:
			PrintLCD(" DEC ");	break;

		default:
			PrintLCD(" ERR ");
	}

	tm+=2;
	temp=*tm;
	num2char(temp,nbr);
	PrintLCD("20");
	PrintLCD(nbr);

	gotoxy(5,1);
	tm-=4;
	temp=*tm;
	if(temp==0x00)
	{		
		temp=0x12;
		pm_flag=0;
	}
	else
	{
		if(temp<0x12)
			pm_flag=0;
		if(temp==0x12)
			pm_flag=1;
		if(temp>0x12)
		{
			temp-=0x12;
			pm_flag=1;
		}
	}
	if(temp==0x0E)
		temp=0x08;
	if(temp==0x0F)
		temp=0x09;
	num2char(temp,nbr);
   	PrintLCD(nbr);	//Hour
	if(temp<0x10)
		h_flag=1;

	PrintLCD(":");
	tm--;
	temp=*tm;
	num2char(temp,nbr);
	PrintLCD(nbr);
	PrintLCD(":");
	tm--;
	temp=*tm;
	num2char(temp,nbr);
	PrintLCD(nbr);	//Seconds

	if(pm_flag)
		PrintLCD(" PM");
	else
		PrintLCD(" AM");

	if(h_flag)
	{
		gotoxy(5,1);
		PrintLCD(" ");
	}

	tm++;
	temp=*tm;
	if(temp==0x00 && !almflag)
	{
		almflag=1;
		tm++;
		AlmCount=hex2dec(*tm);
		counter=0;
		TL1=0;
		TH1=0;
		TR1=1;
	}
}

void num2char(unsigned char num,unsigned char *ch)
{
	unsigned char temp;
	temp=num & 0xF0;
	temp=temp>>4;
	*ch=temp+48;
	ch++;
	temp=num & 0x0F;
	*ch=temp+48;
}

unsigned char increment(unsigned char n)
{
	unsigned char t,u;
	u=n & 0x0F;
	t=n>>4;
	u++;
	if(u>9)
	{
		t++;
		u=0;
	}
	t=t<<4;
	t=t | u;
	return t;
}

void ldelay(void)
{
	for(i=0;i<5;i++)
		delay(0xFFF0);
}

///////////////--Calander function (accepts date, month and year and write day in DS1302)--///////
void Cal(unsigned char date,unsigned char month,unsigned char year)
{
	unsigned char t2,t3;
	year=hex2dec(year);
	date=hex2dec(date);
	
	switch(month)
	{
		case 0x10:
		case 0x01:	t2=0;	break;
		case 0x11:
		case 0x02:
		case 0x03:	t2=3;	break;
		case 0x07:
		case 0x04:	t2=6;	break;
		case 0x05:	t2=1;	break;
		case 0x06:	t2=4;	break;
		case 0x08:	t2=2;	break;
		case 0x12:
		case 0x09:	t2=5;	break;
	}
	if(month>0x02)
	{
		if(year==0 || year>3)
		{
			t3=year & 0x03;
			if(t3==0)
				t2++;
		}
	}
	t2=t2+year+date;
	if(year==0)
		t2--;
	if(t2>6)
		t2=t2%7;
	t2++;
	WriteClk(0x8A,t2);
}

unsigned char hex2dec(unsigned char num)
{
	unsigned char hexnum;
	i=num & 0xF0;
	i=i>>4;
	i=i*10;
	j=num & 0x0F;
	hexnum=i+j;

	return hexnum;
}

void timer1(void) interrupt 3
{
		
}


		
	
	



	
