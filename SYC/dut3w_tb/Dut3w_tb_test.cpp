/*
 * Copyright (c) 2025, Nicolas Sauzede <nicolas.sauzede@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ut/ut.h>

#include "Lis3dh_stub.cc"
#include "Spi_master.cc"

#define SPI_CHANGE_COEF

SC_MODULE (LogicNot) {
    sc_in<sc_logic>     in;
    sc_out<sc_logic>    out;
    void LogicNegate() {
        if (in.read() == SC_LOGIC_1) out.write(SC_LOGIC_0);
        else if (in.read() == SC_LOGIC_0) out.write(SC_LOGIC_1);
        else out.write(SC_LOGIC_X);
    }
    SC_CTOR(LogicNot) {
        SC_METHOD(LogicNegate);
        sensitive << in;
    }
};

const int time_mult = 1000;
const sc_time_unit time_unit = SC_PS;
const int clk_period = 10;
void Wait(int t) { wait(sc_time(t * time_mult, time_unit)); }
void Wait(const sc_signal<bool>& sb) {
    while (sb.read() != true) {
        wait(sb.value_changed_event());
    }
}
SC_MODULE(ClockGen) {
    sc_out<bool> clk;
    sc_time half_period;
    SC_CTOR(ClockGen) : half_period(time_mult*clk_period/2, time_unit) { SC_THREAD(run); }
    void run() { while (1) {clk.write(false);wait(half_period);clk.write(true);wait(half_period);}}
};

SC_MODULE(Dut3w_tb) {
    ClockGen* ClockGen1;
    sc_signal<bool>             clk;
    LogicNot* ResetGen1;
    sc_signal_resolved          reset;
    Lis3dh_stub* Lis3dh_stub1;
    sc_signal<sc_uint<16> >     out_x_resp;
    sc_signal<bool>             out_x_l_flag;
    sc_signal_resolved          spi_csn;
    sc_signal_resolved          spi_sck;
    sc_signal_resolved          spi_mosi;
    sc_signal_resolved          spi_miso;
    Spi_master* Spi_master1;
    sc_signal<sc_uint<16> >     div_coef;
    sc_signal_resolved          nrst;
    sc_signal<bool>             request;
    sc_signal<sc_uint<5> >      nbits;
    sc_signal<sc_uint<32> >     mosi_data;
    sc_signal<sc_uint<32> >     miso_data;
    sc_signal<bool>             ready;

    std::string name;
    SC_CTOR(Dut3w_tb, sc_trace_file* tf=0, const std::string& hier = ""):name(hier+"."+__func__) {
        if (tf) {
            sc_trace(tf, div_coef, name+".div_coef");
            sc_trace(tf, clk, name+".clk");
            sc_trace(tf, nrst, name+".nrst");
            sc_trace(tf, request, name+".request");
            sc_trace(tf, nbits, name+".nbits");
            sc_trace(tf, mosi_data, name+".mosi_data");
            sc_trace(tf, miso_data, name+".miso_data");
            sc_trace(tf, ready, name+".ready");
            sc_trace(tf, spi_csn, name+".spi_csn");
            sc_trace(tf, spi_sck, name+".spi_sck");
            sc_trace(tf, spi_mosi, name+".spi_mosi");
            sc_trace(tf, spi_miso, name+".spi_miso");
            sc_trace(tf, out_x_resp, name+".out_x_resp");
            sc_trace(tf, reset, name+".reset");
        }
        ClockGen1 = new ClockGen("ClockGen1");
        ClockGen1->clk(clk);
        ResetGen1 = new LogicNot("ResetGen1");
        ResetGen1->in(reset);
        ResetGen1->out(nrst);
        Lis3dh_stub1 = new Lis3dh_stub("Lis3dh_stub1", tf, name);
        Lis3dh_stub1->clk(clk);
        Lis3dh_stub1->out_x_resp(out_x_resp);
        Lis3dh_stub1->out_x_l_flag(out_x_l_flag);
        Lis3dh_stub1->csn(spi_csn);
        Lis3dh_stub1->sck(spi_sck);
        Lis3dh_stub1->mosi(spi_mosi);
        Lis3dh_stub1->miso(spi_miso);
        Spi_master1 = 0;
        setbuf(stdout, 0);
        Spi_master1 = new Spi_master("Spi_master1", tf, name);
        Spi_master1->div_coef(div_coef);
        Spi_master1->clk(clk);
        Spi_master1->nrst(nrst);
        Spi_master1->request(request);
        Spi_master1->nbits(nbits);
        Spi_master1->mosi_data(mosi_data);
        Spi_master1->miso_data(miso_data);
        Spi_master1->ready(ready);
        Spi_master1->csn(spi_csn);
        Spi_master1->sck(spi_sck);
        Spi_master1->mosi(spi_mosi);
        Spi_master1->miso(spi_miso);

        SC_THREAD(run);
    }
    ~Dut3w_tb() {
        delete Lis3dh_stub1;
        delete Spi_master1;
        delete ClockGen1;
        delete ResetGen1;
    }
    void run() {
        reset.write(sc_logic(1));
        Wait(50); reset.write(sc_logic(0));

#ifdef SPI_CHANGE_COEF
        Wait(clk_period);
        nbits.write(0);
        mosi_data.write(0x80800000);
        request.write(1);
        Wait(clk_period*2);
        request.write(0);
        mosi_data.write(0);
        Wait(clk_period);
#endif

#ifdef SPI3WIRE
#ifdef SPI3WIREACTIVE
        // By default, both master and slave are in 4-wire mode; we will turn them to 3-wire mode sequentially
/* Begin switch the slave to 3-wire mode */
        // write SIM=1 (01h) in CTRL_REG4 (23h) to set slave to 3-wire mode
        nbits.write(15);
        mosi_data.write(0x2301);
        Wait(50); request.write(1);
        Wait(10); request.write(0);
        Wait(ready);
