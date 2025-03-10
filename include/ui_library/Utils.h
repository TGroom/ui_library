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
#include <functional>

#include "ui_library/Config.h"


#define _USE_MATH_DEFINES

#define HORIZONTAL false
#define VERTICAL true

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#define RAD_1     M_PI / 180.0
#define RAD_90    M_PI_2
#define RAD_180   M_PI
#define RAD_270   M_PI * 1.5
#define RAD_360   M_PI * 2.0

#define DOUBLE_CLICK_THRESHOLD 0.2f

#define CURSOR_NORMAL 0
#define CURSOR_HRESIZE 1
#define CURSOR_VRESIZE 2


extern GLFWwindow* G_WINDOW;

extern unsigned char G_DRAG_CURSOR_BITMAP[15*25*4];


class UI {
public:
    UI() {};
    ~UI() {};
	int G_WIDTH = 800;
	int G_HEIGHT = 800;
	double G_MOUSE_X = 0;
	double G_MOUSE_Y = 0;
	double G_SCROLL_X = 0;
	double G_SCROLL_Y = 0;
	bool G_SCROLL_TRIGGER = false;
	bool G_MOUSE_DOUBLE_CLICK = false;
	bool G_RESIZE_FLAG = false;
	int G_LEFT_MOUSE_STATE = -1;
	int G_MIDDLE_MOUSE_STATE = -1;
	int G_RIGHT_MOUSE_STATE = -1;
	bool G_LEFT_MOUSE_DRAG = false;
	int G_MOUSE_DRAG_START_X = -1;
	int G_MOUSE_DRAG_START_Y = -1;
	int G_MOUSE_DRAG_END_X = -1;
	int G_MOUSE_DRAG_END_Y = -1;
	bool G_MIDDLE_MOUSE_DRAG = false;
	glm::vec2 G_MOUSE_DRAG_DELTA = glm::vec2(0.0f, 0.0f);
	int G_SET_CURSOR = 0;
	int G_SET_CURSOR_MODE = GLFW_CURSOR_NORMAL;

	bool G_SHIFT_DOWN = false;      // Keys
	bool G_ENTER_PRESS = false;
	bool G_BACKSPACE_PRESS = false;
	bool G_DELETE_PRESS = false;
	bool G_LEFT_ARROW_PRESS = false;
	bool G_RIGHT_ARROW_PRESS = false;
	bool G_SHIFT_PRESS = false;
	bool G_SHIFT_RELEASE = false;
	bool G_HOME_PRESS = false;
	bool G_END_PRESS = false;
	bool G_CTRL_DOWN = false;
	bool G_CTRL_X_PRESS = false;
	bool G_CTRL_C_PRESS = false;
	bool G_CTRL_V_PRESS = false;
	char G_CHAR_INPUT = 0;
	bool G_CHAR_CALLBACK_FLAG = false;
};


std::string wstring_to_utf8(const std::wstring& wstr);

std::wstring utf8_to_wstring(const std::string& str);

std::wstring StringToWString(const std::string& str);

std::string WStringToString(const std::wstring& wstr);



struct Shape {
	std::vector<GLfloat> verts;
	std::vector<GLuint> inds;
};

struct Colour {
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;

    // Default constructor with 3 arguments
    Colour(GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}

    // Single value constructor, sets all components to the same value
    Colour(GLfloat value) : r(value), g(value), b(value), a(1.0f) {}

    Colour(Colour c, GLfloat value) : r(c.r), g(c.g), b(c.b), a(value) {}

	// Division operator
    Colour operator/(GLfloat value) const {
        return Colour(r / value, g / value, b / value, a / value);
    }

    // Multiplication operator
    Colour operator*(GLfloat value) const {
        return Colour(r * value, g * value, b * value, a * value);
    }

