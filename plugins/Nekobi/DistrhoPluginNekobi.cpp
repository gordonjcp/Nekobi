/*
 * DISTRHO Nekobi Plugin, based on Nekobee by Gordonjcp and others.
 * Copyright (C) 2004 Sean Bolton and others
 * Copyright (C) 2005, 2023 Gordonjcp
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

#include "DistrhoPluginNekobi.hpp"

#include "nekobee-src/synth.c"
//#include "nekobee-src/nekobee_voice.c"
#include "nekobee-src/voice_render.c"

#define XSYNTH_NUGGET_SIZE 64
#include "nekobee-src/voice.h"

// -----------------------------------------------------------------------
// nekobee_handle_raw_event

void nekobee_handle_raw_event(nekobee_synth_t *const synth, const uint8_t size, const uint8_t *const data)
{
    if (size != 3)
        return;

    switch (data[0] & 0xf0)
    {
    case 0x80:
        nekobee_synth_note_off(synth, data[1], data[2]);
        break;
    case 0x90:
        if (data[2] > 0)
            nekobee_synth_note_on(synth, data[1], data[2]);
        else
            nekobee_synth_note_off(synth, data[1], 64); // Note On 0 Velocity, for running status
        break;
    default:
        break;
    }
}

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

DistrhoPluginNekobi::DistrhoPluginNekobi()
    : Plugin(paramCount, 0, 0) // 0 programs, 0 states
{
    nekobee_init_tables();

    // init synth
    fSynth.sample_rate = getSampleRate();
    fSynth.deltat = 1.0f / (float)getSampleRate();
    fSynth.nugget_remains = 0;

    fSynth.vcf_accent = 0.0f;
    fSynth.vca_accent = 0.0f;

    for (int i = 0; i < 8; ++i)
        fSynth.held_keys[i] = -1;

    // FIXME
    //fSynth.voice = nekobee_voice_new();
    fSynth.voice = (nekobee_voice_t *)calloc(sizeof(nekobee_voice_t), 1);
    bzero(fSynth.held_keys, KEY_BUFFER);
    

    // Default values
    setParameterValue(paramWaveform, 0);    // squarewave
    setParameterValue(paramTuning, 0);      // centre
    setParameterValue(paramCutoff, 27);     // up a bit
    setParameterValue(paramResonance, 0);   // resonance off
    setParameterValue(paramEnvMod, 33);     // on some
    setParameterValue(paramDecay, 100);     // maximum
    setParameterValue(paramAccent, 50);     // half
    setParameterValue(paramVolume, 75);     // mostly up

    // reset
    deactivate();
}

DistrhoPluginNekobi::~DistrhoPluginNekobi()
{
    std::free(fSynth.voice);
}

// -----------------------------------------------------------------------
// Init

void DistrhoPluginNekobi::initAudioPort(bool input, uint32_t index, AudioPort &port)
{
    port.groupId = kPortGroupMono;

    Plugin::initAudioPort(input, index, port);
}

void DistrhoPluginNekobi::initParameter(uint32_t index, Parameter &parameter)
{
    switch (index)
    {
    case paramWaveform:
        parameter.hints = kParameterIsAutomatable | kParameterIsInteger;
        parameter.name = "Waveform";
        parameter.symbol = "waveform";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.enumValues.count = 2;
        parameter.enumValues.restrictedMode = true;
        parameter.midiCC = 70; // Sound Variation
        {
            ParameterEnumerationValue *const enumValues = new ParameterEnumerationValue[2];
            enumValues[0].value = 0.0f;
            enumValues[0].label = "Square";
            enumValues[1].value = 1.0f;
            enumValues[1].label = "Triangle";
            parameter.enumValues.values = enumValues;
        }
        break;
    case paramTuning:
        parameter.hints = kParameterIsAutomatable; // was 0.5 <-> 2.0, log
        parameter.name = "Tuning";
        parameter.symbol = "tuning";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = -12.0f;
        parameter.ranges.max = 12.0f;
        parameter.midiCC = 75;
        break;
    case paramCutoff:
        parameter.hints = kParameterIsAutomatable; // modified x2.5
        parameter.name = "Cutoff";
        parameter.symbol = "cutoff";
        parameter.unit = "%";
        parameter.ranges.def = 27.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        parameter.midiCC = 74;
        break;
    case paramResonance:
        parameter.hints = kParameterIsAutomatable; // modified x100
        parameter.name = "VCF Resonance";
        parameter.symbol = "resonance";
        parameter.unit = "%";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        parameter.midiCC = 71;
        break;
    case paramEnvMod:
        parameter.hints = kParameterIsAutomatable; // modified x100
        parameter.name = "Env Mod";
        parameter.symbol = "env_mod";
        parameter.unit = "%";
        parameter.ranges.def = 33.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        parameter.midiCC = 1; // Mod Wheel
        break;
    case paramDecay:
        parameter.hints = kParameterIsAutomatable; // was 0.000009 <-> 0.0005, log
        parameter.name = "Decay";
        parameter.symbol = "decay";
        parameter.unit = "%";
        parameter.ranges.def = 100.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        parameter.midiCC = 72;
        break;
    case paramAccent:
        parameter.hints = kParameterIsAutomatable; // modified x100
        parameter.name = "Accent";
        parameter.symbol = "accent";
        parameter.unit = "%";
        parameter.ranges.def = 50.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        parameter.midiCC = 76;
        break;
    case paramVolume:
        parameter.hints = kParameterIsAutomatable; // modified x100
        parameter.name = "Volume";
        parameter.symbol = "volume";
        parameter.unit = "%";
        parameter.ranges.def = 66.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 100.0f;
        parameter.midiCC = 7; // Volume
        break;
    }
}

// -----------------------------------------------------------------------
// Internal data

float DistrhoPluginNekobi::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramWaveform:
        return fParams.waveform;
    case paramTuning:
        return fParams.tuning;
    case paramCutoff:
        return fParams.cutoff;
    case paramResonance:
        return fParams.resonance;
    case paramEnvMod:
        return fParams.envMod;
    case paramDecay:
        return fParams.decay;
    case paramAccent:
        return fParams.accent;
    case paramVolume:
        return fParams.volume;
    }

    return 0.0f;
}

void DistrhoPluginNekobi::setParameterValue(uint32_t index, float value)
{
    switch (index)
    {
    case paramWaveform:
        // Waveform is just 0 or 1 for square or saw
        fParams.waveform = value;
        fSynth.waveform = value;
        DISTRHO_SAFE_ASSERT(fSynth.waveform == 0.0f || fSynth.waveform == 1.0f);
        break;
    case paramTuning:
        // converted from +/- 12 semitones to a multiplier
        fParams.tuning = value;
        fSynth.tuning = exp2f(value / 12.0f);
        DISTRHO_SAFE_ASSERT(fSynth.tuning >= 0.5f && fSynth.tuning <= 2.0f);
        break;
    case paramCutoff:
        // cutoff pot is 50k log
        fParams.cutoff = value;
        fSynth.cutoff = 1.611 * powf(32, value / 100.0f) - 1.611;
        DISTRHO_SAFE_ASSERT(fSynth.cutoff >= 0.0f && fSynth.cutoff <= 50.0f);
        break;
    case paramResonance:
        // 50k linear pot, but we don't really need to care about its value
        fParams.resonance = value;
        fSynth.resonance = value / 100.0f;
        DISTRHO_SAFE_ASSERT(fSynth.resonance >= 0.0f && fSynth.resonance <= 1.0f);
        break;
    case paramEnvMod:
        // Env Mod pot is 50k log, same as cutoff (and wired similarly)
        fParams.envMod = value;
        fSynth.envmod = 1.611 * powf(32, value / 100.0f) - 1.611;
        DISTRHO_SAFE_ASSERT(fSynth.envmod >= 0.0f && fSynth.envmod <= 50.0f);
        break;
    case paramDecay:
        // decay is a 1M log pot
        fParams.decay = value;
        fSynth.decay = 32.255 * powf(32, value / 100.0f) - 32.255;
        DISTRHO_SAFE_ASSERT(fSynth.decay >= 0.0f && fSynth.decay <= 1000.0f);
        break;
    case paramAccent:
        // accent is a 50k linear pot, again we don't really need to care about exact value
        fParams.accent = value;
        fSynth.accent = value / 100.0f;
        DISTRHO_SAFE_ASSERT(fSynth.accent >= 0.0f && fSynth.accent <= 1.0f);
        break;
    case paramVolume:
        // 50k log pot, but we don't care about value
        // so this can just be 0..1 converted exponentially
        fParams.volume = value;
        fSynth.volume = 0.032255 * powf(32, value / 100.0f) - 0.032255;
        DISTRHO_SAFE_ASSERT(fSynth.volume >= 0.0f && fSynth.volume <= 1.0f);
        break;
    }
}

// -----------------------------------------------------------------------
// Process

void DistrhoPluginNekobi::activate()
{
    fSynth.nugget_remains = 0;

    if (fSynth.voice != nullptr)
        nekobee_synth_all_voices_off(&fSynth);
}

void DistrhoPluginNekobi::deactivate()
{
    if (fSynth.voice != nullptr)
        nekobee_synth_all_voices_off(&fSynth);
}

void DistrhoPluginNekobi::run(const float **, float **outputs, uint32_t frames, const MidiEvent *midiEvents, uint32_t midiEventCount)
{
    uint32_t framesDone = 0;
    uint32_t curEventIndex = 0;
    uint32_t burstSize;

    float *out = outputs[0];

    //printf("params: cutoff %04f envmod %04f decay %04f\n", fSynth.cutoff, fSynth.envmod, fSynth.decay);

    if (fSynth.voice == nullptr)
    {
        std::memset(out, 0, sizeof(float) * frames);
        return;
    }

    while (framesDone < frames)
    {
        if (fSynth.nugget_remains == 0)
            fSynth.nugget_remains = XSYNTH_NUGGET_SIZE;

        /* process any ready events */
        for (; curEventIndex < midiEventCount && framesDone == midiEvents[curEventIndex].frame; ++curEventIndex)
        {
            if (midiEvents[curEventIndex].size > MidiEvent::kDataSize)
                continue;

            nekobee_handle_raw_event(&fSynth, midiEvents[curEventIndex].size, midiEvents[curEventIndex].data);
        }

        /* calculate the sample count (burstSize) for the next nekobee_voice_render() call to be the smallest of:
         * - control calculation quantization size (XSYNTH_NUGGET_SIZE, in samples)
         * - the number of samples remaining in an already-begun nugget (synth->nugget_remains)
         * - the number of samples until the next event is ready
         * - the number of samples left in this run
         */
        burstSize = XSYNTH_NUGGET_SIZE;

        /* we're still in the middle of a nugget, so reduce the burst size
         * to end when the nugget ends */
        if (fSynth.nugget_remains < burstSize)
            burstSize = fSynth.nugget_remains;

        /* reduce burst size to end when next event is ready */
        if (curEventIndex < midiEventCount && midiEvents[curEventIndex].frame - framesDone < burstSize)
            burstSize = midiEvents[curEventIndex].frame - framesDone;

        /* reduce burst size to end at end of this run */
        if (frames - framesDone < burstSize)
            burstSize = frames - framesDone;

        /* render the burst */
        nekobee_synth_render_voice(&fSynth, out + framesDone, burstSize, (burstSize == fSynth.nugget_remains));
        framesDone += burstSize;
        fSynth.nugget_remains -= burstSize;
    }
}

// -----------------------------------------------------------------------

Plugin *createPlugin()
{
    return new DistrhoPluginNekobi();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
