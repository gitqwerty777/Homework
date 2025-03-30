module calculator (
	input [5:0] a,
	input [5:0] b,
	input [1:0] type,
  output [11:0] s,
);

always @(a or b or type)
begin
	if(type == 0)
		add_2_v add0(a, b, s); 
	else if(type == 1)
		sub_6 sub1(a, b, s);
	else if(type == 2)
		multiply6bits mul2(s, a, b);
end

endmodule 