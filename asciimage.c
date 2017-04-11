
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asciifont.h"

char* piperead(unsigned long *length)
{
    *length = 0;
    const unsigned long addsize = sizeof(char) * 1024 * 1024; // 1mb
    unsigned long size = addsize;
    char *ctt = malloc(size);
    memset(ctt, '\0', size);
    char c;
    char *cttp = ctt;
    while ((c = getchar()) != EOF) {
        if (*length >= size) {
            size += addsize;
            char *cttr = realloc(ctt, size);
            if (cttr == NULL) {
                return ctt;
            }
            if (ctt != cttr) {
                cttp = cttr + (size - addsize);
                ctt = cttr;
            }
        }
        (*length)++;
        *cttp++ = c;
    }
    return ctt;
}

int main(int ac, char **av)
{
    char *arg;
    char *filename = NULL;
    char *ctt;
    
    if (ac != 3) {
        printf("usage: cat [filename] | ./asciimage -ft[1,2,3] [filename]\n");
        exit(1);
    }
    
    arg = av[1];
    filename = av[2];
    
    if (strncasecmp("-ft", arg, 3)) {
        printf("usage: cat [filename] | ./asciimage -ft[1,2,3] [filename]\n");
        exit(1);
    }
    
    int type = arg[3]-'0';
    if (type<1 || type>3) {
        printf("usage: cat [filename] | ./asciimage -ft[1,2,3] [filename]\n");
        exit(1);
    }
    
    unsigned long inlen;
    ctt = piperead(&inlen);
    draw_txt(ctt, inlen, type, filename);
    
    return 0;
}

