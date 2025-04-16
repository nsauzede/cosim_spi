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

static int lis3dh_stub(int out_x_resp, int *out_x_l_flag, int csn, int sck, int mosi, int *miso) {
    static enum { IDLE = 0, RECEIVING = 1, PROCESSING = 2, RESPONDING = 3 } state = IDLE;
    static int sck_d = 1;
    static int bit_count = 0;
    static int shift_reg = 0;
    static int misoff = 1;
    static int response = 0;
    static int out_x_l_flagff = 0;
    switch (state) {
        case IDLE:
            bit_count = 0;
            shift_reg = 0;
            misoff = 1;
            response = 0;
            out_x_l_flagff = 0;
            if (!csn && !sck) {
                state = RECEIVING;
            }
            break;
        case RECEIVING:
            if (sck && !sck_d) {
                shift_reg = (shift_reg << 1) | mosi;
                bit_count++;
                if (bit_count == 8) {
                    state = PROCESSING;
                }
            }
            break;
        case PROCESSING:
            if ((shift_reg & 0x3f) == 0xf) {
                response = 0x33;
            } else if ((shift_reg & 0x3f) == 0x28) {
                response = (out_x_resp & 0xff);
                out_x_l_flagff = 1;
            }
            state = RESPONDING;
            bit_count = 0;
            break;
        case RESPONDING:
            if (csn) {
                state = IDLE;
            } else if (!sck && sck_d) {
                misoff = !!(response & 0x80);
                response <<= 1;
                bit_count++;
                if (bit_count == 8) {
                    if (out_x_l_flagff) {
                        response = (out_x_resp >> 8) & 0xff;
                    } else {
                        response = 0;
                    }
                }
            }
            break;
    }
    sck_d = sck;
    if (out_x_l_flag) { *out_x_l_flag = out_x_l_flagff; }
    if (miso) { *miso = !csn && (state == RESPONDING) ? misoff : 1; }
    return 0;
}
