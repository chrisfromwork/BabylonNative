#pragma once

#include "jsi/jsi.h"
#include "IXrContext.h"

namespace BabylonReactNative
{
    bool TryGetXrContext(facebook::jsi::Runtime& jsiRuntime, const facebook::jsi::Value& xrSession, IXrContext*& xrContext)
    {
        xrContext = nullptr;
        if (!xrSession.isObject() ||
            !xrSession.asObject(jsiRuntime).hasProperty(jsiRuntime, "nativeExtension"))
        {
            return false;
        }

        auto nativeExtensionPtr = static_cast<uintptr_t>(xrSession.asObject(jsiRuntime).getProperty(jsiRuntime, "nativeExtension").asNumber());
        xrContext = reinterpret_cast<IXrContext*>(nativeExtensionPtr);
        return true;
    }
}