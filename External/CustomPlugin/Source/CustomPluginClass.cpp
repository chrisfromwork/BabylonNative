#include "CustomPluginClass.h"
#include <chrono>
#include <string>

namespace Babylon::Plugins::Internal
{
	namespace
	{
		static constexpr auto JS_CLASS_NAME = "CustomPluginClass";
		static constexpr auto JS_INSTANCE_NAME = "customPluginClass";
	}

	void CustomPluginClass::Initialize(Napi::Env env)
	{
		// Keeps the object alive in the heap while interacting with native components:
        // https://github.com/nodejs/node-addon-api/blob/master/doc/handle_scope.md
		Napi::HandleScope scope{env};

		Napi::Function func = DefineClass(
			env,
			"CustomPluginClass",
			{
				// There are a variety of different values, accessor and method definitions available
				StaticAccessor("getDateAndTime", &CustomPluginClass::GetDateAndTime, nullptr),
				InstanceAccessor("getCreationDateAndTime", &CustomPluginClass::GetCreationDateAndTime, nullptr )
			});

		// Register the class definition as the class constructor
		env.Global().Set(JS_CLASS_NAME, func);

		// Register a new instance of the class
		Napi::Object instance = func.New({});
        env.Global().Set(JS_INSTANCE_NAME, instance);
	}

	CustomPluginClass::CustomPluginClass(const Napi::CallbackInfo& info) : Napi::ObjectWrap<CustomPluginClass>{ info }
	{
		_creationDateAndTime = GetCurrentDateAndTime();
	}

	Napi::Value CustomPluginClass::GetDateAndTime(const Napi::CallbackInfo& info)
	{
		std::string currentDateAndTime = GetCurrentDateAndTime();
		return Napi::String::New(info.Env(), currentDateAndTime);
	}

	Napi::Value CustomPluginClass::GetCreationDateAndTime(const Napi::CallbackInfo& info)
	{
		return Napi::String::New(info.Env(), _creationDateAndTime);
	}

	std::string CustomPluginClass::GetCurrentDateAndTime()
	{
		auto now = std::chrono::system_clock::now();
		auto now_time = std::chrono::system_clock::to_time_t(now);

		char str[26];
		ctime_s(str, sizeof(str), &now_time);
		return std::string(str);
	}
}
