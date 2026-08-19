#include <cmath>
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

Chorus     chorus;
Oscillator wow;     // slow, uneven pitch drift, like tape speed wander
Oscillator flutter; // faster, subtler jitter on top of the wow
Svf        tape_filter;

AdcChannelConfig adc_cfg;

static const float kBaseDelayMs = 7.f;

void AudioCallback(AudioHandle::InputBuffer  in,
                    AudioHandle::OutputBuffer out,
                    size_t                    size)
{
    // Pot on pin 22 (A0) controls how "worn out" the tape is: at zero it's
    // a clean, steady chorus; turned up it drifts and wobbles more.
    float depth = hw.adc.GetFloat(0);

    for(size_t i = 0; i < size; i++)
    {
        float wow_ms     = wow.Process() * 1.6f * depth;
        float flutter_ms = flutter.Process() * 0.35f * depth;
        chorus.SetDelayMs(kBaseDelayMs + wow_ms + flutter_ms);

        // Guitar in on AUDIO IN R (pin 17)
        float dry = in[1][i];
        float wet = chorus.Process(dry);

        tape_filter.Process(wet);
        float dark = tape_filter.Low();

        // A touch of soft saturation for tape-ish warmth.
        float sat = tanhf(dark * 1.3f) / 1.3f;

        float mixed = dry * 0.3f + sat * 0.95f;
        out[0][i]   = mixed;
        out[1][i]   = mixed;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartLog(false);

    float samplerate = hw.AudioSampleRate();

    chorus.Init(samplerate);
    chorus.SetLfoFreq(0.35f);
    chorus.SetLfoDepth(0.8f);
    chorus.SetFeedback(0.25f);
    chorus.SetDelayMs(kBaseDelayMs);

    wow.Init(samplerate);
    wow.SetWaveform(Oscillator::WAVE_SIN);
    wow.SetFreq(0.15f);
    wow.SetAmp(1.f);

    flutter.Init(samplerate);
    flutter.SetWaveform(Oscillator::WAVE_SIN);
    flutter.SetFreq(6.3f);
    flutter.SetAmp(1.f);

    tape_filter.Init(samplerate);
    tape_filter.SetFreq(6500.f);
    tape_filter.SetRes(0.1f);
    tape_filter.SetDrive(0.1f);

    // GetPin() indexes into the GPIO-capable pins only, skipping the
    // audio/power pins (16-21) -- physical pin 22 (A0 / ADC_INP10) is
    // therefore index 15, not 22.
    adc_cfg.InitSingle(hw.GetPin(15));
    hw.adc.Init(&adc_cfg, 1);
    hw.adc.Start();

    hw.StartAudio(AudioCallback);

    for(;;)
    {
        hw.PrintLine("drift depth: %.3f", hw.adc.GetFloat(0));
        System::Delay(200);
    }
}
