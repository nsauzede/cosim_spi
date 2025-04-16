extern "C" {
#include "../../src/spi_master.c"
}

#include "systemc.h"

SC_MODULE (Spi_master) {
    sc_inout<sc_uint<16> >      div_coef;

    sc_in_clk                   clk;          // Logic clock
    sc_in<sc_logic>             nrst;         // SPI is active when nreset is HIGH
    sc_in<bool>                 request;      // Request to start transfer: Active HIGH
    sc_in<sc_uint<5> >          nbits;
    sc_in<sc_uint<32> >         mosi_data;
    sc_out<sc_uint<32> >        miso_data;
    sc_out<bool>                ready;
    sc_out<sc_logic>            csn;            // SPI chip select (active low)
    sc_out<sc_logic>            sck;            // SPI clock
    sc_inout<sc_logic>          mosi;           // SPI master output slave input (default 4-wire); or m/s i/o (3-wire enabled)
    sc_in<sc_logic>             miso;           // SPI master data input, slave data output

    sc_uint<32>                 debug;
    sc_uint<32>                 miso_reg;
    bool                        oe;
    sc_uint<64> PROFILE;
    std::string name;
    SC_CTOR(Spi_master, sc_trace_file *tf = 0, const std::string& hier = ""):name(hier+"."+__func__) {
#ifdef SPI3WIRE
        PROFILE = 0x3357205359; // "3W SY"
#else
        PROFILE = 0x5f5f205359; // "__ SY"
#endif
        if (tf) {
            sc_trace(tf, PROFILE, name+".PROFILE");
            sc_trace(tf, div_coef, name+".div_coef");
            sc_trace(tf, debug, name+".debug");
            sc_trace(tf, miso_reg, name+".miso_reg");
            sc_trace(tf, oe, name+".oe");
            sc_trace(tf, clk, name+".clk");
            sc_trace(tf, nrst, name+".nrst");
            sc_trace(tf, request, name+".request");
            sc_trace(tf, nbits, name+".nbits");
            sc_trace(tf, mosi_data, name+".mosi_data");
            sc_trace(tf, miso_data, name+".miso_data");
            sc_trace(tf, ready, name+".ready");
            sc_trace(tf, csn, name+".csn");
            sc_trace(tf, sck, name+".sck");
            sc_trace(tf, mosi, name+".mosi");
            sc_trace(tf, miso, name+".miso");
        }
        SC_METHOD(process);
        sensitive << clk.pos();
    }

    void process() {
        int div_coef_ = div_coef.read();
        int nrst_ = nrst.read().value(), request_ = request.read(), nbits_ = nbits.read(), mosi_data_ = mosi_data.read(), miso_ = miso.read().value();
        int miso_data_ = 0, ready_ = 0, csn_ = 0, sck_ = 0, mosi_ = mosi.read().value();
        int debug_ = 0, miso_reg_ = 0;
        ::spi_master(&div_coef_, nrst_, mosi_data_, &miso_data_, nbits_, request_, &ready_, &csn_, &sck_, &mosi_, miso_, &debug_, &miso_reg_);
        div_coef.write(div_coef_);
        debug = debug_;
        miso_reg = miso_reg_;
        oe = debug[0];
        miso_data.write(miso_data_);
        ready.write(ready_);
        csn.write(sc_logic(csn_));
        sck.write(sc_logic(sck_));
        //if (mosi_ != 2)
            mosi.write(sc_logic(mosi_));
    }
};
