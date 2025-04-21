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

/*
    Simple LIS3DH SPI stub (STMicroelectronics LIS3DH accelerometer)
*/

static int lis3dh_stub(int out_x_resp, int *out_x_l_flag, int csn, int sck, int *mosi, int *miso, int *state_, int *bit_count_, int *shift_reg_, int *rd_, int *oe_, int *spi3w_) {
    const int Z = 2;    // High-Z; akin to Verilog's 1'bz and iverilog VPI's vpiZ
    static enum { IDLE = 0, RECEIVING = 1, PROCESSING = 2, RESPONDING = 3 } state = IDLE;
    static int stateff = 0;
    static int sck_d = 1;
    static int bit_count = 0;
    static int bit_countff = 0;
    static int shift_reg = 0;
    static int shift_regff = 0;
    static int misoff = 1;
//    int mosiff = mosi ? *mosi : Z;
    static int response = 0;
    static int out_x_l_flagff = 0;
    static int rd = 0;
    static int oe = 0;
    static int spi3w = 0;
#ifdef SPI3WIRE
    static int spi3w_flag = 0;
    static int spi3wff = 0;
#endif
    switch (state) {
        case IDLE:
#ifdef SPI3WIRE
            oe = 0;
            rd = 0;
#endif
            bit_count = 0;
            shift_reg = 0;
            misoff = 1;
            response = 0;
            out_x_l_flagff = 0;
            if (!csn && !sck) {
#ifdef SPI3WIRE
                rd = bit_countff == 0 ? (mosi ? *mosi : 0) : rd;
#endif
                state = RECEIVING;
            }
            break;
        case RECEIVING:
            if (sck && !sck_d) {
                shift_reg = (shift_regff << 1) | (mosi ? *mosi : 0);
                bit_count++;
                if (bit_countff == 7) {
                    state = PROCESSING;
                }
            }
            break;
        case PROCESSING:
            if ((shift_regff & 0x3f) == 0xf) {
                response = 0x33;
#ifdef SPI3WIRE
            } else if ((shift_regff & 0x3f) == 0x23) {
                spi3w_flag = 1;
                response = 0;
#endif
            } else if ((shift_regff & 0x3f) == 0x28) {
                response = (out_x_resp & 0xff);
                out_x_l_flagff = 1;
            } else {
                response = 0;
            }
            state = RESPONDING;
            bit_count = 0;
            break;
        case RESPONDING:
            if (csn) {
#ifdef SPI3WIRE
                spi3w_flag = 0;
                spi3w = spi3wff;
                oe = 0;
#endif
                state = IDLE;
            } else if (!sck && sck_d) {
#ifdef SPI3WIRE
                oe = 1;
#endif
                misoff = !!(response & 0x80);
                response <<= 1;
                bit_count++;
                if (bit_countff == 7) {
                    if (out_x_l_flagff) {
                        response = (out_x_resp >> 8) & 0xff;
                    } else {
                        response = 0;
                    }
                }
#ifdef SPI3WIRE
                if (spi3w_flag) {
                    if (bit_countff == 7) {
                        spi3wff = (mosi ? *mosi : 0);
                        spi3w_flag = 0;
                    }
                }
#endif
            }
            break;
    }
    if (out_x_l_flag) { *out_x_l_flag = !csn ? out_x_l_flagff : 0; }
#ifdef SPI3WIRE
    if (mosi) { *mosi = spi3w && oe && rd ? misoff : *mosi; }
    if (miso) { *miso = !spi3w && oe && rd ? misoff : !spi3w && !oe && rd ? 1 : Z; }
#else
    if (miso) { *miso = (state == IDLE) ? Z : misoff; }
#endif
    if (rd_) { *rd_ = rd; }
    if (oe_) { *oe_ = oe; }
    if (spi3w_) { *spi3w_ = spi3w; }
    if (state_) { *state_ = stateff; }
    if (bit_count_) { *bit_count_ = bit_countff; }
    if (shift_reg_) { *shift_reg_ = shift_regff; }
    stateff = state;
    bit_countff = bit_count;
    shift_regff = shift_reg;
    sck_d = sck;
    return 0;
}
