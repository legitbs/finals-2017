#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: %s <sample 1> <sample 2>\n", argv[0]);
        return 0;
    }

    FILE *file1 = fopen(argv[1], "r");
    FILE *file2 = fopen(argv[2], "r");
    fseek(file1, 0, SEEK_END);
    fseek(file2, 0, SEEK_END);
    unsigned int fsize1 = ftell(file1);
    unsigned int fsize2 = ftell(file2);
    fseek(file1, 0, SEEK_SET);
    fseek(file2, 0, SEEK_SET);

    unsigned char *sample1 = calloc(fsize1, sizeof(unsigned char));
    unsigned char *sample2 = calloc(fsize2, sizeof(unsigned char));
    if (fread(sample1, sizeof(unsigned char), fsize1, file1) < fsize1 * sizeof(unsigned char)) {
        puts("Could not read from input file 1");
        return -1;
    }
    if (fread(sample2, sizeof(unsigned char), fsize2, file2) < fsize2 * sizeof(unsigned char)) {
        puts("Could not read from input file 2");
        return -1;
    }

    fclose(file1);
    fclose(file2);

    unsigned int ptr1 = 0;
    unsigned int ptr2 = 0;
    unsigned int fsize3 = fmax(fsize1, fsize2);
    unsigned char *sample3 = calloc(fsize3, sizeof(unsigned char));
    for (int i = 0; i < fsize3; i++) {
        unsigned int s1 = sample1[ptr1];
        unsigned int s2 = sample2[ptr2];
        sample3[i] = s1 + s2 - (s1 * s2 / (UCHAR_MAX + 1));
        if (++ptr1 > fsize1) { ptr1 = 0; }
        if (++ptr2 > fsize2) { ptr2 = 0; }
    }

    FILE *file3 = fopen("out.raw", "w");
    fwrite(sample3, sizeof(unsigned char), fsize3, file3);
    fclose(file3);

    return 0;
}
