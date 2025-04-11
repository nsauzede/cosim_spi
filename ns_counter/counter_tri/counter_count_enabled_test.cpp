#include <ut/ut.h>
#include "counter_tb.cpp"

    TESTMETHOD(test_counter_counts_with_enable) {
        CounterTest t;
        t.SetUp();

        auto tu = SC_NS;
        std::cout << "Initial sc_time_stamp: " << sc_time_stamp() << "\n";
        sc_start(1, tu);

        t.clk.write(false);
        t.reset.write(true);
        sc_start(1, tu);
        EXPECT_EQ(t.count.read(), 0);

        t.reset.write(false);
        sc_start(1, tu);

        t.clk.write(true);
        sc_start(1, tu);
        t.clk.write(false);
        sc_start(1, tu);

        EXPECT_EQ(t.count.read(), 0);

        t.enable.write(true);

        sc_start(1, tu);
        t.clk.write(true);
        sc_start(1, tu);
        t.clk.write(false);
        sc_start(1, tu);

        EXPECT_EQ(t.count.read(), 1);

        std::cout << "Final sc_time_stamp: " << sc_time_stamp() << "\n";
        sc_stop();

        t.TearDown();
    }
