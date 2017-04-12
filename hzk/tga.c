
#include "tga-internal.h"
#include "tga.h"

//internal function define
bool load_rle_data(FILE* pFile, struct tga_image* tga)
{
    int width   = tga->header->data_width;
    int height  = tga->header->data_height;
    int bpp = (tga->header->data_pixel_len >> 3);
    unsigned long pixelcount = width*height;
    unsigned long currentpixel = 0;
    unsigned long currentbyte  = 0;
    struct tga_color colorbuffer;
    do {
        unsigned char chunkheader = 0;
        int err = fread(&chunkheader, 1, 1, pFile);
        if (err == 0) {
            printf("an error occured while reading the data\n");
            return 0;
        }
        if (chunkheader<128) {
            chunkheader++;
            for (int i=0; i<chunkheader; i++) {
                int err = fread(colorbuffer.raw, 1, bpp, pFile);
                if (err == 0) {
                    printf("an error occured while reading the header\n");
                    return 0;
                }
                for (int t=0; t<bpp; t++)
                    tga->data[currentbyte++] = colorbuffer.raw[t];
                currentpixel++;
                if (currentpixel>pixelcount) {
                    printf("Too many pixels read\n");
                    return 0;
                }
            }
        } else {
            chunkheader -= 127;
            int err = fread(colorbuffer.raw, 1, bpp, pFile);
            if (err == 0) {
                printf("an error occured while reading the header\n");
                return 0;
            }
            for (int i=0; i<chunkheader; i++) {
                for (int t=0; t<bpp; t++)
                    tga->data[currentbyte++] = colorbuffer.raw[t];
                currentpixel++;
                if (currentpixel>pixelcount) {
                    printf("Too many pixels read\n");
                    return 0;
                }
            }
        }
    } while (currentpixel < pixelcount);
    return 1;
}

bool unload_rle_data(FILE* pFile, struct tga_image* tga)
{
    int width   = tga->header->data_width;
    int height  = tga->header->data_height;
    int bpp = (tga->header->data_pixel_len >> 3);
    const unsigned char max_chunk_length = 128;
    unsigned long npixels = width*height;
    unsigned long curpix = 0;
    while (curpix<npixels) {
        unsigned long chunkstart = curpix*bpp;
        unsigned long curbyte = curpix*bpp;
        unsigned char run_length = 1;
        bool raw = 1;
        while (curpix+run_length<npixels && run_length<max_chunk_length) {
            bool succ_eq = 1;
            for (int t=0; succ_eq && t<bpp; t++) {
                succ_eq = ((tga->data[curbyte+t])==(tga->data[curbyte+t+bpp]));
            }
            curbyte += bpp;
            if (1==run_length) {
                raw = !succ_eq;
            }
            if (raw && succ_eq) {
                run_length--;
                break;
            }
            if (!raw && !succ_eq) {
                break;
            }
            run_length++;
        }
        curpix += run_length;
        
        int ch_len = raw?run_length-1:run_length+127;
        int err = fwrite(&ch_len, 1, 1, pFile);
        if (err == 0) {
            fclose(pFile);
            printf("can't dump the tga file\n");
            return 0;
        }
        int err1 = fwrite((tga->data+chunkstart), 1, (raw?run_length*bpp:bpp), pFile);
        if (err1 == 0) {
            fclose(pFile);
            printf("can't dump the tga file\n");
            return 0;
        }
    }
    return 1;
}

