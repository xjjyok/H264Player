#ifndef H264PLAYER_H
#define H264PLAYER_H

#include <iostream>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"

#ifdef __cplusplus
}
#endif

typedef struct {
    int profile_ide;
    int constraint_set0_flag;
    int constraint_set1_flag;
    int constraint_set2_flag;
    int constraint_set3_flag;
    int constraint_set4_flag;
    int constraint_set5_flag;
    int reserved_zero_2bits;
    int level_idc;

    int log2_max_frame_num_minus4;
    int pic_order_cnt_type;

}Sps;

class H264Player
{
public:
    H264Player();
    ~H264Player();

    void Init();
    int Open(const std::string &filename);
    // extract h264 file
    int ExtractH264(FILE *h264file);
    std::string GetSpsPpsInfo(std::string fileName);

private:
    AVFormatContext *m_pFmtCtx;
    AVCodecContext   *m_pVideoCodecCtx;
    AVCodec         *m_pVideoCodec;
    int             m_nVideoStreamIndex;
    int             m_nAudioStreamIndex;
    AVBSFContext    *m_pBsfCtx;

};

#endif // H264PLAYER_H
