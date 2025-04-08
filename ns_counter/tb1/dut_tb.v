`timescale 1ns / 1ps
`ifndef DUT_TB_VCD
`define DUT_TB_VCD "dut_tb.vcd"
`endif

/*
How to test:
- undefine `SPI3WIRE => set only 4-wire (standard) mode
- define `SPI3WIRE but not `SPI3WIREACTIVE => set 3-wire support, but always use 4-wire mode
- define `SPI3WIRE and `SPI3WIREACTIVE => set 3-wire support and dynamically switch b/w 3/4-wire modes
*/
// This will show how to change the flexible divider coefficient
`define SPI_CHANGE_COEF

module dut_tb;
    localparam clk_period = 10;

    reg clk = 0;
    reg reset = 1;
    reg [4:0] nbits = 0;
    reg [31:0] mosi_data = 0;
    reg request = 0;
    wire ready;
`ifdef SPI3WIRE
    reg spi3w = 0;
`endif

    wire      spi_csn;
    wire      spi_sck;
    wire      spi_mosi;
    wire      spi_miso;

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
`ifdef SPI_CHANGE_COEF
        #5
        request = 1;
        nbits = 0;
        mosi_data = 16'd0;      // can use arbitrary divider coefficents here
        #5
        request = 0;
        #5
        mosi_data = 0;
`endif
        #50       reset = 0;

`ifdef SPI3WIRE
`ifdef SPI3WIREACTIVE
        // By default, both master and slave are in 4-wire mode (spi3w==0)
        // write SIM=1 (01h) in CTRL_REG4 (23h) to set slave to 3-wire mode
        nbits = 15;
        mosi_data = 32'h2301;
        #50       request = 1;
        #10 request = 0;
        wait (ready)
        spi3w = 1;      // switch the master to 3-wire mode
`endif
`endif
        // 3 or 4-wire whoami
        nbits = 15;
        mosi_data = 32'h8f00;
        #50       request = 1;
        #10 request = 0;
        wait (ready)

`ifdef SPI3WIRE
`ifdef SPI3WIREACTIVE
        spi3w = 0;      // switch the master back to 4-wire mode
        // write SIM=0 (00h) in CTRL_REG4 (23h) to set slave back to 4-wire mode
        nbits = 15;
        mosi_data = 32'h2300;
        #50       request = 1;
        #10 request = 0;
        wait (ready)

        // 4-wire whoami
        nbits = 15;
        mosi_data = 32'h8f00;
        #50       request = 1;
        #10 request = 0;
        wait (ready)
`endif
`endif

        #200
        $finish;
    end

`ifdef SPI_CHANGE_COEF
    spi_master #(.DIV_COEF(0)) spi_master1 (
`else
    spi_master #(.DIV_COEF(1)) spi_master1 (
`endif
        .clk_in(clk),
        .nrst(!reset),
        .nbits(nbits),
        .mosi_data(mosi_data),
        .ready(ready),
        .request(request),
        .spi_csn(spi_csn),
        .spi_sck(spi_sck),
`ifdef SPI3WIRE
        .spi3w(spi3w),
`endif
        .spi_mosi(spi_mosi),
        .spi_miso(spi_miso)
    );
    lis3dh_stub lis3dh_stub1(
        .clk(clk),
        .csn(spi_csn),
        .sck(spi_sck),
        .mosi(spi_mosi),
        .miso(spi_miso)
    );
endmodule
