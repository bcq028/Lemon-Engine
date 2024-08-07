#pragma once
#include <memory>
// Platform Detection
#include "PlatformDetection.h"

#define API_INPUT_WINDOWS
#ifdef LEMON_PLATFORM_WINDOW
	#if LEMON_DYNAMIC_LINK
		#if LEMON_BUILD_DLL
			#define LEMON_API __declspec(dllexport)
		#else
			#define LEMON_API __declspec(dllimport)
		#endif
	#else
		#define LEMON_API
#endif
#else
	#error Current Application only support windows
#endif

#define BIT(x) (1 << x)
#define FORCE_INLINE inline

#ifdef LEMON_DEBUG
	#define LEMON_CORE_ASSERT(x, ...)   { if(!(x)) { LEMON_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define LEMON_CLIENT_ASSERT(x, ...) { if(!(x)) { LEMON_CLIENT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define LEMON_CORE_ASSERT(x, ...) 
	#define LEMON_CLIENT_ASSERT(x, ...)
#endif

#define Check(x) LEMON_CLIENT_ASSERT(x, "")

#define STRUCT_OFFSET( struc, member )	offsetof(struc, member)

namespace Lemon
{
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T1, typename T2>
	T1* RefCast(const Ref<T2>& object)
	{
		return static_cast<T1*>(object.get());
	}
}


