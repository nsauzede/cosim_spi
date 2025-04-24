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

DUT?=dut
#DUT_TB?=$(DUT)_tb

#VDUT?=V$(DUT_TB)
VDUT?=V$(DUT)

SV?=../SV
SRC?=$(SV)/src
RTL?=$(SV)/rtl
#COSIM:=1
#SPI3WIRE:=1
#DEBUG:=1

GTKWAVE:=gtkwave
#VALGRIND:=valgrind --leak-check=full --show-leak-kinds=all
VLATOR:=verilator
ifdef UBUNTU
# Ubuntu 24.04.1 ships a lib stb?! and verilator doesn't support --quiet-stats
GUI_LDFLAGS+=-lstb
else
VLATOR+=--quiet-stats
endif

VOPT_TRACE?=--trace
VOPT:=
VOPT+=$(VOPT_TRACE)

VTMP:=vtmp
VOUT:=./$(VTMP)/$(VDUT).cpp
LINTED:=.linted
VCD_FILE:=$(VDUT).vcd
GTKW:=$(VDUT).gtkw
TOP_MODULE:=--top-module $(DUT)

TARGET:=$(VCD_FILE)

RTLS:=$(patsubst %,$(RTL)/%,$(RTL_MODULES))
ifdef COSIM
COSIM_RTLS:=$(patsubst %,$(RTL)/%_cosim.sv,$(COSIM_MODULES))
else
COSIM_RTLS:=$(patsubst %,$(RTL)/%.sv,$(COSIM_MODULES))
endif
RTLS+=$(COSIM_RTLS)

SRCS:=$(patsubst %,$(SRC)/%,$(SRC_MODULES))

VSRC:=$(RTLS) $(SRCS)

ifdef DEBUG
OUTP:=| tee out_cosim$(COSIM)
endif

#VOPT:=-DVCD_FILE=\"$(VCD_FILE)\"
#VOPT+=-DSPI=1
#VOPT+=-DSIMULATION=1
#VOPT+=-DBOARD_ID=0
#VOPT+=-DBOARD_CK=32000000
#VOPT+=-D__UARTSPEED__=115200
#VOPT+=-I$(RTL)

#ifdef SPI3WIRE
#ifeq ($(SPI3WIRE),1)
#$(DUT_TB).vvp: VOPT+=-DSPI3WIREACTIVE
#endif
#$(DUT_TB).vvp: VOPT+=-DSPI3WIRE
#endif

#ifdef COSIM
## IVERILOG_VPI_MODULE_PATH=.
## Cadence PLI Modules (Verilog-XL)
## vvp -mcadpli a.out -cadpli=./product.so:my_boot
#VOPT+=-L.
#VPI:=$(patsubst %,%_vpi.vpi,$(COSIM_MODULES))
#VOPT+=$(patsubst %,-m %_vpi,$(COSIM_MODULES))
#endif

all: $(TARGET)

view: $(VCD_FILE)
	$(GTKWAVE) ./$< $$(test -f $(GTKW) && echo $(GTKW)) &

.PRECIOUS:$(VCD_FILE)
$(VCD_FILE): simulate

./$(VTMP)/$(VDUT).mk: $(VSRC)
	$(AT)$(VLATOR) -cc $(VOPT) $(TOP_MODULE) --Mdir $(VTMP) -exe $^ && touch $@

./$(VTMP)/$(VDUT): ./$(VTMP)/$(VDUT).mk
	$(MAKE) -C ./$(VTMP) -f $(VDUT).mk CXXFLAGS=-DVCD_FILE='\"$(VCD_FILE)\"'

simulate: lint simulate_
simulate_: ./$(VTMP)/$(VDUT)
	$(AT)$(VALGRIND) ./$<

#$(DUT_TB).vvp $(VPI)
#	$(VALGRIND) vvp $(VVPOPT) $< $(OUTP)

$(VOUT): $(RTLS) $(VPI)
	$(VLATOR) -cc $(VOPT) -o $@ $(RTLS)

#VPI_CFLAGS := $(shell iverilog-vpi --cflags)
#ifdef DEBUG
#VPI_CFLAGS+=-DDEBUG
#endif
#ifdef SPI3WIRE
#VPI_CFLAGS+=-DSPI3WIRE
#endif
#%.o: $(SRC)/%.c
#	$(CC) -c $^ $(VPI_CFLAGS) -Wall -Werror
#%.vpi: %.o
#	iverilog-vpi $<

./$(LINTED): $(RTLS)
	$(AT)$(VLATOR) --lint-only $(VOPT) $^ && touch $@

lint: ./$(LINTED)

#clean:
#	$(RM) *.o *.vvp *.vpi *.vcd vgcore.*

clean:
	$(AT)rm -rf ./$(VTMP) ./$(LINTED) ./$(VCD_FILE)
#	$(AT)ut clean
