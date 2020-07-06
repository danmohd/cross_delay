#pragma once
#include <public.sdk/source/vst/vsteditcontroller.h>

namespace Steinberg
{
namespace Vst
{
namespace Danish
{

class CrossDelayEditor : public EditController
{
public:
    CrossDelayEditor();

    tresult PLUGIN_API setState(IBStream* stream) SMTG_OVERRIDE;
    tresult PLUGIN_API getState(IBStream* stream) SMTG_OVERRIDE;
    tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
    tresult PLUGIN_API terminate() SMTG_OVERRIDE;

    static FUnknown* createInstance(void*) { return (IEditController*)new CrossDelayEditor(); }
};

}
}
}