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
/*
    Minor debug tweaks
    Copyright (c) 2025, Nicolas Sauzede <nicolas.sauzede@gmail.com>
*/
`timescale 1ns / 1ps
`include "../rtl/config.vh"

module sequencer (
`ifdef SIMULATION
    input wire x_l_flag,
    output reg [15:0] x_l_response,
`endif
    input wire clk_in,
    input wire nrst,

    output reg [31:0] spi_mosi_data,
    input wire [31:0] spi_miso_data,
    output reg [4:0] spi_nbits,

    output spi_request,
    input  spi_ready,

    output reg [7:0] led_out
);

    localparam
        STATE_Whoami = 4'd0,
        STATE_Whoami_Wait = 4'd1,
        STATE_Init = 4'd2,
        STATE_Init_Wait = 4'd3,
        STATE_Init1 = 4'd4,
        STATE_Init1_Wait = 4'd5,
        STATE_Init2 = 4'd6,
        STATE_Init2_Wait = 4'd7,
        STATE_Read = 4'd8,
        STATE_Read_Wait = 4'd9,
        STATE_LEDout = 4'd10,
        STATE_Halt = 4'd15;

    reg [3:0] state;
    reg spi_requestff = 1'b0;
    assign spi_request = spi_requestff;

    localparam
        DEBUG_0 = 8'hff,
        DEBUG_1 = 8'hfe,
        DEBUG_2 = 8'hfd,
        DEBUG_3 = 8'hfb,
        DEBUG_4 = 8'hf7;

    localparam expected_who_am_i = 8'h33;
`ifdef SIMULATION
    localparam expected_HIZ = 8'hzz;
`else
    localparam expected_HIZ = 8'hff;
`endif
    reg signed [7:0] saved_acc;
`ifdef SIMULATION
    reg [7:0] expected_data = 8'h9a;
`endif

    always @(posedge clk_in or negedge nrst) begin
        if (nrst == 1'b0) begin
            led_out <= DEBUG_0;
            state <= STATE_Whoami;

            spi_mosi_data <= 32'b0;
            spi_nbits <= 5'b0;
            spi_requestff <= 1'b0;

            saved_acc <= 8'b0;
`ifdef SIMULATION
            x_l_response <= 8'b0;
`endif
        end else begin
            case (state)
                // 1. Read WHO_AM_I register (Addr 0x0F)
                STATE_Whoami: begin
                    led_out <= DEBUG_1;
                    if (~spi_ready) begin
                        state <= STATE_Whoami_Wait;
                    end
                    spi_requestff <= 1'b1;
                    spi_nbits <= 5'd15;
                    spi_mosi_data <= 31'b10001111_00000000;
                end

                STATE_Whoami_Wait: begin
                    if (spi_ready) begin
                        if (spi_miso_data[7:0] != expected_who_am_i) begin
                            state <= STATE_Halt;
`ifdef SIMULATION
                            $display("Bad Whoami response: %02x (expected %02x)", spi_miso_data[7:0], expected_who_am_i);
                            $fatal(1);
`endif
                        end else begin
                            state <= STATE_Init;
                        end
                    end
                    spi_requestff <= 1'b0;
                end

                // 2. Write ODR in CTRL_REG1 (Addr 0x20)
                STATE_Init: begin
                    led_out <= DEBUG_2;
                    if (~spi_ready) begin
                        state <= STATE_Init_Wait;
                    end
                    spi_requestff <= 1'b1;
                    spi_nbits <= 5'd15;
                    spi_mosi_data <= 31'b00100000_01110111;
                end

                STATE_Init_Wait: begin
                    if (spi_ready) begin
                        if (spi_miso_data[7:0] != expected_HIZ) begin
                            state <= STATE_Halt;
`ifdef SIMULATION
                            $display("Bad Ctrlreg1 response: %02x (expected %02x)", spi_miso_data[7:0], expected_HIZ);
                            $fatal(1);
