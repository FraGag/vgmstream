#include "meta.h"
#include "../util/endianness.h"


/* .SBK - EA Redwood Shores soundbank (Simpsons Game, Godfather) */
VGMSTREAM* init_vgmstream_ea_sbk(STREAMFILE* sf) {
    VGMSTREAM* vgmstream = NULL;
    int target_stream = sf->stream_index;
    off_t sdat_offset;
    size_t sdat_size;
    read_u32_t read_u32;

    /* checks */
    if (!is_id32be(0x00, sf, "sbnk") && /* sbnk */
        !is_id32le(0x00, sf, "sbnk"))   /* knbs */
        return NULL;

    if (!check_extensions(sf, "sbk"))
        return NULL;


    read_u32 = is_id32be(0x00, sf, "sbnk") ? read_u32le : read_u32be;

    sdat_offset = read_u32(0x10, sf);
    sdat_size = read_u32(0x14, sf);

    /* lots of other unk data between here and the sdat chunk */

    if (sdat_offset + sdat_size != get_streamfile_size(sf))
        goto fail;


    if (target_stream < 0) goto fail;
    if (target_stream == 0) target_stream = 1;
    target_stream -= 1;

    if (is_id32be(sdat_offset, sf, "BNKl") ||
        is_id32be(sdat_offset, sf, "BNKb")) {
        /* The Godfather */

        vgmstream = load_vgmstream_ea_bnk(sf, sdat_offset, target_stream, 0);
        if (!vgmstream) goto fail;

        vgmstream->meta_type = meta_EA_SBK;
    }
    else if (is_id32be(sdat_offset, sf, "sdat") || /* sdat */
             is_id32le(sdat_offset, sf, "sdat")) { /* tads */
        /* The Simpsons Game */

        int total_streams;
        off_t entry_offset, stream_offset;

        eaac_meta_t info = {0};


        total_streams = read_u32(sdat_offset + 0x04, sf);
        if (total_streams < 1 || target_stream + 1 > total_streams)
            goto fail;

        /* For each entry:
         * 0x00: stream index
         * 0x04: 0x0313BABE (?)
         * 0x08: stream offset
         * 0x0C: 0xFEEDFEED (?)
         */
        entry_offset = sdat_offset + 0x08 + target_stream * 0x10;

        if (read_u32(entry_offset + 0x00, sf) != target_stream) goto fail;
        stream_offset = sdat_offset + read_u32(entry_offset + 0x08, sf);

        info.sf_head = sf;
        info.sf_body = sf;
        info.head_offset = stream_offset;
        //info.body_offset
        info.type = meta_EA_SBK;

        vgmstream = load_vgmstream_ea_eaac(&info);
        if (!vgmstream) goto fail;

        vgmstream->num_streams = total_streams;
    }
    else {
        VGM_LOG("EA SBK: unsupported sound data chunk\n");
        goto fail;
    }

    return vgmstream;

fail:
    close_vgmstream(vgmstream);
    return NULL;
}
