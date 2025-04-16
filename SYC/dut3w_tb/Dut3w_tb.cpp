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

#include "Lis3dh_stub.cc"
#include "Spi_master.cc"

TESTCASE(TestDut3w)

class Dut3wTest {
public:
    const char *name;
    int clk_period;
    sc_time_unit tu;
    Dut3wTest(int clk_period = 10, sc_time_unit tu = SC_NS):name(__func__),clk_period(clk_period),tu(tu) {}

    Lis3dh_stub* Lis3dh_stub1;
    sc_signal<bool>             clk;
    sc_signal<sc_uint<16> >     out_x_resp;
    sc_signal<bool>             out_x_l_flag;
    sc_signal<bool>             spi_csn;
    sc_signal<bool>             spi_sck;
    sc_signal_resolved          spi_mosi;
    sc_signal<bool>             spi_miso;

    Spi_master* Spi_master1;
    sc_signal<sc_uint<16> >     div_coef;
    sc_signal<bool>             nrst;
    sc_signal<bool>             request;
    sc_signal<sc_uint<5> >      nbits;
    sc_signal<sc_uint<32> >     mosi_data;
    sc_signal<sc_uint<32> >     miso_data;
    sc_signal<bool>             ready;

    sc_trace_file *wf;

    void Wait(int clocks) {
        for (int i=0;i<clocks / clk_period;i++) {
            clk.write(false);
            sc_start(clk_period / 2, tu);
            clk.write(true);
            sc_start(clk_period / 2, tu);
        }
    }
    void SetUp() {
        wf = sc_create_vcd_trace_file("dut3w_tb/Dut3w_tb");
        wf->set_time_unit(1, SC_NS);
        std::string n = name;
        sc_trace(wf, div_coef, n + ".div_coef");
        sc_trace(wf, clk, n + ".clk");
        sc_trace(wf, nrst, n + ".nrst");
        sc_trace(wf, request, n + ".request");
        sc_trace(wf, nbits, n + ".nbits");
        sc_trace(wf, mosi_data, n + ".mosi_data");
        sc_trace(wf, miso_data, n + ".miso_data");
        sc_trace(wf, ready, n + ".ready");
        sc_trace(wf, spi_csn, n + ".spi_csn");
        sc_trace(wf, spi_sck, n + ".spi_sck");
        sc_trace(wf, spi_mosi, n + ".spi_mosi");
        sc_trace(wf, spi_miso, n + ".spi_miso");
        sc_trace(wf, out_x_resp, n + ".out_x_resp");
        Lis3dh_stub1 = new Lis3dh_stub("Lis3dh_stub1", wf, name);
        Lis3dh_stub1->clk(clk);
        Lis3dh_stub1->out_x_resp(out_x_resp);
        Lis3dh_stub1->out_x_l_flag(out_x_l_flag);
        Lis3dh_stub1->csn(spi_csn);
        Lis3dh_stub1->sck(spi_sck);
        Lis3dh_stub1->mosi(spi_mosi);
        Lis3dh_stub1->miso(spi_miso);
        Spi_master1 = 0;
        setbuf(stdout, 0);
        Spi_master1 = new Spi_master("Spi_master1");
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
    }

    void TearDown() {
        sc_close_vcd_trace_file(wf);
        delete Lis3dh_stub1;
        delete Spi_master1;
    }
};
