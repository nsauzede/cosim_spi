/*
🧪 If you're curious: A Proof-of-Concept Dynamic Module
If you want to try this on your own:
*/

// dummy_plugin.c
#include "qemu/osdep.h"
#include "qemu/plugin.h"
#include "hw/sysbus.h"

static void __attribute__((constructor)) my_plugin_init(void) {
    printf("Dummy plugin loaded!\n");
    // you could call type_register_static(&my_device_info) here
}

/*
Compile it:

bash
Copier
Modifier
gcc -fPIC -shared -o dummy_plugin.so dummy_plugin.c
Then you’d need to modify QEMU’s startup code to load this .so — e.g., via dlopen() — and hook it up.
*/
