
//******************************************************************************************//
//    Project      : Multiplexer 2x1
//    File         : mux2_1.v
//    Author       : NexTek Service
//    Company      : NexTek Service
//    Start Date   : June 1st, 2004
//    Last Updated : June 1st, 2004
//    Version      : Final
//    Abstract     : This module implements 2x1 multiplexer.
//    Modification History:
//=========================================================================================
//    Date                      By               Version           Change Description
//=========================================================================================
//   June 1st, 2004.       NexTek Service         Final             Original Version
//******************************************************************************************//

// Design Block
module  mux2x1 (//inputs
                in1, 
                in2, 
                enb,
                //outputs
                out
                );

// inputs and outputs declaration
input   in1;
input   in2; 
input   enb;

output out ;

wire out ;

assign out = (enb) ? in1 : in2 ;

endmodule