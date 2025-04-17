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

static struct {unsigned high, low; } sim_time = {0, 0};
#include "spi_master.c"

#include <vpi_user.h>
#include <string.h>

#define UNUSED(v) v = v
//#define SHOW_SIGNALS

static int spi_master_stub_compiletf(char *user_data) {
    UNUSED(user_data);
    return 0;
}
static int spi_master_stub_calltf(char *user_data) {
    UNUSED(user_data);
    int div_coef = 0;
    int nrst = 0, mosi_data = 0, nbits = 0, request = 0, spi_miso = 0;
    int miso_data = 0, ready = 0, spi_csn = 0, spi_sck = 0, spi_mosi = 0;
    vpiHandle systfref, args_iter, arg_h;
    s_vpi_value arg_val;
    systfref = vpi_handle(vpiSysTfCall, NULL);
    args_iter = vpi_iterate(vpiArgument, systfref);
    while ((arg_h = vpi_scan(args_iter)) != NULL) {
        const char *name = vpi_get_str(vpiName, arg_h);
        if (!strcmp(name, "div_coef")) {
            arg_val.format = vpiIntVal;
            vpi_get_value(arg_h, &arg_val);
            div_coef = arg_val.value.integer;
        } else if (!strcmp(name, "nrst")) {
            arg_val.format = vpiScalarVal;
            vpi_get_value(arg_h, &arg_val);
            nrst = arg_val.value.scalar;
        } else if (!strcmp(name, "mosi_data")) {
            arg_val.format = vpiIntVal;
            vpi_get_value(arg_h, &arg_val);
            mosi_data = arg_val.value.integer;
        } else if (!strcmp(name, "nbits")) {
            arg_val.format = vpiIntVal;
            vpi_get_value(arg_h, &arg_val);
            nbits = arg_val.value.integer;
        } else if (!strcmp(name, "request")) {
            arg_val.format = vpiScalarVal;
            vpi_get_value(arg_h, &arg_val);
            request = arg_val.value.scalar;
        } else if (!strcmp(name, "spi_miso")) {
            arg_val.format = vpiScalarVal;
            vpi_get_value(arg_h, &arg_val);
            spi_miso = arg_val.value.scalar;
        } else if (!strcmp(name, "spi_mosi")) {
            arg_val.format = vpiScalarVal;
            vpi_get_value(arg_h, &arg_val);
            spi_mosi = arg_val.value.scalar;
        }
    }
  s_vpi_time time_s;
  time_s.type = vpiSimTime;
  vpi_get_time(NULL, &time_s);
  sim_time.high = time_s.high;
  sim_time.low = time_s.low;
//  vpi_printf("Current simulation time: %u%u\n", time_s.high, time_s.low);
//  vpi_printf("Current simulation time size: %u %u\n", sizeof(time_s.high), sizeof(time_s.low));
    spi_master_stub(div_coef, nrst, mosi_data, &miso_data, nbits, request, &ready, &spi_csn, &spi_sck, &spi_mosi, spi_miso);
    systfref = vpi_handle(vpiSysTfCall, NULL);
    args_iter = vpi_iterate(vpiArgument, systfref);
    while ((arg_h = vpi_scan(args_iter)) != NULL) {
        const char *name = vpi_get_str(vpiName, arg_h);
        if (!strcmp(name, "miso_data")) {
            arg_val.format = vpiIntVal;
            arg_val.value.integer = miso_data;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "ready")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = ready;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "spi_csn")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = spi_csn;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "spi_sck")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = spi_sck;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        } else if (!strcmp(name, "spi_mosi")) {
            arg_val.format = vpiScalarVal;
            arg_val.value.scalar = spi_mosi;
            vpi_put_value(arg_h, &arg_val, NULL, vpiNoDelay);
        }
    }
    return 0;
}

static void spi_master_stub_register(void) {
//    vpi_printf("%s\n", __func__);
    s_vpi_systf_data tf_data;
    memset(&tf_data, 0, sizeof(tf_data));

    tf_data.type      = vpiSysTask;
    tf_data.sysfunctype = vpiSysTask;
    tf_data.tfname    = "$spi_master_stub";
    tf_data.calltf    = spi_master_stub_calltf;
    tf_data.compiletf = spi_master_stub_compiletf;
    tf_data.sizetf    = 0;
    tf_data.user_data = 0;
    vpi_register_systf(&tf_data);
}

void (*vlog_startup_routines[])(void) = {
   spi_master_stub_register,
   0
};
