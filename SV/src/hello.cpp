#include "Vhello.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
/******************************************************************************/
#include <stdio.h>
#ifndef VCD_FILE
#define VCD_FILE "hello.vcd"
#endif
class TB {
    Vhello* dut;
    VerilatedVcdC* tfp;
public:
    TB(int argc = 0, char **argv = 0):dut(0),tfp(0) {
        Verilated::commandArgs(argc, argv);
        dut = new Vhello;
#ifdef VCD_FILE
        tfp = new VerilatedVcdC;
        Verilated::traceEverOn(true);
        dut->trace(tfp, 99);
        tfp->open(VCD_FILE);
#endif
        for (int i = 0; i < 10; i++) {
            dut->clk = !dut->clk;
            step();
        }

        step();
        if (tfp){tfp->close();delete tfp;}
        delete dut;
    }
    void step(int inc = 10) {
        dut->eval();
        tfp->dump(Verilated::time());
        //printf("%d %d %d %d\t at #%ld\n", dut->a, dut->b, dut->sel, dut->y, Verilated::time());
        Verilated::timeInc(inc);
    }
};

int main(int argc, char *argv[]) {
    TB tb(argc, argv);
    return 0;
}
