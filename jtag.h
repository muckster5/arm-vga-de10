#pragma once

#define JTAG_BASE 0xFF211020
#define JTAG_REQUEST_SLIDERS 0x3
#define JTAG_READY_MASK 0x8000
#define JTAG_DATA_MASK 0xFFF
#define CARWORLD_MAX_SLIDER 255
#define CARWORLD_SLIDER_OFFSET 128

void jtag_start_request();
void jtag_fifo_flush();
void jtag_read_fifo();

typedef struct _jtag {
    int data;
    int control;
} jtag_t;

struct _carworld {
    signed char x;
    signed char y;
    signed char z;
    int buffer_index;
    int data_ready;
} carworld;

volatile jtag_t* const jtag = (jtag_t*)JTAG_BASE;

void jtag_start_request() {
    carworld.buffer_index = 0;
    carworld.data_ready = 0;

    jtag_fifo_flush();

    // request sliders
    jtag->data = JTAG_REQUEST_SLIDERS;
}

void jtag_fifo_flush() {
    int data;
    do {
        data = jtag->data;
    } while(data & JTAG_READY_MASK);
}

void jtag_read_fifo() {
    int data = jtag->data;
    if(data & JTAG_READY_MASK) {
        if(carworld.buffer_index == 0) {
            carworld.buffer_index++;
        }
        else if(carworld.buffer_index == 1) {
            carworld.x = data & JTAG_DATA_MASK;
            carworld.buffer_index++;
        }
        else if(carworld.buffer_index == 2) {
            carworld.y = data & JTAG_DATA_MASK;
            carworld.buffer_index++;
        }
        else if(carworld.buffer_index == 3) {
            carworld.z = data & JTAG_DATA_MASK;
            carworld.buffer_index = 0;
            carworld.data_ready = 1;
        }
    }
}

struct _carworld* get_carworld_data() {
    carworld.data_ready = 0;
    return &carworld;
}

void carworld_to_colour(struct _carworld* car, colour_t* colour) {
    colour->red = map((car->x+CARWORLD_SLIDER_OFFSET), 0, CARWORLD_MAX_SLIDER, 0, VGA_MAX_RED);
    colour->green = map((car->y+CARWORLD_SLIDER_OFFSET), 0, CARWORLD_MAX_SLIDER, 0, VGA_MAX_GREEN);
    colour->blue = map((car->z+CARWORLD_SLIDER_OFFSET), 0, CARWORLD_MAX_SLIDER, 0, VGA_MAX_BLUE);
}