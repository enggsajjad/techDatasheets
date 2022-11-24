
//******************************************************************************************//
//    Project      : Multiplexer 2x1
//    File         : mux2_1.v
//    Author       : NexTek Service
//    Company      : NexTek Service
//    Start Date   : June 1st, 2004
//    Last Updated : June 1st, 2004
//    Version      : Final
//    Abstract     : This module implements the test bench of 2x1 multiplexer.
//    Modification History:
//=========================================================================================
//    Date                      By               Version           Change Description
//=========================================================================================
//   June 1st, 2003.       NexTek Service         Final             Original Version
//******************************************************************************************//

// Stimulus Block
module   tb_mux2_1();
  
reg in1, in2, enb ;
wire out ;

// Generate possible inputs to mux
initial
begin
  in1=1'b0;  // initialize   inputs
  in2=1'b0;
  enb=1'b0;
  //change  input 1 in1 and input 2 in2 ;
  in1=1'b1;
  in2=1'b0; 
  enb=1'b1;
  #1 $display(" in1=%b ,in2=%b, enable=%b, output=%b",in1,in2,enb,out);    
  // change  input 1 in1 and input 2 in2 ;
  in1=1'b0;
  in2=1'b1;
  enb=1'b0;
  #1 $display(" in1=%b ,in2=%b, enable=%b, output=%b",in1,in2,enb,out);    
end

// Instanciate design block here
mux2x1  mux2x1_inst(//inputs
                    in1, 
                    in2, 
                    enb,
                    //outputs
                    out
                    );
endmodule