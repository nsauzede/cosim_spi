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

#include <stdio.h>

static int spi_master_stub(int div_coef, int nrst, int mosi_data, int *miso_data, int nbits, int request, int *ready, int *spi_csn, int *spi_sck, int *spi_mosi, int spi_miso) {
    static int divider_pre = 0;
    int divider_out_pre = 0;
    static int divider_out = 0;
    typedef enum { STATE_Idle = 0, STATE_Run = 1, STATE_High = 2, STATE_Low = 3, STATE_Finish = 4, STATE_End = 5 } State_t;
    static State_t state = STATE_Idle;
    static int data_in_reg = 0;
    static int nbits_reg = 0;
    static int bit_counter = 0;
#ifdef DEBUG
    static int divider = 0;
    State_t stateff = state;
    int data_in_regff = data_in_reg;
    int nbits_regff = nbits_reg;
    int bit_counterff = bit_counter;
#endif

    static int miso_dataff = 0;
    static int readyff = 0;
    //int readyff2 = readyff;
    static int spi_csnff = 1;
    static int spi_sckff = 1;
    static int spi_mosiff = 1;

    if (!nrst) {
        divider_pre = 0;
        divider_out_pre = 0;

        spi_csnff = 1;
        spi_sckff = 1;
        spi_mosiff = 1;
        readyff = 0;
        miso_dataff = 0;

        data_in_reg = 0;
        nbits_reg = 0;
        bit_counter = 0;

        state = STATE_Idle;
    } else {
        if (divider_pre != div_coef) {
            divider_pre++;
            divider_out_pre = 0;
        } else {
            divider_pre = 0;
            divider_out_pre = 1;
        }

        switch (state) {
            case STATE_Idle:
                spi_csnff = 1;
                spi_sckff = 1;
                spi_mosiff = 1;
                if (request) {
                    state = STATE_Run;
                    readyff = 0;
                    spi_csnff = 0;
                    data_in_reg = mosi_data;
                    nbits_reg = nbits;
                    bit_counter = nbits;
                    miso_dataff = 0;
                }
                break;
            case STATE_Run:
                if (nbits_reg == 0x1f) {
                    state = STATE_High;
                } else {
                    data_in_reg <<= 1;
                    nbits_reg++;
                }
                break;
            case STATE_High:
                if (divider_out) {
                    state = STATE_Low;
                    spi_sckff = 0;
                    spi_mosiff = 1 & (data_in_reg >> 31);
                }
                break;
            case STATE_Low:
                if (divider_out) {
                    if (bit_counter == 0) {
                        state = STATE_Finish;
                    } else {
                        state = STATE_High;
                        bit_counter--;
                        data_in_reg <<= 1;
                    }
                    spi_sckff = 1;
                    miso_dataff = ((miso_dataff & 0x7fffffff) << 1) | spi_miso;
                }
                break;
            case STATE_Finish:
                if (divider_out) {
                    state = STATE_End;
                    spi_csnff = 1;
                    spi_sckff = 1;
                    spi_mosiff = 0;
                }
                break;
            case STATE_End:
                if (divider_out) {
                    state = STATE_Idle;
                    readyff = 1;
                }
                break;
        }
    }
#ifdef DEBUG
    printf("%08u nrst%d mosi_data%08x nbits%02x request%d spi_miso%d divider%d divider_out%d data_in_reg%08x nbits_reg%02x bit_counter%02x state%1d\n", sim_time.low, nrst, mosi_data, nbits, request, spi_miso, divider, divider_out, data_in_regff, nbits_regff, bit_counterff, stateff);
    divider = divider_pre;
#endif
    divider_out = divider_out_pre;

    if (miso_data) { *miso_data = miso_dataff; }
    if (ready) { *ready = readyff; }
    if (spi_csn) { *spi_csn = nrst ? spi_csnff : Z; }
    if (spi_sck) { *spi_sck = nrst ? spi_sckff : Z; }
    if (spi_mosi) { *spi_mosi = nrst ? spi_mosiff : Z; }
    return 0;
}
