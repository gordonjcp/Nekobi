/*
 * DISTRHO Nekobi Plugin, based on Nekobee by Gordonjcp and others.
 * Copyright (C) 2023 Gordonjcp
 * Copyright (C) 2013-2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#include "synth.h"
#include "voice.h"

float nekobee_pitch[128];

void nekobee_init_tables(void) {
    // create an expo pitch table

    int i;

    // having a 6-bit CV DAC means we only need 64 possible pitches
    for (i = 0; i < 64; i++) {
        nekobee_pitch[i] = powf(2, (i - 24) / 12.0f);
    }

    return;
}

void vco(nekobee_synth_t *synth, uint32_t count) {
    // generate a bandlimited oscillator
    // uses polyblep for bandlimiting
    // massive and endless thanks to Mystran
    // https://www.kvraudio.com/forum/viewtopic.php?t=398553

    nekobee_voice_t *voice = synth->voice;
    blosc_t *osc = &voice->osc;

    uint32_t i;

    float phase = osc->phase; // current running phase 0..1
    float delay = osc->delay; // delay sample for polyblep
    float out, t;             // output sample, temporary value for blep

    // calculate omega for phase shift
    float target_w = nekobee_pitch[(voice->key) - 24] * 130.81 * synth->deltat;

    float w = osc->w;
    if (!synth->glide)
        w = target_w;


    // FIXME this only does saws

    for (i = 0; i < count; i++) {

        // glide timeconstant is 100k * 0.22uF
        // for some reason this doesn't sound right - needs a rethink FIXME
        w = ((target_w - w) * (33 * synth->deltat)) + w;
        phase += w;
        out = delay;
        delay = 0;

        if (phase > 1.0f) {
            t = (phase - 1) / w;
            out -= 0.5 * t * t; // other polynomials are available
            t = 1 - t;
            delay += 0.5 * t * t;
            phase -= 1.0f;
        }
        delay += phase; // save value for next time
        voice->osc_audio[i] =
            0.5 - out; // save output in buffer, remove DC offset
    }
    osc->phase = phase;
    osc->delay = delay;
    osc->w = w;
}

void nekobee_voice_render(nekobee_synth_t *synth, float *out,
                          unsigned long sample_count, int do_control_update) {

    vco(synth, sample_count);
    memcpy(out, synth->voice->osc_audio, sample_count * sizeof(float));

    return;
    (void)do_control_update;
}