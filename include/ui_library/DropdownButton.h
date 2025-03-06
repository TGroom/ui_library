// Copyright (c) 2025 Thomas Groom

#pragma once

#include <glad/glad.h>
#include <cmath>
#include <vector>
#include <functional> // Required for std::function
#include <filesystem>
#include <future>
#include "Button.h"
#include "Utils.h"
#include "Text.h"
#include "Texture.h"


class DropdownButton : public Button {
public:
    DropdownButton(UI* _ui, std::shared_ptr<Text> tr, std::wstring _text, Text::Align align = Text::CENTER_MIDDLE, Boundary container = {0, 0, 50, 20},
                   int r = 5, float z = 0.0f, int _textx = 0, int _texty = 0, float a = 1.0f, Colour _Colour = BUTTON_COLOUR,
			Colour _HColour = BUTTON_HOVER_COLOUR, Colour _DColour = BUTTON_DISABLED_COLOUR, Colour _DHColour = BUTTON_DISABLED_HOVER_COLOUR);

    //void Register() override;
    void Draw() override;

    // Add child buttons to the dropdown
    void SetChildButtons(std::vector<std::shared_ptr<Button>> childButton);

    // Toggle dropdown visibility
    void ToggleDropdown();

    // Optionally reflect the selected option in the main button
    void SetReflectSelectedOption(bool reflect);

    void SetActiveOption(int n);
    void SetChildWidth(int childWidth) { mChildWidth = childWidth; };

    int getNumOfOptions(){
        return static_cast<int>(mChildButtons.size());
    }

private:
    std::vector<std::shared_ptr<Button>> mChildButtons;
    bool mDropdownToggled = false;
    bool mDropdownVisible = false;
    bool mReflectSelectedOption = false;
    Primitive dropDownPrim;
    Boundary mDropDownBox = {0, 0, 0, 0};
    int mChildWidth = 100.0f;
    bool childrenRegistered = false;
    int mButtonHeight = 22;

    // Helper to draw the dropdown menu
    void DrawDropdown();

    Button noneBtn;
};

