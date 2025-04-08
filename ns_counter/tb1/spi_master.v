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
    - flexible divider coefficient (see below)

    Define `SPI3WIRE to enable 3-wire mode support,
    and set .spi3w=1 to switch the SPI master to 3-wire mode.
    (SPI slave must also be set to 3-wire by some means accordingly of course)

    Divider coefficient (10000 by default when DIV_COEF==0) can be changed while in reset:
    - maintain nrst=0
     - set nbits=0
     - set mosi_data[15:0]=divider (eg: 16'd20000)
     - set request=1
    - after some clocks:
     - set request=0
*/
module spi_master #( parameter integer DIV_COEF = 0 ) (
    input               clk_in,
    input               nrst,

    input               request,
    input   [4:0]       nbits,
    input  [31:0]       mosi_data,
    output [31:0]       miso_data,
    output              ready,

    output              spi_cen,
    output              spi_scl,
`ifdef SPI3WIRE
//    inout               spi_sdio,     // -- should be only this but keep 4wire signals to not break gtkw
    input               spi3w,          // 1=3-wire, 0=4-wire (default/standard)
    inout               spi_sdi,        // spi3w=1: SPI master/slave output/input, spi3w=0: master output, slave input
    input               spi_sdo         // should be removed
`else
    output              spi_sdi,
    input               spi_sdo
`endif
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
    reg cenff = 1;
    reg sclff = 1;
    reg sdiff = 1;
    reg readyff = 0;
    reg [31:0] mosi_reg = 0;
    reg [31:0] miso_reg = 0;
    reg [4:0] nbits_reg = 0;
    reg [4:0] bit_counter = 0;
`ifdef SPI3WIRE
    //reg spi3w = 1'b0;
    reg oe = 1'b1;
//    assign spi_sdi = spi3w ? (oe ? sdiff : 1'bz) : sdiff;
//    assign spi_sdi = spi3w && !oe ? 1'bz : sdiff;
    assign spi_sdi = !oe ? 1'bz : sdiff;
`else
    reg spi3w = 1;              // should remove
    reg oe = 1'b1;              // should remove
    assign spi_sdi = sdiff;
`endif
    assign ready = readyff;
    assign miso_data = miso_reg;
    assign spi_cen = cenff;
    assign spi_scl = sclff;

    // Frequency divider
    reg divider_out = 0;
    reg [15:0] divider = 0;
    reg [15:0] div_coef = div_coef_;
    always @(posedge clk_in or negedge nrst) begin
        if (nrst == 1'b0) begin
            divider <= 0;
            divider_out <= 0;
            div_coef <= request && (nbits == 5'b0) ? mosi_data[15:0] : div_coef;
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
            oe <= 1;
`endif
            cenff <= 1;
            sclff <= 1;
            sdiff <= 1;
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
`endif
                    if (request) begin
                        mosi_reg <= mosi_data;
                        nbits_reg <= nbits;
                        bit_counter <= nbits;
                        cenff <= 0;
                        readyff <= 0;
                        state <= STATE_Run;
                    end
                end
                STATE_Run: begin
                    if (nbits_reg == 5'd31) begin
                        state <= STATE_High;
                    end else begin
                        mosi_reg <= mosi_reg << 1;
                        nbits_reg <= nbits_reg + 5'd1;
                    end
                end
                STATE_High: if (divider_out) begin
                    sclff <= 0;
                    sdiff <= mosi_reg[31];
                    state <= STATE_Low;
`ifdef SPI3WIRE
                        if (bit_counter == (nbits - 5'd8)) begin
                            if (spi3w) begin
                                oe <= 0;
                            end
                        end
`endif
                end
                STATE_Low: if (divider_out) begin
                    sclff <= 1;
`ifdef SPI3WIRE
                    miso_reg <= {miso_reg[30:0], spi3w ? spi_sdi : spi_sdo};
`else
                    miso_reg <= {miso_reg[30:0], spi_sdo};
`endif
                    if (bit_counter == 5'd0) begin
                        state <= STATE_Finish;
                    end else begin
`ifdef SPI3WIRE_
                        if (bit_counter == (nbits - 5'd8)) begin
                            oe <= 0;
                        end
`endif
                        bit_counter <= bit_counter - 5'd1;
                        mosi_reg <= mosi_reg << 1;
                        state <= STATE_High;
                    end
                end
                STATE_Finish: if (divider_out) begin
                    cenff <= 1;
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
