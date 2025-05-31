#include <stdio.h>
#include <stdint.h>
#include <locale.h>

typedef uint_fast8_t ubyte;
typedef uint_fast16_t uhex;

#include "border_chars.c"

void goxy(uhex x, uhex y) {
	printf("\x1B[%lu;%luf", y+1, x+1);
}
void clrscreen() {
	printf("\x1B[2J");
}
void printbox(uhex w, uhex h, uhex x, uhex y, const wchar_t charset[8], int solid) {
	printf("\x1B[%lu;%luf%lc", y+1, x+1, charset[4]);
	for (uhex c=0; c<(w-2); ++c) {
		printf("%lc", charset[0]);
	}
	printf("%lc\x1B[1B\x1B[%luD", charset[5], w);
	
	for (uhex r=0; r<(h-2); ++r) {
		printf("%lc", charset[3]);
		if (solid) {
			for (uhex c=0; c<(w-2); ++c) {
				printf(" ");
			}
		} else {
			printf("\x1B[%luC", w-2);
		}
		printf("%lc\x1B[1B\x1B[%luD", 
			charset[1], w);
	}

	printf("%lc", charset[7]);
	for (uhex c=0; c<(w-2); ++c) {
		printf("%lc", charset[2]);
	}
	printf("%lc", charset[6]);
}
void printbox_color(uhex w, uhex h, uhex x, uhex y, const wchar_t charset[8], ubyte fgclr, ubyte bgclr, int solid) {
	printf("\x1B[38;5;%dm\x1B[48;5;%dm", fgclr, bgclr);
	printbox(w, h, x, y, charset, solid);
	printf("\x1B[0m");
}

// very much WIP!!
void printgrid(
	uhex grid_w, 
	uhex grid_h, 
	uhex grid_nx, 
	uhex grid_ny,
	uhex x,
	uhex y,
	const wchar_t charset[13]) 
{
	if (grid_nx == 0 || grid_ny == 0) {
		return;
	}
	const uhex TOTAL_WIDTH = grid_nx*(grid_w-1)+1;

	// top row
	printf("\x1B[%lu;%luf%lc", y+1, x+1, charset[4]);
	for (uhex c=0; c<(grid_nx-1); ++c) {
		for (uhex i=0; i<(grid_w-2); ++i) {
			printf("%lc", charset[0]);
		}
		printf("%lc", charset[8]);
	}
	for (uhex i=0; i<(grid_w-2); ++i) {
		printf("%lc", charset[0]);
	}
	printf("%lc", charset[5]);

	// between rows
	for (uhex r=0; r<(grid_ny-1); ++r) {
		for (uhex i=0; i<(grid_h-2); ++i) {
			printf("\x1B[1B\x1B[%luD%lc", 
				TOTAL_WIDTH,
				charset[3]);
			for (uhex c=0; c<grid_nx; ++c) {
				printf("\x1B[%luC%lc", grid_w-2, charset[3]);
			}
		}
		printf("\x1B[1B\x1B[%luD%lc",
			TOTAL_WIDTH,
			charset[11]);
		for (uhex c=0; c<(grid_nx-1); ++c) {
			for (uhex i=0; i<(grid_w-2); ++i) {
				printf("%lc", charset[0]);
			}
			printf("%lc", charset[12]);
		}
		for (uhex i=0; i<(grid_w-2); ++i) {
			printf("%lc", charset[0]);
		}
		printf("%lc", charset[9]);
	}

	for (uhex i=0; i<(grid_h-2); ++i) {
		printf("\x1B[1B\x1B[%luD%lc",
			TOTAL_WIDTH,
			charset[3]);
		for (uhex c=0; c<grid_nx; ++c) {
			printf("\x1B[%luC%lc", grid_w-2, charset[3]);
		}
	}
	
	// bottom row
	printf("\x1B[1B\x1B[%luD%lc", 
		TOTAL_WIDTH, 
		charset[7]);
	for (uhex c=0; c<(grid_nx-1); ++c) {
		for (uhex i=0; i<(grid_w-2); ++i) {
			printf("%lc", charset[0]);
		}
		printf("%lc", charset[10]);
	}
	for (uhex i=0; i<(grid_w-2); ++i) {
		printf("%lc", charset[0]);
	}
	printf("%lc", charset[6]);
}

void print_in() {}

int main(void) {
	setlocale(LC_ALL, "");
	clrscreen();
	goxy(5,5);
	const uhex OFFSET[2] = { 2, 1 };
	// printbox(15, 5, 1, 1, W1T1_BORDERCHARS);
	// printbox(15, 5, 17, 1, W1T2_BORDERCHARS);
	// printbox(15, 5, 1, 7, W2T1_BORDERCHARS);
	// printbox(15, 5, 17, 7, W1T3_BORDERCHARS);
	printgrid(17,9, 3,3, OFFSET[0],OFFSET[1], W2T1_GRIDCHARS);
	for (int r=0; r<3; ++r) {
		for (int c=0; c<3; ++c) {
			printgrid(5,3, 3,3, 2+16*c+OFFSET[0],1+8*r+OFFSET[1], W1T1_GRIDCHARS);
		}
	}
	// int selected = 4;
	// int row = selected/3;
	// int col = selected%3;
	goxy(0,26+OFFSET[1]);

	return 0;
}
