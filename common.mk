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

SRC:=../src
RTL:=../rtl
#COSIM:=1
#SPI3WIRE:=1
#DEBUG:=1

GTKWAVE:=gtkwave
#VALGRIND:=valgrind --leak-check=full --show-leak-kinds=all

TARGET:=$(DUT_TB).vcd

RTLS:=$(patsubst %,$(RTL)/%,$(RTL_MODULES))
ifdef COSIM
COSIM_RTLS:=$(patsubst %,$(RTL)/%_cosim.v,$(COSIM_MODULES))
else
COSIM_RTLS:=$(patsubst %,$(RTL)/%.v,$(COSIM_MODULES))
endif
RTLS+=$(COSIM_RTLS)

ifdef DEBUG
OUTP:=| tee out_cosim$(COSIM)
endif

IVOPT:=-DDUT_TB_VCD=\"$(DUT_TB).vcd\"
IVOPT+=-DSPI=1
IVOPT+=-DSIMULATION=1
IVOPT+=-DBOARD_ID=0
IVOPT+=-DBOARD_CK=32000000
IVOPT+=-D__UARTSPEED__=115200
IVOPT+=-I$(RTL)

ifdef SPI3WIRE
ifeq ($(SPI3WIRE),1)
$(DUT_TB).vvp: IVOPT+=-DSPI3WIREACTIVE
endif
$(DUT_TB).vvp: IVOPT+=-DSPI3WIRE
endif

ifdef COSIM
# IVERILOG_VPI_MODULE_PATH=.
# Cadence PLI Modules (Verilog-XL)
# vvp -mcadpli a.out -cadpli=./product.so:my_boot
IVOPT+=-L.
VPI:=$(patsubst %,%_vpi.vpi,$(COSIM_MODULES))
IVOPT+=$(patsubst %,-m %_vpi,$(COSIM_MODULES))
endif

all: $(TARGET)

view: $(DUT_TB).vcd
	$(GTKWAVE) ./$< ./$(DUT_TB).gtkw &

.PRECIOUS:$(DUT_TB).vcd
$(DUT_TB).vcd: $(DUT_TB).vvp $(VPI)
	$(VALGRIND) vvp $(VVPOPT) $< $(OUTP)

$(DUT_TB).vvp: $(RTLS) $(VPI)
	iverilog $(IVOPT) -o $@ $(RTLS)

VPI_CFLAGS := $(shell iverilog-vpi --cflags)
# High-Z; akin to Verilog's 1'bz and iverilog VPI's vpiZ
VPI_CFLAGS+=-DZ=2
ifdef DEBUG
VPI_CFLAGS+=-DDEBUG
endif
ifdef SPI3WIRE
VPI_CFLAGS+=-DSPI3WIRE
endif
%.o: $(SRC)/%.c
	$(CC) -c $^ $(VPI_CFLAGS) -Wall -Werror
%.vpi: %.o
	iverilog-vpi $<

clean:
	$(RM) *.o *.vvp *.vpi *.vcd vgcore.*
