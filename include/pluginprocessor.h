#pragma once
#include <iostream>
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <pluginterfaces/vst/vsttypes.h>

namespace Steinberg
{
namespace Vst
{
namespace Danish
{

class CrossDelayProcessor : public AudioEffect
{
public:
    CrossDelayProcessor();

    tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
    tresult PLUGIN_API terminate() SMTG_OVERRIDE;
    tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
                                          SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;
    tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;
    tresult PLUGIN_API setupProcessing(ProcessSetup& setup) SMTG_OVERRIDE;
    tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;
    tresult PLUGIN_API setProcessing(TBool state) SMTG_OVERRIDE;
    tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE;
    uint32 PLUGIN_API getTailSamples() SMTG_OVERRIDE { return kInfiniteTail; }

    tresult PLUGIN_API setState(IBStream* stream) SMTG_OVERRIDE;
    tresult PLUGIN_API getState(IBStream* stream) SMTG_OVERRIDE;

    void PLUGIN_API processBlock(Sample32** inputs, Sample32** outputs, int32 numSamples);

    static FUnknown* createInstance(void*) { return (IAudioProcessor*)new CrossDelayProcessor(); }

private:
    const int maxDelayTimeInSeconds = 2;
    int32 delayLineSizeInSamples;

    bool bypassed;
    float delayTimeL;
    float delayTimeR;
    float feedbackL;
    float feedbackR;
    float crossFeedback;    // Issue with stability. What should be the Range?
    float mixL;
    float mixR;

    int32 delayTimeInSamplesL;
    int32 delayTimeInSamplesR;
    int32 delayLineWriteHeadL;
    int32 delayLineReadHeadL;
    int32 delayLineWriteHeadR;
    int32 delayLineReadHeadR;

    Sample32* delayBufferL;
    Sample32* delayBufferR;
};

}
}
}