
#include "tga-internal.h"
#include "tga.h"

struct tga_color white = {{{255, 255, 255}}, RGB};
struct tga_color black = {{{0, 0, 0}}, RGB};
struct tga_color red   = {{{0, 0, 255}}, RGB};
struct tga_color blue  = {{{255, 0, 0}}, RGB};
struct tga_color green = {{{0, 255, 0}}, RGB};

#include "asciifont-internal.h"
#include "asciifont.h"

void draw_char(unsigned char ch, int idx, struct font_rep* rep, struct tga_image* tga)
{
    //check
    if (ch < 32 || ch > 127) {
        return;
    }
    //almost correct
    unsigned char* ftrep = rep->ftrep; int ftw = rep->ftw; int fth = rep->fth;
    int x = ftw+ftw*(idx%(tga_width(tga)/ftw-2));
    int y = ftw+1.2*fth*(idx/(tga_width(tga)/ftw-2));
    ftrep += (ch-32)*(ftw/8)*fth;
    int x0 = x;
    for (int i = 0; i < fth; i++, y++, ftrep+=ftw/8) {
        x = x0;
        for (int j = 0; j < 8; j++, x++) {
            for (int b = 0; b < ftw/8; b++) {
                tga_set(x+8*b, y, ((*(ftrep+b) >> (7-j)) & 1) ? &white : &black, tga);
            }
        }
    }
}

void draw_txt(char* str, int len, int type, char* filename)
{
    struct tga_image tga;
    tga_create(1024, 768, RGB, &tga);
    struct font_rep rep = ft_rp[type];
    for (int i = 0; i < len; i++) {
        draw_char((unsigned char)str[i], i, &rep, &tga);
    }
    //tga_flip_v(&tga);
    tga_write(filename, 1, &tga);
}
