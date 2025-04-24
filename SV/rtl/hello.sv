module hello(input bit clk);

    bit             b=1;            // 1-bit
    logic           l=1'bz;         // 1-bit, 4-state
    logic [7:0]     octet = 8'ha5;  // 8-bit logic
    int             i=42;           // 32-bit signed
    real            r=3.14;         // Floating-point

    initial begin
        $display("Hello, SystemVerilog!");
        $finish;
    end
endmodule
