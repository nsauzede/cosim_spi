#include <ut/ut.h>
#include "Lis3dh_stub_tb.cpp"

    TESTMETHOD(test_Lis3dh_stub) {
        Lis3dh_stubTest t;
        t.SetUp();

        auto tu = SC_NS;
        sc_start(1, tu);

        t.clk.write(false);
        //t.reset.write(true);
        sc_start(1, tu);
        EXPECT_EQ(t.out_x_l_flag.read(), 0);

        sc_stop();

        t.TearDown();
    }
