#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <stdio.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	printf("successful terminal initialization!\n");
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_shift() 
{
	for (size_t y = 0; y < (VGA_HEIGHT - 1); ++y) {
		for (size_t x = 0; x < VGA_WIDTH; ++x) {
			terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; ++x) {
		terminal_buffer[(VGA_HEIGHT - 1) * (VGA_WIDTH) + x] = vga_entry(' ', terminal_color);
	}
	terminal_column = 0;
	terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c)
{
	if (terminal_column >= VGA_WIDTH) {
		++terminal_row;
		terminal_column = 0;
	}
	if (c == '\n') {
		++terminal_row;
		terminal_column = 0;
	}
	if (terminal_row >= VGA_HEIGHT) {
		terminal_shift();
	}
	if (c != '\n') {
		terminal_putentryat(c, terminal_color, terminal_column++, terminal_row);
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

void backspace() {
	terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
	if ((terminal_row != 0) || (terminal_column != 0)) {
		if (terminal_column == 0) {
			--terminal_row;
			terminal_column = VGA_WIDTH - 1;
		} else {
			--terminal_column;
		}
	}
}
