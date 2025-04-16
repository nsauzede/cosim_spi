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

`timescale 1ns / 1ns
`ifndef DUT_TB_VCD
`define DUT_TB_VCD "dutcosim_tb.vcd"
`endif
module dutcosim_tb;
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

    dutcosim #(.SPI_DIV_COEF(SPI_DIV_COEF)) dutcosim1 (
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
        $dumpfile(`DUT_TB_VCD);
        $dumpvars(0, dutcosim_tb);
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
