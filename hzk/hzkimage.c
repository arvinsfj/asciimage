
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tga-internal.h"
#include "tga.h"

#include "gb2312_to_unicode.h"

struct tga_color white = {{{255, 255, 255}}, RGB};
struct tga_color black = {{{0, 0, 0}}, RGB};
struct tga_color red   = {{{0, 0, 255}}, RGB};
struct tga_color blue  = {{{255, 0, 0}}, RGB};
struct tga_color green = {{{0, 255, 0}}, RGB};

unsigned short unicode2gb2312(unsigned short int unicode)
{
    for(int i=0; i<21243; i++){
        if (GB2312_To_Unicode[i][1]==unicode){
            return GB2312_To_Unicode[i][0];
        }
    }
    return 0xFFFF; 
}

unsigned short utf82gb2312(unsigned char* pText, int len)
{
    int n = 3;
    if (pText[3] != 0) {
        n = 4;
    }
    unsigned char unicodec[2];
    if (n == 3) {
        unicodec[0] = (pText[0]  << (n+1)) + ((pText[1] & 0x3F) >> (5-n));
        unicodec[1] = ((pText[1] & 0x3F) << (n+3)) + (pText[2] & 0x3F);
    }
    if (n == 4) {
        unicodec[0] = (pText[0]  << (n+1)) + ((pText[1] & 0x3F) >> (5-n));
        unicodec[1] = ((pText[1] & 0x3F) << (n+3)) + ((pText[2] & 0x3F) << 1) + ((pText[3] & 0x3F) >> 5);
    }
    unsigned short unicode = unicodec[0];
    unicode = ((unicode << 8)&0xFF00)+unicodec[1];
    unsigned short gb2312 = unicode2gb2312(unicode);
    
    return gb2312;
}

int main(int argc, char** argv)
{
    struct tga_image tga;
    tga_create(1024, 768, RGB, &tga);
    
    int i=0,j=0,k=0;
    unsigned char mat[16][2];
    unsigned char pText[240][5]={
        {"房"},{"字"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"},
        {"好"},{"好"}, {"学"}, {"习"}, {"，"}, {"天"}, {"天"}, {"向"}, {"上"}, {"！"}
    };
    
    unsigned short gbs[240];
    for (int index = 0; index < 240; index++) {
        unsigned short gb = utf82gb2312(pText[index], 5);
        gbs[index] = gb;
    }
    
    FILE *HZK;
    if((HZK=fopen("HZK16F","rb"))==NULL){
        exit(0);
    }
    for (int index = 0; index < 240; index++) {
        unsigned char hzk[2] = {((gbs[index]>>8)&0x00FF), (gbs[index]&0x00FF)};
        i=hzk[1]-0xa0;j=hzk[0]-0xa0;/*获得区码与位码*/
        fseek(HZK,((i-1)*94+(j-1))*32l,SEEK_SET);
        fread(mat,32,1,HZK);
        
        for(j=0;j<16;j++){
            for(i=0;i<2;i++){
                for(k=0;k<8;k++){
                    if(mat[j][i]&(0x80>>k)){/*测试为1的位则显示*/
                        tga_set(16+(i*8+k)+16*(index%62), 16+j+1.5*16*(index/62), &white, &tga);
                    }
                }
            }
        }
    }
    fclose(HZK);
    
    //tga_flip_v(&tga);
    tga_write("text.tga", 1, &tga);
    
    return 0;
}
