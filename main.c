#include "address_map_arm.h"
#include "vga.h"
#include "jtag.h"
#include "interrupts.h"

int main(void) {
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
    jtag_start_request();
    while(1) {
        if(carworld.data_ready == 1) {
            colour_t col;
            struct _carworld* data = get_carworld_data();
            carworld_to_colour(data, &col);
            jtag_start_request();
            vga_set_current_segment(col);
            vga_draw_current_segment();
            // use the following functions to either increment to the next segment
            // or decrement to the previous segment
            // Note: they will handle overflows by just remaining at the first or last segment
            // vga_increment_selection()
            // vga_decrement_selection()
        }
        else {
            jtag_read_fifo();
        }
    }
}