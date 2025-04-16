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

TESTCASE(TestLis3dh_stub)

class Lis3dh_stubTest {
public:
    Lis3dh_stub* Lis3dh_stub1;
    sc_signal<bool>             clk;
    sc_signal<sc_uint<16> >     out_x_resp;
    sc_signal<bool>             out_x_l_flag;
    sc_signal<bool>             csn;
    sc_signal<bool>             sck;
    sc_signal_resolved          mosi;
    sc_signal<bool>             miso;

    void SetUp() {
        Lis3dh_stub1 = new Lis3dh_stub("Lis3dhStub1");
        Lis3dh_stub1->clk(clk);
        Lis3dh_stub1->out_x_resp(out_x_resp);
        Lis3dh_stub1->out_x_l_flag(out_x_l_flag);
        Lis3dh_stub1->csn(csn);
        Lis3dh_stub1->sck(sck);
        Lis3dh_stub1->mosi(mosi);
        Lis3dh_stub1->miso(miso);
    }

    void TearDown() {
        delete Lis3dh_stub1;
    }
};
