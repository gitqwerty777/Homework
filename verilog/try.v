module sub_6 (
	input [5:0] a,
	input [5:0] b,
	input       ci,
	input [5:0] two_comp,
  output [11:0] s,
	output      co
);

wire [4:0] carry;

function fa_s(input x, input y, input z); 
		fa_s = x ^ y ^ z;
endfunction 

function fa_co(input x, input y, input z);
		fa_co = x & y | x & z | y & z;
endfunction 

assign two_comp = ~b + 1;
assign s[0]     = fa_s (a[0], two_comp[0], ci);
assign carry[0] = fa_co(a[0], two_comp[0], ci);

assign s[1]     = fa_s (a[1], two_comp[1], carry[0]);
assign carry[1] = fa_co(a[1], two_comp[1], carry[0]);

assign s[2]     = fa_s (a[2], two_comp[2], carry[1]);
assign carry[2] = fa_co(a[2], two_comp[2], carry[1]);

assign s[3]     = fa_s (a[3], two_comp[3], carry[2]);
assign carry[3] = fa_co(a[3], two_comp[3], carry[2]);

assign s[4]     = fa_s (a[4], two_comp[4], carry[3]);
assign carry[4] = fa_co(a[4], two_comp[4], carry[3]);   

assign s[5]     = fa_s (a[5], two_comp[5], carry[4]);
assign co       = fa_co(a[5], two_comp[5], carry[4]);   

endmodule 

module HA(sout,cout,a,b); 
output sout,cout; 
input a,b; 
assign sout=a^b; 
assign cout=(a&b); 
endmodule 
 
module FA(sout,cout,a,b,cin); 
output sout,cout; 
input a,b,cin; 
assign sout=(a^b^cin); 
assign cout=((a&b)|(a&cin)|(b&cin)); 
endmodule 
 
module multiply6bits(product,inp1,inp2); 
output [11:0]product; 
input [5:0]inp1; 
input [5:0]inp2; 
assign product[0]=(inp1[0]&inp2[0]); 
wire c1_1, c2_1, c3_1, c4_1, c5_1, c6_1, c2_2, c3_2, c4_2, c5_2, c6_2, c7_2, c3_3, c4_3, c5_3, c6_3, c7_3, c8_3;
wire c4_4, c5_4, c6_4, c7_4, c8_4, c9_4, c5_5, c6_5, c7_5, c8_5, c9_5;
wire x2_1, x3_1, x4_1, x5_1, x6_1, x3_2, x4_2, x5_2, x6_2, x7_2, x4_3, x5_3, x6_3, x7_3, x8_3;
wire x5_4, x6_4, x7_4, x8_4, x9_4;
//run1
HA HA1(product[1], c1_1, inp1[1]&inp2[0], inp1[0]&inp2[1]);
FA FA1(x2_1, c2_1, inp1[2]&inp2[0], inp1[1]&inp2[1], c1_1);
FA FA2(x3_1, c3_1, inp1[3]&inp2[0], inp1[2]&inp2[1], c2_1);
FA FA3(x4_1, c4_1, inp1[4]&inp2[0], inp1[3]&inp2[1], c3_1);
FA FA4(x5_1, c5_1, inp1[5]&inp2[0], inp1[4]&inp2[1], c4_1);
HA HA2(x6_1, c6_1, c5_1, inp1[5]&inp2[1]);
//run2
HA HA3(product[2], c2_2, x2_1, inp1[0]&inp2[2]);
FA FA5(x3_2, c3_2, x3_1, inp1[1]&inp2[2], c2_2);
FA FA6(x4_2, c4_2, x4_1, inp1[2]&inp2[2], c3_2);
FA FA7(x5_2, c5_2, x5_1, inp1[3]&inp2[2], c4_2);
FA FA8(x6_2, c6_2, x6_1, inp1[4]&inp2[2], c5_2);
FA FA9(x7_2, c7_2, c6_2, inp1[5]&inp2[2], c6_1);
//run3
HA HA4(product[3], c3_3, x3_2, inp1[0]&inp2[3]);
FA FA10(x4_3, c4_3, x4_2, inp1[1]&inp2[3], c3_3);
FA FA11(x5_3, c5_3, x5_2, inp1[2]&inp2[3], c4_3);
FA FA12(x6_3, c6_3, x6_2, inp1[3]&inp2[3], c5_3);
FA FA13(x7_3, c7_3, x7_2, inp1[4]&inp2[3], c6_3);
FA FA14(x8_3, c8_3, c7_3, inp1[5]&inp2[3], c7_2);
//run4
HA HA5(product[4], c4_4, x4_3, inp1[0]&inp2[4]);
FA FA15(x5_4, c5_4, x5_3, inp1[1]&inp2[4], c4_4);
FA FA16(x6_4, c6_4, x6_3, inp1[2]&inp2[4], c5_4);
FA FA17(x7_4, c7_4, x7_3, inp1[3]&inp2[4], c6_4);
FA FA18(x8_4, c8_4, x8_3, inp1[4]&inp2[4], c7_4);
FA FA19(x9_4, c9_4, c8_4, inp1[5]&inp2[4], c8_3);
//run5
HA HA6(product[5], c5_5, x5_4, inp1[0]&inp2[5]);
FA FA20(product[6], c6_5, x6_4, inp1[1]&inp2[5], c5_5);
FA FA21(product[7], c7_5, x7_4, inp1[2]&inp2[5], c6_5);
FA FA22(product[8], c8_5, x8_4, inp1[3]&inp2[5], c7_5);
FA FA23(product[9], c9_5, x9_4, inp1[4]&inp2[5], c8_5);
FA FA24(product[10], product[11], c9_5, inp1[5]&inp2[5], c9_4);

endmodule 

module add_2_v (
	input [5:0] a,
	input [5:0] b,
	input       ci,
	output [11:0] s,
	output      co
);

wire [4:0] carry;

function fa_s(input x, input y, input z); 
		fa_s = x ^ y ^ z;
endfunction 

function fa_co(input x, input y, input z);
		fa_co = x & y | x & z | y & z;
endfunction 

assign s[0]     = fa_s (a[0], b[0], ci);
assign carry[0] = fa_co(a[0], b[0], ci);

assign s[1]     = fa_s (a[1], b[1], carry[0]);
assign carry[1] = fa_co(a[1], b[1], carry[0]);

assign s[2]     = fa_s (a[2], b[2], carry[1]);
assign carry[2] = fa_co(a[2], b[2], carry[1]);

assign s[3]     = fa_s (a[3], b[3], carry[2]);
assign carry[3] = fa_co(a[3], b[3], carry[2]);

assign s[4]     = fa_s (a[4], b[4], carry[3]);
assign carry[4] = fa_co(a[4], b[4], carry[3]);   

assign s[5]     = fa_s (a[5], b[5], carry[4]);
assign co       = fa_co(a[5], b[5], carry[4]);   

endmodule

 

