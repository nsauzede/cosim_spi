#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "qapi/error.h"
#include "hw/registerfields.h"
//#include "hw/mem/ram.h"
#include "hw/loader.h"
#include "hw/qdev-properties.h"

#define TYPE_DARKRISCV_DEVICE "darkriscv-mmio"
#define DARKRISCV_DEVICE(obj) \
    OBJECT_CHECK(DarkriscvDeviceState, (obj), TYPE_DARKRISCV_DEVICE)

typedef struct {
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    uint32_t reg;
} DarkriscvDeviceState;

struct DARKIO {
    unsigned char board_id; // 00
    unsigned char board_cm; // 01
    unsigned char core_id;  // 02
    unsigned char irq;      // 03
    struct DARKUART {
        unsigned char  stat; // 04
        unsigned char  fifo; // 05
        unsigned short baud; // 06/07
    } uart;
    unsigned int led;        // 08
    unsigned int timer;      // 0c
    unsigned int timeus;     // 10
    unsigned int iport;      // 14
    unsigned int oport;      // 18
    struct DARKSPI {
        union {
            unsigned char  spi8;  // 1c                              r: {data}
            unsigned short spi16; // 1c/1d       w: {cmd,data}       r: {dlo,dhi}
            unsigned int   spi32; // 1c/1d/1e/1f w: {00,cmd,dlo,dhi} r: {status,00,dlo,dhi}
        };
    } spi;
};
volatile struct DARKIO io =
{
    0, 100, 0, 0,   // ctrl = { board id, fMHz, fkHz }
    //4, 100, 0, 0,   // ctrl = { board id, fMHz, fkHz }
    { 0, 0, 0 },    // uart = { stat, fifo, baud }
    0,              // led
    1000000,        // timer
    0,              // timerus
    0,              // iport
    0               // oport
};
static void leds_write(uint32_t data) {
    printf("%s: data=0x%08"PRIx32"\n", __func__, data);
}
static void spi_master_write16(uint16_t data) {
    printf("%s: data=0x%04"PRIx16"\n", __func__, data);
}
static void spi_master_write32(uint32_t data) {
    printf("%s: data=0x%08"PRIx32"\n", __func__, data);
}
static uint8_t spi_master_read8_1f(void) {
    printf("%s:\n", __func__);
    return 2;
}
static uint8_t spi_master_read8_1c(void) {
    printf("%s:\n", __func__);
    return 0x33+1;
}
static uint64_t darkriscv_read(void *opaque, hwaddr addr, unsigned size)
{//    DarkriscvDeviceState *s = opaque;
    uint64_t res = 0;
    unsigned char *bytes = (unsigned char *)&io;
    //printf("DarkriscvDevice: read %u bytes at addr 0x%" HWADDR_PRIx "\n", size, addr);
    switch (addr) {
        case 0: // ctrl = { board id, fMHz, fkHz }
            switch (size) {
                case 1:
                    return bytes[addr];
                    break;
            }
            break;
        case 0x1c:
            switch (size) {
                case 1:
                    return spi_master_read8_1c();
                    break;
            }
            break;
        case 0x1f:
            switch (size) {
                case 1:
                    return spi_master_read8_1f();
                    break;
            }
            break;
    }
    printf("DarkriscvDevice: read %u bytes at addr 0x%" HWADDR_PRIx "\n", size, addr);
    exit(1);
    return res;
}
static void darkriscv_write(void *opaque, hwaddr addr, uint64_t data, unsigned size)
{//DarkriscvDeviceState *s = opaque;
    //printf("DarkriscvDevice: write %u bytes 0x%lx to addr 0x%" HWADDR_PRIx "\n", size, data, addr);
    //unsigned char *bytes = (unsigned char *)&io;
    switch (addr) {
        case 0x8: // leds
            switch (size) {
                case 1:
                case 4:
                    leds_write(data);
                    return;
            }
            break;
        case 0x1c: // spi_master
            switch (size) {
                case 2:
                    spi_master_write16(data);
                    return;
                case 4:
                    spi_master_write32(data);
                    return;
            }
            break;
    }
    printf("DarkriscvDevice: write %u bytes 0x%lx to addr 0x%" HWADDR_PRIx "\n", size, data, addr);
    exit(1);
}

static const MemoryRegionOps darkriscv_ops = {
    .read = darkriscv_read,
    .write = darkriscv_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid.min_access_size = 1,
    .valid.max_access_size = 4,
};

static void darkriscv_init(Object *obj)
{
    DarkriscvDeviceState *s = DARKRISCV_DEVICE(obj);

    s->reg = 0;
    printf("%s: reg=%x\n", __func__, s->reg);
    //exit(1);
    memory_region_init_io(&s->mmio, obj, &darkriscv_ops, s, TYPE_DARKRISCV_DEVICE, 0x1000);     // last arg 0x1000 is the mapping size
    printf("%s: CALLING sysbus_init_mmio                (FIRST)\n", __func__);
    sysbus_init_mmio(SYS_BUS_DEVICE(s), &s->mmio);
    //exit(1);
}

static void darkriscv_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->desc = "Darkriscv 32-bit MMIO Device";
}

static const TypeInfo darkriscv_info = {
    .name = TYPE_DARKRISCV_DEVICE,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DarkriscvDeviceState),
    .instance_init = darkriscv_init,
    .class_init = darkriscv_class_init,
};

static void darkriscv_register_types(void)
{
    printf("%s: CALLING type_register_static\n", __func__);
    type_register_static(&darkriscv_info);
}

type_init(darkriscv_register_types)
