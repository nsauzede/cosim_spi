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

module dutcosim #(parameter integer SPI_DIV_COEF = 0) (
`ifdef SIMULATION
    input wire x_l_flag,
    output [15:0] x_l_response,
`endif
    input rx,
    output tx,
    input spi_miso,
    output spi_mosi,
    output spi_csn,
    output spi_sck,
    output [7:0] leds,
    input reset,
    input clk
);

wire [31:0] spi_mosi_data;
wire [31:0] spi_miso_data;
wire [4:0] spi_nbits;
wire spi_request;
wire spi_ready;
assign tx = 1;

sequencer sequencer1 (
`ifdef SIMULATION
    .x_l_flag(x_l_flag),
    .x_l_response(x_l_response),
`endif
    .clk_in(clk),
    .nrst(~reset),

    .spi_mosi_data(spi_mosi_data),
    .spi_miso_data(spi_miso_data),
    .spi_nbits(spi_nbits),

    .spi_request(spi_request),
    .spi_ready(spi_ready),

    .led_out(leds)
);

spi_master #(.DIV_COEF(SPI_DIV_COEF)) spi_master1 (
    .clk_in(clk),
    .nrst(~reset),

    .spi_sck(spi_sck),
    .spi_mosi(spi_mosi),
    .spi_miso(spi_miso),
    .spi_csn(spi_csn),

    .mosi_data(spi_mosi_data),
    .miso_data(spi_miso_data),
    .nbits(spi_nbits),

    .request(spi_request),
    .ready(spi_ready)
);

endmodule
