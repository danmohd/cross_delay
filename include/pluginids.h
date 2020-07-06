#pragma once
#include <pluginterfaces/vst/vsttypes.h>

namespace Steinberg
{
namespace Vst
{
namespace Danish
{

enum ParamIDs : ParamID
{
    kBypass,
    kDelayTimeL,
    kDelayTimeR,
    kFeedbackL,
    kFeedbackR,
    kCrossFeedback,
    kMixL,
    kMixR
};

static const FUID ProcessorUID(0x4AD67B3B, 0x129A45AD, 0xBC42C1D9, 0x6294595A);
static const FUID EditorUID(0xED8D9499, 0x57D845C5, 0x93A8B5B6, 0xE2A0E74B);

}
}
}