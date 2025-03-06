#include "ui_library/Application.h"


// Static callbacks that forward to the singleton instance.
void Application::handleMouseButton(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        mUIContext->G_LEFT_MOUSE_STATE = action;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        mUIContext->G_MIDDLE_MOUSE_STATE = action;
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
        mUIContext->G_RIGHT_MOUSE_STATE = action;
}

void Application::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
        if (action == GLFW_PRESS) {
            mUIContext->G_SHIFT_PRESS = true;
            mUIContext->G_SHIFT_DOWN = true;
        } else if (action == GLFW_RELEASE) {
            mUIContext->G_SHIFT_DOWN = false;
            mUIContext->G_SHIFT_RELEASE = true;
        }
    }
    if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
        if (action == GLFW_PRESS)
            mUIContext->G_ENTER_PRESS = true;
    }
    if (key == GLFW_KEY_BACKSPACE) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            mUIContext->G_BACKSPACE_PRESS = true;
    }
    if (key == GLFW_KEY_DELETE) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            mUIContext->G_DELETE_PRESS = true;
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            mUIContext->G_LEFT_ARROW_PRESS = true;
    }
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
            mUIContext->G_RIGHT_ARROW_PRESS = true;
    }
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
        if (action == GLFW_PRESS)
            mUIContext->G_CTRL_DOWN = true;
        else if (action == GLFW_RELEASE)
            mUIContext->G_CTRL_DOWN = false;
    }
    if (key == GLFW_KEY_X) {
        if (mUIContext->G_CTRL_DOWN && action == GLFW_PRESS)
            mUIContext->G_CTRL_X_PRESS = true;
    } else if (key == GLFW_KEY_C) {
        if (mUIContext->G_CTRL_DOWN && action == GLFW_PRESS)
            mUIContext->G_CTRL_C_PRESS = true;
    } else if (key == GLFW_KEY_V) {
        if (mUIContext->G_CTRL_DOWN && action == GLFW_PRESS)
            mUIContext->G_CTRL_V_PRESS = true;
    }
    if (key == GLFW_KEY_HOME) {
        if (action == GLFW_PRESS)
            mUIContext->G_HOME_PRESS = true;
    }
    if (key == GLFW_KEY_END) {
        if (action == GLFW_PRESS)
            mUIContext->G_END_PRESS = true;
    }
}

void Application::handleScroll(GLFWwindow* window, double xoffset, double yoffset) {
    mUIContext->G_SCROLL_X = xoffset;
    mUIContext->G_SCROLL_Y = yoffset;
    mUIContext->G_SCROLL_TRIGGER = true;
}

void Application::handleCharacter(GLFWwindow* window, unsigned int codepoint) {
    mUIContext->G_CHAR_INPUT = static_cast<char>(codepoint);
    mUIContext->G_CHAR_CALLBACK_FLAG = true;
}

void Application::handleFramebufferSize(GLFWwindow* window, int width, int height)
{
	// Only if not minimized
	if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) == 0) {
		// make sure the viewport matches the new window dimensions
		mUIContext->G_WIDTH = width;
		mUIContext->G_HEIGHT = height;
		glViewport(0, 0, mUIContext->G_WIDTH, mUIContext->G_HEIGHT);
		mUIContext->G_RESIZE_FLAG = true;
		
		onUpdate();
		glfwSwapBuffers(window);
	}
}


