#include "Vtb.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
/******************************************************************************/
#include <stdio.h>
#ifndef VCD_FILE
//#define VCD_FILE "tb.vcd"
#endif
class TB {
    Vtb* dut;
#ifdef VCD_FILE_
    VerilatedVcdC* tfp;
#endif
public:
    TB(int argc = 0, char **argv = 0):dut(0)
#ifdef VCD_FILE_
    ,tfp(0) 
#endif
    {
        Verilated::commandArgs(argc, argv);
        dut = new Vtb;
        Verilated::traceEverOn(true);
#ifdef VCD_FILE_
        tfp = new VerilatedVcdC;
        dut->trace(tfp, 99);
        tfp->open(VCD_FILE);
#endif
        for (int i = 0; i < 11; i++) {
            dut->clk = !dut->clk;
            step();
        }

#ifdef VCD_FILE_
        if (tfp){tfp->close();delete tfp;}
#endif
        delete dut;
    }
    void step(int inc = 10) {
        dut->eval();
#ifdef VCD_FILE_
        if (tfp)tfp->dump(Verilated::time());
#endif
        printf("%d\t at #%ld\n", dut->clk, Verilated::time());
        Verilated::timeInc(inc);
    }
};

int main(int argc, char *argv[]) {
    TB tb(argc, argv);
    return 0;
}