    // Addition operator
    Colour operator+(const Colour& other) const {
        return Colour(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    // Subtraction operator
    Colour operator-(const Colour& other) const {
        return Colour(r - other.r, g - other.g, b - other.b, a - other.a);
    }
};


class ColourParser {
public:
    // Converts a CSS colour string (any supported format) to a Colour.
    static Colour fromString(const std::string& str) {
        // A few named colours. Expand this as needed.
        static const std::unordered_map<std::string, Colour> namedColours = {
            {"black",   {0.0f, 0.0f, 0.0f}},
            {"silver",  {0.75f, 0.75f, 0.75f}},
            {"gray",    {0.5f, 0.5f, 0.5f}},
            {"white",   {1.0f, 1.0f, 1.0f}},
            {"maroon",  {0.5f, 0.0f, 0.0f}},
            {"red",     {1.0f, 0.0f, 0.0f}},
            {"purple",  {0.5f, 0.0f, 0.5f}},
            {"fuchsia", {1.0f, 0.0f, 1.0f}},
            {"green",   {0.0f, 0.5f, 0.0f}},
            {"lime",    {0.0f, 1.0f, 0.0f}},
            {"olive",   {0.5f, 0.5f, 0.0f}},
            {"yellow",  {1.0f, 1.0f, 0.0f}},
            {"navy",    {0.0f, 0.0f, 0.5f}},
            {"blue",    {0.0f, 0.0f, 1.0f}},
            {"teal",    {0.0f, 0.5f, 0.5f}},
            {"aqua",    {0.0f, 1.0f, 1.0f}},
            // TODO: Add extended CSS colors
        };

        if(str.empty()) return {1.0f, 1.0f, 1.0f};
        // Work in lower-case for named colours and function names.
        std::string lower = toLower(str);
        auto it = namedColours.find(lower);
        if (it != namedColours.end()) return it->second;

        // Dispatch based on the format.
        if (!str.empty() && str[0] == '#')
            return parseHex(str);
        if (startsWith(lower, "rgba("))
            return parseRGBA(lower);
        if (startsWith(lower, "rgb("))
            return parseRGB(lower);
        if (startsWith(lower, "hsla("))
            return parseHSLA(lower);
        if (startsWith(lower, "hsl("))
            return parseHSL(lower);
        
        std::cout << "Invalid colour format: " << str << std::endl;
        //throw std::invalid_argument("Invalid colour format: " + str);
        return Colour(1.0f);
    }

private:
    // --- Utility functions ---
    
    // Remove leading/trailing whitespace.
    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }
    
    // Convert a string to all lower-case.
    static std::string toLower(const std::string & s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Checks if string s starts with prefix.
    static bool startsWith(const std::string & s, const std::string & prefix) {
        return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
    }
    
    // Splits string s on delimiter.
    static std::vector<std::string> split(const std::string & s, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(s);
        std::string token;
        while (std::getline(ss, token, delimiter))
            tokens.push_back(trim(token));
        return tokens;
    }
    
    // --- Parsers for specific formats ---
    
    // Parses hex format: #RRGGBB or #RRGGBBAA.
    static Colour parseHex(const std::string & str) {
        unsigned int r, g, b, a = 255;
        if (str.length() == 7) { // #RRGGBB
            if (std::sscanf(str.c_str(), "#%02x%02x%02x", &r, &g, &b) != 3)
                throw std::invalid_argument("Invalid hex format: " + str);
        } else if (str.length() == 9) { // #RRGGBBAA
            if (std::sscanf(str.c_str(), "#%02x%02x%02x%02x", &r, &g, &b, &a) != 4)
                throw std::invalid_argument("Invalid hex format: " + str);
        } else {
            throw std::invalid_argument("Invalid hex format: " + str);
        }
        return Colour(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
    
    // Parses rgb(...), where components can be integer (0–255) or percentage.
    static Colour parseRGB(const std::string & str) {
        size_t start = str.find('(');
        size_t end = str.find(')');
        if (start == std::string::npos || end == std::string::npos)
            throw std::invalid_argument("Invalid rgb() format: " + str);
        std::string inside = str.substr(start + 1, end - start - 1);
        auto tokens = split(inside, ',');
        if (tokens.size() != 3)
            throw std::invalid_argument("rgb() requires 3 components: " + str);
        float r = parseComponent(tokens[0], false);
        float g = parseComponent(tokens[1], false);
        float b = parseComponent(tokens[2], false);
        return Colour(r, g, b);
    }
    
    // Parses rgba(...), where r,g,b can be int (0–255) or percentage and alpha may be float (0–1) or percentage.
    static Colour parseRGBA(const std::string & str) {
        size_t start = str.find('(');
        size_t end = str.find(')');
        if (start == std::string::npos || end == std::string::npos)
            throw std::invalid_argument("Invalid rgba() format: " + str);
        std::string inside = str.substr(start + 1, end - start - 1);
        auto tokens = split(inside, ',');
        if (tokens.size() != 4)
            throw std::invalid_argument("rgba() requires 4 components: " + str);
        float r = parseComponent(tokens[0], false);
        float g = parseComponent(tokens[1], false);
        float b = parseComponent(tokens[2], false);
        float a = parseComponent(tokens[3], true);
        return Colour(r, g, b, a);
    }
    
    // Parses hsl(...). Expects hue (in degrees), saturation and lightness (percentages or normalized).
    static Colour parseHSL(const std::string & str) {
        size_t start = str.find('(');
        size_t end = str.find(')');
        if (start == std::string::npos || end == std::string::npos)
            throw std::invalid_argument("Invalid hsl() format: " + str);
        std::string inside = str.substr(start + 1, end - start - 1);
        auto tokens = split(inside, ',');
        if (tokens.size() != 3)
            throw std::invalid_argument("hsl() requires 3 components: " + str);
        float h = std::stof(tokens[0]);
        float s = parsePercent(tokens[1]);
        float l = parsePercent(tokens[2]);
        return hslToRGB(h, s, l, 1.0f);
    }
    
    // Parses hsla(...).
    static Colour parseHSLA(const std::string & str) {
        size_t start = str.find('(');
        size_t end = str.find(')');
        if (start == std::string::npos || end == std::string::npos)
            throw std::invalid_argument("Invalid hsla() format: " + str);
        std::string inside = str.substr(start + 1, end - start - 1);
        auto tokens = split(inside, ',');
        if (tokens.size() != 4)
            throw std::invalid_argument("hsla() requires 4 components: " + str);
        float h = std::stof(tokens[0]);
        float s = parsePercent(tokens[1]);
        float l = parsePercent(tokens[2]);
        float a = parseComponent(tokens[3], true);
        return hslToRGB(h, s, l, a);
    }
    
    // --- Component Parsing Helpers ---
    
    // For RGB channels, if a token ends with '%', the value is assumed to be a percentage (100% => 1.0);
    // otherwise, the number is assumed to be in 0–255 (divided by 255). For alpha, if no '%' the value is taken as is.
    static float parseComponent(const std::string & token, bool isAlpha) {
        std::string t = trim(token);
        bool percentage = false;
        if (!t.empty() && t.back() == '%') {
            percentage = true;
            t.pop_back(); // Remove '%'
        }
        float value = std::stof(t);
        if (percentage) {
            return value / 100.0f;
        } else {
            if (!isAlpha) {
                // For rgb components, assume value is in 0–255.
                return value / 255.0f;
            } else {
                // For alpha, assume value is already normalized (0–1).
                return value;
            }
        }
    }
    
    // For HSL saturation and lightness. They are usually percentages (with '%' indicating 100% => 1.0),
    // but if no '%' is present, we assume the value is already normalized.
    static float parsePercent(const std::string & token) {
        std::string t = trim(token);
        bool percentage = false;
        if (!t.empty() && t.back() == '%') {
            percentage = true;
            t.pop_back();
        }
        float value = std::stof(t);
        return percentage ? (value / 100.0f) : value;
    }
    
    // --- HSL to RGB conversion ---
    
    // Converts HSL (h in degrees, s and l in [0,1]) plus alpha to an RGB Colour.
    static Colour hslToRGB(float h, float s, float l, float a) {
        auto hueToRGB = [](float p, float q, float t) -> float {
            if(t < 0) t += 1;
            if(t > 1) t -= 1;
            if(t < 1.0f/6.0f) return p + (q - p) * 6 * t;
            if(t < 1.0f/2.0f) return q;
            if(t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6;
            return p;
        };
        float h_fraction = h / 360.0f;
        float q = (l < 0.5f) ? (l * (1 + s)) : (l + s - l * s);
        float p = 2 * l - q;
        float r = hueToRGB(p, q, h_fraction + 1.0f/3.0f);
        float g = hueToRGB(p, q, h_fraction);
        float b = hueToRGB(p, q, h_fraction - 1.0f/3.0f);
        return Colour(r, g, b, a);
    }
};


enum State {
	ENABLED,
	GHOST,
	DISABLED
};

#define HEADER_2_HEIGHT		26

const Colour HEADER_COLOUR					= Colour(0.125f);
const Colour HEADER_2_COLOUR				= Colour(0.225f);
const Colour BACKDROP_COLOUR				= Colour(0.175f);

const Colour BUTTON_COLOUR					= Colour(0.125);
const Colour BUTTON_HOVER_COLOUR			= Colour(70.0f,	110.0f,	160.0f, 255.0f) / 255.0f;
const Colour BUTTON_DISABLED_COLOUR			= Colour(0.2f);
const Colour BUTTON_DISABLED_HOVER_COLOUR	= Colour(90.0f,	130.0f,	180.0f, 255.0f) / 255.0f;

const Colour FIELD_COLOUR					= Colour(0.225f);
const Colour FIELD_HOVER_COLOUR				= Colour(0.325f);
const Colour FIELD_EDIT_COLOUR				= Colour(0.125f);
const Colour FIELD_EDIT_DISABLED_COLOUR		= Colour(0.15f);
const Colour FIELD_ACTIVE_COLOUR			= Colour(70.0f,	110.0f,	160.0f, 255.0f) / 255.0f;
const Colour FIELD_ACTIVE_DISABLED_COLOUR	= Colour(55.0f,	90.0f,	120.0f, 255.0f) / 255.0f;
const Colour FIELD_DISABLED_COLOUR			= Colour(0.19f);
const Colour FIELD_DISABLED_HOVER_COLOUR	= Colour(0.21f);

const Colour FIELD_TEXT_COLOUR				= Colour(0.9f);
const Colour FIELD_TEXT_DISABLED_COLOUR		= Colour(0.7f);


struct Boundary{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	Boundary(int _x, int _y, int _width, int _height)
        : x(_x), y(_y), width(_width), height(_height) {}
};


bool isMouseInBounds(UI* _ui, Boundary* bounds, int margin = 0, int pos_x = -1, int pos_y = -1);


class MouseHandler {
public:
    MouseHandler() 
        : mContainer({0, 0, 0, 0}), isDrawn(false), mZ(0.0f), layer(0) 
    { 
        instances.push_back(this);
    }

    virtual ~MouseHandler() {
        instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
    }

    static const std::vector<MouseHandler*>& getInstances() {
        return instances;
    }

    void SetZ(float z) { mZ = z; };

    Boundary mContainer;
    bool isDrawn;
    float mZ;
    int layer;

    virtual void MouseCallback(int _mouseState) {
        // Default behavior. Override in derived classes.
    }

private:
    static std::vector<MouseHandler*> instances;
};


class MouseInputSingleton {
public:
    static MouseInputSingleton& getInstance() {
        static MouseInputSingleton instance;
        return instance;
    }

    void grantMouseInput(UI* _ui);

    void resetMouseInput();

private:
    MouseInputSingleton() {}
    ~MouseInputSingleton() {}

    std::vector<MouseHandler*> selectedCallbacks;

    MouseInputSingleton(const MouseInputSingleton&) = delete;
    void operator=(const MouseInputSingleton&) = delete;
};


class Primitive : public MouseHandler 
{
public:
	Primitive();
	~Primitive(){};

	Primitive& Rect(int xtl, int ytl, int w, int h, int r, float z = 0.0f);
	//void SetZOffset(GLfloat z);
	Primitive& SetColour(Colour c);
	Primitive& SetAlpha(float a);
	Primitive& Draw();
	Primitive& SetCorners(std::vector<bool> corners) { mCorners = corners; return *this; };

private:
	std::vector<bool> mCorners = {true, true, true, true};
	Shape target;

	VAO VAO1;
	VBO VBO1;
	EBO EBO1;

    bool isValid = false;
	Shader shaderProgram;
	GLuint uMVPMatrixID;
    glm::mat4 mMVPMatrix;

	void Arc(int x, int y, int r, float begin, float end, float step);
	void AddVert(GLfloat x, GLfloat y);
	void CalcInds();
	void UpdateMVPMatrix();
};


class WorkspaceComponent {
public:
	WorkspaceComponent() {}
    virtual ~WorkspaceComponent() {}

    // Common interface method for all components
    //virtual void Register() = 0;
    virtual void Draw() = 0;

};


void addVertex(std::vector<GLfloat>& vertices,
               float x, float y, float z,
               float u, float v,
               float nx, float ny, float nz);
