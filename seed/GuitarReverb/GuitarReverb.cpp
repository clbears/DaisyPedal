#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// ReverbSc's internal buffers are large (~400KB), so it's placed in SDRAM
// rather than the much smaller internal SRAM.
static ReverbSc DSY_SDRAM_BSS verb;
static DcBlock                dcblock;

AdcChannelConfig adc_cfg;

void AudioCallback(AudioHandle::InputBuffer  in,
                    AudioHandle::OutputBuffer out,
                    size_t                    size)
{
    // Pot on pin 22 controls the reverb decay time.
    float knob = hw.adc.GetFloat(0);
    verb.SetFeedback(0.6f + knob * 0.38f); // ~0.6 (short) to ~0.98 (long)
    verb.SetLpFreq(10000.f);

    for(size_t i = 0; i < size; i++)
    {
        // Guitar in on AUDIO IN R (pin 17)
        float dry = in[1][i];

        float wetL, wetR;
        verb.Process(dry, dry, &wetL, &wetR);
        float wet = dcblock.Process(wetL);

        float mixed = dry * 0.6f + wet * 0.9f;
        out[0][i]   = mixed;
        out[1][i]   = mixed;
    }
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(4);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartLog(false); // don't block waiting for a terminal to connect

    float samplerate = hw.AudioSampleRate();

    verb.Init(samplerate);
    verb.SetFeedback(0.85f);
    verb.SetLpFreq(10000.f);

    dcblock.Init(samplerate);

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
