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

void nekobee_voice_render(nekobee_synth_t *synth, nekobee_voice_t *voice,
                         float *out, unsigned long sample_count,
                         int do_control_update) {
    return;
    (void)synth; (void)voice, (void)out; (void)sample_count; (void)do_control_update;
}