`endif
                        end else begin
                            state <= STATE_Init1;
                        end
                    end
                    spi_requestff <= 1'b0;
                end

                // 3. Enable temperature sensor (Addr 0x1F)
                STATE_Init1: begin
                    led_out <= DEBUG_3;
                    if (~spi_ready) begin
                        state <= STATE_Init1_Wait;
                    end
                    spi_requestff <= 1'b1;
                    spi_nbits <= 5'd15;
                    spi_mosi_data <= 31'b00011111_11000000;
                end

                STATE_Init1_Wait: begin
                    if (spi_ready) begin
                        if (spi_miso_data[7:0] != expected_HIZ) begin
                            state <= STATE_Halt;
`ifdef SIMULATION
                            $display("Bad Tempcfgreg response: %02x (expected %02x)", spi_miso_data[7:0], expected_HIZ);
                            $fatal(1);
`endif
                        end else begin
                            state <= STATE_Init2;
                        end
                    end
                    spi_requestff <= 1'b0;
                end

                // 4. Enable BDU, High resolution (Addr 0x23)
                STATE_Init2: begin
                    led_out <= DEBUG_4;
                    if (~spi_ready) begin
                        state <= STATE_Init2_Wait;
                    end
                    spi_requestff <= 1'b1;
                    spi_nbits <= 5'd15;
                    spi_mosi_data <= 31'b00100011_10001000;
                end

                STATE_Init2_Wait: begin
                    if (spi_ready) begin
                        if (spi_miso_data[7:0] != expected_HIZ) begin
                            state <= STATE_Halt;
`ifdef SIMULATION
                            $display("Bad Ctrlreg4 response: %02x (expected %02x)", spi_miso_data[7:0], expected_HIZ);
                            $fatal(1);
`endif
                        end else begin
                            state <= STATE_Read;
`ifdef SIMULATION
                            //$display("STATE_Init2_Wait => STATE_Read");
`endif
                        end
                    end
                    spi_requestff <= 1'b0;
                end

                // 5. Read OUT_X_L (Addr 0x28)
                STATE_Read: begin
`ifdef SIMULATION
                    x_l_response <= expected_data;
`endif
                    if (~spi_ready) begin
                        state <= STATE_Read_Wait;
`ifdef SIMULATION
                        //$display("STATE_Read => STATE_Read_Wait");
`endif
                    end
                    spi_requestff <= 1'b1;
                    spi_nbits <= 5'd23;
                    spi_mosi_data <= 31'b11101000_00000000_00000000;//28: OUT_X_L
                    //spi_mosi_data <= 31'b11101010_00000000_00000000;//2a: OUT_Y_L
                    //spi_mosi_data <= 31'b11101100_00000000_00000000;//2c: OUT_Z_L
                end

                STATE_Read_Wait: begin
                    if (spi_ready) begin
`ifdef SIMULATION
                        //if (spi_miso_data[7:0] != expected_data) begin
                        if (spi_miso_data[15:8] != expected_data) begin
                            //$display("Bad Read response: %02x (wanted 0xda)", spi_miso_data[7:0]);
                            $display("Bad Read response: %02x (wanted %02x)", spi_miso_data[15:8], expected_data);
                            $fatal(1);
                        end
                        expected_data <= expected_data + 32;
                        //$display("STATE_Read_Wait => STATE_LEDout - spi_miso_data=%02x", spi_miso_data);
`endif
                        state <= STATE_LEDout;
                        //saved_acc <= spi_miso_data[7:0];
                        saved_acc <= spi_miso_data[15:8];
                    end
                    spi_requestff <= 1'b0;
                end

                // 6. Set LED output according to accelerometer value
                STATE_LEDout: begin
                    state <= STATE_Read;
`ifdef SIMULATION
                    //$display("STATE_LEDout => STATE_Read - saved_acc=%02x", saved_acc);
`endif
                    led_out <= 1 << ((saved_acc + 8'Sb1000_0000) >> 5);
                    //led_out <= saved_acc;
                end
                STATE_Halt: begin
                    // Do nothing
                end
            endcase
        end
    end
endmodule
