extern "C" {
#include "../../src/lis3dh_stub.c"
}

#include "systemc.h"

SC_MODULE (Lis3dh_stub) {
    sc_in_clk             clk;            // Clock input of the design

    sc_in<sc_uint<16> >   out_x_resp;
    sc_out<bool>          out_x_l_flag;

    sc_in<bool>           csn;            // SPI chip select (active low)
    sc_in<bool>           sck;            // SPI clock
    sc_inout<sc_logic>    mosi;           // SPI master output slave input (default 4-wire); or m/s i/o (3-wire enabled)
    sc_out<bool>          miso;           // SPI master data input, slave data output

    void process() {
        int out_x_resp_ = out_x_resp.read(), csn_ = csn.read(), sck_ = sck.read();
        int out_x_l_flag_ = 0, mosi_ = mosi.read().value(), miso_ = 1;
        ::lis3dh_stub(out_x_resp_, &out_x_l_flag_, csn_, sck_, &mosi_, &miso_);
        out_x_l_flag.write(out_x_l_flag);
        miso.write(miso_);
//        if (mosi_ != 2)mosi.write(sc_logic(mosi_));
    }

    SC_CTOR(Lis3dh_stub, sc_trace_file *wf = 0, const char *hier = 0) {
        if (wf) {
            std::string n = hier ? hier : "";
            if (n != "") {
                n += ".";
            }
            n += name();
            n += ".clk";
            printf("%s: name=%s, Wow we're tracing %s\n", __func__, name(), n.c_str());
            sc_trace(wf, clk, n);
        }
        SC_METHOD(process);
        sensitive << clk.pos();
    }
};