void Application::run(const WindowSettings& settings, const GLFWimage* icon) {  // TODO: Allow user to set other settings rather than the hardcoded ones below VVV ALSO Add defaults for each
	// Use the settings provided or fallback to defaults.
    mUIContext->G_WIDTH = settings.width;
    mUIContext->G_HEIGHT = settings.height;

    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
	
    // Set window hints based on user settings.
    glfwWindowHint(GLFW_SAMPLES, settings.samples);
    glfwSwapInterval(settings.vsyncEnabled ? 1 : 0);
	
    // Get monitor information (for future use if needed)
    int monitorCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    const GLFWvidmode* mode = (monitorCount > 0) ? glfwGetVideoMode(monitors[0]) : nullptr;

    // Create the window using user-specified title and dimensions.
    G_WINDOW = glfwCreateWindow(mUIContext->G_WIDTH, mUIContext->G_HEIGHT, settings.title.c_str(), nullptr, nullptr);
    if (G_WINDOW == nullptr) {
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(G_WINDOW);

    // Optionally set the window icon if provided.
    if (icon) {
        glfwSetWindowIcon(G_WINDOW, 1, icon);
    }

    // Set window size limits and maximize if requested.
    glfwSetWindowSizeLimits(G_WINDOW, settings.minWidth, settings.minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
    if (settings.maximizeWindow) {
        glfwMaximizeWindow(G_WINDOW);
    }
    glfwGetFramebufferSize(G_WINDOW, &mUIContext->G_WIDTH, &mUIContext->G_HEIGHT); // Update UI dimensions

	
	// Error checking if window fails to create VVV
	if (G_WINDOW == NULL) {
		throw std::runtime_error("Failed to create GLFW window");

	}

	//gladLoadGL(); // Load OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    glViewport(0, 0, mUIContext->G_WIDTH, mUIContext->G_HEIGHT);
    glEnable(GL_MULTISAMPLE); // Enable MSAA
    glEnable(GL_SCISSOR_TEST);

    mCursorLUT[0] = nullptr;
    mCursorLUT[1] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    mCursorLUT[2] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    mCursorLUT[3] = glfwCreateCursor(&dragAndDropCursorImage, 0, 0);
    mCursorLUT[4] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);

    // Store this Application instance so that the callbacks can retrieve it.
    glfwSetWindowUserPointer(G_WINDOW, this);

    auto framebufferResizeCallback = [](GLFWwindow* window, int newWidth, int newHeight) {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->handleFramebufferSize(window, newWidth, newHeight);
    };
    auto mouseButtonCallback = [](GLFWwindow* window, int button, int action, int mods) {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->handleMouseButton(window, button, action, mods);
    };
    auto keyInputCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->handleKey(window, key, scancode, action, mods);
    };
    auto scrollInputCallback = [](GLFWwindow* window, double xOffset, double yOffset) {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->handleScroll(window, xOffset, yOffset);
    };
    auto characterInputCallback = [](GLFWwindow* window, unsigned int codepoint) {
        static_cast<Application*>(glfwGetWindowUserPointer(window))->handleCharacter(window, codepoint);
    };

    // Set GLFW callbacks.
    glfwSetFramebufferSizeCallback(G_WINDOW, framebufferResizeCallback);
    glfwSetMouseButtonCallback(G_WINDOW, mouseButtonCallback);
    glfwSetKeyCallback(G_WINDOW, keyInputCallback);
    glfwSetScrollCallback(G_WINDOW, scrollInputCallback);
    glfwSetCharCallback(G_WINDOW, characterInputCallback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Failed to initialize GLAD");
	}
	
    onInit();

    while (running) {

		// If the application is not minimized
		if (glfwGetWindowAttrib(G_WINDOW, GLFW_ICONIFIED) != 0) {
			glfwWaitEvents();
			return;
		}

		glfwGetCursorPos(G_WINDOW, &mUIContext->G_MOUSE_X, &mUIContext->G_MOUSE_Y);
		
		glm::vec2 currentMousePos(mUIContext->G_MOUSE_X, mUIContext->G_MOUSE_Y);
		glm::vec2 mouseDelta = currentMousePos - previousMousePos;

		if (glfwGetMouseButton(G_WINDOW, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && (mouseDelta.x != 0 || mouseDelta.y != 0)) {
			mUIContext->G_LEFT_MOUSE_DRAG = true;
		}

		if (mUIContext->G_LEFT_MOUSE_STATE == GLFW_PRESS) {
			double currentTime = glfwGetTime();  // Get the current time in seconds
			mUIContext->G_MOUSE_DOUBLE_CLICK = (currentTime - lastClickTime <= DOUBLE_CLICK_THRESHOLD);
			lastClickTime = currentTime;  // Store the time of the last click
		}

		if (glfwGetMouseButton(G_WINDOW, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && (mouseDelta.x != 0 || mouseDelta.y != 0)) {
			mUIContext->G_MIDDLE_MOUSE_DRAG = true;
		}

		if (mUIContext->G_MIDDLE_MOUSE_STATE == GLFW_RELEASE) {
			mUIContext->G_MIDDLE_MOUSE_DRAG = false;
		}

		if (mUIContext->G_LEFT_MOUSE_STATE == GLFW_PRESS || mUIContext->G_MIDDLE_MOUSE_STATE == GLFW_PRESS) {
			mUIContext->G_MOUSE_DRAG_START_X = static_cast<int>(mUIContext->G_MOUSE_X);
			mUIContext->G_MOUSE_DRAG_START_Y = static_cast<int>(mUIContext->G_MOUSE_Y);
			mUIContext->G_MOUSE_DRAG_END_X = static_cast<int>(mUIContext->G_MOUSE_X);
			mUIContext->G_MOUSE_DRAG_END_Y = static_cast<int>(mUIContext->G_MOUSE_Y);
		}

		if (mUIContext->G_LEFT_MOUSE_STATE == GLFW_RELEASE || mUIContext->G_MIDDLE_MOUSE_STATE == GLFW_RELEASE) {
			mUIContext->G_MOUSE_DRAG_END_X = static_cast<int>(mUIContext->G_MOUSE_X);
			mUIContext->G_MOUSE_DRAG_END_Y = static_cast<int>(mUIContext->G_MOUSE_Y);
		}
		
		glfwSetInputMode(G_WINDOW, GLFW_CURSOR, mUIContext->G_SET_CURSOR_MODE);
		mUIContext->G_SET_CURSOR_MODE = GLFW_CURSOR_NORMAL;
		mUIContext->G_MOUSE_DRAG_DELTA = mouseDelta;
		previousMousePos = currentMousePos;

		MouseInputSingleton::getInstance().grantMouseInput(mUIContext);

        onUpdate();
		
		glfwSetCursor(G_WINDOW, mCursorLUT[mUIContext->G_SET_CURSOR]);

		if (mUIContext->G_LEFT_MOUSE_STATE == GLFW_RELEASE) {
			mUIContext->G_LEFT_MOUSE_DRAG = false;
		}

		// Reset states and flags
		mUIContext->G_LEFT_MOUSE_STATE = -1;
		mUIContext->G_MIDDLE_MOUSE_STATE = -1;
		mUIContext->G_RIGHT_MOUSE_STATE = -1;
		mUIContext->G_RESIZE_FLAG = false;
		mUIContext->G_SET_CURSOR = CURSOR_NORMAL;
		mUIContext->G_ENTER_PRESS = false;
		mUIContext->G_BACKSPACE_PRESS = false;
		mUIContext->G_DELETE_PRESS = false;
		mUIContext->G_LEFT_ARROW_PRESS = false;
		mUIContext->G_RIGHT_ARROW_PRESS = false;
		mUIContext->G_SHIFT_PRESS = false;
		mUIContext->G_SHIFT_RELEASE = false;
		mUIContext->G_CHAR_CALLBACK_FLAG = false;
		mUIContext->G_SCROLL_TRIGGER = false;
		mUIContext->G_HOME_PRESS = false;
		mUIContext->G_END_PRESS = false;
		mUIContext->G_CTRL_X_PRESS = false;
		mUIContext->G_CTRL_C_PRESS = false;
		mUIContext->G_CTRL_V_PRESS = false;

		// Swap front and back buffers to see the pixels
		glfwSwapBuffers(G_WINDOW);
		glfwPollEvents();
    }

    onShutdown();

	glfwDestroyWindow(G_WINDOW);
	// Terminate GLFW (crashes with Linux NVidia drivers) [ ] TODO: Test if this crashes in linux
	#if defined(_WIN32)
		glfwTerminate();
	#endif
}