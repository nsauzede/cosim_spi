#include "counter.cc"

#include <ut/ut.h>

TESTCASE(TestCounter)
    TESTMETHOD(test_counter_testbench) {


// Testbench for the 4-bit up-counter ---------------->
// Example from www.asic-world.com (with fixes)
//-----------------------------------------------------
  sc_signal<bool>   clock;
  sc_signal<bool>   reset;
  sc_signal<bool>   enable;
  sc_signal<sc_uint<4> > counter_out;
  int i = 0;
  // Connect the DUT
  first_counter counter("COUNTER");
  counter.clock(clock);
  counter.reset(reset);
  counter.enable(enable);
  counter.counter_out(counter_out);

        std::cout << "Initial sc_time_stamp: " << sc_time_stamp() << "\n";
  sc_start(1, SC_NS);

        EXPECT_EQ(0, clock.read());
        EXPECT_EQ(reset, 0);
        EXPECT_EQ(enable, 0);
        EXPECT_EQ(0, counter_out.read());


  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("counter/test_counter_testbench");
  // Dump the desired signals
  sc_trace(wf, clock, "clock");
  sc_trace(wf, reset, "reset");
  sc_trace(wf, enable, "enable");
  sc_trace(wf, counter_out, "count");

  // Initialize all variables
  reset = 0;       // initial value of reset
  enable = 0;      // initial value of enable
  for (i=0;i<5;i++) {
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
        EXPECT_EQ(0, counter_out.read());
  reset = 1;    // Assert the reset
  cout << "@" << sc_time_stamp() <<" Asserting reset\n" << endl;
  for (i=0;i<10;i++) {
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
        EXPECT_EQ(0, counter_out.read());
  reset = 0;    // De-assert the reset
  cout << "@" << sc_time_stamp() <<" De-Asserting reset\n" << endl;
  for (i=0;i<5;i++) {
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
        EXPECT_EQ(0, counter_out.read());
  cout << "@" << sc_time_stamp() <<" Asserting Enable\n" << endl;
  enable = 1;  // Assert enable
  for (i=0;i<20;i++) {
        EXPECT_EQ(i % (2 << 3), counter_out.read());
    clock = 0; 
    sc_start(1, SC_NS);
    clock = 1; 
    sc_start(1, SC_NS);
  }
        EXPECT_EQ(20 % (2 << 3), counter_out.read());
  cout << "@" << sc_time_stamp() <<" De-Asserting Enable\n" << endl;
  enable = 0; // De-assert enable
    sc_start(2*2, SC_NS);

  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);

        std::cout << "Final sc_time_stamp: " << sc_time_stamp() << "\n";
        sc_stop();
    }
