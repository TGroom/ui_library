// Copyright (c) 2025 Thomas Groom

#include "ui_library/DropdownButton.h"


DropdownButton::DropdownButton(UI* _ui, std::shared_ptr<Text> tr, std::wstring _text, Text::Align align, Boundary container,
                               int r, float z, int textx, int texty, float a, Colour _Colour, Colour _HColour, Colour _DColour, Colour _DHColour)
                                : Button(_ui, tr, _text, align, container, r, z, textx, texty, a, _Colour, _HColour, _DColour, _DHColour) ,
                                  noneBtn(_ui, tr, L"None", Text::LEFT_MIDDLE) {

    SetOnClickCallback(std::bind(&DropdownButton::ToggleDropdown, this));

    noneBtn.setState(DISABLED);
    noneBtn.setDColour(mMainColour);
    noneBtn.setDHColour(mMainColour);
}


void DropdownButton::SetChildButtons(std::vector<std::shared_ptr<Button>> childButton) {
    mChildButtons = childButton;
    for (std::shared_ptr<Button> child : mChildButtons) {
        child->setColour(mMainColour);
        child->setHColour(mHoverColour);
        child->setDColour(mDisabledColour);
        child->setDHColour(mDisabledHoverColour);
    }
}


void DropdownButton::ToggleDropdown() {
    mDropdownVisible = !mDropdownVisible;
    mDropdownToggled = true;
}


void DropdownButton::SetReflectSelectedOption(bool reflect) {
    mReflectSelectedOption = reflect;
}



void DropdownButton::Draw() {

    if (mDropdownVisible) {
        if (mChildButtons.size() == 0) {
            noneBtn.SetPos(mContainer.x + 5, mContainer.y + mContainer.height + 4, std::max(mChildWidth, mContainer.width), mButtonHeight).SetZ(mZ + 0.032f);
        }
        for (size_t i = 0; i < mChildButtons.size(); ++i) {
            if (mChildButtons[i]) {
                mChildButtons[i]->SetPos(mContainer.x + 5, mContainer.y + mContainer.height + 4 + i * mButtonHeight, std::max(mChildWidth, mContainer.width), mButtonHeight).SetZ(mZ + 0.032f);
                mChildButtons[i]->setTriggerEdge(GLFW_RELEASE);
            }
        }
    }

    // Draw the main button
    Button::Draw();
    // TODO: Check dropdown function to set mDropdownVisible if mouse outside of large container
    if (mDropdownVisible) {
        DrawDropdown();
    }
}


void DropdownButton::SetActiveOption(int n) {
    n = std::clamp(n, 0, static_cast<int>(mChildButtons.size()) - 1);
    if (mChildButtons.size() > 0) {
        auto& child = mChildButtons[n];
        mText = child->mText; // Update main button text
    }
}

void DropdownButton::DrawDropdown() {

    int stateOverall = -1;
    int dropdownHeight = std::max(static_cast<int>(mChildButtons.size()), 1) * mButtonHeight + 8;
    // Draw the dropdown background
    mDropDownBox = {mContainer.x, mContainer.y + mContainer.height, std::max(mChildWidth, mContainer.width) + 10, dropdownHeight};
    dropDownPrim.Rect(mDropDownBox.x, mDropDownBox.y, mDropDownBox.width, mDropDownBox.height, 5, mZ + 0.031f);
    dropDownPrim.SetColour(mMainColour);
    dropDownPrim.Draw();

    if (mChildButtons.size() == 0) {
        stateOverall = noneBtn.getState();
        noneBtn.Draw();
    }
    else {
        // Draw each child button
        for (size_t i = 0; i < mChildButtons.size(); i++) {
            if (mChildButtons[i]) {
                mChildButtons[i]->Draw();
                int state = mChildButtons[i]->getState();  // State only valid after draw call
                // Check for clicks on child buttons
                if (state == 4) {
                    ToggleDropdown();         // Close dropdown after selection
                    if (mReflectSelectedOption) {
                        SetActiveOption(i);
                    }
                }
                if (state >= stateOverall) {
                    stateOverall = state;
                }
            }
        }
    }

    if (stateOverall < 1 || (!mDropdownToggled && !isMouseInBounds(mUI, &mDropDownBox) && mUI->G_LEFT_MOUSE_STATE == GLFW_PRESS)) {
        ToggleDropdown();
    }
    mDropdownToggled = false;
}