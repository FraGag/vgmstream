#include "meta.h"
#include "../layout/layout.h"
#include "../util.h"

typedef struct {
    int is_music;

    int total_subsongs;

    int channel_count;
    int sample_rate;
    int codec;
    int num_samples;

    size_t block_count;
    size_t block_size;

    off_t stream_offset;
    size_t stream_size;

} ivaud_header;

static int parse_ivaud_header(STREAMFILE* streamFile, ivaud_header* ivaud);


/* .ivaud - from GTA IV (PC) */
VGMSTREAM * init_vgmstream_ivaud(STREAMFILE *streamFile) {
    VGMSTREAM * vgmstream = NULL;
    ivaud_header ivaud = {0};
    int loop_flag;

    /* checks */
    /* (hashed filenames are likely extensionless and .ivaud is added by tools) */
    if (!check_extensions(streamFile, "ivaud,"))
        goto fail;

    /* check header */
    if (!parse_ivaud_header(streamFile, &ivaud))
        goto fail;


    loop_flag = 0;


    /* build the VGMSTREAM */
    vgmstream = allocate_vgmstream(ivaud.channel_count,loop_flag);
    if (!vgmstream) goto fail;

    vgmstream->sample_rate = ivaud.sample_rate;
    vgmstream->num_samples = ivaud.num_samples;
    vgmstream->num_streams = ivaud.total_subsongs;
    vgmstream->stream_size = ivaud.stream_size;
    vgmstream->meta_type = meta_IVAUD;

    switch(ivaud.codec) {
        case 0x0001: /* common in sfx, uncommon in music (ex. EP2_SFX/MENU_MUSIC) */
            vgmstream->coding_type = coding_PCM16LE;
            vgmstream->layout_type = ivaud.is_music ? layout_blocked_ivaud : layout_none;
            vgmstream->full_block_size = ivaud.block_size;
            break;

        case 0x0400:
            vgmstream->coding_type = coding_IMA_int;
            vgmstream->layout_type = ivaud.is_music ? layout_blocked_ivaud : layout_none;
            vgmstream->full_block_size = ivaud.block_size;
            break;

        default:
            VGM_LOG("IVAUD: unknown codec 0x%x\n", ivaud.codec);
            goto fail;
    }


    if (!vgmstream_open_stream(vgmstream,streamFile,ivaud.stream_offset))
        goto fail;

    if (vgmstream->layout_type == layout_blocked_ivaud)
        block_update_ivaud(ivaud.stream_offset, vgmstream);

    return vgmstream;

fail:
    close_vgmstream(vgmstream);
    return NULL;
}

/* Parse Rockstar's .ivaud header (much info from SparksIV). */
static int parse_ivaud_header(STREAMFILE* streamFile, ivaud_header* ivaud) {
    off_t block_table_offset, channel_table_offset, channel_info_offset;


    /* use bank's block_count to detect */
    ivaud->is_music = (read_32bitLE(0x10,streamFile) == 0);

    if (ivaud->is_music)  {
        /* music header */
        block_table_offset = read_32bitLE(0x00,streamFile); /* 64b */
        ivaud->block_count = read_32bitLE(0x08,streamFile);
        ivaud->block_size = read_32bitLE(0x0c,streamFile); /* 64b, uses padded blocks */
        channel_table_offset = read_32bitLE(0x14,streamFile); /* 64b */
        /* 0x1c(8): block_table_offset again? */
        ivaud->channel_count = read_32bitLE(0x24,streamFile);
        /* 0x28(4): unknown entries? */
        ivaud->stream_offset = read_32bitLE(0x2c,streamFile);
        channel_info_offset = channel_table_offset + ivaud->channel_count*0x10;

        if ((ivaud->block_count * ivaud->block_size) + ivaud->stream_offset != get_streamfile_size(streamFile)) {
            VGM_LOG("IVAUD: bad file size\n");
            goto fail;
        }

        /* channel table (one entry per channel, points to channel info) */
        /* 0x00(8): offset within channel_info_offset */
        /* 0x08(4): hash */
        /* 0x0c(4): size */

        /* channel info (one entry per channel) */
        /* 0x00(8): offset to unknown data? */
        /* 0x08(4): hash */
        /* 0x0c(4): samples in bytes? */
        ivaud->num_samples = read_32bitLE(channel_info_offset+0x10,streamFile); /* data size? (double of 0x0c) */ //todo
        //num_samples = read_32bitLE(channel_info_offset+0x10,streamFile); /* data size? (double of 0x0c) */ //todo
        /* 0x14(4): unknown (-1) */
        /* 0x18(2): sample rate */
        /* 0x1a(2): unknown */
        ivaud->codec = read_32bitLE(channel_info_offset+0x1c,streamFile);
        /* (when codec is IMA) */
        /* 0x20(8): adpcm states offset, 0x38: num states? (reference for seeks?) */
        /* rest: unknown data */

        /* block table (one entry per block) */
        /* 0x00: data size processed up to this block (doesn't count block padding) */
        ivaud->sample_rate = read_32bitLE(block_table_offset + 0x04,streamFile);
        /* sample_rate should agree with each channel in the channel table */


        ivaud->total_subsongs = 1;
        ivaud->stream_size = get_streamfile_size(streamFile);
    }
    else {
        /* bank header */
        goto fail;
    }


    return 1;
fail:
    return 0;
}
