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

#include "lis3dh_stub.c"

#include <vpi_user.h>
#include <string.h>

#define UNUSED(v) v = v
//#define SHOW_SIGNALS

static int lis3dh_stub_compiletf(char *user_data) {
    UNUSED(user_data);
    return 0;
}
static int lis3dh_stub_calltf(char *user_data) {
    UNUSED(user_data);
    int out_x_resp = 0, csn = 0, sck = 0;
    int out_x_l_flag = 0, mosi = 0, miso = 0;
    int state = 0, bit_count = 0, shift_reg = 0, rd = 0, oe = 0, spi3w = 0;
    // we use 2 ffs because $lis3dh_stub is called on both clk edges
    static int rdff = 0, rdff2 = 0;
    static int oeff = 0, oeff2 = 0;
    static int spi3wff = 0, spi3wff2 = 0;
    static int misoff = 0, misoff2 = 0;
//    int mosi_pre = 0;
    vpiHandle systfref, args_iter, arg_h;
    s_vpi_value arg_val;
    // Get argument handles
    systfref = vpi_handle(vpiSysTfCall, NULL);
    args_iter = vpi_iterate(vpiArgument, systfref);
    // Process each argument
    while ((arg_h = vpi_scan(args_iter)) != NULL) {
        const char *name = vpi_get_str(vpiName, arg_h);
        if (!strcmp(name, "out_x_resp")) {
            arg_val.format = vpiIntVal;
            vpi_get_value(arg_h, &arg_val);
            out_x_resp = arg_val.value.integer;
#ifdef SHOW_SIGNALS
            vpi_printf("Signal %s value: %d\n", name, out_x_resp);
#endif
            continue;
        }
        arg_val.format = vpiScalarVal;
        vpi_get_value(arg_h, &arg_val);
#ifdef SHOW_SIGNALS
        const char *val_str;
        switch(arg_val.value.scalar) {
            case vpi0: val_str = "0"; break;
            case vpi1: val_str = "1"; break;
            case vpiZ: val_str = "Z"; break;
            case vpiX: val_str = "X"; break;
            default:   val_str = "?"; break;
        }
        vpi_printf("Signal %s value: %s\n", name, val_str);
#endif
        if (!strcmp(name, "sck")) { sck = arg_val.value.scalar; }
        else if (!strcmp(name, "csn")) { csn = arg_val.value.scalar; }
        else if (!strcmp(name, "mosi")) { mosi = arg_val.value.scalar; }
    }
    int mosi_ = mosi;
    lis3dh_stub(out_x_resp, &out_x_l_flag, csn, sck, &mosi, &miso, &state, &bit_count, &shift_reg, &rd, &oe, &spi3w);
    // Get argument handles
    systfref = vpi_handle(vpiSysTfCall, NULL);
    args_iter = vpi_iterate(vpiArgument, systfref);
    // Process each argument
    while ((arg_h = vpi_scan(args_iter)) != NULL) {
        const char *name = vpi_get_str(vpiName, arg_h);
        if (!strcmp(name, "out_x_l_flag")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = out_x_l_flag;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "misoff")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = misoff2;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "mosi")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = rdff2 && oeff2 && spi3wff2 ? mosi : mosi_;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "state")) {
            arg_val.format = vpiIntVal;
            arg_val.value.integer = state;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "bit_count")) {
            arg_val.format = vpiIntVal;
            arg_val.value.integer = bit_count;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "shift_reg")) {
            arg_val.format = vpiIntVal;
            arg_val.value.integer = shift_reg;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "rd")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = rdff2;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "oe")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = oeff2;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "spi3w")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = spi3wff2;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        }
    }
    rdff2 = rdff; rdff = rd;
    oeff2 = oeff; oeff = oe;
    spi3wff2 = spi3wff; spi3wff = spi3w;
    misoff2 = misoff; misoff = miso;
//    vpi_printf("%s out_x_resp=%d sck=%d csn=%d mosi=%d miso=%d\n", __func__, out_x_resp, sck, csn, mosi, miso);
    return 0;
}

static void lis3dh_stub_register(void) {
//    vpi_printf("%s\n", __func__);
    s_vpi_systf_data tf_data;
    memset(&tf_data, 0, sizeof(tf_data));

    tf_data.type      = vpiSysTask;
    tf_data.sysfunctype = vpiSysTask;
    tf_data.tfname    = "$lis3dh_stub";
    tf_data.calltf    = lis3dh_stub_calltf;
    tf_data.compiletf = lis3dh_stub_compiletf;
    tf_data.sizetf    = 0;
    tf_data.user_data = 0;
    vpi_register_systf(&tf_data);
}

void (*vlog_startup_routines[])(void) = {
   lis3dh_stub_register,
   0
};
