#include "address_map_arm.h"
#include "vga.h"

int main(void) {
    vga_init();
    vga_set_screen(BLACK);
    vga_show_screen();
    int i;
    for(i = 0; i < VGA_WIDTH_BINS*VGA_HEIGHT_BINS; i++) {
        vga_set_segment(i, RED);
    }
    return 0;
}