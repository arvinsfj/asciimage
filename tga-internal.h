#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TGA_INTERNAL_H
#define TGA_INTERNAL_H

typedef char bool;

//data define
#pragma pack(push, 1)
struct tga_header
{
    unsigned char   info_len;//0-255
    unsigned char   color_table_type;//0 or 1
    unsigned char   data_type_code;//2 3 10 11
    
    unsigned short  color_table_addr;
    unsigned short  color_table_len;
    unsigned char   color_table_item_len;//16 or 24 or 32
    
    unsigned short  data_x_origin;
    unsigned short  data_y_origin;
    unsigned short  data_width;
    unsigned short  data_height;
    unsigned char   data_pixel_len;//16 or 24 or 32
    unsigned char   data_desc;
    //03:每像素对应的属性位的位数,0 or 1 or 8
    //4 :保留,必须为0
    //5 :屏幕起始位置标志,0 = 原点在左下角,1 = 原点在左上角,truevision图像必须为0
    //67:交叉数据存储标志,00 = 无交叉,01 = 两路奇/偶交叉,10 = 四路交叉,11 = 保留
    
    //可变字段
    //18:info   //0-255字符
    //color_table
    //data      //RGB颜色数据存放顺序为：BGR(A)
};
#pragma pack(pop)

struct tga_footer
{
    unsigned char   dev_area[4];
    unsigned char   ext_area[4];
    unsigned char   footer[18];
};

struct tga_image
{
    struct tga_header*  header;
    char*               info;
    unsigned char*      color_table;
    unsigned char*      data;
    struct tga_footer*  footer;
};

enum tga_fmt {
    GRAYSCALE = 8, RGB = 24, RGBA = 32
};

struct tga_color
{
    union {
        struct {
            unsigned char b, g, r, a;
        };
        unsigned char raw[4];
        unsigned int val;
    };
    int bpp;
};

//function define
bool load_rle_data(FILE* fd, struct tga_image* tga);
bool unload_rle_data(FILE* fd, struct tga_image* tga);


#endif
