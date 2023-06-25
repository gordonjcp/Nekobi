/* nekobee DSSI software synthesizer plugin
 *
 * Copyright (C) 2004 Sean Bolton and others.
 *
 * Portions of this file may have come from Peter Hanappe's
 * Fluidsynth, copyright (C) 2003 Peter Hanappe and others.
 * Portions of this file may have come from alsa-lib, copyright
 * and licensed under the LGPL v2.1.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef _XSYNTH_SYNTH_H
#define _XSYNTH_SYNTH_H

#include "nekobee_types.h"

/*
 * nekobee_synth_t
 */
struct _nekobee_synth_t {
    /* output */
    unsigned long   sample_rate;
    float           deltat;            /* 1 / sample_rate */
    unsigned long   nugget_remains;

    /* voice tracking and data */
    unsigned int    note_id;           /* incremented for every new note, used for voice-stealing prioritization */
    float           last_noteon_pitch; /* glide start pitch for non-legato modes */
    signed char     held_keys[8];      /* for monophonic key tracking, an array of note-ons, most recently received first */
    float           vcf_accent;        /* used to emulate the circuit that sweeps the vcf at full resonance */
    float           vca_accent;        /* used to smooth the accent pulse, removing the click */

    nekobee_voice_t *voice;

    /* patch parameters */
    float          tuning;
    float          waveform;
    float          cutoff;
    float          resonance;
    float          envmod;
    float          decay;
    float          accent;
    float          volume;
};


void nekobee_synth_note_off(nekobee_synth_t *synth, unsigned char key,
                            unsigned char rvelocity);

void nekobee_synth_note_on(nekobee_synth_t *synth, unsigned char key,
                           unsigned char velocity);


void nekobee_synth_render_voices(nekobee_synth_t *synth, float *out,
                                 unsigned long sample_count,
                                 int do_control_update);

#endif /* _XSYNTH_SYNTH_H */
