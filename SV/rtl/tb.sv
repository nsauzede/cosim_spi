module tb(input logic clk);
  //always #5 clk = ~clk;

  initial begin
    $dumpfile("Vtb.vcd");
    $dumpvars;
    //#100 
    $finish;
  end
endmodule
