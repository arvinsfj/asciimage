
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* fileread(const char *filename, unsigned long *length)
{
    *length = 0;
    FILE *fd;
    if ((fd = fopen(filename, "rb")) == NULL) {
        printf("file %s can not open\n", filename);
        exit(1);
    }
    fseek(fd, 0, SEEK_END);
    *length = ftell(fd);
    rewind(fd);
    char *ctt = malloc(*length);
    memset(ctt, '\0', *length);
    
    *length = fread(ctt, 1, *length, fd);
    
    fclose(fd);
    
    return ctt;
}

int main(int ac, char **av)
{
    char *filename = NULL;
    char *ctt;
    
    if (ac != 1) {
        printf("usage: ./ucdos_ascii12 > asc12.txt\n");
        exit(1);
    }
    
    unsigned long inlen;
    ctt = fileread("ASC12", &inlen);
    printf("length: %lu\n", inlen);
    for (int i = 0; i < inlen; i+=12) {
        for (int j = 0; j < 12; j++) {
            printf("0x%02hhX,", ctt[i+j]);
        }
        printf(" // -%c-\n\n", (i/12+32));
    }
    
    return 0;
}

