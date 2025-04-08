TARGET:=dut_tb.vcd
SPI3WIRE:=1

all: $(TARGET)

IVOPT:=
ifdef SPI3WIRE
ifeq ($(SPI3WIRE),1)
dut_tb.vvp: IVOPT+=-DSPI3WIREACTIVE
endif
dut_tb.vvp: IVOPT+=-DSPI3WIRE
endif
dut_tb.vvp: dut_tb.v spi_master.v lis3dh_stub.v

.PRECIOUS:%.vvp %.vcd
%.vcd: %.vvp
	vvp $<
%.vvp: %.v
	iverilog -o $@ $^ $(IVOPT)

clean:
	$(RM) *.vvp *.vcd
