

//*********************************************************************************************************************//
//    Project      : NexTek Service
//    File         : uar_sm.v
//    Author       : NexTek Service
//    Company      : NexTek Service
//    Start Date   : Jan 06th, 2004.
//    Last Updated : Feb 17th, 2004.
//    Version      : Final
//    Device	   : xc2s150-5pq208 
//    PROM	   : xcf01s
//    Abstract     : State machine for Rx
// 
//    Modification History:
//==========================================================================================
//    Date                       By                 Version                Change Description
//==========================================================================================
//    Feb 17th, 2004          NexTek Service         Final                 Original Version
//******************************************************************************************//

module uar_sm(// Inputs
              clk,
              rst_n,
              din_rdy,
              shift_count,
	      count_sample,
              // Outputs
              start_bit_sig,
              data_bits_sig,
              stop_bit_sig
             );  

// Inputs
input       clk ;
input       rst_n ;
input       din_rdy ;
input [3:0] shift_count ;
input [3:0] count_sample;

// Outputs
output      start_bit_sig ;
output      data_bits_sig ;
output      stop_bit_sig ;

// Register and Wire Declaration 

reg   [3:0] state ;

wire      start_bit_sig ;
wire      data_bits_sig ;
wire      stop_bit_sig ;

parameter [3:0] IDLE          = 4'b0001 ;
parameter [3:0] START_BIT_ST  = 4'b0010 ;
parameter [3:0] DATA_BITS_ST  = 4'b0100 ;
parameter [3:0] STOP_BIT_ST   = 4'b1000 ;

// State Register and Next State Logic
always @(posedge clk or negedge rst_n)
begin
  if(~rst_n) 
  begin
     state <= IDLE ;
  end   
  else 
  begin
    case(state)
        IDLE         : begin
                          if(din_rdy)
                             state <= START_BIT_ST ;
                          else     
                             state <= IDLE ;
                        end     
                        
         START_BIT_ST : begin
			if(shift_count == 4'd1)
                             state <= DATA_BITS_ST ;
                          else     
                             state <= START_BIT_ST ;
                        end
                        
         DATA_BITS_ST : begin
			  if(shift_count == 4'd9)
                            state <= STOP_BIT_ST ;
                          else     
                            state <= DATA_BITS_ST ;
                        end
                        
         STOP_BIT_ST  : begin
			  if(count_sample == 4'd9)
                           state <= IDLE ;
                          else     
                           state <= STOP_BIT_ST ;
                        end     
         default      : begin
                           state <= IDLE ;
                        end   
      endcase
   end   
end

assign  start_bit_sig = (state == START_BIT_ST) ? 1'b1 : 1'b0 ;
assign  data_bits_sig = (state == DATA_BITS_ST) ? 1'b1 : 1'b0 ;
assign  stop_bit_sig  = (state == STOP_BIT_ST) ? 1'b1 : 1'b0 ;

endmodule