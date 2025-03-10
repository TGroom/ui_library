// Copyright (c) 2025 Thomas Groom

#include "ui_library/DropdownButton.h"


DropdownButton::DropdownButton(UI* _ui, std::shared_ptr<Text> tr, std::wstring _text, Text::Align align, Boundary container,
                               int r, float z, int textx, int texty, Colour _Colour, Colour _HColour, Colour _DColour, Colour _DHColour)
                                : Button(_ui, tr, _text, align, container, r, z, textx, texty, _Colour, _HColour, _DColour, _DHColour) ,
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
    int dropdownHeight = std::max(static_cast<int>(mChildButtons.size()), 1) * mButtonHeight + 8;
    int width = std::max(mChildWidth, mContainer.width) + 10;
    mDropDownBox.height = dropdownHeight;
    mDropDownBox.width = width;
    mDropDownBox.x = mContainer.x + ((mContainer.x + width < mUI->G_WIDTH) ? 0 : mContainer.width - width);  // Right window edge constraint
    mDropDownBox.y = mContainer.y +  ((mContainer.y + dropdownHeight < mUI->G_HEIGHT) ? mContainer.height : -dropdownHeight);  // Bottom window edge constrained

    if (mDropdownVisible) {
        if (mChildButtons.size() == 0) {
            noneBtn.SetPos(mDropDownBox.x + 5, mDropDownBox.y + 4, mDropDownBox.width - 10, mButtonHeight).SetZ(0.9f);
        }
        for (size_t i = 0; i < mChildButtons.size(); ++i) {
            if (mChildButtons[i]) {
                mChildButtons[i]->SetPos(mDropDownBox.x + 5, mDropDownBox.y + 4 + i * mButtonHeight, mDropDownBox.width - 10, mButtonHeight);
                mChildButtons[i]->SetZ(0.901f);
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
    glDisable(GL_SCISSOR_TEST);

    int stateOverall = -1;
    
    // Draw the dropdown background
    dropDownPrim.Rect(mDropDownBox.x, mDropDownBox.y, mDropDownBox.width, mDropDownBox.height, 5, 0.9f);
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

    glEnable(GL_SCISSOR_TEST);
}