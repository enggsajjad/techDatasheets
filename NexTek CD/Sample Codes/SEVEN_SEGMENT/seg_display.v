
//*****************************************************************************************************//
//    Project      : LED_DISPLAY
//    File         : led_display.v
//    Author       : NexTek Service
//    Company      : NexTek Service
//    Start Date   : June 6, 2004
//    Last Updated : June 6, 2004
//    Version      : 1.0
//    Device	   : xc2s150-5pq208 
//    PROM	   : xcf01s
//    Abstract     : This program uses the seven segment display of the board. It can also be used 
//		     for debugging purpose.   
//    Modification History:
//=====================================================================================================
//    Date                       By                        Version                Change Description
//=====================================================================================================
//  June 6, 2004.          NexTek Service                    1.0                   Original Version                            
//*****************************************************************************************************//

// Design Block
module seg_display(//Inputs
                   clk,        
	                rst_n,             
                   //Outputs
                   LED_a,
                   LED_b,
                   LED_c,
                   LED_d,
                   LED_e,
                   LED_f,
                   LED_g,
		   LED_dot,
		   LED_a1,
                   LED_b1,
                   LED_c1,
                   LED_d1,
                   LED_e1,
                   LED_f1,
                   LED_g1,
		   LED_dot1
                  );    

//inputs
input       clk;
input       rst_n;

//outputs
output      LED_a;
output      LED_b;
output      LED_c;
output      LED_d;
output      LED_e;
output      LED_f;
output      LED_g;
output 	    LED_dot;

output      LED_a1;
output      LED_b1;
output      LED_c1;
output      LED_d1;
output      LED_e1;
output      LED_f1;
output      LED_g1;
output	    LED_dot1;

// reg and wire
reg  [22:0] count_div_1Hz;
reg  [3:0]  address_generator;
reg  [23:0] delay_counter;
reg         clk_1Hz;
reg         LED_a;
reg         LED_b;
reg         LED_c;
reg         LED_d;
reg         LED_e;
reg         LED_f;
reg         LED_g;
reg 	    LED_dot;

wire       LED_a1;
wire       LED_b1;
wire       LED_c1;
wire       LED_d1;
wire       LED_e1;
wire       LED_f1;
wire       LED_g1;
wire	   LED_dot1;

assign  LED_a1 = LED_a; 
assign  LED_b1 = LED_b; 
assign  LED_c1 = LED_c; 
assign  LED_d1 = LED_d; 
assign  LED_e1 = LED_e; 
assign  LED_f1 = LED_f; 
assign  LED_g1 = LED_g; 
assign  LED_dot1 = LED_dot; 


