module adder(input logic [3:0] a, b, output logic [4:0] sum);
  assign sum = a + b;
endmodule

module inst(input bit clk, input logic [3:0] x, y, output logic [4:0] z);
  //logic [3:0] x, y;
  //logic [4:0] z;

  adder add1(.a(x), .b(y), .sum(z));
endmodule
