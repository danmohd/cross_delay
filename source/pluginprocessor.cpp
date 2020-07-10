#include <algorithm>
#include "../include/pluginprocessor.h"
#include "../include/pluginids.h"
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <base/source/fstreamer.h>

namespace Steinberg
{
namespace Vst
{
namespace Danish
{

CrossDelayProcessor::CrossDelayProcessor()
:
delayLineSizeInSamples(0),
bypassed(false),
delayTimeL(0.0),
delayTimeR(0.0),
feedbackL(0.0),
feedbackR(0.0),
crossFeedback(0.0),
mixL(0.0),
mixR(0.0),
delayTimeInSamplesL(0.0),
delayTimeInSamplesR(0.0),
delayLineWriteHeadL(0),
delayLineReadHeadL(0),
delayLineWriteHeadR(0),
delayLineReadHeadR(0),
delayBufferL(nullptr),
delayBufferR(nullptr)
{
    setControllerClass(EditorUID);
}

tresult PLUGIN_API CrossDelayProcessor::initialize(FUnknown* context)
{
    tresult result = AudioEffect::initialize(context);
    if (result == kResultFalse) return kResultFalse;

    bypassed = false;
    delayTimeL = 0.5;
    delayTimeR = 0.5;
    feedbackL = 0.5;
    feedbackR = 0.5;
    crossFeedback = 0.0;
    mixL = 0.5;
    mixR = 0.5;

    removeAudioBusses();
    addAudioInput(STR16("Stereo Input"), SpeakerArr::kStereo, kMain);
    addAudioOutput(STR16("Stereo Output"), SpeakerArr::kStereo, kMain);

    return kResultTrue;
}

tresult PLUGIN_API CrossDelayProcessor::terminate()
{
    tresult result = AudioEffect::terminate();
    return result;
}

tresult PLUGIN_API CrossDelayProcessor::setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
                                                           SpeakerArrangement* outputs, int32 numOuts)
{
    if (numIns == 1 && numOuts == 1)
    {
        if (inputs[0] == SpeakerArr::kStereo && outputs[0] == SpeakerArr::kStereo)
        {
            removeAudioBusses();
            addAudioInput(STR16("Stereo Input"), SpeakerArr::kStereo, kMain);
            addAudioOutput(STR16("Stereo Output"), SpeakerArr::kStereo, kMain);
            return kResultTrue;
        }
        else
        {
            removeAudioBusses();
            addAudioInput(STR16("Stereo Input"), SpeakerArr::kStereo, kMain);
            addAudioOutput(STR16("Stereo Output"), SpeakerArr::kStereo, kMain);
            return kResultFalse;
        }
    }

    removeAudioBusses();
    addAudioInput(STR16("Stereo Input"), SpeakerArr::kStereo, kMain);
    addAudioOutput(STR16("Stereo Output"), SpeakerArr::kStereo, kMain);
    return kResultFalse;
}


tresult PLUGIN_API CrossDelayProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
    if (symbolicSampleSize == kSample32) return kResultTrue;
    if (symbolicSampleSize == kSample64) return kResultFalse;
    return kResultFalse;
}

tresult PLUGIN_API CrossDelayProcessor::setupProcessing(ProcessSetup& setup)
{
    processSetup = setup;
    delayLineSizeInSamples = processSetup.sampleRate * maxDelayTimeInSeconds;
    if (canProcessSampleSize(processSetup.symbolicSampleSize) != kResultTrue) return kResultFalse;

    delayTimeInSamplesL = static_cast<int32>(delayTimeL * delayLineSizeInSamples);
    delayTimeInSamplesR = static_cast<int32>(delayTimeR * delayLineSizeInSamples);
    delayLineWriteHeadL = 0;
    delayLineWriteHeadR = 0;
    delayLineReadHeadL = (delayLineWriteHeadL - delayTimeInSamplesL + delayLineSizeInSamples) % delayLineSizeInSamples;
    delayLineReadHeadR = (delayLineWriteHeadR - delayTimeInSamplesR + delayLineSizeInSamples) % delayLineSizeInSamples;

    return kResultTrue;
}

tresult PLUGIN_API CrossDelayProcessor::setActive(TBool state)
{
    if (state)
    {
        delayBufferL = (Sample32*)malloc(delayLineSizeInSamples * sizeof(Sample32));
        delayBufferR = (Sample32*)malloc(delayLineSizeInSamples * sizeof(Sample32));
        memset(delayBufferL, 0, delayLineSizeInSamples * sizeof(Sample32));
        memset(delayBufferR, 0, delayLineSizeInSamples * sizeof(Sample32));
    }
    else
    {
        free(delayBufferL);
        delayBufferL = nullptr;
        free(delayBufferR);
        delayBufferR = nullptr;
    }
    
    return kResultTrue;
}

tresult PLUGIN_API CrossDelayProcessor::process(ProcessData& data)
{
    IParameterChanges* inputParameterChanges = data.inputParameterChanges;
    if (inputParameterChanges)
    {
        int32 numParamsChanged = inputParameterChanges->getParameterCount();
        for (int32 index = 0; index < numParamsChanged; ++index)
        {
            IParamValueQueue* paramQueue = inputParameterChanges->getParameterData(index);
            if (paramQueue)
            {
                ParamID paramId {paramQueue->getParameterId()};
                int32 numPoints {paramQueue->getPointCount()};
                ParamValue value {0.0};
                int32 sampleOffset {0};

                switch (paramId)
                {
                    case kBypass:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                        bypassed = value > 0.5;
                    break;

                    case kDelayTimeL:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                    {
                        delayTimeL = value;
                        delayTimeInSamplesL = std::max<int32>(static_cast<int32>(delayLineSizeInSamples * value), 1);
                        delayLineReadHeadL = (delayLineWriteHeadL - delayTimeInSamplesL + delayLineSizeInSamples) % delayLineSizeInSamples;
                    }
                    break;

                    case kDelayTimeR:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                    {
                        delayTimeR = value;
                        delayTimeInSamplesR = std::max<int32>(static_cast<int32>(delayLineSizeInSamples * value), 1);
                        delayLineReadHeadR = (delayLineWriteHeadR - delayTimeInSamplesR + delayLineSizeInSamples) % delayLineSizeInSamples;
                    }
                    break;

                    case kFeedbackL:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                        feedbackL = value;
                    break;

                    case kFeedbackR:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                        feedbackR = value;
                    break;

                    case kCrossFeedback:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                        crossFeedback = value;
                    break;

                    case kMixL:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                        mixL = value;
                    break;

                    case kMixR:
                    if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                        mixR = value;
                    break;
                }
            }
        }
    }

    int32 numIns = data.numInputs;
    int32 numOuts = data.numOutputs;
    
    if (numIns == 0 || numOuts == 0) return kResultTrue;

    Sample32** inputs = data.inputs[0].channelBuffers32;
    int32 numChannels = data.inputs[0].numChannels;
    Sample32** outputs = data.outputs[0].channelBuffers32;
    int32 numSamples = data.numSamples;

    if (data.inputs[0].silenceFlags > 0)
    {
        data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;
        for (int32 channel = 0; channel < numChannels; ++channel)
            if ((data.outputs[0].silenceFlags & (1 << channel)) > 0) memset(outputs[channel], 0, numSamples * sizeof(Sample32));

        return kResultTrue;
    }

    data.outputs[0].silenceFlags = 0;

    if (bypassed)
    {
        for (int32 channel = 0; channel < numChannels; ++channel)
            memcpy(outputs[channel], inputs[channel], numSamples * sizeof(Sample32));
    }
    else
    {
        processBlock(inputs, outputs, numSamples);
    }

    return kResultTrue;
}

tresult PLUGIN_API CrossDelayProcessor::setState(IBStream* stream)
{
    IBStreamer streamer {stream};

    bool savedBypassed {false};
    if (!streamer.readBool(savedBypassed)) return kResultFalse;
    bypassed = savedBypassed;

    float savedDelayTimeL {0};
    if (!streamer.readFloat(savedDelayTimeL)) return kResultFalse;
    delayTimeL = savedDelayTimeL;

    float savedDelayTimeR {0};
    if (!streamer.readFloat(savedDelayTimeR)) return kResultFalse;
    delayTimeR = savedDelayTimeR;

    float savedFeedbackL {0};
    if (!streamer.readFloat(savedFeedbackL)) return kResultFalse;
    feedbackL = savedFeedbackL;

    float savedFeedbackR {0};
    if (!streamer.readFloat(savedFeedbackR)) return kResultFalse;
    feedbackR = savedFeedbackR;

    float savedCrossFeedback {0};
    if (!streamer.readFloat(savedCrossFeedback)) return kResultFalse;
    crossFeedback = savedCrossFeedback;

    float savedMixL {0};
    if (!streamer.readFloat(savedMixL)) return kResultFalse;
    mixL = savedMixL;

    float savedMixR {0};
    if (!streamer.readFloat(savedMixR)) return kResultFalse;
    mixR = savedMixR;

    int32 savedDelayLineWriteHeadL {0};
    if (!streamer.readInt32(savedDelayLineWriteHeadL)) return kResultFalse;
    delayLineWriteHeadL = savedDelayLineWriteHeadL;
    
    int32 savedDelayLineReadHeadL {0};
    if (!streamer.readInt32(savedDelayLineReadHeadL)) return kResultFalse;
    delayLineReadHeadL = savedDelayLineReadHeadL;

    int32 savedDelayLineWriteHeadR {0};
    if (!streamer.readInt32(savedDelayLineWriteHeadR)) return kResultFalse;
    delayLineWriteHeadR = savedDelayLineWriteHeadR;

    int32 savedDelayLineReadHeadR {0};
    if (!streamer.readInt32(savedDelayLineReadHeadR)) return kResultFalse;
    delayLineReadHeadR = savedDelayLineReadHeadR;

    int32 savedDelayTimeInSamplesL {0};
    if (!streamer.readInt32(savedDelayTimeInSamplesL)) return kResultFalse;
    delayTimeInSamplesL = savedDelayTimeInSamplesL;

    int32 savedDelayTimeInSamplesR {0};
    if (!streamer.readInt32(savedDelayTimeInSamplesR)) return kResultFalse;
    delayTimeInSamplesR = savedDelayTimeInSamplesR;

    return kResultTrue;
}

tresult PLUGIN_API CrossDelayProcessor::getState(IBStream* stream)
{
    IBStreamer streamer {stream};

    streamer.writeBool(bypassed);
    streamer.writeFloat(delayTimeL);
    streamer.writeFloat(delayTimeR);
    streamer.writeFloat(feedbackL);
    streamer.writeFloat(feedbackR);
    streamer.writeFloat(crossFeedback);
    streamer.writeFloat(mixL);
    streamer.writeFloat(mixR);
    streamer.writeInt32(delayTimeInSamplesL);
    streamer.writeInt32(delayTimeInSamplesR);
    streamer.writeInt32(delayLineWriteHeadL);
    streamer.writeInt32(delayLineReadHeadL);
    streamer.writeInt32(delayLineWriteHeadR);
    streamer.writeInt32(delayLineReadHeadR);

    return kResultTrue;
}

void PLUGIN_API CrossDelayProcessor::processBlock(Sample32** inputs, Sample32** outputs, int32 numSamples)
{
    Sample32* inputL = inputs[0];
    Sample32* inputR = inputs[1];

    Sample32* outputL = outputs[0];
    Sample32* outputR = outputs[1];

    for (int32 sample = 0; sample < numSamples; ++sample)
    {
        Sample32 delayedL = delayBufferL[delayLineReadHeadL];
        Sample32 delayedR = delayBufferR[delayLineReadHeadR];

        outputL[sample] = (1.0f - mixL) * inputL[sample] + mixL * delayedL;
        outputR[sample] = (1.0f - mixR) * inputR[sample] + mixR * delayedR;

        delayBufferL[delayLineWriteHeadL] = inputL[sample] + feedbackL * delayedL + crossFeedback * delayedR;
        delayBufferR[delayLineWriteHeadR] = inputR[sample] + feedbackR * delayedR + crossFeedback * delayedL;

        delayLineReadHeadL = (delayLineReadHeadL + 1) % delayLineSizeInSamples;
        delayLineReadHeadR = (delayLineReadHeadR + 1) % delayLineSizeInSamples;
        delayLineWriteHeadL = (delayLineWriteHeadL + 1) % delayLineSizeInSamples;
        delayLineWriteHeadR = (delayLineWriteHeadR + 1) % delayLineSizeInSamples;
    }
}

}
}
}