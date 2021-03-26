#include "address_map_arm.h"
#include "vga.h"

int main(void) {
    vga_init();
    vga_set_screen(BLACK);
    vga_show_screen();
    return 0;
}