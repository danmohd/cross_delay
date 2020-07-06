#include <iostream>
#include <public.sdk/source/main/pluginfactory.h>
#include "../include/plugineditor.h"
#include "../include/pluginprocessor.h"
#include "../include/pluginids.h"

bool InitModule()
{
	std::cout << "--------->>> Factory InitModule\n";
	return true;
}

bool DeinitModule()
{
	return true;
}

BEGIN_FACTORY_DEF("Mohammed Danish",
	"https://www.github.com/danmohd",
	"mailto:mohd.dan@outlook.com")

	DEF_CLASS2(
		INLINE_UID_FROM_FUID(Steinberg::Vst::Danish::ProcessorUID),
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		"Danish Cross Delay VST3",
		Vst::kDistributable,
		"Delay",
		"1",
		kVstVersionString,
		Steinberg::Vst::Danish::CrossDelayProcessor::createInstance
	)

	DEF_CLASS2(
		INLINE_UID_FROM_FUID(Steinberg::Vst::Danish::EditorUID),
		PClassInfo::kManyInstances,
		kVstComponentControllerClass,
		"Danish Cross Delay VST3 Controller",
		0,
		"",
		"1",
		kVstVersionString,
		Steinberg::Vst::Danish::CrossDelayEditor::createInstance
	)

END_FACTORY