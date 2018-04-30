#include <nds.h>
#include <nds/memory.h>
#include <unistd.h>    //sbrk()

#include "avcodec.h"

#include "../memory_handler_gba/mem_handler.h"

//deprecated: avcodec_alloc_frame()

AVFrame *av_frame_alloc(void)
{
    AVFrame *frame = (AVFrame *)sbrk_alloc(sizeof(*frame));

if (!frame)
    return NULL;

//frame->extended_data = NULL;
//get_frame_defaults(frame);

return frame;
}

  
void av_freep(AVFrame *frame){
    sbrk_dealloc(sizeof(*frame));
}

void get_frame_defaults(AVFrame *frame)
{

//if (frame->extended_data != frame->data)
//    av_freep(&frame->extended_data);

    memset(frame, 0, sizeof(*frame));

    //frame->pts                   =
    //frame->pkt_dts               =
    //frame->pkt_pts               = AV_NOPTS_VALUE;
    //av_frame_set_best_effort_timestamp(frame, AV_NOPTS_VALUE);
    //av_frame_set_pkt_duration         (frame, 0);
    //av_frame_set_pkt_pos              (frame, -1);
    //av_frame_set_pkt_size             (frame, -1);
    //frame->key_frame           = 1;
    //frame->sample_aspect_ratio = (AVRational){ 0, 1 };
    //frame->format              = -1; /* unknown */
    //frame->extended_data       = frame->data;
    //frame->color_primaries     = AVCOL_PRI_UNSPECIFIED;
    //frame->color_trc           = AVCOL_TRC_UNSPECIFIED;
    //frame->colorspace          = AVCOL_SPC_UNSPECIFIED;
    //frame->color_range         = AVCOL_RANGE_UNSPECIFIED;
    //frame->chroma_location     = AVCHROMA_LOC_UNSPECIFIED;
}