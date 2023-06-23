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

