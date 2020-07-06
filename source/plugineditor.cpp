#include "../include/plugineditor.h"
#include "../include/pluginids.h"
#include <public.sdk/source/vst/vstparameters.h>
#include <base/source/fstreamer.h>

namespace Steinberg
{
namespace Vst
{
namespace Danish
{

CrossDelayEditor::CrossDelayEditor()
{

}

tresult PLUGIN_API CrossDelayEditor::initialize(FUnknown* context)
{
    tresult result = EditController::initialize(context);
    if (result == kResultFalse) return kResultFalse;

    parameters.addParameter(STR16("Bypass"), nullptr, 1, 0, ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass, kBypass);
    parameters.addParameter(new RangeParameter(STR16("Delay Time L"), kDelayTimeL, STR16("ms"), 0.0, 2000.0, 1000.0, 0, ParameterInfo::kCanAutomate));
    parameters.addParameter(new RangeParameter(STR16("Delay Time R"), kDelayTimeR, STR16("ms"), 0.0, 2000.0, 1000.0, 0, ParameterInfo::kCanAutomate));
    parameters.addParameter(new RangeParameter(STR16("Feedback L"), kFeedbackL, STR16("%"), 0.0, 100.0, 50.0, 0, ParameterInfo::kCanAutomate));
    parameters.addParameter(new RangeParameter(STR16("Feedback R"), kFeedbackR, STR16("%"), 0.0, 100.0, 50.0, 0, ParameterInfo::kCanAutomate));
    parameters.addParameter(new RangeParameter(STR16("Cross Feedback"), kCrossFeedback, STR16("%"), 0.0, 100.0, 0.0, 0, ParameterInfo::kCanAutomate));
    parameters.addParameter(new RangeParameter(STR16("Mix L"), kMixL, STR16("%"), 0.0, 100.0, 50.0, 0, ParameterInfo::kCanAutomate));
    parameters.addParameter(new RangeParameter(STR16("Mix R"), kMixR, STR16("%"), 0.0, 100.0, 50.0, 0, ParameterInfo::kCanAutomate));

    return kResultTrue;
}

tresult PLUGIN_API CrossDelayEditor::terminate()
{
    tresult result = EditController::terminate();
    return result;
}

tresult PLUGIN_API CrossDelayEditor::setState(IBStream* stream)
{
    IBStreamer streamer {stream};

    bool savedBypassed {false};
    if (!streamer.readBool(savedBypassed)) return kResultFalse;
    setParamNormalized(kBypass, savedBypassed ? 1.0 : 0.0);

    float savedDelayTimeL{ 0.0 };
    if (!streamer.readFloat(savedDelayTimeL)) return kResultFalse;
    setParamNormalized(kDelayTimeL, savedDelayTimeL);

    float savedDelayTimeR{ 0.0 };
    if (!streamer.readFloat(savedDelayTimeR)) return kResultFalse;
    setParamNormalized(kDelayTimeR, savedDelayTimeR);

    float savedFeedbackL {0};
    if (!streamer.readFloat(savedFeedbackL)) return kResultFalse;
    setParamNormalized(kFeedbackL, savedFeedbackL);

    float savedFeedbackR {0};
    if (!streamer.readFloat(savedFeedbackR)) return kResultFalse;
    setParamNormalized(kFeedbackR, savedFeedbackR);

    float savedCrossFeedback {0};
    if (!streamer.readFloat(savedCrossFeedback)) return kResultFalse;
    setParamNormalized(kCrossFeedback, savedCrossFeedback);

    float savedMixL {0};
    if (!streamer.readFloat(savedMixL)) return kResultFalse;
    setParamNormalized(kMixL, savedMixL);

    float savedMixR {0};
    if (!streamer.readFloat(savedMixR)) return kResultFalse;
    setParamNormalized(kMixR, savedMixR);

    return kResultTrue;
}

tresult PLUGIN_API CrossDelayEditor::getState(IBStream* stream)
{
    IBStreamer streamer {stream};

    streamer.writeBool(getParamNormalized(kBypass) > 0.5);
    streamer.writeFloat(getParamNormalized(kDelayTimeL));
    streamer.writeFloat(getParamNormalized(kDelayTimeR));
    streamer.writeFloat(getParamNormalized(kFeedbackL));
    streamer.writeFloat(getParamNormalized(kFeedbackR));
    streamer.writeFloat(getParamNormalized(kCrossFeedback));
    streamer.writeFloat(getParamNormalized(kMixL));
    streamer.writeFloat(getParamNormalized(kMixR));

    return kResultTrue;
}

}
}
}