# DISTRHO Nekobi

Simple single-oscillator synth based on the Roland TB-303.

This is a DPF'ied build of [nekobee](https://github.com/gordonjcp/nekobee), allowing LV2, VST2 and VST3 builds of the plugin,<br/>
plus a nicer UI with a simple cat animation. 🐈

## Screenshot
![Nekobi](https://raw.githubusercontent.com/DISTRHO/nekobi/master/plugins/Nekobi/Screenshot.png "Nekobi")<br/>

## Building
Nekobi uses git submodules for the DPF libraries

    $ git submodule update --init --recursive # pull in dependencies
    $ make # actually build it

## Technical notes


### Controls
A large part of the "sound" of the 303 comes from the clever way its controls interact.
One of the things that the original nekobee didn't really get right was the way the controls behave.

As a big part of the cleanup, the controls have been re-implemented to work better and feed more easily into
a model of the circuitry. In particular, some of the controls use log pots (Cutoff, Env Mod, Decay, and Volume)
which will change how they behave. Almost all TDv8's controls follow the same 0-100 range except the waveform
switch (0 or 1), and tuning (-12 to +12). Internally, they are converted to an appropriate scale.

The cutoff and env mod pots is converted using an exponential function from 0 to 100 to 0 to 50 (units are in
kOhms) with the centre point of the pot (50% rotation) corresponding to 7.5kOhms (around 15% of the track value).
Similarly the volume pot scales its output from 0 to 1 (the real volume pot is 50k log), and the decay pot scales
from 0 to 1M log, again both with 50% rotation corresponding to 15% value.

This exponentiation is "expensive" so is only carried out when the pot value is set.

No special consideration is given to the linear pots (resonance and accent) but the tuning pot is scaled so its
output goes from 0.5 to 2, so it can be applied directly to the tuning word sent to the oscillator.
