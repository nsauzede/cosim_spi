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

#ifdef DEBUG
#include <stdio.h>
#endif

static int spi_master(int *div_coef, int nrst, int mosi_data, int *miso_data, int nbits, int request, int *ready, int *spi_csn, int *spi_sck, int *spi_mosi, int spi_miso, int *debug, int *miso_reg_) {
    const int Z = 2;    // High-Z; akin to Verilog's 1'bz and iverilog VPI's vpiZ
#ifdef SPI3WIRE
    const int X = 3;    // Unknown; akin to Verilog's 1'bx and iverilog VPI's vpiX
#endif
    typedef enum { STATE_Idle = 0, STATE_Run = 1, STATE_High = 2, STATE_Low = 3, STATE_Finish = 4, STATE_End = 5 } State_t;
    static State_t state = STATE_Idle;
    static int data_in_reg = 0;
    static int nbits_reg = 0;
    static int bit_counter = 0;
    static int oe = 0;
#ifdef SPI3WIRE
    static int rd = 0;
    static int spi3w = 0;
    static int rd_tri = 0;
#endif
#ifdef DEBUG
    static int divider = 0;
    State_t stateff = state;
    int data_in_regff = data_in_reg;
    int nbits_regff = nbits_reg;
    int bit_counterff = bit_counter;
#endif

    static int miso_reg = 0;
    static int readyff = 0;
    //int readyff2 = readyff;
    static int spi_csnff = 1;
    static int spi_sckff = 1;
    static int spi_mosiff = 1;

    // Frequency divider
    static int divider_pre = 0;
    int divider_out_pre = 0;
    static int divider_out = 0;
    static int configure = 0;
    static int div_coef_ = -1;
    if (div_coef_ == -1) div_coef_ = div_coef ? *div_coef : 0;

    // Frequency divider
    if (!nrst || configure) {
        divider_pre = 0;
        divider_out_pre = 0;
        if (nrst && configure) {
            div_coef_ = (nbits == 0) && ((mosi_data & (1 << 31)) && (mosi_data & (1 << 23))) ? mosi_data & 0xffff : div_coef_;
            //printf("Set div_coef_=%x\n", div_coef_);
        }
    } else {
        if (divider_pre <= div_coef_) {
            divider_pre++;
            divider_out_pre = 0;
        } else {
            divider_pre = 0;
            divider_out_pre = 1;
        }
    }

    if (!nrst ) {
#ifdef SPI3WIRE
        rd_tri = 0;
        rd = 0;
#endif
        oe = 0;
        spi_csnff = 1;
        spi_sckff = 1;
        spi_mosiff = 1;
        readyff = 0;
        miso_reg = 0;

        data_in_reg = 0;
        nbits_reg = 0;
        bit_counter = 0;

        state = STATE_Idle;
    } else {
        switch (state) {
            case STATE_Idle:
                configure = 0;
                spi_csnff = 1;
                spi_sckff = 1;
                spi_mosiff = 1;
                if (request) {
                    if (nbits == 0) {
#ifdef SPI3WIRE
                        spi3w = (nbits == 0) && ((mosi_data & (1 << 31)) && (mosi_data & (1 << 24))) ? !!(mosi_data & (1 << 16)) : spi3w;
                        //printf("Set spi3w=%d\n", spi3w);
#endif
                        configure = 1;
                    } else {
                        oe = 1;
                        state = STATE_Run;
                        readyff = 0;
                        spi_csnff = 0;
                        data_in_reg = mosi_data;
                        nbits_reg = nbits;
                        bit_counter = nbits;
                        miso_reg = 0;
                    }
                }
                break;
            case STATE_Run:
                if (nbits_reg == 0x1f) {
#ifdef SPI3WIRE
                    rd = !!(data_in_reg & (1 << 31));
#endif
                    state = STATE_High;
                } else {
                    data_in_reg <<= 1;
                    nbits_reg++;
                }
                break;
            case STATE_High:
                if (divider_out) {
                    spi_sckff = 0;
                    spi_mosiff = 1 & (data_in_reg >> 31);
                    state = STATE_Low;
#ifdef SPI3WIRE
                    if (bit_counter == (nbits - 8)) {
                        if (spi3w && rd) {
                            rd_tri = 1;
                        }
                    }
#endif
                }
                break;
            case STATE_Low:
                if (divider_out) {
                    spi_sckff = 1;
#ifdef SPI3WIRE
                    miso_reg = ((miso_reg & 0x7fffffff) << 1) | (spi3w ? (spi_mosi ? *spi_mosi : X) : rd ? spi_miso : 0);
                    //printf("Read from %s\n", spi3w ? (spi_mosi ? "MOSI" : "Z") : rd ? "MISO" : "0");
#else
                    miso_reg = ((miso_reg & 0x7fffffff) << 1) | spi_miso;
#endif
                    if (bit_counter == 0) {
                        state = STATE_Finish;
                    } else {
#ifdef SPI3WIRE
                        if (bit_counter == (nbits - 8)) {
                            if (spi3w && rd) {
                                rd_tri = 1;
                            }
                        }
#endif
                        bit_counter--;
                        data_in_reg <<= 1;
                        state = STATE_High;
                    }
                }
                break;
            case STATE_Finish:
                if (divider_out) {
                    spi_csnff = 1;
                    spi_sckff = 1;
                    spi_mosiff = 0;
                    state = STATE_End;
                }
                break;
            case STATE_End:
                if (divider_out) {
                    readyff = 1;
                    oe = 0;
#ifdef SPI3WIRE
                    rd = 0;
                    rd_tri = 0;
#endif
                    state = STATE_Idle;
                }
                break;
        }
    }
#ifdef DEBUG
    printf("%08u nrst%d mosi_data%08x nbits%02x request%d spi_miso%d divider%d divider_out%d data_in_reg%08x nbits_reg%02x bit_counter%02x state%1d\n", sim_time.low, nrst, mosi_data, nbits, request, spi_miso, divider, divider_out, data_in_regff, nbits_regff, bit_counterff, stateff);
    divider = divider_pre;
#endif
    divider_out = divider_out_pre;

    if (miso_data) { *miso_data = miso_reg; }
    if (ready) { *ready = readyff; }
    if (spi_csn) { *spi_csn = oe ? spi_csnff : Z; }
    if (spi_sck) { *spi_sck = oe ? spi_sckff : Z; }
#ifdef SPI3WIRE
    if (spi_mosi) { *spi_mosi = !oe || (spi3w && rd_tri) ? Z : spi_mosiff; }
#else
    if (spi_mosi) { *spi_mosi = oe ? spi_mosiff : Z; }
#endif
    if (debug) { *debug = oe; }
    if (div_coef) { *div_coef = div_coef_; }
    if (miso_reg_) { *miso_reg_ = miso_reg; }
    return 0;
}
