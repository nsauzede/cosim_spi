extern "C" {
#include "../../src/lis3dh_stub.c"
}

#include "systemc.h"

SC_MODULE (Lis3dh_stub) {
    sc_in_clk           clk;            // Clock input of the design
    sc_in<sc_uint<16> > out_x_resp;
    sc_out<bool>        out_x_l_flag;
    sc_in<sc_logic>     csn;            // SPI chip select (active low)
    sc_in<sc_logic>     sck;            // SPI clock
    sc_inout<sc_logic>  mosi;           // SPI master output slave input (default 4-wire); or m/s i/o (3-wire enabled)
    sc_out<sc_logic>    miso;           // SPI master data input, slave data output

    std::string name;
    sc_uint<2>          state;
    sc_uint<4>          bit_count;
    sc_uint<8>          shift_reg;
    sc_signal<bool>     rd, oe, spi3w;
    SC_CTOR(Lis3dh_stub, sc_trace_file *tf = 0, const std::string& hier = ""):name(hier+"."+__func__),state(0),bit_count(0) {
        if (tf) {
            sc_trace(tf, state, name+".state");
            sc_trace(tf, bit_count, name+".bit_count");
            sc_trace(tf, shift_reg, name+".shift_reg");
            sc_trace(tf, rd, name+".rd");
            sc_trace(tf, oe, name+".oe");
            sc_trace(tf, spi3w, name+".spi3w");

            sc_trace(tf, clk, name+".clk");
            sc_trace(tf, out_x_resp, name+".out_x_resp");
            sc_trace(tf, out_x_l_flag, name+".out_x_l_flag");
            sc_trace(tf, csn, name+".csn");
            sc_trace(tf, sck, name+".sck");
            sc_trace(tf, mosi, name+".mosi");
            sc_trace(tf, miso, name+".miso");
        }
        SC_METHOD(process);
        sensitive << clk.pos();
    }

    void process() {
        int out_x_resp_ = out_x_resp.read(), csn_ = csn.read().value(), sck_ = sck.read().value();
        int out_x_l_flag_ = 0, mosi_ = mosi.read().value(), miso_ = 1;
        int state_ = 0, bit_count_ = 0, shift_reg_ = 0, rd_ = 0, oe_ = 0, spi3w_ = 0;
        ::lis3dh_stub(out_x_resp_, &out_x_l_flag_, csn_, sck_, &mosi_, &miso_, &state_, &bit_count_, &shift_reg_, &rd_, &oe_, &spi3w_);
        out_x_l_flag.write(out_x_l_flag);
        miso.write(sc_logic(miso_));
        if (rd_ && oe_ && spi3w_) {
            mosi.write(sc_logic(mosi_));
        } else {
            mosi.write(sc_logic(2));
        }
        rd.write(rd_);
        oe.write(oe_);
        spi3w.write(spi3w_);
        state = state_;
        bit_count = bit_count_;
        shift_reg = shift_reg_;
    }
};
