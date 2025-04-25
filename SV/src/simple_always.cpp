
#define EXPECT_EQ(l,r) do{assert((l) == (r));}while(0)

#include "Vsimple_always.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
/******************************************************************************/
#include <stdio.h>
#ifndef VCD_FILE
#define VCD_FILE "simple_always.vcd"
#endif
class TB {
    Vsimple_always* dut;
    VerilatedVcdC* tfp;
public:
    TB(int argc = 0, char **argv = 0):dut(0),tfp(0) {
        Verilated::commandArgs(argc, argv);
        dut = new Vsimple_always;
#ifdef VCD_FILE
        tfp = new VerilatedVcdC;
        Verilated::traceEverOn(true);
        dut->trace(tfp, 99);
        tfp->open(VCD_FILE);
#endif
        dut->clk = 0;
        dut->rst = 0;
        step();
        EXPECT_EQ(dut->q, 0);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->q, 1);

        dut->clk = 0;
        step();
        EXPECT_EQ(dut->q, 1);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->q, 0);

        dut->clk = 0;
        step();
        EXPECT_EQ(dut->q, 0);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->q, 1);

        dut->rst = 1;
        step();
        EXPECT_EQ(dut->q, 0);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->q, 0);

        dut->clk = 0;
        dut->rst = 0;
        step();
        EXPECT_EQ(dut->q, 0);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->q, 1);

        step();
        if (tfp){tfp->close();delete tfp;}
        delete dut;
    }
    void step(int inc = 10) {
        dut->eval();
        tfp->dump(Verilated::time());
        printf("%d %d %d %d\t at #%ld\n", dut->clk, dut->rst, dut->q, Verilated::time());
        Verilated::timeInc(inc);
    }
};

int main(int argc, char *argv[]) {
    TB tb(argc, argv);
    return 0;
}
