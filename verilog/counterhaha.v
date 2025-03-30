
module SevenSegmentDisplayDecoder(ssOut, nIn);
  output reg [6:0] ssOut;
  input [4:0] nIn;
 
  // ssOut format {g, f, e, d, c, b, a}
 
  always @(nIn)
    case (nIn)
      4'h0: ssOut = 7'b0111111;
      4'h1: ssOut = 7'b0000110;
      4'h2: ssOut = 7'b1011011;
      4'h3: ssOut = 7'b1001111;
      4'h4: ssOut = 7'b1100110;
      4'h5: ssOut = 7'b1101101;
      4'h6: ssOut = 7'b1111101;
      4'h7: ssOut = 7'b0000111;
      4'h8: ssOut = 7'b1111111;
      4'h9: ssOut = 7'b1100111;
      4'hA: ssOut = 7'b1110111;
      4'hB: ssOut = 7'b1111100;
      4'hC: ssOut = 7'b0111001;
      4'hD: ssOut = 7'b1011110;
      4'hE: ssOut = 7'b1111001;
      4'hF: ssOut = 7'b1110001;
      default: ssOut = 7'b1001001;
    endcase
endmodule

module paobiao(clk_in,MsCounter,CLR,REC,START,MSH,MSL,SH,SL,MH,ML,MSH1,MSL1,SH1,SL1,MH1,ML1,MSH2,MSL2,SH2,SL2,MH2,ML2, out1, out2, out2, out3, out4, out5, out6, out7, out8);  
      input clk_in,CLR,REC;
      input START;
      output [4:0] MsCounter, MSH, MSL, SH, SL, MH, ML;
      reg [4:0] MSH,MSL,SH,SL,MH,ML;
      reg [18:0] MsCounter;
      reg CLK,cn1,cn2;                                //cn1: ms to s; cn2: s to m
      reg [1:0]WHO;
      //we make another registers to memorize the two time sets
      output [4:0] MSH1, MSL1, SH1, SL1, MH1, ML1;
      reg [4:0] MSH1,MSL1,SH1,SL1,MH1,ML1;
      output [4:0] MSH2, MSL2, SH2, SL2, MH2, ML2;
      reg [4:0] MSH2,MSL2,SH2,SL2,MH2,ML2;      
      output [6:0] out1, out2, out3, out4, out5, out6, out7, out8;
      //initial only happens at time0
      initial begin
        {MSH, MSL, SH, SL, MH, ML, cn1, cn2, MsCounter, CLK}<=7'd0;
        {MSH1,MSL1,SH1,SL1,MH1,ML1}<=4'd0;
        {MSH2,MSL2,SH2,SL2,MH2,ML2}<=4'd0; 
        WHO<=1'd1;
      end
      //reset
      always @(CLR) begin
        MsCounter<=7'd0;
        {MSH,MSL}<=2'd0;
        cn1<=0;
        {SH,SL}<=2'd0;
        cn2<=0;
        {MH,ML}<=2'd0;     
      end
      //record two records
      always @(REC) begin
        if(WHO == 1) begin
          MSH1<=MSH; 
          MSL1<=MSL;
          SH1<=SH;
          SL1<=SL;
          MH1<=MH;
          ML1<=ML;
          WHO<=1'd0;
        end
        else begin
          MSH2<=MSH; 
          MSL2<=MSL;
          SH2<=SH;
          SL2<=SL;
          MH2<=MH;
          ML2<=ML;
          WHO<=1'd1;
        end
      end            
      //add in 1ms if 5*10^5 times of clk_in
      always @(posedge clk_in)begin
           if(START == 1)begin
                 if(MsCounter==500000-1)begin
                   MsCounter<=0;
                   CLK<=1;
                 end
                 else begin
                   MsCounter<=MsCounter + 1;
                   CLK<=0;
                 end
            end
          
      end
      //ms counter, if 100 counts then cn1 += 1
      always @(posedge CLK) begin
              if(MSL==9) begin                 //if MSL == 9 then add another count => MSL = 0
                   MSL<=0;
                   if(MSH==9) begin
                              MSH<=0;             //if MSL == MSH == 9
                              cn1<=1;        
                        end
                   else                      //else MSH != 9
                              MSH<=MSH+1;  
              end
              else begin                    //else MSL != 9
                MSL<=MSL+1;   
                cn1<=0;        
              end
              
             
              
      end
     
      //s counter, if 60 counts then cn2 += 1
      always @(posedge cn1)begin //here we check once cn1 changes or "reset"
              if(SL==9) begin
                  SL<=0;                //if SL == 9
                  if(SH==5) begin
                        SH<=0;            //if SL == 9 and SH == 5
                        cn2<=1;          
                  end
                  else
                        SH<=SH+1;         //else SH != 5
              end
              else begin                  //else SL != 9
                SL<=SL+1;             
                cn2<=0;               
              end
      end
 
      //m counter, if 60 counts then all elements reseted
      always @(posedge cn2)begin //if cn2 changes or "reset" then run it
              if(ML==9) begin       //esle if cn2, and ML == 9  
                  ML<=0;             
                  if(MH==5)
                        MH<=0;
                  else
                        MH<=MH+1;       
              end
              else
                  ML<=ML+1;            //else if cn2, and ML != 9
      end
      
       //print out 7-segment number
              SevenSegmentDisplayDecoder SSDD1(.ssOut(out1), .nIn(MSH));
              SevenSegmentDisplayDecoder SSDD2(.ssOut(out2), .nIn(MSL));
              SevenSegmentDisplayDecoder SSDD3(.ssOut(out3), .nIn(SH));
              SevenSegmentDisplayDecoder SSDD4(.ssOut(out4), .nIn(SL));
              SevenSegmentDisplayDecoder SSDD5(.ssOut(out5), .nIn(MH));
              SevenSegmentDisplayDecoder SSDD6(.ssOut(out6), .nIn(ML));
              //SevenSegmentDisplayDecoder SSDD7(.ssOut(out7), .nIn(HH));
              //SevenSegmentDisplayDecoder SSDD8(.ssOut(out8), .nIn(HL));
endmodule

