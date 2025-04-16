/*
 * Copyright (c) 2025, Nicolas Sauzede <nicolas.sauzede@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
    wire [31:0] miso_data;
    reg request = 0;
    wire ready;

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
        #50       reset = 0;

`ifdef SPI_CHANGE_COEF
/* Begin change divcoef */
        #clk_period
        nbits = 5'd0;
        mosi_data[31:16] = 16'h8080;    // `set divcoef as mosi_data[15:0]`
        mosi_data[15:0] = 16'd0;        // can use arbitrary divider coefficents here
        request = 1;
        #(clk_period*2)
        request = 0;
        mosi_data = 0;
        #clk_period
/* End change divcoef */
`endif


`ifdef SPI3WIRE
`ifdef SPI3WIREACTIVE
        // By default, both master and slave are in 4-wire mode; we will turn them to 3-wire mode sequentially
/* Begin switch the slave to 3-wire mode */
        // write SIM=1 (01h) in CTRL_REG4 (23h) to set slave to 3-wire mode
        nbits = 15;
        mosi_data = 32'h2301;
        #50       request = 1;
        #10 request = 0;
        wait (ready)
/* End switch the slave to 3-wire mode */
/* Begin switch the master to 3-wire mode */
        #clk_period
        nbits = 5'd0;
        mosi_data[31:0] = 32'h81010000;    // `set 3-wire on`
        request = 1;
        #(clk_period*2)
        request = 0;
        mosi_data = 0;
        #clk_period
/* End switch the master to 3-wire mode */
`endif
`endif
        // 3 or 4-wire whoami
        nbits = 15;
        mosi_data = 32'h8f00;
        #50       request = 1;
        #10 request = 0;
        wait (ready)
        if (miso_data[7:0] !== 8'h33) begin     // note we use the strict inequality in case the signal has X or Z
            $display("BAD miso_data=%x", miso_data);
            reset = 1'bx;
            #1000
            $stop;
        end

`ifdef SPI3WIRE
`ifdef SPI3WIREACTIVE
/* Begin switch the master back to 4-wire mode */
        #clk_period
        nbits = 5'd0;
        mosi_data[31:0] = 32'h81000000;    // `set 3-wire off (4-wire default)`
        request = 1;
        #(clk_period*2)
        request = 0;
        mosi_data = 0;
        #clk_period
/* End switch the master back to 4-wire mode */

/* Begin switch the slave back to 4-wire mode */
        // write SIM=0 (00h) in CTRL_REG4 (23h) to set slave back to 4-wire mode
        nbits = 15;
        mosi_data = 32'h2300;
        #50       request = 1;
        #10 request = 0;
        wait (ready)
/* End switch the slave back to 4-wire mode */

        // 4-wire whoami
        nbits = 15;
        mosi_data = 32'h8f00;
        #50       request = 1;
        #10 request = 0;
        wait (ready)
        if (miso_data[7:0] !== 8'h33) begin     // note we use the strict inequality in case the signal has X or Z
            $display("BAD miso_data=%x", miso_data);
            reset = 1'bx;
            #1000
            $stop;
        end
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
        .miso_data(miso_data),
        .ready(ready),
        .request(request),
        .spi_csn(spi_csn),
        .spi_sck(spi_sck),
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