//
always@(posedge clk or negedge rst_n)
begin
  if(~rst_n)
  begin
    delay_counter     <= 24'd0;   
    address_generator <= 4'd0 ;    
  end 
  else if (delay_counter == 24'd16000000)
  begin
  	 delay_counter <= 24'd0 ;                // by force reset to obtain mod counter 
    address_generator <= address_generator + 1;  // increment the delay counter 
  end   
  else
  begin
    delay_counter     <= delay_counter + 1;   // increment the delay counter 
    address_generator <= address_generator ;  // increment the delay counter     
  end  
end 

// output logic for Seven Segment Display
always @(posedge clk or negedge rst_n)
begin
  if(~rst_n)
  begin
    LED_a  <= 1'b0;
    LED_b  <= 1'b0;
    LED_c  <= 1'b0;
    LED_d  <= 1'b0;
    LED_e  <= 1'b0;
    LED_f  <= 1'b0;
    LED_g  <= 1'b0;
    LED_dot <=1'b0;
  end   
  else
  begin  
     case({address_generator})
       4'b0000 :
                begin
                  LED_a  <= 1'b1;  // 0 display at seven - segment
                  LED_b  <= 1'b1;
                  LED_c  <= 1'b1;
                  LED_d  <= 1'b1;
                  LED_e  <= 1'b1;
                  LED_f  <= 1'b1;
                  LED_g  <= 1'b0;
   			   LED_dot <=1'b0;
                end       
       4'b0001 :
                begin     
                  LED_a  <= 1'b0;  // 1 display at seven - segment
                  LED_b  <= 1'b1;
                  LED_c  <= 1'b1;
                  LED_d  <= 1'b0;
                  LED_e  <= 1'b0;
                  LED_f  <= 1'b0;
                  LED_g  <= 1'b0;
 		  LED_dot <=1'b0;
                end       
       4'b0010 :          
                begin     
                  LED_a  <= 1'b1;  // 2  display at seven - segment 
                  LED_b  <= 1'b1;
                  LED_c  <= 1'b0;
                  LED_d  <= 1'b1;
                  LED_e  <= 1'b1;
                  LED_f  <= 1'b0;
                  LED_g  <= 1'b1;
		  LED_dot <=1'b0;
                end       
       4'b0011 :          
                begin     
                  LED_a  <= 1'b1;  // 3  display at seven - segment 
                  LED_b  <= 1'b1;
                  LED_c  <= 1'b1;
                  LED_d  <= 1'b1;
                  LED_e  <= 1'b0;
                  LED_f  <= 1'b0;
                  LED_g  <= 1'b1;
		  LED_dot <=1'b0;
                end       
       4'b0100 :          
                begin     
                  LED_a  <= 1'b0;  // 4  display at seven - segment 
                  LED_b  <= 1'b1;
                  LED_c  <= 1'b1;
                  LED_d  <= 1'b0;
                  LED_e  <= 1'b0;
                  LED_f  <= 1'b1;
                  LED_g  <= 1'b1;
		  LED_dot <=1'b0;
                end       
       4'b0101 :          
                begin     
                   LED_a  <= 1'b1;   // 5  display at seven - segment
                   LED_b  <= 1'b0;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b1;
                   LED_e  <= 1'b0;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
		   LED_dot <=1'b0;
                end       
       4'b0110 :          
                begin     
                   LED_a  <= 1'b1;   // 6  display at seven - segment
                   LED_b  <= 1'b0;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b1;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
		   LED_dot <=1'b0;
                end       
       4'b0111 :          
                begin     
                   LED_a  <= 1'b1;   // 7  display at seven - segment
                   LED_b  <= 1'b1;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b0;
                   LED_e  <= 1'b0;
                   LED_f  <= 1'b0;
                   LED_g  <= 1'b0;
		   LED_dot <=1'b0;
                end       
       4'b1000 :          
                begin     
                   LED_a  <= 1'b1;   // 8  display at seven - segment
                   LED_b  <= 1'b1;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b1;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
		   LED_dot <=1'b0;
                end       
       4'b1001 :          
                begin     
                   LED_a  <= 1'b1;   // display 9 at seven - segment
                   LED_b  <= 1'b1;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b0;
                   LED_e  <= 1'b0;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
		   LED_dot <=1'b0;
                end  
		4'b1010 :          
                begin     
                   LED_a  <= 1'b1;   // display A on seven - segment
                   LED_b  <= 1'b1;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b0;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
		   LED_dot <=1'b1;
                end  
		4'b1011 :          
                begin     
                   LED_a  <= 1'b0;   // b  display at seven - segment
                   LED_b  <= 1'b0;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b1;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
		   LED_dot <=1'b1;
                end  
		4'b1100 :          
                begin     
                   LED_a  <= 1'b0;   // c  display at seven - segment
                   LED_b  <= 1'b0;
                   LED_c  <= 1'b0;
                   LED_d  <= 1'b1;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b0;
                   LED_g  <= 1'b1;
		   LED_dot <=1'b1;
                end  
		4'b1101 :          
                begin     
                   LED_a  <= 1'b0;   // d  display at seven - segment
                   LED_b  <= 1'b1;
                   LED_c  <= 1'b1;
                   LED_d  <= 1'b1;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b0;
                   LED_g  <= 1'b1;
						 LED_dot <=1'b1;
                end  
		4'b1110 :          
                begin     
                   LED_a  <= 1'b1;   // e  display at seven - segment
                   LED_b  <= 1'b0;
                   LED_c  <= 1'b0;
                   LED_d  <= 1'b1;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
						 LED_dot <=1'b1;
                end  
		4'b1111 :          
                begin     
                   LED_a  <= 1'b1;   // f  display at seven - segment
                   LED_b  <= 1'b0;
                   LED_c  <= 1'b0;
                   LED_d  <= 1'b0;
                   LED_e  <= 1'b1;
                   LED_f  <= 1'b1;
                   LED_g  <= 1'b1;
						 LED_dot <=1'b1;
                end  
      default :
                begin       
                   LED_a  <= LED_a;  // maintain output LEDs
                   LED_b  <= LED_b;
                   LED_c  <= LED_c;
                   LED_d  <= LED_d;
                   LED_e  <= LED_e;
                   LED_f  <= LED_f;
                   LED_g  <= LED_g;
						 LED_dot <=LED_dot;
                end

      endcase
      end 
end

endmodule