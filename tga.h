
#include "tga-internal.h"

#ifndef TGA_H
#define TGA_H

//interface function define
void tga_create(int w, int h, int bpp, struct tga_image* tga);
bool tga_read(const char *filename, struct tga_image* tga);
bool tga_write(const char *filename, bool rle, struct tga_image* tga);
bool tga_flip_h(struct tga_image* tga);
bool tga_flip_v(struct tga_image* tga);
bool tga_scale(int w, int h, struct tga_image* tga);
struct tga_color* tga_get(int x, int y, struct tga_image* tga);
bool tga_set(int x, int y, struct tga_color* c, struct tga_image* tga);
int tga_width(struct tga_image* tga);
int tga_height(struct tga_image* tga);
int tga_bpp();
unsigned char* tga_buffer(struct tga_image* tga);
void tga_clear(struct tga_image* tga);


#endif //TGA_H




