
#define EXPECT_EQ(l,r) do{assert((l) == (r));}while(0)

#include "Vinst.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
/******************************************************************************/
#include <stdio.h>
#ifndef VCD_FILE
#define VCD_FILE "inst.vcd"
#endif
class TB {
    Vinst* dut;
    VerilatedVcdC* tfp;
public:
    TB(int argc = 0, char **argv = 0):dut(0),tfp(0) {
        Verilated::commandArgs(argc, argv);
        dut = new Vinst;
#ifdef VCD_FILE
        tfp = new VerilatedVcdC;
        Verilated::traceEverOn(true);
        dut->trace(tfp, 99);
        tfp->open(VCD_FILE);
#endif
        dut->clk = 0;
        dut->x = 0;
        dut->y = 0;
        step();
        EXPECT_EQ(dut->z, 0);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->z, 0);

        dut->clk = 0;
        dut->x = 1;
        step();
        EXPECT_EQ(dut->z, 1);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->z, 1);

        dut->clk = 0;
        dut->y = 3;
        step();
        EXPECT_EQ(dut->z, 4);

        dut->clk = 1;
        step();
        EXPECT_EQ(dut->z, 4);
#if 0
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
#endif
        step();
        if (tfp){tfp->close();delete tfp;}
        delete dut;
    }
    void step(int inc = 10) {
        dut->eval();
        tfp->dump(Verilated::time());
        printf("%d %d %d %d\t at #%ld\n", dut->clk, dut->x, dut->y, dut->z, Verilated::time());
        Verilated::timeInc(inc);
    }
};

int main(int argc, char *argv[]) {
    TB tb(argc, argv);
    return 0;
}
