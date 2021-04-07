#include "address_map_arm.h"
#include "vga.h"
#include "interrupts.h"

int main(void) {
    //disable_A9_interrupts();
    set_A9_IRQ_stack();
    config_GIC();
    config_KEYs();
    enable_A9_interrupts();
    vga_init();
    vga_set_screen(BLACK);
    vga_show_screen();
    int i;
    for(i = 0; i < VGA_WIDTH_BINS*VGA_HEIGHT_BINS; i++) {
        vga_set_segment(i, RED);
    }
    

    while(1){
        ;
    }

    return 0;
}