/* End switch the slave to 3-wire mode */
/* Begin switch the master to 3-wire mode */
        Wait(clk_period);
        nbits.write(0);
        mosi_data.write(0x81010000);    // `set 3-wire on`
        request.write(1);
        Wait(clk_period*2);
        request.write(0);
        mosi_data.write(0);
        Wait(clk_period);
/* End switch the master to 3-wire mode */
#endif
#endif
        // 3 or 4-wire whoami
        nbits.write(15);
        mosi_data.write(0x8f00);
        Wait(50); request.write(1);
        Wait(10); request.write(0);
        Wait(ready);
        int miso_data_ = miso_data.read();
        if (!EXPECT_EQ(miso_data_ & 0xff, 0x33, "BAD miso_data=%x", miso_data_)) {
            reset.write(sc_logic(3));
            Wait(1000);
            goto fatal;
        }

#ifdef SPI3WIRE
#ifdef SPI3WIREACTIVE
/* Begin switch the master back to 4-wire mode */
        Wait(clk_period);
        nbits.write(0);
        mosi_data.write(0x81000000);    // `set 3-wire off (4-wire default)`
        request.write(1);
        Wait(clk_period*2);
        request.write(0);
        mosi_data.write(0);
        Wait(clk_period);
/* End switch the master back to 4-wire mode */

/* Begin switch the slave back to 4-wire mode */
        // write SIM=0 (00h) in CTRL_REG4 (23h) to set slave back to 4-wire mode
        nbits.write(15);
        mosi_data.write(0x2300);
        Wait(50); request.write(1);
        Wait(10); request.write(0);
        Wait(ready);
/* End switch the slave back to 4-wire mode */

        // 4-wire whoami
        nbits.write(15);
        mosi_data.write(0x8f00);
        Wait(50); request.write(1);
        Wait(10); request.write(0);
        Wait(ready);
        miso_data_ = miso_data.read();
        if (!EXPECT_EQ(miso_data_ & 0xff, 0x33, "BAD miso_data=%x", miso_data_)) {
            reset.write(sc_logic(3));
            Wait(1000);
            goto fatal;
        }
#endif
#endif

        Wait(200);
fatal:
        sc_stop();
    }
};

TESTMETHOD(test_Dut3w) {
    sc_trace_file* tf = sc_create_vcd_trace_file("dut3w_tb/Dut3w_tb");
    tf->set_time_unit(1, time_unit);
    Dut3w_tb Dut3w_tb("Dut3w_tb1", tf);
    sc_start();
    sc_close_vcd_trace_file(tf);
}
