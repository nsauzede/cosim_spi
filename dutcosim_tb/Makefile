#
# Copyright (c) 2025, Nicolas Sauzede <nicolas.sauzede@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

COSIM:=1
GTKWAVE:=gtkwave

DUT:=dutcosim
SRC:=../src
RTL:=../rtl
RTLS:=$(RTL)/$(DUT)_tb.v
RTLS+=$(RTL)/$(DUT).v
RTLS+=$(RTL)/sequencer.v
ifdef COSIM
RTLS+=$(RTL)/spi_master_cosim.v
RTLS+=$(RTL)/lis3dh_stub_cosim.v
else
RTLS+=$(RTL)/spi_master.v
RTLS+=$(RTL)/lis3dh_stub.v
endif
IVOPT:=-DDUT_VCD=\"$(DUT)_tb.vcd\"
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

all: $(DUT)_tb.vcd

view: $(DUT)_tb.vcd
	$(GTKWAVE) ./$< ./$(DUT)_tb.gtkw &

#DEBUG:=1
ifdef DEBUG
OUTP:=| tee $(COSIM)
endif
.PRECIOUS:$(DUT)_tb.vcd
$(DUT)_tb.vcd: $(DUT)_tb.vvp $(VPI)
	$(VALGRIND) vvp $(VVPOPT) $< $(OUTP)

$(DUT)_tb.vvp: $(RTLS) $(VPI)
	iverilog $(IVOPT) -o $@ $(RTLS)

VPI_CFLAGS := $(shell iverilog-vpi --cflags)
# High-Z; akin to Verilog's 1'bz and iverilog VPI's vpiZ
VPI_CFLAGS+=-DZ=2
ifdef DEBUG
VPI_CFLAGS+=-DDEBUG
endif
%.o: $(SRC)/%.c
	$(CC) -c $^ $(VPI_CFLAGS) -Wall -Werror
%.vpi: %.o
	iverilog-vpi $<

clean:
	$(RM) *.o *.vvp *.vpi $(DUT)_tb.vcd vgcore.*
