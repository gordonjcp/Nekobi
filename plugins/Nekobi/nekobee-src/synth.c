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


void nekobee_synth_all_voices_off(nekobee_synth_t *synth)
{
    return;
    (void)synth;
}

void nekobee_synth_note_off(nekobee_synth_t *synth, unsigned char key,
                            unsigned char rvelocity)
{
    return;
    (void)synth;
    (void)key;
    (void)rvelocity;
}

void nekobee_synth_note_on(nekobee_synth_t *synth, unsigned char key,
                           unsigned char velocity)
{
    return;
    (void)synth;
    (void)key;
    (void)velocity;
}

void nekobee_synth_render_voice(nekobee_synth_t *synth, float *out, unsigned long sample_count,
                                 int do_control_update)
{
    return;
    (void)synth;
    (void)out;
    (void)sample_count;
    (void)do_control_update;
}