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

float nekobee_pitch[128];

void nekobee_init_tables(void) {
    // stub
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
    // float w = nekobee_pitch[voice->key] * 261.63 * synth->deltat;
    float w = 261.63 * synth->deltat;

    // FIXME this only does saws

    for (i = 0; i < count; i++) {
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
}

void nekobee_voice_render(nekobee_synth_t *synth, float *out,
                          unsigned long sample_count, int do_control_update) {

    vco(synth, sample_count);
    memcpy(out, synth->voice->osc_audio, sample_count * sizeof(float));

    return;
    (void)do_control_update;
}