#pragma once

#include <napi/napi.h>

namespace Babylon::Plugins::Internal
{
    class CustomPluginClass : public Napi::ObjectWrap<CustomPluginClass>
    {
    public:
        static void Initialize(Napi::Env env);

        // Without a constructor you will encounter non-intuitive errors
        explicit CustomPluginClass(const Napi::CallbackInfo& info);

    private:
        static Napi::Value GetDateAndTime(const Napi::CallbackInfo& info);
        Napi::Value GetCreationDateAndTime(const Napi::CallbackInfo& info);

        static std::string GetCurrentDateAndTime();
        std::string _creationDateAndTime;
    };
}