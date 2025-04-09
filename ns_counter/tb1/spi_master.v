/*
    SPI Master (3/4-wire support, flexible divider coefficient)
    Copyright (C) 2025 Nicolas Sauzede (nicolas dot sauzede at gmail dot com)
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
/*
    SPI master
    - 3/4-wire mode support (4-wire by default)
    - flexible divider coefficient
    Note that it is mainly targeting the STMicro LIS3DH sensor SPI protocol.
    In particular:
    - SPI clock polarity/phase
    - 3-wire mode (ie: tristate window)
    Maybe there are some specifics that should/could be reworked to make it more general..

    Divider coefficient (10000 by default when DIV_COEF==0) can be changed while in reset:
    - maintain nrst=0
     - set nbits=5'd31
     - set mosi_data[31:16]=16'h8080    `set divcoef as mosi_data[15:0]`
     - set mosi_data[15:0]=divider      (eg: 16'h4e20 (20000))
     - set request=1
    - after some clocks:
     - set request=0
     - release nrst and set request/nbits/mosi_data as needed

    Define `SPI3WIRE to configure 3-wire mode support and enable it (see below) to switch the SPI master to 3-wire mode.
    (SPI slave must also be set to 3-wire by some means accordingly of course)
    To enable the 3-wire mode (or disable it to standard/default 4-wire mode):
    - maintain nrst=0
     - set nbits=5'd31
     - Either:
        - set mosi_data[31:0]=16'h81010000 `set 3-wire on`
     - Or:
        - set mosi_data[31:0]=16'h81000000 `set 3-wire off (default 4-wire)`
     - set request=1
    - after some clocks:
     - set request=0
     - release nrst and set request/nbits/mosi_data as needed
*/
module spi_master #( parameter integer DIV_COEF = 0 ) (
    input               clk_in,         // Logic clock
    input               nrst,           // SPI is active when nreset is HIGH

    input               request,        // Request to start transfer: Active HIGH
    input   [4:0]       nbits,          // Number of bits: nbits==0 means 1 bit
    input  [31:0]       mosi_data,      // Parallel FPGA data write to SPI
    output [31:0]       miso_data,      // Parallel FPGA data read from SPI
    output              ready,          // Active HIGH when transfer has finished

    output              spi_csn,        // SPI CSN output (active LOW)
    output              spi_sck,        // SPI clock output
`ifdef SPI3WIRE
    inout               spi_mosi,       // SPI master output slave input (default 4-wire); SPI master/slave output/input (3-wire enabled)
`else
    output              spi_mosi,       // SPI master data output, slave data input
`endif
    input               spi_miso        // SPI master data input, slave data output
);
`ifdef SPI_DIV_COEF
    localparam div_coef_ = `SPI_DIV_COEF;
`else
    localparam div_coef_ = (DIV_COEF == 0) ? 16'd10000 : DIV_COEF - 1;
`endif

    localparam
        STATE_Idle = 0,
        STATE_Run = 1,
        STATE_High = 2,
        STATE_Low = 3,
        STATE_Finish = 4,
        STATE_End = 5;
    reg [2:0] state = STATE_Idle;
    reg csnff = 1;
    reg sckff = 1;
    reg mosiff = 1;
    reg readyff = 0;
    reg [31:0] mosi_reg = 0;
    reg [31:0] miso_reg = 0;
    reg [4:0] nbits_reg = 0;
    reg [4:0] bit_counter = 0;
`ifdef SPI3WIRE
    reg spi3w = 1'b0;
    reg oe = 1'b1;
    reg rd = 1'b0;
//    assign spi_mosi = spi3w ? (oe ? mosiff : 1'bz) : mosiff;
//    assign spi_mosi = spi3w && !oe ? 1'bz : mosiff;
    assign spi_mosi = !oe ? 1'bz : mosiff;
`else
    reg spi3w = 0;              // should remove
    reg oe = 1'b1;              // should remove
    reg rd = 1'b0;              // should remove
    assign spi_mosi = mosiff;
`endif
    assign ready = readyff;
    assign miso_data = miso_reg;
    assign spi_csn = csnff;
    assign spi_sck = sckff;

    // Frequency divider
    reg divider_out = 0;
    reg [15:0] divider = 0;
    reg [15:0] div_coef = div_coef_;
    always @(posedge clk_in or negedge nrst) begin
        if (nrst == 1'b0) begin
            divider <= 0;
            divider_out <= 0;
            div_coef <= request && (nbits == 5'd31) && (mosi_data[31] && mosi_data[23]) ? mosi_data[15:0] : div_coef;
        end else begin
            if (divider <= div_coef) begin
                divider <= divider + 16'd1;
                divider_out <= 0;
            end else begin
                divider <= 0;
                divider_out <= 1;
            end
        end
    end

    always @(posedge clk_in or negedge nrst) begin
        if (nrst == 1'b0) begin
`ifdef SPI3WIRE
            spi3w <= request && (nbits == 5'd31) && (mosi_data[31] && mosi_data[24]) ? mosi_data[16] : spi3w;
`endif
            csnff <= 1;
            sckff <= 1;
            mosiff <= 1;
            readyff <= 0;
            mosi_reg <= 0;
            miso_reg <= 0;
            nbits_reg <= 0;
            bit_counter <= 0;
            state <= STATE_Idle;
        end else begin
            case (state)
                STATE_Idle: begin
`ifdef SPI3WIRE
                    oe <= 1;
                    rd <= 0;
`endif
                    if (request) begin
                        mosi_reg <= mosi_data;
                        nbits_reg <= nbits;
                        bit_counter <= nbits;
                        csnff <= 0;
                        readyff <= 0;
                        state <= STATE_Run;
                    end
                end
                STATE_Run: begin
                    if (nbits_reg == 5'd31) begin
`ifdef SPI3WIRE
                        rd <= mosi_reg[31];
`endif
                        state <= STATE_High;
                    end else begin
                        mosi_reg <= mosi_reg << 1;
                        nbits_reg <= nbits_reg + 5'd1;
                    end
                end
                STATE_High: if (divider_out) begin
                    sckff <= 0;
                    mosiff <= mosi_reg[31];
                    state <= STATE_Low;
`ifdef SPI3WIRE
                        if (bit_counter == (nbits - 5'd8)) begin
                            if (spi3w && rd) begin
                                oe <= 0;
                            end
                        end
`endif
                end
                STATE_Low: if (divider_out) begin
                    sckff <= 1;
`ifdef SPI3WIRE
                    miso_reg <= {miso_reg[30:0], spi3w ? spi_mosi : spi_miso};
`else
                    miso_reg <= {miso_reg[30:0], spi_miso};
`endif
                    if (bit_counter == 5'd0) begin
                        state <= STATE_Finish;
                    end else begin
`ifdef SPI3WIRE_
                        if (bit_counter == (nbits - 5'd8)) begin
                            if (spi3w && rd) begin
                                oe <= 0;
                            end
                        end
`endif
                        bit_counter <= bit_counter - 5'd1;
                        mosi_reg <= mosi_reg << 1;
                        state <= STATE_High;
                    end
                end
                STATE_Finish: if (divider_out) begin
                    csnff <= 1;
                    state <= STATE_End;
                end
                STATE_End: if (divider_out) begin
                    readyff <= 1;
                    state <= STATE_Idle;
                end
            endcase
        end
    end
endmodule
