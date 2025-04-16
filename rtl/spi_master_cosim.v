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

    input  [31:0]       mosi_data,        // Parallel FPGA data write to SPI
    output [31:0]       miso_data,        // Parallel FPGA data read from SPI
    input  [4:0]        nbits,            // Number of bits: nbits==0 means 1 bit

    input               request,          // Request to start transfer: Active HIGH
    output              ready,            // Active HIGH when transfer has finished

    output              spi_csn,          // SPI CSN output (active LOW)
    output              spi_sck,          // SPI clock output
    output              spi_mosi,         // SPI master data output, slave data input
    input               spi_miso          // SPI master data input, slave data output
);
`ifdef SPI_DIV_COEF
localparam div_coef = `SPI_DIV_COEF;
`else
localparam div_coef = (DIV_COEF == 0) ? 32'd10000 : DIV_COEF;
`endif
always @(posedge clk_in or negedge nrst) begin
    $spi_master_stub(div_coef, nrst, mosi_data, miso_data, nbits, request, ready, spi_csn, spi_sck, spi_mosi, spi_miso);
end
endmodule
