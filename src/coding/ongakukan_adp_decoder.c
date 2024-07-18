#include <stdlib.h>
#include "coding.h"
#include "libs/ongakukan_adp_lib.h"

struct ongakukan_adp_data
{
    void* handle;
    int16_t* samples;
    int32_t samples_done;
    bool samples_filled; /* false - no, true - yes */
    int32_t getting_samples; /* initialized to 2 on decode_ongakukan_adp. i mean, we literally get two decoded samples here. */
    STREAMFILE* sf;
};

ongakukan_adp_data* init_ongakukan_adp(STREAMFILE* sf, int32_t data_offset, int32_t data_size,
    bool sound_is_adpcm)
{
    ongakukan_adp_data* data = NULL;

    data = calloc(1, sizeof(ongakukan_adp_data));
    if (!data) goto fail;

    /* reopen STREAMFILE from here, then pass it as an argument for our init function. */
    data->sf = reopen_streamfile(sf, 0);
    if (!data->sf) goto fail;
    data->handle = init_ongakukan_adpcm(data->sf, (long int)(data_offset), (long int)(data_size),
        sound_is_adpcm);
    if (!data->handle) goto fail;

    return data;
fail:
    free_ongakukan_adp(data);
    return NULL;
}

void decode_ongakukan_adp(VGMSTREAM* vgmstream, sample_t* outbuf, int32_t samples_to_do)
{
    ongakukan_adp_data* data = vgmstream->codec_data;

    data->getting_samples = 2;
    data->samples_done = 0;
    data->samples_filled = false;
    /* ^ samples_filled is boolean here because we need to simplify how decoding will work here.
     * so, rather than making samples_filled into a long int counter,
     * we make it into a boolean flag instead so as to let data->samples_done shine as a counter
     * and the decoder to do its job without worry. */

    while (data->samples_done < samples_to_do)
    {
        if (data->samples_filled)
        {
            memcpy(outbuf + data->samples_done,
                data->samples,
                data->getting_samples * sizeof(int16_t));
            data->samples_done += data->getting_samples;

            data->samples_filled = false;
        }
        else { decode_ongakukan_adpcm_data(data->handle);
        data->samples_filled = true;
        data->samples = (int16_t*)get_sample_hist_from_ongakukan_adpcm(data->handle); }
    }
}

void reset_ongakukan_adp(ongakukan_adp_data* data)
{
    if (!data) return;
    ongakukan_adpcm_reset(data->handle);
}

void seek_ongakukan_adp(ongakukan_adp_data* data, int32_t current_sample)
{
    if (!data) return;
    ongakukan_adpcm_seek(data->handle, current_sample);
}

void free_ongakukan_adp(ongakukan_adp_data* data)
{
    if (!data) return;
    close_streamfile(data->sf);
    ongakukan_adpcm_free(data->handle);
    free(data->samples);
    free(data);
}

int32_t ongakukan_adp_get_samples(ongakukan_adp_data* data)
{
    if (!data) return 0;
    return (int32_t)get_num_samples_from_ongakukan_adpcm(data->handle);
}
