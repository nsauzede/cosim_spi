/*
    MAX10 Demos
    Copyright (C) 2016-2020 Victor Pecanins (vpecanins at gmail dot com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
`timescale 1ns / 1ps
`include "../rtl/config.vh"

module spi_master #( parameter integer DIV_COEF = 0 ) (
    input               clk_in,           // Logic clock
    input               nrst,             // SPI is active when nreset is HIGH

    input  [31:0]       mosi_data,        // Parallel FPGA data write to SPI
    output [31:0]       miso_data,        // Parallel FPGA data read from SPI
    input  [5:0]        nbits,            // Number of bits: nbits==0 means 1 bit

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
`ifdef COSIM
    always @(posedge clk_in or negedge nrst) begin
        $spi_master_stub(div_coef, nrst, mosi_data, miso_data, nbits, request, ready, spi_csn, spi_sck, spi_mosi, spi_miso);
`else
// Frequency divider
reg [31:0] divider;
reg divider_out;
always @(posedge clk_in or negedge nrst) begin
    if (nrst == 1'b0) begin
        divider <= 32'b0;
        divider_out <= 1'b0;
    end else begin
        if (divider != div_coef) begin
            divider <= divider + 1;
            divider_out <= 1'b0;
        end else begin
            divider <= 32'b0;
            divider_out <= 1'b1;
        end
    end
end

localparam
    STATE_Idle = 4'd0,
    STATE_Run = 4'd1,
    STATE_High = 4'd2,
    STATE_Low = 4'd3,
    STATE_Finish = 4'd4,
    STATE_End = 4'd5;
reg [3:0] state;
reg [31:0] data_in_reg;
reg [5:0] nbits_reg;
reg [5:0] bit_counter;
reg [31:0] miso_dataff = 32'b0;
reg spi_csnff = 1'b1;
reg spi_sckff = 1'b1;
reg spi_mosiff = 1'b1;
reg readyff = 1'b0;
assign ready = nrst ? readyff : 1'b0;
assign miso_data = nrst ? miso_dataff : 32'b0;
assign spi_csn = nrst ? spi_csnff : 1'b1;
assign spi_sck = nrst ? spi_sckff : 1'b1;
assign spi_mosi = nrst ? spi_mosiff : 1'b1;

always @(posedge clk_in or negedge nrst) begin
    if (nrst == 1'b0) begin
        spi_csnff <= 1'b1;
        spi_sckff <= 1'b1;
        spi_mosiff <= 1'b1;
        readyff <= 1'b0;
        miso_dataff <= 32'b0;

        data_in_reg <= 32'b0;
        nbits_reg <= 6'b0;
        bit_counter <= 6'b0;

        state <= STATE_Idle;
    end else begin
        case (state)
            STATE_Idle: begin
                spi_csnff <= 1'b1;
                spi_sckff <= 1'b1;
                spi_mosiff <= 1'b1;
                if (request) begin
                    state <= STATE_Run;
                    readyff <= 1'b0;
                    spi_csnff <= 1'b0;
                    data_in_reg <= mosi_data;
                    nbits_reg <= nbits;
                    bit_counter <= nbits;
                    miso_dataff <= 32'b0;
                end
            end

            // Shift left output data word to align MSBit to position 31
            STATE_Run: begin
                if (nbits_reg == 6'b011111) begin
                    state <= STATE_High;
                end else begin
                    data_in_reg <= data_in_reg << 1;
                    nbits_reg <= nbits_reg + 6'b1;
                end
            end

            // During this state SCK is High
            // Transition to SCK=LOW and output MOSI data in position 31
            STATE_High: if (divider_out) begin
                state <= STATE_Low;
                spi_sckff <= 1'b0;
                spi_mosiff <= data_in_reg[31];
            end

            // During this state SCK is LOW & DATA is in the MOSI line
            // Transition to SCK==HIGH and sample MISO line
            STATE_Low: if (divider_out) begin
                if (bit_counter == 6'b0) begin
                    state <= STATE_Finish;
                end else begin
                    state <= STATE_High;
                    bit_counter <= bit_counter - 6'b1;
                    data_in_reg <= data_in_reg << 1'b1; // this must be out of the if (counter==0)?
                end
                spi_sckff <= 1'b1;
                miso_dataff <= {miso_dataff[30:0], spi_miso}; // Sample MISO at SCK posedge
            end

            STATE_Finish: if (divider_out) begin
                state <= STATE_End;
                spi_csnff <= 1'b1;
                spi_sckff <= 1'b1;
                spi_mosiff <= 1'b0;
            end

            STATE_End: if (divider_out) begin
                state <= STATE_Idle;
                readyff <= 1'b1;
            end
        endcase
    end
//$display("%08t div_coef=%1d nrst=%d mosi_data=%4x nbits=%x request=%d spi_miso=%d divider=%1d divider_out=%d data_in_reg=%x nbits_reg=%x bit_counter=%x state=%d", $time, div_coef, nrst, mosi_data, nbits, request, spi_miso, divider, divider_out, data_in_reg, nbits_reg, bit_counter, state);
//$display("%08t div_coef%1d nrst%d mosi_data%4x nbits%x request%d spi_miso%d divider%1d divider_out%d data_in_reg%x nbits_reg%x bit_counter%x state%1d", $time, div_coef, nrst, mosi_data, nbits, request, spi_miso, divider, divider_out, data_in_reg, nbits_reg, bit_counter, state);
$display("%08t nrst%d mosi_data%4x nbits%x request%d spi_miso%d divider%1d divider_out%d data_in_reg%x nbits_reg%x bit_counter%x state%1d", $time, nrst, mosi_data, nbits, request, spi_miso, divider, divider_out, data_in_reg, nbits_reg, bit_counter, state);
`endif
//    $display("%08t div_coef=%1d nrst=%d mosi_data=%4x miso_dataff=%4x nbits=%x request=%d readyff=%d spi_csnff=%d spi_sckff=%d spi_mosiff=%d spi_miso=%d", $time, div_coef, nrst, mosi_data, miso_dataff, nbits, request, readyff, spi_csnff, spi_sckff, spi_mosiff, spi_miso);
//    $display("%08t div_coef=%1d nrst=%d mosi_data=%4x miso_data=%4x nbits=%x request=%d ready=%d spi_csn=%d spi_sck=%d spi_mosi=%d spi_miso=%d", $time, div_coef, nrst, mosi_data, miso_data, nbits, request, ready, spi_csn, spi_sck, spi_mosi, spi_miso);
end
endmodule
