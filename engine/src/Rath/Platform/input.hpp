#pragma once

// Rath files
#include "Rath/Core/defines.hpp"
#include "window.hpp"

namespace Rath {
	class Input {
		public:
			Input(Window& _window);
			~Input();
			Input(const Input& other) = delete;
			Input& operator=(const Input& other) = delete;

			bool isKeyPressed(int key);
			bool isMousePressed(int button);

		private:
			Window& window;

	};
} // namsepace Rath