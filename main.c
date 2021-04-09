#include "address_map_arm.h"
#include "vga.h"
#include "jtag.h"
#include "interrupts.h"

#define BORDER_THICKNESS 1
#define EDIT_BORDER_THICKNESS 1

// global variable to track if user is editing a segment
// needs to be global and volatile because interrupt modifies 
// forward declared in interrupts.h
static volatile int is_editing = 0;

int main(void) {
    set_A9_IRQ_stack();
    config_GIC();
    config_KEYs();
    enable_A9_interrupts();
    vga_init();
    vga_set_border_thickness(BORDER_THICKNESS);
    vga_set_screen(GREY);
    vga_show_screen();
    jtag_start_request();
    while(1) {

        if(carworld.data_ready == 1) {
            jtag_start_request();
            if(is_editing == 1) {
                colour_t col;
                struct _carworld* data = get_carworld_data();
                vga_set_border_thickness(EDIT_BORDER_THICKNESS);
                carworld_to_colour(data, &col);
                vga_set_current_segment(col);
            }
            else {
                vga_set_border_thickness(BORDER_THICKNESS);
            }
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