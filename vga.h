#pragma once

#include "address_map_arm.h"

#define VGA_HEIGHT 240
#define VGA_WIDTH 320
#define VGA_WIDTH_BINS 4
#define VGA_HEIGHT_BINS 2

#define VGA_MAX_RED   31
#define VGA_MAX_GREEN 63
#define VGA_MAX_BLUE  31

typedef struct {
    int red;
    int green;
    int blue;
} colour_t;

volatile static short* const vga_buffer = (short*)FPGA_PIXEL_BUF_BASE;

static colour_t const BLACK = { 0, 0, 0 };
static colour_t const RED = { 31, 0, 0 };
static colour_t const GREEN = { 0, 63, 0 };
static colour_t const BLUE = { 0, 0, 31 };
static colour_t const PURPLE = { 31, 0, 31 };
static colour_t const TURQUOISE = { 0, 63, 31 };
static colour_t const WHITE = { 31, 63, 31 };
static colour_t const YELLOW = { 31, 63, 0 };
static colour_t const ORANGE = { 31, 32, 0 };
static colour_t const GREY = { 10, 20, 10 };

static colour_t colour_buffer[VGA_WIDTH_BINS*VGA_HEIGHT_BINS];
static int vga_segment_selection = 0, vga_previous_segment_selection = 0;
static int border_thickness = 5;

void vga_set_screen(colour_t colour);
void vga_set_current_segment(colour_t colour);
void vga_draw_current_segment();
void vga_draw_segment(int segment);
colour_t get_inverted_colour(colour_t colour);
void vga_set_border_thickness(int thickness);

void vga_init() {
    vga_set_screen(BLACK);
}

int map(int value, int from_low, int from_high, int to_low, int to_high) {
    float slope = (float)(to_high - to_low)/(float)(from_high - from_low);
    return to_low + (int)(slope*(value - from_low));
}

short vga_pixel_to_short(colour_t colour) {
    short result = 0;
    result |= map(colour.blue, 0, 255, 0, 31) & 0x1F;
    result |= (map(colour.green, 0, 255, 0, 63) & 0x3F) << 5;
    result |= (map(colour.red, 0, 255, 0, 31) & 0x1F) << 11;
    return result;
}


void vga_set_pixel(int x, int y, colour_t colour) {
    volatile short *address=(volatile short*)(FPGA_PIXEL_BUF_BASE + (y << 10) + (x << 1));
    *address = (colour.blue & 0x1F) | (colour.green & 0x3F) << 5 | (colour.red & 0x1F) << 11;
}

void vga_set_screen(colour_t colour) {
    int i;
    for(i = 0; i < VGA_WIDTH_BINS*VGA_HEIGHT_BINS; i++) {
        colour_buffer[i] = colour;
    }
}

void vga_show_screen() {
    int segment;
    for(segment = 0; segment < VGA_WIDTH_BINS*VGA_HEIGHT_BINS; segment++) {
        vga_draw_segment(segment);
    }
}

colour_t get_inverted_colour(colour_t colour) {
    colour_t inverted = { VGA_MAX_RED - colour.red, VGA_MAX_GREEN - colour.green, VGA_MAX_BLUE - colour.blue };
    return inverted;
}

void vga_draw_segment(int segment) {
    const int bin_width_size = VGA_WIDTH/VGA_WIDTH_BINS;
    const int bin_height_size = VGA_HEIGHT/VGA_HEIGHT_BINS;
    int x, y;
    int x_start = (segment%VGA_WIDTH_BINS)*bin_width_size, x_end = x_start + bin_width_size;
    int y_start = (segment/VGA_WIDTH_BINS)*bin_height_size, y_end = y_start + bin_height_size;
    for(x = x_start; x < x_end; x++) {
        for(y = y_start; y < y_end; y++) {
            if(segment == vga_segment_selection && (x - x_start <= (border_thickness-1) || (x_end-1)-x <= (border_thickness-1) || y - y_start <= (border_thickness-1) || (y_end-1)-y <= (border_thickness-1))) {
                vga_set_pixel(x,y,get_inverted_colour(colour_buffer[segment]));
            }
            else {
                vga_set_pixel(x,y,colour_buffer[segment]);
            }
        }
    }
}

void vga_increment_selection() {
    vga_previous_segment_selection = vga_segment_selection;
    if(vga_segment_selection != VGA_WIDTH_BINS*VGA_HEIGHT_BINS - 1) vga_segment_selection++;
    vga_draw_segment(vga_previous_segment_selection);
    vga_draw_segment(vga_segment_selection);
}

void vga_decrement_selection() {
    vga_previous_segment_selection = vga_segment_selection;
    if(vga_segment_selection != 0) vga_segment_selection--;
    vga_draw_segment(vga_previous_segment_selection);
    vga_draw_segment(vga_segment_selection);
}

void vga_set_segment(int segment, colour_t colour) {
    colour_buffer[segment] = colour;
}

void vga_set_current_segment(colour_t colour) {
    vga_set_segment(vga_segment_selection, colour);
}

colour_t vga_get_current_segment() {
    return colour_buffer[vga_segment_selection];
}

void vga_draw_current_segment() {
    vga_draw_segment(vga_segment_selection);
}

void vga_set_border_thickness(int thickness) {
    border_thickness = thickness;
}