# cosim_spi
Experiment cosimulation between pure Verilog (Icarus Verilog), external VPI functions ("COSIM"), SystemC ("SYC"), Qemu ("ISS")
using LIS3DH SPI (and I2C) sensor model and SPI master, that I contributed to the darkriscv open-source project:\
https://github.com/darklife/darkriscv/blob/master/rtl/lib/spi/spi_master.v
https://github.com/darklife/darkriscv/blob/master/rtl/lib/spi/lis3dh_stub.v

This work is used as a self-learning vehicle to reinforce my knowledge in digital-design (Verilog, testbenchs..),
embedded software (SPI/I2C, riscv32e, bare-metal C OS/drivers..) and cosimulation/coemulation (VPI/C, SystemC, Qemu..).

For more information about the context and how to run darkriscv SPI demos, see here:\
https://github.com/darklife/darkriscv/blob/master/boards/max1000_max10/README.md

Unit-tests: uses my own Unit-test framework:\
https://github.com/nsauzede/.ut

LIS3DH: Refer to STMicroelectronics datasheet here:\
https://www.st.com/resource/en/datasheet/lis3dh.pdf

SPI master: Supports the 4 and 3-wire (eg: bidirectional) interface of the LIS3DH sensor.\
Credits to Victor Pecanins for the initial simple spi_master version included in MAX10 Demos:\
https://github.com/vpecanins/max1000-tutorial

# Roadmap
- [X] pure Verilog
- [X] external VPI functions
- [X] SystemC models
- [X] Support 3-wire SPI in cosim/SYC too
- [ ] Idea: try to connect to qemu riscv32e + spidemo FW as alternative SPI testbench
- [ ] Idea: add similar support for I2C

# Developer Backlog
See [here](README_dev.md).
