
//*****************************************************************************************//
//    Project      : Clock Divider
//    File         : clkdiv.v
//    Author       : NexTek Service
//    Company      : NexTek Service
//    Start Date   : Apr 06th, 2003.
//    Last Updated : Apr 06th  2003
//    Version      : 1.0
//    Abstract     : This module implements the Clock Divider.                                
//    Modification History:
//==========================================================================================
//    Date                       By                 Version              Change Description
//==========================================================================================
//   June 04th  2003        NexTek Service            1.0                  Original Version
//******************************************************************************************//

// module
module clkdiv (//>>>>>>>>INPUTS>>>>>>>>
	       clk16MHz, 
	       reset, 
	       //<<<<<<<<OUTPUTS<<<<<<<
	       clk2MHz, 
               clk1MHz
	      ) ;

input clk16MHz ;
input reset ;

output clk2MHz ;
output clk1MHz ;

reg      clk2MHz ;
reg      clk1MHz ;
reg [3:0]counter;


// This module generates 2 MHz clock by counting the negedge of 16 MHz clock.
always @ (negedge clk16MHz or posedge reset)
begin
  if(reset)
  begin
    clk2MHz = 1;
    counter = 0;
  end
  else
  begin
    counter = counter + 1;
    if(counter == 4) // Over Flow value for the clock to generate 2 MHz
    begin
      clk2MHz = ~clk2MHz;
      counter = 0 ;		
    end
  end	
end

// This module generats 1 MHz Clock by using 2 MHz clock.
always @ (posedge clk2MHz or posedge reset)
begin
  if(reset)
    clk1MHz = 1;
  else
    clk1MHz = ~clk1MHz;
end

endmodule