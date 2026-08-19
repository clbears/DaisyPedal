#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

// Up to 2 seconds of delay time at 48kHz.
#define MAX_DELAY static_cast<size_t>(48000 * 2.f)

DaisySeed hw;

static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delay_line;

AdcChannelConfig adc_cfg;

static const float kDelaySeconds = 0.9f; // fixed, ambient-length echo

void AudioCallback(AudioHandle::InputBuffer  in,
                    AudioHandle::OutputBuffer out,
                    size_t                    size)
{
    // Pot on pin 22 (A0) controls feedback / number of repeats.
    float knob     = hw.adc.GetFloat(0);
    float feedback = knob * 0.92f; // near-infinite at the top, but capped

    for(size_t i = 0; i < size; i++)
    {
        // Guitar in on AUDIO IN R (pin 17)
        float dry = in[1][i];

        float delayed = delay_line.Read();
        delay_line.Write(dry + delayed * feedback);

        float mixed = dry * 0.6f + delayed * 0.5f;
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

    delay_line.Init();
    delay_line.SetDelay(kDelaySeconds * samplerate);

    // GetPin() indexes into the GPIO-capable pins only, skipping the
    // audio/power pins (16-21) -- physical pin 22 (A0 / ADC_INP10) is
    // therefore index 15, not 22.
    adc_cfg.InitSingle(hw.GetPin(15));
    hw.adc.Init(&adc_cfg, 1);
    hw.adc.Start();

    hw.StartAudio(AudioCallback);

    for(;;)
    {
        hw.PrintLine("feedback: %.3f", hw.adc.GetFloat(0) * 0.92f);
        System::Delay(200);
    }
}
