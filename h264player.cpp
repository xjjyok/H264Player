#include "h264player.h"
#include <iostream>


H264Player::H264Player()
{
    std::cout << "ffmpeg version:" << av_version_info() << std::endl;
    m_pFmtCtx           = nullptr;
    m_pVideoCodec       = nullptr;
    m_pVideoCodecCtx    = nullptr;
    m_nVideoStreamIndex = 0;
    m_nAudioStreamIndex = 0;
    m_pBsfCtx           = nullptr;
}

H264Player::~H264Player()
{
    if (m_pFmtCtx) {
        avformat_free_context(m_pFmtCtx);
        m_pFmtCtx = nullptr;
    }

    if (m_pVideoCodecCtx) {
        avcodec_free_context(&m_pVideoCodecCtx);
        m_pVideoCodecCtx = nullptr;
    }

    if (m_pBsfCtx) {
        av_bsf_free(&m_pBsfCtx);
        m_pBsfCtx = nullptr;
    }
}

void H264Player::Init()
{
    av_register_all();
    m_pFmtCtx = avformat_alloc_context();
    if (!m_pFmtCtx) {
        std::cout << "alloc format context fail" << std::endl;
        return;
    }

}

int H264Player::Open(const std::string &filename)
{
    if (filename.empty()) {
        std::cout << "filename error" << std::endl;
        return -1;
    }

    int nRet = avformat_open_input(&m_pFmtCtx, filename.c_str(), nullptr, nullptr);
    if (nRet != 0) {
        std::cout << "avformat_open_input fail" << std::endl;
        return -1;
    }

    avformat_find_stream_info(m_pFmtCtx, nullptr); // hao shi

    av_dump_format(m_pFmtCtx, -1, nullptr, 0);

    /*
     *  * @param wanted_stream_nb  user-requested stream number,
 *                          or -1 for automatic selection
 * @param related_stream    try to find a stream related (eg. in the same
 *                          program) to this one, or -1 if none
 * @param decoder_ret       if non-NULL, returns the decoder for the
    */
    m_nVideoStreamIndex = av_find_best_stream(m_pFmtCtx, AVMEDIA_TYPE_VIDEO, -1,
                                              -1, nullptr, 0);
    m_nAudioStreamIndex = av_find_best_stream(m_pFmtCtx, AVMEDIA_TYPE_AUDIO, -1,

                                              -1, nullptr, 0);

    m_pVideoCodec       = avcodec_find_decoder(m_pFmtCtx->streams[m_nVideoStreamIndex]->codecpar->codec_id);
    if (!m_pVideoCodec) {
        std::cout << "can not find video decoder" << std::endl;
        return -1;
    }

    m_pVideoCodecCtx = avcodec_alloc_context3(m_pVideoCodec);
    if (!m_pVideoCodecCtx) {
        std::cout << "alloc video codeccontex fail" << std::endl;
        return -1;
    }

    if (avcodec_parameters_to_context(m_pVideoCodecCtx, m_pFmtCtx->streams[m_nVideoStreamIndex]->codecpar)) {
        std::cout << "avcodec_parameters_to_context fail" << std::endl;
        return -1;
    }

    if (avcodec_open2(m_pVideoCodecCtx, m_pVideoCodec, nullptr)) {
        std::cout << "avcodec_open2 fail" << std::endl;
        return -1;
    }


    const AVBitStreamFilter *bitstreamFilter = av_bsf_get_by_name("h264_mp4toannexb");
    if (av_bsf_alloc(bitstreamFilter, &m_pBsfCtx)) {
        std::cout << "alloc bsf fail" << std::endl;
        return -1;
    }

    avcodec_parameters_copy(m_pBsfCtx->par_in, m_pFmtCtx->streams[m_nVideoStreamIndex]->codecpar);
    av_bsf_init(m_pBsfCtx);

    return 0;
}

int H264Player::ExtractH264(FILE *h264file)
{
    if (!h264file) {
        std::cout << "parameter error" << std::endl;
        return -1;
    }

    int nRet = 0;

    AVPacket *pAvPkt = av_packet_alloc();
    if (!pAvPkt) {
        std::cout << "alloc avpacket fail" << std::endl;
        return -1;
    }

    char starCode[] = {0, 0, 0, 1};

    // write sps pps
    fwrite(starCode, 4, 1, h264file);
    fwrite(m_pVideoCodecCtx->extradata, m_pVideoCodecCtx->extradata_size, 1, h264file);

    char *baseline_profile = (char *)m_pVideoCodecCtx->extradata;
    //std::cout << "baseline_profile = " << *baseline_profile << std::endl;
    printf("byte[0] = %x\n", *(char *)baseline_profile);
    printf("byte[1] = %x\n", *(char *)(baseline_profile + 1) & 0xff);
    printf("byte[2] = %x\n", *(char *)(baseline_profile + 2) & 0xff);
    printf("byte[3] = %x\n", *(char *)(baseline_profile + 3) & 0xff);


    while (av_read_frame(m_pFmtCtx, pAvPkt) == 0) {
        if (pAvPkt->stream_index != m_nVideoStreamIndex) {
            av_packet_unref(pAvPkt);
            continue;
        }

        nRet = av_bsf_send_packet(m_pBsfCtx, pAvPkt);
        if (nRet) {
            std::cout << "av_bsf_send_packet fail" << std::endl;
        }

        if (nRet == 0) {
            av_bsf_receive_packet(m_pBsfCtx, pAvPkt);
            fwrite(starCode, 4, 1, h264file); // add startcode
            fwrite(pAvPkt->data + 4, pAvPkt->size, 1, h264file); // before 4 is len of nalu
        }

        av_packet_unref(pAvPkt);

    }
}

std::string H264Player::GetSpsPpsInfo(std::string fileName)
{
    if (fileName.empty()) {
        std::cout << "fileName is error" << std::endl;
    }

    std::string spsPpsInfo;

    return spsPpsInfo;
}
