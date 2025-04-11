#include "counter.cc"

TESTCASE(TestCounter)

class CounterTest {
public:
    first_counter* counter;
    sc_signal<bool> clk;
    sc_signal<bool> reset;
    sc_signal<bool> enable;
    sc_signal<sc_uint<4> > count;
    sc_signal<sc_lv<4> > data;

    void SetUp() {
        counter = new first_counter("Counter");
        counter->clock(clk);
        counter->reset(reset);
        counter->enable(enable);
        counter->counter_out(count);
        counter->data(data);
    }

    void TearDown() {
        delete counter;
    }
};
