// Copyright (c) 2025 Thomas Groom

#pragma once

#include <glad/glad.h>
#include <cmath>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Shader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <variant>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional> // Required for std::function

#include "ui_library/Config.h"
#include "Utils.h"


class Application {
public:
    Application() : dragAndDropCursorImage({15, 25, G_DRAG_CURSOR_BITMAP}){
        mUIContext = new UI();
    }

    virtual ~Application() {
        delete mUIContext;
        
        // Clean up the cursors if they were created.
        for (int i = 0; i < 5; ++i) {
            if (mCursorLUT[i] != nullptr)
                glfwDestroyCursor(mCursorLUT[i]);
        }
    }

    struct WindowSettings {
        int width = 800;
        int height = 600;
        int minWidth = 300;
        int minHeight = 200;
        int samples = 4;         // Anti-Aliasing (MSAA) samples
        bool vsyncEnabled = true;
        bool maximizeWindow = true;
        std::string title = "Application";
    };

    void handleMouseButton(GLFWwindow* window, int button, int action, int mods);
    void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    void handleScroll(GLFWwindow* window, double xoffset, double yoffset);
    void handleCharacter(GLFWwindow* window, unsigned int codepoint);
    void handleFramebufferSize(GLFWwindow* window, int width, int height);

    virtual void onInit() {}
    virtual void onUpdate() {}
    virtual void onShutdown() {}

    void run(const WindowSettings& settings, const GLFWimage* icon = nullptr);

    void stop() { running = false; }

    void SetRunning(bool _isRunning) {
        running = _isRunning;
    }
    
    UI* mUIContext;


private:
    bool running = true;
	glm::vec2 previousMousePos = glm::vec2(0.0f, 0.0f);
    
    double lastClickTime = 0;

    GLFWimage dragAndDropCursorImage;
    GLFWcursor* mCursorLUT[5] = {};

};
    
