#pragma once
#include <vulkan/vulkan.h>
#include <Rath/Core/defines.hpp>

namespace RATH {
	class Context {
	public:
		Context();
		~Context();
		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

	private:
		void createInstance();
	};
} // namespace RATH