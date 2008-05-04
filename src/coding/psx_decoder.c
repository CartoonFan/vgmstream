#include "psx_decoder.h"
#include "../util.h"

double VAG_f[5][2] = { { 0.0          ,   0.0        },
    {  60.0 / 64.0 ,   0.0        },
    { 115.0 / 64.0 , -52.0 / 64.0 },
    {  98.0 / 64.0 , -55.0 / 64.0 } ,
    { 122.0 / 64.0 , -60.0 / 64.0 } } ;

void decode_psx(VGMSTREAMCHANNEL * stream, sample * outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {

    int predict_nr, shift_factor, sample;
    int16_t hist1=stream->adpcm_history1_16;
    int16_t hist2=stream->adpcm_history2_16;

    int scale;
    int i;
    int32_t sample_count;

    predict_nr = read_8bit(stream->offset,stream->streamfile) >> 4;
    shift_factor = read_8bit(stream->offset,stream->streamfile) & 0xf;

    for (i=first_sample,sample_count=0; i<first_sample+samples_to_do; i++,sample_count+=channelspacing) {
        int sample_byte = (short)read_8bit(stream->offset+2+i/2,stream->streamfile);

        scale = (short)((i&1 ?
                    sample_byte >> 4 :
                    sample_byte & 0x0f)<<12);

        sample=(int)((scale >> shift_factor)+hist1*VAG_f[predict_nr][0]+hist2*VAG_f[predict_nr][1]);
        outbuf[sample_count] = clamp16(sample);

        hist2=hist1;
        hist1=sample;
    }
    stream->adpcm_history1_16=hist1;
    stream->adpcm_history2_16=hist2;
}

