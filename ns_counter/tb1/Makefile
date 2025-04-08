TARGET:=dut_tb.vcd

all: $(TARGET)

dut_tb.vvp: dut_tb.v spi_master.v lis3dh_stub.v

.PRECIOUS:%.vvp %.vcd
%.vcd: %.vvp
	vvp $<
%.vvp: %.v
	iverilog -o $@ $^

clean:
	$(RM) *.vvp *.vcd
