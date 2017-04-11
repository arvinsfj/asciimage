
#include "tga-internal.h"
#include "tga.h"

struct tga_color white = {{{255, 255, 255}}, RGB};
struct tga_color black = {{{0, 0, 0}}, RGB};
struct tga_color red   = {{{0, 0, 255}}, RGB};
struct tga_color blue  = {{{255, 0, 0}}, RGB};
struct tga_color green = {{{0, 255, 0}}, RGB};

#include "asciifont-internal.h"
#include "asciifont.h"

void draw_char_8_8(int x, int y, unsigned char ch, struct tga_image* tga)
{
    if (ch < 128 && ch > 31){
        unsigned char *ch_rep = Ascii_W8_H8+(ch-32)*8;
        int x0 = x;
        for (int i = 0; i < 8; i++, y++, ch_rep++){
            x = x0;
            for (int j = 0; j < 8; j++, x++){
                tga_set(x, y, ((*ch_rep >> (7-j)) & 1) ? &white : &black, tga);
            }
        }
    }
}

void draw_char_8_16(int x, int y, unsigned char ch, struct tga_image* tga)
{
    if (ch < 128 && ch > 31){
        unsigned char *ch_rep = Ascii_W8_H16+(ch-32)*16;
        int x0 = x;
        for (int i = 0; i < 16; i++, y++, ch_rep++){
            x = x0;
            for (int j = 0; j < 8; j++, x++){
                tga_set(x, y, ((*ch_rep >> (7-j)) & 1) ? &white : &black, tga);
            }
        }
    }
}

void draw_char_16_24(int x, int y, unsigned char ch, struct tga_image* tga)
{
    if (ch < 128 && ch > 31){
        unsigned char *ch_rep = Ascii_W16_H24+(ch-32)*48;
        int x0 = x;
        for (int i = 0; i < 24; i++, y++, ch_rep+=2){
            x = x0;
            for (int j = 0; j < 8; j++, x++){
                tga_set(x, y, ((*ch_rep >> (7-j)) & 1) ? &white : &black, tga);
                tga_set(x+8, y, ((*(ch_rep+1) >> (7-j)) & 1) ? &white : &black, tga);
            }
        }
    }
}

void draw_txt(char* str, int len, int type, char* filename)
{
    struct tga_image tga;
    tga_create(1024, 768, RGB, &tga);
    if (type == ASCII_W8_H8) {
        for (int i = 0; i < len; i++) {
            draw_char_8_8(8*(i%126)+8, 10*(i/126)+8, (unsigned char)str[i], &tga);
        }
    }
    if (type == ASCII_W8_H16) {
        for (int i = 0; i < len; i++) {
            draw_char_8_16(8*(i%126)+8, 20*(i/126)+8, (unsigned char)str[i], &tga);
        }
    }
    if (type == ASCII_W16_H24) {
        for (int i = 0; i < len; i++) {
            draw_char_16_24(16*(i%62)+16, 30*(i/62)+16, (unsigned char)str[i], &tga);
        }
    }
    //tga_flip_v(&tga);
    tga_write(filename, 1, &tga);
}
