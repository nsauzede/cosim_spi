COSIM:=1
GTKWAVE:=gtkwave

DUT:=dut_icarus
SRC:=dut_tb.v
SRC+=dut.v
SRC+=sequencer.v
SRC+=spi_master.v
SRC+=lis3dh_stub.v
IVOPT:=-DDUT_VCD=\"$(DUT).vcd\"
IVOPT+=-DSPI=1
IVOPT+=-DSIMULATION=1
IVOPT+=-DBOARD_ID=0
IVOPT+=-DBOARD_CK=32000000
IVOPT+=-D__UARTSPEED__=115200
ifdef COSIM
# IVERILOG_VPI_MODULE_PATH=.
# Cadence PLI Modules (Verilog-XL)
# vvp -mcadpli a.out -cadpli=./product.so:my_boot
MCOSIM:=lis3dh_vpi
VPI:=$(MCOSIM).vpi
IVOPT+=-DCOSIM=1
IVOPT+=-L. -m $(MCOSIM)
#VVPOPT:=-M. -m $(MCOSIM)
endif

#VALGRIND:=valgrind --leak-check=full --show-leak-kinds=all

all: $(DUT).vcd

view: $(DUT).vcd
	$(GTKWAVE) ./$< ./$(DUT).gtkw &

.PRECIOUS:$(DUT).vcd
$(DUT).vcd: $(DUT).vvp $(VPI)
	$(VALGRIND) vvp $(VVPOPT) $<

$(DUT).vvp: $(SRC) $(VPI)
	iverilog $(IVOPT) -o $@ $(SRC)

%.vpi: %.c
	iverilog-vpi $<

clean:
	$(RM) *.o *.vvp *.vpi $(DUT).vcd vgcore.*
