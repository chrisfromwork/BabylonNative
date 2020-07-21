#include "../Include/Babylon/Plugins/CustomPlugin.h"
#include "CustomPluginClass.h"

namespace Babylon::Plugins::CustomPlugin
{
	void Initialize(Napi::Env env)
    {
		// Initialize classes for their javascript counterparts here
		Babylon::Plugins::Internal::CustomPluginClass::Initialize(env);
    }
}
