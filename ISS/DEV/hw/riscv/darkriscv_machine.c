#include "hw/misc/darkriscv_soc.h"
#include "qemu/osdep.h"
#include "qemu/units.h"
#include "hw/boards.h"
#include "target/riscv/cpu.h"
#include "hw/loader.h"
#include "qapi/error.h"
#define DARKIO_BASE 0x40000000
static void darkriscv_board_init(MachineState *machine)
{
    // Your machine init logic (RAM + MMIO + CPU hookup)
    MemoryRegion *sysmem = get_system_memory();
    MemoryRegion *ram = g_new(MemoryRegion, 1);
    // Allocate RAM
    memory_region_init_ram(ram, NULL, "darkriscv.ram", machine->ram_size, &error_abort);
    memory_region_add_subregion(sysmem, 0x00000000, ram);  // Map at address 0x0
    // Create a single CPU
    cpu_create(machine->cpu_type);
    // Instantiate darkio device
#if 0
    DeviceState *darkio = qdev_create(NULL, TYPE_DARKIO_DEVICE);
    qdev_init_nofail(darkio);
    SysBusDevice *sbd = SYS_BUS_DEVICE(darkio);
    sysbus_mmio_map(sbd, 0, DARKIO_BASE);
#else
    //dev = 
    sysbus_create_simple(TYPE_DARKIO_DEVICE, DARKIO_BASE, NULL);
#endif
}
static void darkriscv_board_machine_class_init(MachineClass *mc)
{
    mc->desc = "RISC-V Board compatible with Darkriscv";
    mc->init = darkriscv_board_init;
//    mc->default_ram_size = 32 * KiB;
    mc->default_ram_size = 64 * KiB;
    //mc->default_cpu_type = RISCV_CPU_TYPE_NAME("rv32e");
    mc->default_cpu_type = TYPE_RISCV_CPU_BASE;
}
DEFINE_MACHINE("darkriscv", darkriscv_board_machine_class_init)
