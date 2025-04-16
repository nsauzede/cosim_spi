#include <ut/ut.h>
#include "Dut3w_tb.cpp"

    TESTMETHOD(test_Dut3w) {
        int clk_period = 10;
        Dut3wTest t(clk_period);
        t.SetUp();

        t.nrst.write(0);
        t.Wait(50);
        t.nrst.write(1);

#define SPI_CHANGE_COEF
#ifdef SPI_CHANGE_COEF
        t.Wait(clk_period);
        t.nbits.write(0);
        t.mosi_data.write(0x80800000);
        t.request.write(1);
        t.Wait(clk_period*2);
        t.request.write(0);
        t.mosi_data.write(0);
        t.Wait(clk_period);
#endif

        t.nbits.write(15);
        t.mosi_data.write(0x8f00);
        t.Wait(50);
        t.request.write(1);
        t.Wait(10);
        t.request.write(0);

//        EXPECT_EQ(t.miso_data.read(), 0x33);
        t.Wait(1000);

        t.Wait(200);
        sc_stop();

        t.TearDown();
    }
