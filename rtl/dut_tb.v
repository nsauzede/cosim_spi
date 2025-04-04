`timescale 1ns / 1ps
`ifndef DUT_VCD
`define DUT_VCD "dut.vcd"
`endif
module dut_tb;
    // Parameters
    parameter integer BOARD_CK = 32000000;
    parameter integer SPI_DIV_COEF = 32'd1;

    localparam clk_period = 10;

    reg clk = 0;
    reg rx = 1;
    wire tx;
    reg reset = 1;
    wire [7:0] leds;
`ifdef SIMULATION
    wire x_l_flag;
    wire [15:0] x_l_response;
`endif

    dut #(.SPI_DIV_COEF(SPI_DIV_COEF)) dut1 (
`ifdef SIMULATION
        .x_l_flag(x_l_flag),
        .x_l_response(x_l_response),
`endif
        .rx(rx),
        .tx(tx),
        .spi_miso(spi_miso),
        .spi_mosi(spi_mosi),
        .spi_csn(spi_csn),
        .spi_sck(spi_sck),
        .leds(leds),
        .reset(reset),
        .clk(clk)
    );
    lis3dh_stub lis3dh_stub0 (
`ifdef SIMULATION
        .out_x_l_flag(x_l_flag),
        .out_x_resp(x_l_response),
`endif
        .clk(clk),
        .sck(spi_sck),
        .csn(spi_csn),
        .mosi(spi_mosi),
        .miso(spi_miso)
    );

    // Clock generation
    initial begin
        forever begin
            clk = 1'b0;
            #(clk_period / 2);
            clk = 1'b1;
            #(clk_period / 2);
        end
    end

    // Stimulus process
    initial begin
        reset = 1'b1;
        $dumpfile(`DUT_VCD);
        $dumpvars(0, dut_tb);
        #1e3       reset = 1'b0;
//        #10       reset = 1'b0;

        // Wait more and finish
        //#510695
        //#51069
        #50565
//        #20
        $finish;
    end

    // monitor process
    initial begin
        forever begin
            wait (~spi_csn);
            //$display("spi_csn=%d leds=%02x at time %0t", spi_csn, leds, $time);
            wait (spi_csn);
            //$display("spi_csn=%d leds=%02x at time %0t", spi_csn, leds, $time);
        end
    end

endmodule
