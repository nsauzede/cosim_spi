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

/*
    Simple LIS3DH SPI stub (STMicroelectronics LIS3DH accelerometer)
*/

module lis3dh_stub (
    input               clk,                    // System clock

    input [15:0]        out_x_resp,
    output              out_x_l_flag,

    input               csn,                    // SPI chip select (active low)
    input               sck,                    // SPI clock
    inout               mosi,                   // SPI master output slave input (default 4-wire); or m/s i/o (3-wire enabled)
    output              miso                    // SPI master in slave out
);
`ifdef SPI3WIRE
localparam PROFILE = "3W CO";
`else
localparam PROFILE = "__ CO";
`endif
// we use a "wire+reg" in cosim mode for miso output,
// else the VPI call modifies the reg one clock too early (iverilog-specific ?)
wire misoff;
reg misoff2 = 0;
assign miso = misoff2;
wire [1:0] state;
reg [1:0] stateff = 0;
wire [3:0] bit_count;
reg [3:0] bit_countff = 0;
wire [7:0] shift_reg;
reg [7:0] shift_regff = 0;

// must be sensitive to both clk edges else we only update mosi half the time
always @(posedge clk or negedge clk) begin
    $lis3dh_stub(out_x_resp, out_x_l_flag, csn, sck, mosi, misoff, state, bit_count, shift_reg);
    misoff2 <= misoff;
    stateff <= state;
    bit_countff <= bit_count;
    shift_regff <= shift_reg;
end
endmodule
