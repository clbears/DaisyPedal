#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
Tremolo   trem;

AdcChannelConfig adc_cfg;

void AudioCallback(AudioHandle::InputBuffer  in,
                    AudioHandle::OutputBuffer out,
                    size_t                    size)
{
    // Pot on pin 22 (A0) controls the tremolo rate, ~0.5 - 10 Hz.
    float knob = hw.adc.GetFloat(0);
    trem.SetFreq(0.5f + knob * 9.5f);

    for(size_t i = 0; i < size; i++)
    {
        // Guitar in on AUDIO IN R (pin 17)
        float dry     = in[1][i];
        float wet     = trem.Process(dry);
        out[0][i]     = wet;
        out[1][i]     = wet;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartLog(false);

    float samplerate = hw.AudioSampleRate();

    trem.Init(samplerate);
    trem.SetWaveform(Oscillator::WAVE_SIN);
    trem.SetDepth(0.8f);

    // GetPin() indexes into the GPIO-capable pins only, skipping the
    // audio/power pins (16-21) -- physical pin 22 (A0 / ADC_INP10) is
    // therefore index 15, not 22.
    adc_cfg.InitSingle(hw.GetPin(15));
    hw.adc.Init(&adc_cfg, 1);
    hw.adc.Start();

    hw.StartAudio(AudioCallback);

    for(;;)
    {
        hw.PrintLine("pot (pin 22): %.3f", hw.adc.GetFloat(0));
        System::Delay(200);
    }
}
