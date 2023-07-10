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

#include <stdint.h>
#include <stdio.h>

#include "synth.h"
#include "voice.h"

void nekobee_synth_all_voices_off(nekobee_synth_t *synth) {
    // implement the "All Voices Off" MIDI command
    // should just stop the current voice, and let it tail out
    // splat all the notes in the note buffer
    return;
    (void)synth;
}

void nekobee_synth_note_off(nekobee_synth_t *synth, unsigned char key,
                            unsigned char rvelocity) {

    int i;
    // a note value of 0 means an empty slot, and sending a note off with 0
    // might induce some bad behaviour, so let's avoid that
    // any other note number, even ones we don't play, are fair game
    if (key < 1)
        return;
    // scan for note already being in playing queue
    // (we might get two Note Ons for the same note)
    for (i = 0; i < KEY_BUFFER; i++) {
        if (synth->held_keys[i] == key) {
            // found it
            memmove(&synth->held_keys[i], &synth->held_keys[i + 1],
                    KEY_BUFFER - i - 1);
            synth->held_keys[KEY_BUFFER - 1] = 0;
        }
    }

    printf("------------------------\nafter note off keys are ");
    for (i = 0; i < KEY_BUFFER; i++)
        printf("%02x ", (synth->held_keys[i]));
    printf("\n");

    return;
    (void)key;
    (void)rvelocity;
}

void nekobee_synth_note_on(nekobee_synth_t *synth, unsigned char key,
                           unsigned char velocity) {
    // interpret Note On message

    // check if note is within C1 to C5
    if (key < 24 || key > 72)
        return; // just don't even play it

    // handle note queueing
    // TDv8 always plays the newest note first and will "pull off" to the
    // previous one in the queue.
    int i;

    // scan for note already being in playing queue
    // (we might get two Note Ons for the same note)
    for (i = 0; i < KEY_BUFFER - 1; i++) {
        if (synth->held_keys[i] == key) {
            // found it
            printf("found existing key %02x\n", key);
            memmove(&synth->held_keys[i], &synth->held_keys[i + 1],
                    KEY_BUFFER - i - 1);
        }
    }

    // move the queue up, discarding one off the end
    printf("%p %p\n", &synth->held_keys,
           memmove(&synth->held_keys[1], &synth->held_keys[0], KEY_BUFFER - 1));

    synth->held_keys[0] = key;

    printf("------------------------\nafter note on keys are ");
    for (i = 0; i < KEY_BUFFER; i++)
        printf("%02x ", synth->held_keys[i]);
    printf("\n");

    return;
    (void)velocity;
}

void nekobee_synth_render_voice(nekobee_synth_t *synth, float *out,
                                unsigned long sample_count,
                                int do_control_update) {
    return;
    (void)synth;
    (void)out;
    (void)sample_count;
    (void)do_control_update;
}