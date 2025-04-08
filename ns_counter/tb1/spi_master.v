`timescale 1ns / 1ps
module spi_master (
    input               clk_in,
    input               nrst,

    input               request,
    input   [4:0]       nbits,
    input  [31:0]       mosi_data,
    output [31:0]       miso_data,
    output              ready,

    output              spi_cen,
    output              spi_scl,
    output              spi_sdi,
    input               spi_sdo
);
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
    reg [15:0] divider = 0;
    reg [15:0] div_coef = 0;
    reg divider_out = 0;
    assign ready = readyff;
    assign miso_data = miso_reg;
    assign spi_cen = cenff;
    assign spi_scl = sclff;
    assign spi_sdi = sdiff;
    always @(posedge clk_in or negedge nrst) begin
        if (nrst == 1'b0) begin
            divider <= 0;
            divider_out <= 0;
        end else begin
            if (divider != div_coef) begin
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
                end
                STATE_Low: if (divider_out) begin
                    sclff <= 1;
                    miso_reg <= {miso_reg[30:0], spi_sdo};
                    if (bit_counter == 5'd0) begin
                        state <= STATE_Finish;
                    end else begin
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
