/* nekobee DSSI software synthesizer plugin
 *
 * Copyright (C) 2004 Sean Bolton and others.
 *
 * Portions of this file may have come from Steve Brookes'
 * nekobee, copyright (C) 1999 S. J. Brookes.
 * Portions of this file may have come from Peter Hanappe's
 * Fluidsynth, copyright (C) 2003 Peter Hanappe and others.
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

#ifndef _XSYNTH_VOICE_H
#define _XSYNTH_VOICE_H

#include <string.h>

#include "nekobee_types.h"

/* maximum size of a rendering burst */
#define XSYNTH_NUGGET_SIZE      64

#if 0
struct _nekobee_patch_t
{
    float         tuning;
    unsigned char waveform;
    float         cutoff;
    float         resonance;
    float         envmod;
    float         decay;
    float         accent;
    float         volume;
};
#endif

enum nekobee_voice_status
{
    XSYNTH_VOICE_OFF,       /* silent: is not processed by render loop */
    XSYNTH_VOICE_ON,        /* has not received a note off event */
    XSYNTH_VOICE_SUSTAINED, /* has received note off, but sustain controller is on */
    XSYNTH_VOICE_RELEASED   /* had note off, not sustained, in final decay phase of envelopes */
};

struct blosc
{
    int   last_waveform,    /* persistent */
          waveform,         /* comes from LADSPA port each cycle */
          bp_high;          /* persistent */
    float pos,              /* persistent */
          pw;               /* comes from LADSPA port each cycle */
};

/*
 * nekobee_voice_t
 */
struct _nekobee_voice_t
{
    unsigned int  note_id;

    unsigned char status;
    unsigned char key;
    unsigned char velocity;
    unsigned char rvelocity;   /* the note-off velocity */

    /* persistent voice state */
    float         prev_pitch,
                  target_pitch;
    struct blosc  osc1;
    float         vca_eg,
                  vcf_eg,
                  accent_slug,
                  delay1,
                  delay2,
                  delay3,
                  delay4;
    unsigned char vca_eg_phase,
                  vcf_eg_phase;
    int           osc_index;       /* shared index into osc_audio */
    float         osc_audio[XSYNTH_NUGGET_SIZE];
};

#define _PLAYING(voice)    ((voice)->status != XSYNTH_VOICE_OFF)
#define _ON(voice)         ((voice)->status == XSYNTH_VOICE_ON)
#define _SUSTAINED(voice)  ((voice)->status == XSYNTH_VOICE_SUSTAINED)
#define _RELEASED(voice)   ((voice)->status == XSYNTH_VOICE_RELEASED)
#define _AVAILABLE(voice)  ((voice)->status == XSYNTH_VOICE_OFF)

extern float nekobee_pitch[128];

/* nekobee_voice.c */
nekobee_voice_t *nekobee_voice_new();
void             nekobee_voice_note_on(nekobee_synth_t *synth,
                                     nekobee_voice_t *voice,
                                     unsigned char key,
                                     unsigned char velocity);
void             nekobee_voice_note_off(nekobee_synth_t *synth,
                                      nekobee_voice_t *voice,
                                      unsigned char key,
                                      unsigned char rvelocity);


/* nekobee_voice_render.c */
void nekobee_init_tables(void);
void nekobee_voice_render(nekobee_synth_t *synth, nekobee_voice_t *voice,
                         float *out, unsigned long sample_count,
                         int do_control_update);

/* inline functions */

/*
 * nekobee_voice_off
 *
 * Purpose: Turns off a voice immediately, meaning that it is not processed
 * anymore by the render loop.
 */
static inline void
nekobee_voice_off(nekobee_voice_t* voice)
{
    voice->status = XSYNTH_VOICE_OFF;
    /* silence the oscillator buffer for the next use */
    memset(voice->osc_audio, 0, XSYNTH_NUGGET_SIZE * sizeof(float));
    /* -FIX- decrement active voice count? */
}

/*
 * nekobee_voice_start_voice
 */
static inline void
nekobee_voice_start_voice(nekobee_voice_t *voice)
{
    voice->status = XSYNTH_VOICE_ON;
    /* -FIX- increment active voice count? */
}

#endif /* _XSYNTH_VOICE_H */
