`timescale 1ns / 1ps
`ifndef DUT_TB_VCD
`define DUT_TB_VCD "dut_tb.vcd"
`endif
module dut_tb;
    localparam clk_period = 10;

    reg clk = 0;
    reg reset = 1;
    reg [4:0] nbits = 0;
    reg [31:0] mosi_data = 0;
    reg request = 0;
    wire ready;

    wire      spi_cen;
    wire      spi_scl;
    wire      spi_sdi;
    wire      spi_sdo;

    initial begin
        forever begin
            clk = 0;
            #(clk_period / 2);
            clk = 1;
            #(clk_period / 2);
        end
    end

    initial begin
        reset = 1;
        $dumpfile(`DUT_TB_VCD);
        $dumpvars(0, dut_tb);
        #50       reset = 0;
        nbits = 15;
        mosi_data = 32'h8f00;
        #50       request = 1;
        #10 request = 0;
        wait (ready)
        #200
        $finish;
    end

    spi_master spi_master1(
        .clk_in(clk),
        .nrst(!reset),
        .nbits(nbits),
        .mosi_data(mosi_data),
        .ready(ready),
        .request(request),
        .spi_cen(spi_cen),
        .spi_scl(spi_scl),
        .spi_sdi(spi_sdi),
        .spi_sdo(spi_sdo)
    );
    lis3dh_stub lis3dh_stub1(
        .clk(clk),
        .csn(spi_cen),
        .sck(spi_scl),
        .mosi(spi_sdi),
        .miso(spi_sdo)
    );
endmodule
