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
