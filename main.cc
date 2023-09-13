#include "browse.h"
#include "timing.h"
#define USE_SHELL_OPEN
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"
#include "ColorCorrection.h"

char saveFile[1024];

unsigned char *loadImage(const char *filename, int *Width, int *Height, int *Channels) {
    return (stbi_load(filename, Width, Height, Channels, 0));
}

void saveImage(const char *filename, int Width, int Height, int Channels, unsigned char *Output) {
    memcpy(saveFile + strlen(saveFile), filename, strlen(filename));
    *(saveFile + strlen(saveFile) + 1) = 0;

    if (!tje_encode_to_file(saveFile, Width, Height, Channels, true, Output)) {
        fprintf(stderr, "save JPEG fail.\n");
        return;
    }
#ifdef USE_SHELL_OPEN
    browse(saveFile);
#endif
}

void splitpath(const char *path, char *drv, char *dir, char *name, char *ext) {
    const char *end;
    const char *p;
    const char *s;
    if (path[0] && path[1] == ':') {
        if (drv) {
            *drv++ = *path++;
            *drv++ = *path++;
            *drv = '\0';
        }
    } else if (drv)
        *drv = '\0';
    for (end = path; *end && *end != ':';) end++;
    for (p = end; p > path && *--p != '\\' && *p != '/';)
        if (*p == '.') {
            end = p;
            break;
        }
    if (ext)
        for (s = end; (*ext = *s++);) ext++;
    for (p = end; p > path;)
        if (*--p == '\\' || *p == '/') {
            p++;
            break;
        }
    if (name) {
        for (s = p; s < end;) *name++ = *s++;
        *name = '\0';
    }
    if (dir) {
        for (s = path; s < p;) *dir++ = *s++;
        *dir = '\0';
    }
}

void getCurrentFilePath(const char *filePath, char *saveFile) {
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    splitpath(filePath, drive, dir, fname, ext);
    size_t n = strlen(filePath);
    memcpy(saveFile, filePath, n);
    char *cur_saveFile = saveFile + (n - strlen(ext));
    cur_saveFile[0] = '_';
    cur_saveFile[1] = 0;
}

int main(int argc, char **argv) {
    printf("Local Color Correction demo\n ");
    printf("blog:http://cpuimage.cnblogs.com/ \n ");

    if (argc < 2) {
        printf("usage: %s   image \n ", argv[0]);
        printf("eg: %s   d:\\image.jpg \n ", argv[0]);

        return (0);
    }
    char *szfile = argv[1];

    getCurrentFilePath(szfile, saveFile);

    int Width = 0;
    int Height = 0;
    int Channels = 0;
    unsigned char *inputImage = NULL;

    double startTime = now();
    inputImage = loadImage(szfile, &Width, &Height, &Channels);

    double nLoadTime = calcElapsed(startTime, now());
    printf("load time: %d ms.\n ", (int)(nLoadTime * 1000));
    if ((Channels != 0) && (Width != 0) && (Height != 0)) {
        unsigned char *outputImg = (unsigned char *)stbi__malloc(Width * Channels * Height * sizeof(unsigned char));
        if (inputImage) {
            memcpy(outputImg, inputImage, (size_t)(Width * Channels * Height));
        } else {
            printf("load: %s fail!\n ", szfile);
        }
        startTime = now();
        //  LocalColorCorrection(inputImage, outputImg, Width, Height, Channels);
        LocalExponentialCorrection(inputImage, outputImg, Width, Height, Channels);
        double nProcessTime = calcElapsed(startTime, now());

        printf("process time: %d ms.\n ", (int)(nProcessTime * 1000));

        startTime = now();

        saveImage("done.jpg", Width, Height, Channels, outputImg);
        double nSaveTime = calcElapsed(startTime, now());

        printf("save time: %d ms.\n ", (int)(nSaveTime * 1000));

        if (outputImg) {
            stbi_image_free(outputImg);
        }

        if (inputImage) {
            stbi_image_free(inputImage);
        }
    } else {
        printf("load: %s fail!\n", szfile);
    }

    getchar();
    printf("press any key to exit. \n");

    return (EXIT_SUCCESS);
}