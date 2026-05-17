# Microscope (WIP preview, not yet released)

### A truly microscopic software oscilloscope!

This is the repository for development of a super-minimal software oscilloscope for the demoscene, with the primary goal of absolutely minimizing filesize using sizecoding techniques.

## Important feature targets
- oscilloscope visuals
- audio output
- playing .wav files

## Stretch goals/features
- [ ] Audio oversampling
- [ ] Phosphor decay
- [ ] Configurability
- [ ] Microphone input (WASAPI?)
- [ ] Wider audio format support ([Source Reader](https://learn.microsoft.com/en-us/windows/win32/medfound/source-reader)?)

For the time being, this project will be written specifically for Windows due to core differences in APIs and sizecoding technology between Windows and Linux. However, microscope will probably work fine with Wine, since the core Windows APIs are well supported, although this is not a focus of the project.
