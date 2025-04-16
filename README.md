# cosim_spi
Experiment cosimulation between pure Verilog, external VPI functions, and SystemC
using LIS3DH SPI (and I2C) sensor model and simple SPI master.
When ready, synchronize in production here: https://github.com/darklife/darkriscv/tree/master/rtl/lib/spi

LIS3DH: Refer to STMicroelectronics datasheet here:
https://www.st.com/resource/en/datasheet/lis3dh.pdf

simple SPI master: Credits to Victor Pecanins for initial version included in MAX10 Demos:
https://github.com/vpecanins/max1000-tutorial

- [X] pure Verilog (Icarus Verilog)
- [X] external VPI functions
- [ ] SystemC models
- [ ] Support 3-wire in cosim too
- [ ] Idea: try to connect to qemu-riscv + SW as alternative SPI testbench
- [ ] Idea: add similar support for I2C
