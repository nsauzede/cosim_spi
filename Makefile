COSIM:=1
GTKWAVE:=gtkwave

DUT:=dut_icarus
RTL:=rtl
SRC:=src
RTLS:=$(RTL)/dut_tb.v
RTLS+=$(RTL)/dut.v
RTLS+=$(RTL)/sequencer.v
RTLS+=$(RTL)/spi_master.v
RTLS+=$(RTL)/lis3dh_stub.v
IVOPT:=-DDUT_VCD=\"$(DUT).vcd\"
IVOPT+=-DSPI=1
IVOPT+=-DSIMULATION=1
IVOPT+=-DBOARD_ID=0
IVOPT+=-DBOARD_CK=32000000
IVOPT+=-D__UARTSPEED__=115200
IVOPT+=-Irtl
ifdef COSIM
# IVERILOG_VPI_MODULE_PATH=.
# Cadence PLI Modules (Verilog-XL)
# vvp -mcadpli a.out -cadpli=./product.so:my_boot
IVOPT+=-DCOSIM=1
IVOPT+=-L.
VPI:=
MLIS3DH:=lis3dh_vpi
VPI+=$(MLIS3DH).vpi
IVOPT+=-m $(MLIS3DH)
MSPI_MASTER:=spi_master_vpi
VPI+=$(MSPI_MASTER).vpi
IVOPT+=-m $(MSPI_MASTER)
endif

#VALGRIND:=valgrind --leak-check=full --show-leak-kinds=all

all: $(DUT).vcd

view: $(DUT).vcd
	$(GTKWAVE) ./$< ./$(DUT).gtkw &

.PRECIOUS:$(DUT).vcd
$(DUT).vcd: $(DUT).vvp $(VPI)
	$(VALGRIND) vvp $(VVPOPT) $< | tee cosim$(COSIM)

$(DUT).vvp: $(RTLS) $(VPI)
	iverilog $(IVOPT) -o $@ $(RTLS)

VPI_CFLAGS := $(shell iverilog-vpi --cflags)
%.o: $(SRC)/%.c
	$(CC) -c $^ $(VPI_CFLAGS) -Wall -Werror
%.vpi: %.o
	iverilog-vpi $<

clean:
	$(RM) *.o *.vvp *.vpi $(DUT).vcd vgcore.*
