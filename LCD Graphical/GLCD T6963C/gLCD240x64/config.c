//---------------------------------------------------------------
// CYGNAL Integrated Products 
//
// C Code Configuration Tool: F020 INITIALIZATION/CONFIGURATION CODE
//----------------------------------------------------------------
// This file is read only. To insert the code into your  
// application, simply cut and paste or use the "Save As" 
// command in the file menu to save the file in your project 
// directory. 
//----------------------------------------------------------------

//----------------------------------------------------------------
// INCLUDES
//----------------------------------------------------------------

#include "C8051F020.h"	// Register definition file.

//------------------------------------------------------------------------------------
// Global CONSTANTS
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Config Routine
//------------------------------------------------------------------------------------
void config (void) {

//Local Variable Definitions
    int n = 0;

	

//----------------------------------------------------------------
// Watchdog Timer Configuration
//
// WDTCN.[7:0]: WDT Control
//   Writing 0xA5 enables and reloads the WDT.
//   Writing 0xDE followed within 4 clocks by 0xAD disables the WDT
//   Writing 0xFF locks out disable feature.
//
// WDTCN.[2:0]: WDT timer interval bits
//   NOTE! When writing interval bits, bit 7 must be a 0.
//
//  Bit 2 | Bit 1 | Bit 0
//------------------------     
//    1   |   1   |   1      Timeout interval = 1048576 x Tsysclk
//    1   |   1   |   0      Timeout interval =  262144 x Tsysclk
//    1   |   0   |   1      Timeout interval =   65636 x Tsysclk
//    1   |   0   |   0      Timeout interval =   16384 x Tsysclk
//    0   |   1   |   1      Timeout interval =    4096 x Tsysclk
//    0   |   1   |   0      Timeout interval =    1024 x Tsysclk
//    0   |   0   |   1      Timeout interval =     256 x Tsysclk
//    0   |   0   |   0      Timeout interval = 	 64 x Tsysclk
// 
//------------------------

	WDTCN = 0x07;	// Watchdog Timer Control Register
    WDTCN = 0xDE;   // Disable WDT
    WDTCN = 0xAD;



//----------------------------------------------------------------
// CROSSBAR REGISTER CONFIGURATION
//
// NOTE: The crossbar register should be configured before any  
// of the digital peripherals are enabled. The pinout of the 
// device is dependent on the crossbar configuration so caution 
// must be exercised when modifying the contents of the XBR0, 
// XBR1, and XBR2 registers. For detailed information on 
// Crossbar Decoder Configuration, refer to Application Note 
// AN001, "Configuring the Port I/O Crossbar Decoder". 
//----------------------------------------------------------------

// Configure the XBRn Registers

	XBR0 = 0x00;	// XBAR0: Initial Reset Value
	XBR1 = 0x00;	// XBAR1: Initial Reset Value
	XBR2 = 0x40;	// XBAR2: Initial Reset Value

// Select Pin I/0

// NOTE: Some peripheral I/O pins can function as either inputs or 
// outputs, depending on the configuration of the peripheral. By default,
// the configuration utility will configure these I/O pins as push-pull 
// outputs.
                      // Port configuration (1 = Push Pull Output)
 //   P0MDOUT = 0x00; // Output configuration for P0 
    P0MDOUT = 0x00; // Output configuration for P0 
    P1MDOUT = 0x00; // Output configuration for P1 
    P2MDOUT = 0x00; // Output configuration for P2 
    P3MDOUT = 0x00; // Output configuration for P3 
    P74OUT = 0x00;  // Output configuration for P4-7

    P1MDIN = 0xFF;  // Input configuration for P1

// View port pinout

		// The current Crossbar configuration results in the 
		// following port pinout assignment:
		// Port 0
//		// P0.0 = GP I/O          (Open-Drain Output/Input)
		// P0.0 = SYSCLK          (Push-Pull Output)
		// P0.1 = GP I/O          (Open-Drain Output/Input)
		// P0.2 = GP I/O          (Open-Drain Output/Input)
		// P0.3 = GP I/O          (Open-Drain Output/Input)
		// P0.4 = GP I/O          (Open-Drain Output/Input)
		// P0.5 = GP I/O          (Open-Drain Output/Input)
		// P0.6 = GP I/O          (Open-Drain Output/Input)
		// P0.7 = GP I/O          (Open-Drain Output/Input)

		// Port 1
		// P1.0 = GP I/O          (Open-Drain Output/Input)(Digital)
		// P1.1 = GP I/O          (Open-Drain Output/Input)(Digital)
		// P1.2 = GP I/O          (Open-Drain Output/Input)(Digital)
		// P1.3 = GP I/O          (Open-Drain Output/Input)(Digital)
		// P1.4 = GP I/O          (Open-Drain Output/Input)(Digital)
		// P1.5 = GP I/O          (Open-Drain Output/Input)(Digital)
		// P1.6 = GP I/O          (Open-Drain Output/Input)(Digital)
		// P1.7 = GP I/O          (Open-Drain Output/Input)(Digital)
					
		// Port 2		
		// P2.0 = GP I/O          (Open-Drain Output/Input)
		// P2.1 = GP I/O          (Open-Drain Output/Input)
		// P2.2 = GP I/O          (Open-Drain Output/Input)
		// P2.3 = GP I/O          (Open-Drain Output/Input)
		// P2.4 = GP I/O          (Open-Drain Output/Input)
		// P2.5 = GP I/O          (Open-Drain Output/Input)
		// P2.6 = GP I/O          (Open-Drain Output/Input)
		// P2.7 = GP I/O          (Open-Drain Output/Input)

		// Port 3		
		// P3.0 = GP I/O          (Push-Pull Output)
		// P3.1 = GP I/O          (Push-Pull Output)
		// P3.2 = GP I/O          (Push-Pull Output)
		// P3.3 = GP I/O          (Open-Drain Output/Input)
		// P3.4 = GP I/O          (Push-Pull Output)
		// P3.5 = GP I/O          (Open-Drain Output/Input)
		// P3.6 = GP I/O          (Push-Pull Output)
		// P3.7 = GP I/O          (Open-Drain Output/Input)

    EMI0CF = 0x30;    // External Memory Configuration Register

//----------------------------------------------------------------
// Comparators Register Configuration
//
// Bit 7  | Bit 6  | Bit 5  | Bit 4  | Bit 3 | Bit 2 | Bit 1 | Bit 0
//------------------------------------------------------------------     
//  R/W	  |    R   |  R/W   |  R/W   |  R/W  |  R/W  |  R/W  |  R/W
//------------------------------------------------------------------
// Enable | Output | Rising | Falling|  Positive     |  Negative    
//        | State  | Edge   | Edge   |  Hysterisis   |  Hysterisis    
//        | Flag   | Int.   | Int.   |  00: Disable  |  00: Disable
//        |        | Flag   | Flag   |  01:  5mV     |  01:  5mV  
//        |        |        |        |  10: 10mV     |  10: 10mV
//        |        |        |        |  11: 20mV     |  11: 20mV 
// ----------------------------------------------------------------

	CPT0CN = 0x00;	// Comparator 0 Control Register
	

	CPT1CN = 0x00;	// Comparator 1 Control Register
	
	//Comp1 marker
					
//----------------------------------------------------------------
// Oscillator Configuration
//----------------------------------------------------------------

	OSCXCN = 0x67;	// EXTERNAL Oscillator Control Register	
    for (;n < 255; n++) ;           // XTLVLD blanking interval (>1ms)
    while ( (OSCXCN & 0x80) == 0 );	// wait for xtal osc to start up

	OSCICN = 0x08;	// Internal Oscillator Control Register

	
//----------------------------------------------------------------
// Reference Control Register Configuration
//----------------------------------------------------------------

	REF0CN = 0x00;	// Reference Control Register

//----------------------------------------------------------------
// ADC Configuration
//----------------------------------------------------------------

	AMX0CF = 0x60;	// AMUX Configuration Register
	AMX0SL = 0x00;	// AMUX Channel Select Register
	ADC0CF = 0xF8;	// ADC Configuraion Register
	ADC0CN = 0x00;	// ADC Control Register
	
	ADC0LTH = 0x00;	// ADC Less-Than High Byte Register
	ADC0LTL = 0x00;	// ADC Less-Than Low Byte Register
	ADC0GTH = 0xFF;	// ADC Greater-Than High Byte Register
	ADC0GTL = 0xFF;	// ADC Greater-Than Low Byte Register

    AMX1SL = 0x00;  // AMUX1 Channel Select Register
    ADC1CF = 0xF8;  // ADC1 Configuration Register
    ADC1CN = 0x00;  // ADC1 Control Register

//----------------------------------------------------------------
// DAC Configuration
//----------------------------------------------------------------

	DAC0CN = 0x00;	// DAC0 Control Register
	DAC0L = 0x00;	// DAC0 Low Byte Register
	DAC0H = 0x00;	// DAC0 High Byte Register

	DAC1CN = 0x00;	// DAC1 Control Register
	DAC1L = 0x00;	// DAC1 Low Byte Register
	DAC1H = 0x00;	// DAC1 High Byte Register

//----------------------------------------------------------------
// SPI Configuration
//----------------------------------------------------------------

	SPI0CN = 0x00;	// SPI Control Register
	SPI0CFG = 0x00;	// SPI Configuration Register
	SPI0CKR = 0x00;	// SPI Clock Rate Register

//----------------------------------------------------------------
// UART Configuration
//----------------------------------------------------------------

    SCON0 = 0x00;       // Serial Port Control Register
    SADEN0 = 0x00;      // Serial 0 Slave Address Enable
    SADDR0 = 0x00;      // Serial 0 Slave Address Register

    PCON = 0x00;        // Power Control Register

    SCON1 = 0x00;       // Serial Port 1 Control Register   
    SADEN1 = 0x00;      // Serial 1 Slave Address Enable
	SADDR1 = 0x00;      // Serial 1 Slave Address Register

//----------------------------------------------------------------
// SMBus Configuration
//----------------------------------------------------------------

	SMB0CN = 0x00;	// SMBus Control Register
	SMB0ADR = 0x00;	// SMBus Address Register
	SMB0CR = 0x00;	// SMBus Clock Rate Register


//----------------------------------------------------------------
// PCA Configuration
//----------------------------------------------------------------

    PCA0MD = 0x00;       // PCA Mode Register
    PCA0CN = 0x00;      // PCA Control Register
    PCA0H = 0x00;       // PCA Counter/Timer High Byte	
    PCA0L = 0x00;       // PCA Counter/Timer Low Byte
	

    //Module 0
    PCA0CPM0 = 0x00;    // PCA Capture/Compare Register 0
    PCA0CPL0 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH0 = 0x00;    // PCA Counter/Timer High Byte

    //Module 1
    PCA0CPM1 = 0x00;    // PCA Capture/Compare Register 1
    PCA0CPL1 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH1 = 0x00;    // PCA Counter/Timer High Byte

    //Module 2
    PCA0CPM2 = 0x00;    // PCA Capture/Compare Register 2
    PCA0CPL2 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH2 = 0x00;    // PCA Counter/Timer High Byte

    //Module 3
    PCA0CPM3 = 0x00;    // PCA Capture/Compare Register 3
    PCA0CPL3 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH3 = 0x00;    // PCA Counter/Timer High Byte

    //Module 4
    PCA0CPM4 = 0x00;    // PCA Capture/Compare Register 4
    PCA0CPL4 = 0x00;    // PCA Counter/Timer Low Byte
    PCA0CPH4 = 0x00;    // PCA Counter/Timer High Byte
	
//----------------------------------------------------------------
// Timer Configuration
//----------------------------------------------------------------

    CKCON = 0x00;   // Clock Control Register
    TH0 = 0x00;     // Timer 0 High Byte
    TL0 = 0x00;     // Timer 0 Low Byte
    TH1 = 0x00;     // Timer 1 High Byte
    TL1 = 0x00;	    // Timer 1 Low Byte
    TMOD = 0x00;    // Timer Mode Register
    TCON = 0x00;    // Timer Control Register 

    RCAP2H = 0x00;  // Timer 2 Capture Register High Byte
    RCAP2L = 0x00;  // Timer 2 Capture Register Low Byte	
    TH2 = 0x00;     // Timer 2 High Byte	
    TL2 = 0x00;     // Timer 2 Low Byte	
    T2CON = 0x00;   // Timer 2 Control Register
		
    TMR3RLL = 0x00; // Timer 3 Reload Register Low Byte
    TMR3RLH = 0x00; // Timer 3 Reload Register High Byte
    TMR3H = 0x00;   // Timer 3 High Byte
    TMR3L = 0x00;   // Timer 3 Low Byte
    TMR3CN = 0x00;  // Timer 3 Control Register

    RCAP4H = 0x00;  // Timer 4 Capture Register High Byte
    RCAP4L = 0x00;  // Timer 4 Capture Register Low Byte
    TH4 = 0x00;     // Timer 4 High Byte
    TL4 = 0x00;     // Timer 4 Low Byte
    T4CON = 0x00;   // Timer 4 Control Register

//----------------------------------------------------------------
// Reset Source Configuration
//
// Bit 7  | Bit 6  | Bit 5  | Bit 4  | Bit 3 | Bit 2 | Bit 1 | Bit 0
//------------------------------------------------------------------     
//    R	 |   R/W  |  R/W   |  R/W   |   R   |   R   |  R/W  |  R
//------------------------------------------------------------------
//  JTAG  |Convert | Comp.0 | S/W    | WDT   | Miss. | POR   | HW
// Reset  |Start   | Reset/ | Reset  | Reset | Clock | Force | Pin
// Flag   |Reset/  | Enable | Force  | Flag  | Detect| &     | Reset
//        |Enable  | Flag   | &      |       | Flag  | Flag  | Flag
//        |Flag    |        | Flag   |       |       |       |
//------------------------------------------------------------------
// NOTE! : Comparator 0 must be enabled before it is enabled as a 
// reset source.
//
// NOTE! : External CNVSTR must be enalbed through the crossbar, and
// the crossbar enabled prior to enabling CNVSTR as a reset source 
//------------------------------------------------------------------

	RSTSRC = 0x00;	// Reset Source Register


//----------------------------------------------------------------
// Interrupt Configuration
//----------------------------------------------------------------

    IE = 0x00;          //Interrupt Enable
    IP = 0x00;          //Interrupt Priority
    EIE1 = 0x00;        //Extended Interrupt Enable 1
    EIE2 = 0x00;        //Extended Interrupt Enable 2
    EIP1 = 0x00;        //Extended Interrupt Priority 1
    EIP2 = 0x00;        //Extended Interrupt Priority 2

	

// other initialization code here...



}   //End of config
