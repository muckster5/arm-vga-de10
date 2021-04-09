#include "address_map_arm.h"
#include "vga.h"
#include "jtag.h"
#include "interrupts.h"
#include "hex.h"

#define BORDER_THICKNESS 1
#define EDIT_BORDER_THICKNESS 2

// global variable to track if user is editing a segment
// needs to be global and volatile because interrupt modifies 
// forward declared in interrupts.h
static volatile int is_editing = 0;

static colour_t current_colour;
static struct _carworld* data;
static struct _carworld* last_saved;
int first_run = 1;

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
            data = get_carworld_data();
            jtag_start_request();
            if(is_editing == 1) {
                first_run = 0;
                last_saved = data;
                vga_set_border_thickness(EDIT_BORDER_THICKNESS);
                carworld_to_colour(data, &current_colour);
                vga_set_current_segment(current_colour);
            }
            else {
                vga_set_border_thickness(BORDER_THICKNESS);
            }
            vga_draw_current_segment();

        }
        else {
            jtag_read_fifo();
        }
        if(first_run) {
            int i;
            for(i = 0; i < 6; i++) {
                hex_display(i, 0);
            }
        }
        else {
            current_colour = vga_get_current_segment();
            int red = map(current_colour.red, 0, VGA_MAX_RED, 0, 0xFF);
            int green = map(current_colour.green, 0, VGA_MAX_GREEN, 0, 0xFF);
            if(green == 0xFE) green = 0xFF; // small rounding due to conversion issues (better to have max/min values report correctly)
            int blue = map(current_colour.blue, 0, VGA_MAX_BLUE, 0, 0xFF);
            hex_display(0, blue & 0xF);
            hex_display(1, (blue >> 4) & 0xF);
            hex_display(2, green & 0xF);
            hex_display(3, (green >> 4) & 0xF);
            hex_display(4, red & 0xF);
            hex_display(5, (red >> 4) & 0xF);
        }
    }
}