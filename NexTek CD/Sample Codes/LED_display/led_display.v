
//*****************************************************************************************************//
//    Project      : LED_DISPLAY
//    File         : led_display.v
//    Author       : NexTek Service
//    Company      : NexTek Service
//    Start Date   : June 6, 2004
//    Last Updated : June 6, 2004
//    Version      : 1.0
//    Abstract     : This program is used to blink two LEDs (one at a time).
//                   and the delay between each state is 12.55 msec (with 16 MHz oscillator).   
//    Modification History:
//=====================================================================================================
//    Date                       By                         Version                Change Description
//=====================================================================================================
//  June 6, 2004.           NexTek Service                    1.0                   Original Version                            
//*****************************************************************************************************//

// Design Block 
module led_display(//Inputs
                   clk,        //  16MHz Crystal
                   rst_n,             
                   //Outputs
                   LED_1,
                   LED_2
                  );    

//inputs
input         clk;
input         rst_n;

//outputs
output        LED_1;
output        LED_2;

// reg and wire
reg          LED_1;
reg          LED_2;

reg [15:0] delay_counter;

// Main Design for Toggling Two LEDS with 12.5 msec second delay  
always @(posedge clk or negedge rst_n) 
begin
  if(~rst_n)
  begin
    LED_1 <= 1'b1;            // on reset state This LED will High
    LED_2 <= 1'b0;            // on reset state This LED will Low
    delay_counter <= 16'd0;   // reset of delay counter   
  end  
  else if (delay_counter == 16'd200000)     // 10 msec pin low , mod counter
  begin
    LED_1 <= ~LED_1;                        // just to toggle LED_1 at this state
    LED_2 <= ~LED_2;                        // just to toggle LED_2 at this state
    delay_counter  <= 16'd0 ;               // by force reset to obtain mod counter 
  end    
  else
  begin
    LED_1 <= LED_1;                         // maintain the status of LED_1 
    LED_2 <= LED_2;                         // maintain the status of LED_2 
    delay_counter <= delay_counter + 1;     // increment the delay counter 
  end  
end 

endmodule