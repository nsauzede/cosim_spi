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
`include "../rtl/config.vh"

module spi_master #( parameter integer DIV_COEF = 0 ) (
    input               clk_in,           // Logic clock
    input               nrst,             // SPI is active when nreset is HIGH

    input               request,          // Request to start transfer: Active HIGH
    input  [4:0]        nbits,            // Number of bits (nbits=15 => 16; nbits=0 is reserved)
    input  [31:0]       mosi_data,        // Parallel FPGA data write to SPI
    output [31:0]       miso_data,        // Parallel FPGA data read from SPI
    output              ready,            // Active HIGH when transfer has finished

    output              spi_csn,          // SPI CSN output (active LOW)
    output              spi_sck,          // SPI clock output
    inout               spi_mosi,         // SPI master output slave input (default 4-wire); or m/s i/o (3-wire enabled)
    input               spi_miso          // SPI master data input, slave data output
);
`ifdef SPI3WIRE
    localparam PROFILE = "3W CO";
`else
    localparam PROFILE = "__ CO";
`endif
`ifdef SPI_DIV_COEF
    localparam div_coef_ = `SPI_DIV_COEF;
`else
// Do not decrement non-zero DIV_COEF here! It will be done in $spi_master VPI!
    localparam div_coef_ = (DIV_COEF == 0) ? 16'd10000 : DIV_COEF;
`endif
// Frequency divider
    reg [15:0] div_coef = div_coef_;
    reg [31:0] debug = 0;
    wire oe = debug[0];
// we use a "wire+reg" in cosim mode for ready output,
// else the VPI call modifies the reg one clock too early (iverilog-specific ?)
    wire readyff;
    reg readyff2 = 0;
    assign ready = readyff2;
    wire [31:0] miso_reg;
    reg [31:0] miso_regff = 0;

    always @(posedge clk_in) begin
        $spi_master(div_coef, nrst, mosi_data, miso_data, nbits, request, readyff, spi_csn, spi_sck, spi_mosi, spi_miso, debug, miso_reg);
        readyff2 <= readyff;
        miso_regff <= miso_reg;
    end
endmodule
