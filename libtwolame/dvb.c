/*
 *  TwoLAME: an optimized MPEG Audio Layer Two encoder
 *
 *  Copyright (C) 2001-2004 Michael Cheng
 *  Copyright (C) 2004-2006 The TwoLAME Project
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id$
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "twolame.h"
#include "common.h"
#include "bitbuffer.h"
#include "dvb.h"

#include "bitbuffer_inline.h"

// Returns the desired number of ancillary bits to be
// reserved at the end of the frame
int get_required_dvb_bits(twolame_options * glopts)
{
    int bits = 40;

    if (glopts->dvb_anc.advanced_drc_enabled)
        bits += 24;
    if (glopts->dvb_anc.dialnorm_enabled)
        bits += 8;
    if (glopts->dvb_anc.reprod_enabled)
        bits += 8;
    if (glopts->dvb_anc.acm_enabled)
        bits += 16;

    return bits;
}

// Write DVB ancillary bits at the end of the frame
void write_dvb_bits(twolame_options * glopts, bit_stream * bs)
{
    // dvd_ancillary_data
    buffer_putbits(bs, glopts->dvb_anc.drc_code, 8);
    buffer_put1bit(bs, glopts->dvb_anc.drc_on);
    buffer_putbits(bs, 0, 7);     // reserved

    buffer_putbits(bs, 0xbc, 8);  // extended_ancillary_data_sync

    // bs_info
    buffer_putbits(bs, glopts->version+1, 2);
    buffer_putbits(bs, glopts->dvb_anc.dolby_sur_mode, 2);
    buffer_putbits(bs, 3, 4);  // ancillary_data_bytes

    // ancillary_data_status
    buffer_put1bit(bs, glopts->dvb_anc.advanced_drc_enabled);
    buffer_put1bit(bs, glopts->dvb_anc.dialnorm_enabled);
    buffer_put1bit(bs, glopts->dvb_anc.reprod_enabled);
    buffer_put1bit(bs, 0);     // downmix_levels_MPEG2_status
    buffer_put1bit(bs, 0);     // scale_factor_CRC_status
    buffer_put1bit(bs, glopts->dvb_anc.acm_enabled);
    buffer_put1bit(bs, 0);     // coarse_grain_timecode_status
    buffer_put1bit(bs, 0);     // fine_grain_timecode_status

    if (glopts->dvb_anc.advanced_drc_enabled){
        buffer_putbits(bs, glopts->dvb_anc.advanced_drc_pt0, 8);
        buffer_putbits(bs, glopts->dvb_anc.advanced_drc_pt1, 8);
        buffer_putbits(bs, glopts->dvb_anc.advanced_drc_pt2, 8);
    }

    if (glopts->dvb_anc.dialnorm_enabled)
        buffer_putbits(bs, glopts->dvb_anc.dialnorm, 8);

    if (glopts->dvb_anc.reprod_enabled)
        buffer_putbits(bs, glopts->dvb_anc.reprod_level, 8);

    // downmixing_levels_MPEG2_status=0 because multichannel not supported

    if (glopts->dvb_anc.acm_enabled){
        buffer_putbits(bs, glopts->dvb_anc.acm, 8);  // audio_coding_mode
        buffer_putbits(bs, glopts->dvb_anc.acm_compr, 8);  // Compression
    }
}


// vim:ts=4:sw=4:nowrap:
