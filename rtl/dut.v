module dut #(parameter integer SPI_DIV_COEF = 0) (
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
wire [5:0] spi_nbits;
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
