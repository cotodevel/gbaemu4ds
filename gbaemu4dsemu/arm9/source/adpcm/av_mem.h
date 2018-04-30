#ifndef avmem
#define avmem

#include "avcodec.h"

#ifdef __cplusplus
extern "C" {
#endif

extern AVFrame *av_frame_alloc(void);
extern void av_freep(AVFrame *frame);
extern void get_frame_defaults(AVFrame *frame);

#ifdef __cplusplus
}
#endif


#endif