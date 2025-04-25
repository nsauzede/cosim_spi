/*
✅ What this device does:
Registers a device at physical address 0x40000000
Responds to MMIO reads/writes of 32-bit values
Logs accesses to stdout for debug purposes

🧪 How to integrate:
Place this file in hw/misc/dummy_device.c

Register it in hw/misc/meson.build:

meson
Copier
Modifier
softmmu_ss.add(files('dummy_device.c'))
Rebuild QEMU

🧰 How to instantiate your device:
You can add the device in your QEMU command line like this:

bash
Copier
Modifier
qemu-system-riscv32 -M virt -nographic \
  -device dummy-dev \
  -kernel your_program.elf
By default, QEMU will map it automatically in the MMIO range (0x10000000+), but to force mapping at 0x40000000, you'd need to either:

Modify the device to sysbus_mmio_map() it explicitly

Or create a machine that sets that address

🧩 Dummy device code (dummy_device.c)
*/

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "hw/irq.h"
#include "qapi/error.h"
#include "hw/registerfields.h"
#include "hw/mem/ram.h"
#include "hw/loader.h"
#include "hw/qdev-properties.h"

#define TYPE_DUMMY_DEVICE "dummy-dev"
#define DUMMY_DEVICE(obj) \
    OBJECT_CHECK(DummyDeviceState, (obj), TYPE_DUMMY_DEVICE)

typedef struct {
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    uint32_t reg;
} DummyDeviceState;

static uint64_t dummy_read(void *opaque, hwaddr addr, unsigned size)
{
    DummyDeviceState *s = opaque;
    printf("DummyDevice: read 0x%x at addr 0x%" HWADDR_PRIx "\n", s->reg, addr);
    return s->reg;
}

static void dummy_write(void *opaque, hwaddr addr, uint64_t data, unsigned size)
{
    DummyDeviceState *s = opaque;
    s->reg = (uint32_t)data;
    printf("DummyDevice: write 0x%x to addr 0x%" HWADDR_PRIx "\n", s->reg, addr);
}

static const MemoryRegionOps dummy_ops = {
    .read = dummy_read,
    .write = dummy_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid.min_access_size = 4,
    .valid.max_access_size = 4,
};

static void dummy_init(Object *obj)
{
    DummyDeviceState *s = DUMMY_DEVICE(obj);

    memory_region_init_io(&s->mmio, obj, &dummy_ops, s, TYPE_DUMMY_DEVICE, 0x4);
    sysbus_init_mmio(SYS_BUS_DEVICE(s), &s->mmio);
}

static void dummy_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->desc = "Dummy 32-bit MMIO Device";
}

static const TypeInfo dummy_info = {
    .name = TYPE_DUMMY_DEVICE,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DummyDeviceState),
    .instance_init = dummy_init,
    .class_init = dummy_class_init,
};

static void dummy_register_types(void)
{
    type_register_static(&dummy_info);
}

type_init(dummy_register_types)
