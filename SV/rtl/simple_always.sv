module simple_always(input logic clk, rst, output logic q);

  always_ff @(posedge clk or posedge rst) begin
    if (rst)
      q <= 0;
    else
      q <= ~q;
  end
endmodule
