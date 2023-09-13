#ifndef COLOR_CORRECTION_H
#define COLOR_CORRECTION_H

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

void LocalExponentialCorrection(unsigned char *Input, unsigned char *Output, int Width, int Height, int Channels);

void LocalColorCorrection(unsigned char *Input, unsigned char *Output, int Width, int Height, int Channels);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // COLOR_CORRECTION_H