extern "C" {
#include "../../src/spi_master.c"
}

#include "systemc.h"

SC_MODULE (Spi_master) {
    sc_inout<sc_uint<16> >      div_coef;

    sc_in_clk             clk;          // Logic clock
    sc_in<bool>           nrst;         // SPI is active when nreset is HIGH

    sc_in<bool>           request;      // Request to start transfer: Active HIGH
    sc_in<sc_uint<5> >    nbits;
    sc_in<sc_uint<32> >   mosi_data;
    sc_out<sc_uint<32> >  miso_data;
    sc_out<bool>          ready;

    sc_out<bool>          csn;            // SPI chip select (active low)
    sc_out<bool>          sck;            // SPI clock
    sc_inout<sc_logic>    mosi;           // SPI master output slave input (default 4-wire); or m/s i/o (3-wire enabled)
    sc_in<bool>           miso;           // SPI master data input, slave data output

    void process() {
        int div_coef_ = div_coef.read();
        int nrst_ = nrst.read(), request_ = request.read(), nbits_ = nbits.read(), mosi_data_ = mosi_data.read(), miso_ = miso.read();
        int miso_data_ = 0, ready_ = 0, csn_ = 0, sck_ = 0, mosi_ = mosi.read().value();
        ::spi_master(&div_coef_, nrst_, mosi_data_, &miso_data_, nbits_, request_, &ready_, &csn_, &sck_, &mosi_, miso_);
        div_coef.write(div_coef_);
        miso_data.write(miso_data_);
        ready.write(ready_);
        csn.write(csn_);
        sck.write(sck_);
        //if (mosi_ != 2)
            mosi.write(sc_logic(mosi_));
    }

    SC_CTOR(Spi_master) {
        SC_METHOD(process);
        sensitive << clk.pos();
    }
};