//interface function define
void tga_create(int w, int h, int bpp, struct tga_image* tga)
{
    tga->header = (struct tga_header *)malloc(sizeof(struct tga_header));
    memset((void *)(tga->header), 0, sizeof(struct tga_header));
    tga->header->data_width = w;
    tga->header->data_height = h;
    tga->header->data_pixel_len = bpp;
    tga->header->data_desc = 0x20;
    
    tga->info = NULL;
    tga->color_table = NULL;
    
    unsigned long nbytes = w*h*bpp;
    tga->data = (unsigned char *)malloc(sizeof(unsigned char) * nbytes);
    memset((tga->data), 0, nbytes);
    
    tga->footer = (struct tga_footer *)malloc(sizeof(struct tga_footer));
    memset((void *)(tga->footer), 0, sizeof(struct tga_footer));
    strcpy((char *)(tga->footer->footer), "TRUEVISION-XFILE.");
}
bool tga_read(const char *filename, struct tga_image* tga)
{
    if(tga->data) {
        free (tga->data);
    }
    tga->data = NULL;
    FILE *pFile = fopen(filename,"rb");
    if(pFile == NULL){
        printf("can't open file %s\n", filename);
        return 0;
    }
    int err = fread(tga->header, 1, sizeof(struct tga_header), pFile);
    if (err == 0) {
        fclose(pFile);
        printf("an error occured while reading the header\n");
        return 0;
    }
    int width   = tga->header->data_width;
    int height  = tga->header->data_height;
    int bpp = (tga->header->data_pixel_len >> 3);
    int bpp_num = (tga->header->data_pixel_len);
    if (width<=0 || height<=0 || (bpp_num!=GRAYSCALE && bpp_num!=RGB && bpp_num!=RGBA)) {
        fclose(pFile);
        printf("bad bpp (or width/height) value\n");
        return 0;
    }
    
    unsigned long nbytes = bpp*width*height;
    tga->data = (unsigned char *)malloc(sizeof(unsigned char) * nbytes);
    if (2==tga->header->data_type_code||3==tga->header->data_type_code) {
        int err = fread(tga->data, 1, nbytes, pFile);
        if (err == 0) {
            fclose(pFile);
            printf("an error occured while reading the data\n");
            return 0;
        }
    } else if (10==tga->header->data_type_code||11==tga->header->data_type_code) {
        if (!load_rle_data(pFile, tga)) {
            fclose(pFile);
            printf("an error occured while reading the data\n");
            return 0;
        }
    } else {
        fclose(pFile);
        printf("unknown file format %d\n", tga->header->data_type_code);
        return 0;
    }
    if (!(tga->header->data_desc & 0x20)) {
        tga_flip_v(tga);
    }
    if (tga->header->data_desc & 0x10) {
        tga_flip_h(tga);
    }
    printf("%d x %d / %d\n", width, height, bpp*8);
    fclose(pFile);
    return 1;
}
bool tga_write(const char *filename, bool rle, struct tga_image* tga)
{
    FILE *pFile = fopen(filename, "wb");
    if(pFile == NULL){
        printf("can't open file %s\n", filename);
        return 0;
    }
    int width   = tga->header->data_width;
    int height  = tga->header->data_height;
    int bpp = (tga->header->data_pixel_len >> 3);
    tga->header->data_type_code = (bpp==GRAYSCALE?(rle?11:3):(rle?10:2));
    int err = fwrite(tga->header, 1, sizeof(struct tga_header), pFile);
    if (err == 0) {
        fclose(pFile);
        printf("can't dump the tga file\n");
        return 0;
    }
    if (!rle) {
        int err = fwrite(tga->data, 1, width*height*bpp, pFile);
        if (err == 0) {
            fclose(pFile);
            printf("can't unload raw data\n");
            return 0;
        }
    } else {
        if (!unload_rle_data(pFile, tga)) {
            fclose(pFile);
            printf("can't unload rle data\n");
            return 0;
        }
    }
    int err1 = fwrite(tga->footer, 1, sizeof(struct tga_footer), pFile);
    if (err1 == 0) {
        fclose(pFile);
        printf("can't dump the tga file\n");
        return 0;
    }
    fclose(pFile);
    return 1;
}
bool tga_flip_h(struct tga_image* tga)
{
    if (!(tga->data)) return 0;
    int half = tga->header->data_width >> 1;
    for (int i=0; i<half; i++) {
        for (int j=0; j<(tga->header->data_height); j++) {
            struct tga_color* c1 = tga_get(i, j, tga);
            struct tga_color* c2 = tga_get((tga->header->data_width)-1-i, j, tga);
            tga_set(i, j, c2, tga);
            tga_set((tga->header->data_width)-1-i, j, c1, tga);
        }
    }
    return 1;
}
bool tga_flip_v(struct tga_image* tga)
{
    if (!(tga->data)) return 0;
    unsigned long bytes_per_line = (tga->header->data_width)*(tga->header->data_pixel_len >> 3);
    unsigned char *line = (unsigned char *)malloc(sizeof(unsigned char) * bytes_per_line);
    int half = (tga->header->data_height) >> 1;
    for (int j=0; j<half; j++) {
        unsigned long l1 = j*bytes_per_line;
        unsigned long l2 = ((tga->header->data_height)-1-j)*bytes_per_line;
        memmove((void *)line,      (void *)(tga->data+l1), bytes_per_line);
        memmove((void *)(tga->data+l1), (void *)(tga->data+l2), bytes_per_line);
        memmove((void *)(tga->data+l2), (void *)line,      bytes_per_line);
    }
    free (line);
    return 1;
}
bool tga_scale(int w, int h, struct tga_image* tga)
{
    if (w<=0 || h<=0 || !(tga->data)) return 0;
    unsigned char *tdata  = (unsigned char *)malloc(sizeof(unsigned char) * w*h*(tga->header->data_pixel_len >> 3));
    int nscanline = 0;
    int oscanline = 0;
    int erry = 0;
    unsigned long nlinebytes = w*(tga->header->data_pixel_len >> 3);
    unsigned long olinebytes = (tga->header->data_width)*(tga->header->data_pixel_len >> 3);
    for (int j=0; j<(tga->header->data_height); j++) {
        int errx = (tga->header->data_width)-w;
        int nx   = -(tga->header->data_pixel_len >> 3);
        int ox   = -(tga->header->data_pixel_len >> 3);
        for (int i=0; i<(tga->header->data_width); i++) {
            ox += (tga->header->data_pixel_len >> 3);
            errx += w;
            while (errx>=(int)(tga->header->data_width)) {
                errx -= (tga->header->data_width);
                nx += (tga->header->data_pixel_len >> 3);
                memcpy(tdata+nscanline+nx, (tga->data)+oscanline+ox, (tga->header->data_pixel_len >> 3));
            }
        }
        erry += h;
        oscanline += olinebytes;
        while (erry>=(int)(tga->header->data_height)) {
            if (erry>=(int)(tga->header->data_height)<<1) // it means we jump over a scanline
                memcpy(tdata+nscanline+nlinebytes, tdata+nscanline, nlinebytes);
            erry -= (tga->header->data_height);
            nscanline += nlinebytes;
        }
    }
    free (tga->data);
    tga->data = tdata;
    tga->header->data_width = w;
    tga->header->data_height = h;
    return 1;
}
struct tga_color* tga_get(int x, int y, struct tga_image* tga)
{
    struct tga_color* def_color = (struct tga_color*)malloc(sizeof(struct tga_color));
    if (!(tga->data) || x<0 || y<0 || x>=(tga->header->data_width) || y>=(tga->header->data_height)) {
        return def_color;
    }
    def_color->bpp = (tga->header->data_pixel_len >> 3);
    unsigned char* p = (tga->data)+(x+y*(tga->header->data_width))*(tga->header->data_pixel_len >> 3);
    for (int i=0; i<(def_color->bpp); i++) {
        def_color->raw[i] = p[i];
    }
    return def_color;
}
bool tga_set(int x, int y, struct tga_color* c, struct tga_image* tga)
{
    if (!(tga->data) || x<0 || y<0 || x>=(tga->header->data_width) || y>=(tga->header->data_height)) {
        return 0;
    }
    memcpy(((tga->data)+(x+y*(tga->header->data_width))*(tga->header->data_pixel_len >> 3)), c->raw, (tga->header->data_pixel_len >> 3));
    return 1;
}
int tga_width(struct tga_image* tga)
{
    return (tga->header->data_width);
}
int tga_height(struct tga_image* tga)
{
    return (tga->header->data_height);
}
int tga_bpp(struct tga_image* tga)
{
    return (tga->header->data_pixel_len >> 3);
}
unsigned char* tga_buffer(struct tga_image* tga)
{
    return tga->data;
}
void tga_clear(struct tga_image* tga)
{
    memset(tga->data, 0, (tga->header->data_width)*(tga->header->data_height)*(tga->header->data_pixel_len >> 3));
}
