unsigned static int const sevenseglookup[] = { 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 
                                                0b01111111, 0b01100111, 0b01110111, 0b01111100, 0b01011000, 0b01011110, 0b01111001, 0b01110001 };

void hex_display(int display_num, int value);

// displays a hex digit accounting for an offset into the next register
void hex_display(int display_num, int value) {
    int address = display_num <= 3 ? HEX3_HEX0_BASE : HEX5_HEX4_BASE;
    int offset = 0;

    // make sure we're using the right offset with the given word 
    if(display_num <= 3) offset = display_num;
    else offset = display_num - 4;

    *((int*)address) &= ~(0xFF << offset*8); // clear the two bytes of interest
    *((int*)address) |= (sevenseglookup[value] << offset*8); // set the two bytes
}