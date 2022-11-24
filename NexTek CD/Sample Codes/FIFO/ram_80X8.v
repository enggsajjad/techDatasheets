//*******************************************************************************************//
//    Project      : RAM_80X8
//    File         : ram_80x8.v
//    Author       : NexTek Service
//    Company      : NexTek Service
//    Start Date   : Jan 06th, 2004.
//    Last Updated : Feb 17th, 2004.
//    Version      : Final
//    Abstract     : Single Clock Dual Port RAM_80x8
//                   
//    Modification History:
//==========================================================================================
//    Date                       By                 Version                Change Description
//==========================================================================================
//    Feb 17th, 2004          NexTek Service         Final                 Original Version
//******************************************************************************************//
// Design Block
module ram_80x8(// Inputs
                 clk,           // Input Clock Signal
                 wrt_sig,       // input Write Signal
                 addr_w,        // Write Address
                 addr_r,        // Read  Address
                 din_ram,       // input 8 bit Data
                 // Output
                 dout_ram       // output 8 bit data
               );
// Inputs
input        clk ;
input        wrt_sig ;
input [6:0]  addr_w ;
input [6:0]  addr_r ;
input [7:0]  din_ram ;
// Output
output [7:0] dout_ram ;  

parameter [6:0] DEPTH = 7'd80 ;
parameter [3:0] WIDTH = 4'd8 ;

reg [WIDTH-1:0] FIFO_MEM[0:DEPTH-1] ;

always @(posedge clk)
begin
  if(wrt_sig)
     FIFO_MEM[addr_w] <= #1 din_ram ;
end

assign #1 dout_ram = FIFO_MEM[addr_r] ;

endmodule