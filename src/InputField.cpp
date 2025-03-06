// Copyright (c) 2025 Thomas Groom


#include "ui_library/InputField.h"


InputField::InputField(UI* _ui, std::shared_ptr<Text> tr, std::string label, InputType type,
                       Boundary container, float z, std::string units, bool percent,
                       Text::Align labelAlign, Text::Align fieldAlign)
    : mUI(_ui), mTextRenderer(tr), mLabel(label), mType(type), mUnits(units), mShowPercent(percent), mLabelAlign(labelAlign), mFieldAlign(fieldAlign),
        mLabelContainer(container), mInputContainer(container) {
    mContainer = container;
    mZ = z;
    SetPos(mContainer.x, mContainer.y, mContainer.width, mContainer.height, mZ);
    rectPrim.SetColour(mDefaultColour);

    mCheckboxIconTrue = std::make_shared<Texture2D>(GL_RGBA, GL_RGBA, std::filesystem::path(std::string(UI_LIBRARY_RESOURCES_DIR) + "/icons/checkmark.png"), glm::vec2(20));

    dropDownBtn = new DropdownButton(mUI, mTextRenderer, L"", Text::LEFT_MIDDLE, {0, 0, 140, 20}, 5, mZ + 0.003f, mTextMarginX, 0, 1.0f, BUTTON_COLOUR, FIELD_HOVER_COLOUR, BUTTON_DISABLED_COLOUR, FIELD_DISABLED_HOVER_COLOUR);
    dropDownBtn->SetReflectSelectedOption(true);
    
    if (mType == DOUBLE) {
        mMin = -1000000000.0;
        mMax = 1000000000.0;
    } else if (mType == INT) {
        mMin = -1000000000;
        mMax = 1000000000;
    }
} 

InputField& InputField::setMin(std::variant<double, int> min) {
    // Check for type mismatches only once
    if ((mType == DOUBLE && std::holds_alternative<int>(min)) || (mType == INT && std::holds_alternative<double>(min))) {
        throw std::invalid_argument("Mismatch between InputType and provided min types.");
    }
    mMin = min;
    return *this;
}

InputField& InputField::setMax(std::variant<double, int> max) {
    // Check for type mismatches only once
    if ((mType == DOUBLE && std::holds_alternative<int>(max)) || (mType == INT && std::holds_alternative<double>(max))) {
        throw std::invalid_argument("Mismatch between InputType and provided max types.");
    }
    mMax = max;
    return *this;
}

InputField::~InputField() {
}

std::string FormatDoubleMinimal(double value) {
    if (std::isnan(value)) {
        return "";
    }

    std::stringstream stream;
    stream << std::fixed << std::setprecision(9) << value;

    std::string result = stream.str();
    size_t dotPos = result.find(L'.');

    if (dotPos != std::string::npos) {
        size_t lastNonZero = result.find_last_not_of(L'0');
        if (lastNonZero == dotPos) {
            lastNonZero++;
        }
        result.erase(lastNonZero + 1);
    }
    return result;
}

// Utility function to format float to 3 decimal places
std::string FormatDoubleTo3DP(double value) {
    if (std::isnan(value)) {
        return "";
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(3) << value;
    return stream.str();
}


void InputField::MouseCallback(int _mouseState) {
    if (mState != DISABLED && isMouseInBounds(mUI, &mInputContainer)) {
        mMouseInBounds = true;
        if (mType == BOOLEAN && mUI->G_LEFT_MOUSE_STATE == GLFW_PRESS) {
            mBoolValue = !mBoolValue;
            WriteVar();
        } else if (mType == PATH && mUI->G_LEFT_MOUSE_STATE == GLFW_PRESS) {
            mDialogOpen = true;
        } else if ((mType == DOUBLE || mType == INT || mType == TEXT)
            && mUI->G_LEFT_MOUSE_STATE == GLFW_RELEASE && !mouseDrag
            && (!mUI->G_LEFT_MOUSE_DRAG
                || isMouseInBounds(mUI, &mInputContainer, 0, mUI->G_MOUSE_DRAG_START_X,
                    mUI->G_MOUSE_DRAG_START_Y))) {
            mTextField->isActive = true;

            // Set the value as text in the field
            if (mType == DOUBLE) {
                mTextField->content = StringToWString(FormatDoubleMinimal(mDoubleValue));
            } else if (mType == INT) {
                mTextField->content = std::to_wstring(mIntValue);
            } else if (mType == TEXT) {
                mTextField->content = StringToWString(mTextValue);
            }

            mTextField->selStart = 0;
            mTextField->selEnd = static_cast<int>(mTextField->content.length());
            mTextField->caretPos = mTextField->selEnd;
        }

        if ((mType == DOUBLE || mType == INT) && !mTextField->isActive) {
            mUI->G_SET_CURSOR = CURSOR_HRESIZE;
            if (mUI->G_LEFT_MOUSE_DRAG && isMouseInBounds(mUI, &mInputContainer, 0, mUI->G_MOUSE_DRAG_START_X, mUI->G_MOUSE_DRAG_START_Y)) {
                mouseDrag = true;
            }
        }
    }
}


void InputField::WriteVar() { 
    if (!mVar) return; // Safety check

    if (mType == DOUBLE) {
        if (std::holds_alternative<std::reference_wrapper<double>>(*mVar)) {
            std::get<std::reference_wrapper<double>>(*mVar).get() = mDoubleValue;
        }
    }
    else if (mType == INT || mType == DROPDOWN || mType == OPTIONS) {
        if (std::holds_alternative<std::reference_wrapper<int>>(*mVar)) {
            std::get<std::reference_wrapper<int>>(*mVar).get() = mIntValue;
        }
    }
    else if (mType == BOOLEAN) {
        if (std::holds_alternative<std::reference_wrapper<bool>>(*mVar)) {
            std::get<std::reference_wrapper<bool>>(*mVar).get() = mBoolValue;
        }
    }
    else if (mType == TEXT || mType == PATH) {
        if (std::holds_alternative<std::reference_wrapper<std::string>>(*mVar)) {
            std::get<std::reference_wrapper<std::string>>(*mVar).get() = mTextValue;
        }
    }
}



void InputField::ReadVar() {
    if (!mVar) return; // Safety check

    if (mType == DOUBLE) {
        if (std::holds_alternative<std::reference_wrapper<double>>(*mVar)) {
            mDoubleValue = std::get<std::reference_wrapper<double>>(*mVar).get();
        }
    }
    else if (mType == INT || mType == DROPDOWN || mType == OPTIONS) {
        if (std::holds_alternative<std::reference_wrapper<int>>(*mVar)) {
            mIntValue = std::get<std::reference_wrapper<int>>(*mVar).get();
        }
    }
    else if (mType == BOOLEAN) {
        if (std::holds_alternative<std::reference_wrapper<bool>>(*mVar)) {
            mBoolValue = std::get<std::reference_wrapper<bool>>(*mVar).get();
        }
    }
    else if (mType == TEXT || mType == PATH) {
        if (std::holds_alternative<std::reference_wrapper<std::string>>(*mVar)) {
            mTextValue = std::get<std::reference_wrapper<std::string>>(*mVar).get();
        }
    }
}


void InputField::Draw(int x, int y, int width, int height, std::optional<float> z) {
    isDrawn = true;
    SetPos(x, y, width, height, z.value_or(mZ));
    rectPrim.Rect(mInputContainer.x, mInputContainer.y, mInputContainer.width, mInputContainer.height, r, mZ);

    if (mIsOptional && mState != DISABLED) {
        if (std::isnan(mDoubleValue)) {
            mState = GHOST;
        } else if (mState == GHOST) {
            mState = ENABLED;
        }
    }

    if (mType == PATH) {
        if (mDialogOpen) {
            mDialog = std::make_unique<pfd::open_file>(mPathTitle, mPathDefault, mPathFilters, mPathAllowMultiples);
            mDialogOpen = false;
            mResultReady = false;
        }
        if (mDialog && mDialog->ready(0) && !mResultReady) {   // TODO: reduce timeout time
            mResultReady = true;
            const auto results = mDialog->result();
            if (!results.empty()) {
                mTextValue = results[0];
                WriteVar();
            }
        }
    }
    
    if ((mType == DOUBLE || mType == INT || mType == TEXT) && mTextField->isActive) {
        mTextField->handleInput();
        try {
            if (mType == DOUBLE) {
                if (mTextField->content == L"" && mIsOptional) {
                    mDoubleValue = std::nan("");
                }
                else {
                    if (std::isnan(std::stod(mTextField->content))) throw "";
                    mDoubleValue = std::stod(mTextField->content);
                }
            } else if (mType == INT) {
                mIntValue = std::stoi(mTextField->content);
            } else if (mType == TEXT) {
                mTextValue = WStringToString(mTextField->content);
            }
        } catch (...) {
            // Handle invalid input gracefully
        }
    }
    if ((mType == DOUBLE || mType == INT || mType == TEXT) && ((mUI->G_LEFT_MOUSE_STATE == GLFW_PRESS && !mMouseInBounds) || mUI->G_ENTER_PRESS) && mTextField->isActive) {
        mTextField->isActive = false;
        WriteVar();
    }
    if ((mType == DOUBLE || mType == INT) && !mTextField->isActive) {
        if (mUI->G_LEFT_MOUSE_STATE == GLFW_RELEASE) {
            mouseDrag = false;
        }
        if (mouseDrag) {
            mUI->G_SET_CURSOR = CURSOR_HRESIZE;
            HandleMouseDragForNumericInput();
        }
    }


    if (!mTextField->isActive) {
        ReadVar();
    }
    
    if (mType == DOUBLE) {
        mDoubleValue = std::clamp(mDoubleValue, std::get<double>(mMin), std::get<double>(mMax));
    } else if (mType == INT) {
        mIntValue = std::clamp(mIntValue, std::get<int>(mMin), std::get<int>(mMax));
    } else if (mType == DROPDOWN) {
        mIntValue = std::clamp(mIntValue, 0, static_cast<int>(optionBtns.size() - 1));
    } else if (mType == OPTIONS) {
        mIntValue = std::clamp(mIntValue, mOptionsStartOffset, static_cast<int>(optionBtns.size() - 1) + mOptionsStartOffset);
    }

    // TODO: These should have member variable counterparts? (so that they are not set in stone for all input fields)
    mTextColour = (mState == ENABLED) ? FIELD_TEXT_COLOUR : FIELD_TEXT_DISABLED_COLOUR;  
    mTextRenderer->RenderText(StringToWString(mLabel), mLabelContainer, mZ + 0.002f, mLabelAlign, mTextColour);

    if (mType == DOUBLE || mType == INT || mType == TEXT || mType == PATH) {
        
        rectPrim.SetColour(mMouseInBounds ?
            ((mState == ENABLED) ?  mHoverColour : mDisabledHoverColour)
            : ((mState == ENABLED) ? ((mType == TEXT || mType == PATH) ? BUTTON_COLOUR : mDefaultColour) : mDisabledColour));
        if (mTextField->isActive || mouseDrag) {
            rectPrim.SetColour((mState == ENABLED) ? mEditColour : mEditDisabledColour);
        }
        rectPrim.Draw();

        if (mShowPercent && !mTextField->isActive && (mType == DOUBLE || mType == INT)) {
            float fillWidth;
            if (mType == DOUBLE) {
                fillWidth = (mInputContainer.width * (mDoubleValue - std::get<double>(mMin))) / std::abs((std::get<double>(mMax) - std::get<double>(mMin)));
            } else {
                fillWidth = (mInputContainer.width * (mIntValue - std::get<int>(mMin))) / std::abs((std::get<int>(mMax) - std::get<int>(mMin)));
            }
            rectPrim.Rect(mInputContainer.x, mInputContainer.y, static_cast<int>(fillWidth), mInputContainer.height, 5, mZ + 0.001f);
            rectPrim.SetColour(mActiveColour);
            rectPrim.Draw();
        }
        if (mTextField->isActive) {
            DrawEditableText();
        } else {
            DrawNumericText();
        }
    } else if (mType == BOOLEAN) {
        DrawCheckbox();
    } else if (mType == OPTIONS) {
        DrawOptions();
    } else if (mType == DROPDOWN) {
        dropDownBtn->SetPos(mInputContainer.x, mInputContainer.y, mInputContainer.width, mInputContainer.height).SetZ(mZ + 0.003f);
        dropDownBtn->SetActiveOption(mIntValue);
        dropDownBtn->setState(mState);
        dropDownBtn->Draw();
    }

    mMouseInBounds = false;
}


void InputField::SetPos(int x, int y, int width, int height, std::optional<float> z) {
    width = std::max(width, 10);
    height = std::max(height, 10);
    mContainer.x = x;
    mContainer.y = y;
    mContainer.width = width;
    mContainer.height = height;
    if (mRatio >= 0.0f) {
        mLabelContainer.x = x;
        mLabelContainer.y = y;
        mLabelContainer.width = static_cast<int>((width * mRatio) - 5);
        mLabelContainer.height = height;
        mInputContainer.x = static_cast<int>(x + (width * mRatio) + 5);
        mInputContainer.y = y;
        mInputContainer.width = static_cast<int>((width * (1.0 - mRatio)) - 5);
        mInputContainer.height = height;
    }
    else {
        mInputContainer.x = x;
        mInputContainer.y = y;
        mInputContainer.width = static_cast<int>((width * (1.0f + mRatio)) - 5);
        mInputContainer.height = height;
        mLabelContainer.x = static_cast<int>(x + (width * (1.0f + mRatio)) + 5);
        mLabelContainer.y = y;
        mLabelContainer.width = static_cast<int>((width * (-mRatio)) - 5);
        mLabelContainer.height = height;
    }
    mZ = z.value_or(mZ);
}


void InputField::HandleMouseDragForNumericInput() {
    if (mUI->G_LEFT_MOUSE_DRAG) {
        double delta = mUI->G_MOUSE_DRAG_DELTA.x; // TODO: Function to calculate horizontal mouse drag
        if (mType == DOUBLE) {
            if (std::get<double>(mMax) < 1000000000.0 && std::get<double>(mMin) > -1000000000.0) {
                double modifier = std::clamp(std::get<double>(mMax) - std::get<double>(mMin), 0.001, 10.0) / static_cast<double>(mInputContainer.width);
                mDoubleValue = static_cast<double>(mDoubleValue + delta * modifier);
            } else {
                mDoubleValue = static_cast<double>(mDoubleValue + (static_cast<int>(delta) / 10.0));
            }
        } else if (mType == INT) {
            double range = std::clamp(std::get<int>(mMax) - std::get<int>(mMin), -100, 100);
            mIntValue = static_cast<int>(mIntValue + static_cast<int>((delta * range) / static_cast<double>(mInputContainer.width)));
        }
        if (mType == DOUBLE || mType == INT) {
            WriteVar();
        }
    }
}


void InputField::DrawEditableText() {
    int caretCursor = (mTextField->caretVisibilityCounter > mTextField->caretBlinkRate || !mTextField->isActive) ? -1 : mTextField->caretPos;
    int textMargin = (mType == TEXT || mType == PATH) ? mTextMarginX + 2 : 0;
    mTextRenderer->RenderText(mTextField->content + L" ", {mInputContainer.x + textMargin, mInputContainer.y, mInputContainer.width - (textMargin * 2), mInputContainer.height}, mZ + 0.002f, (mType == TEXT || mType == PATH) ? Text::LEFT_MIDDLE : Text::CENTER_MIDDLE, mTextColour, false, true, mTextField->selStart, mTextField->selEnd, caretCursor);
}


void InputField::DrawNumericText() {
    std::string valueText;
    if (mType == DOUBLE) {
        valueText = FormatDoubleTo3DP(mDoubleValue) + mUnits;
    } else if (mType == INT){
        valueText = std::to_string(mIntValue) + mUnits;
    } else if (mType == TEXT || mType == PATH) {
        valueText = mTextValue;
    }
    int textMargin = (mType == TEXT || mType == PATH) ? mTextMarginX + 2 : 0;
    mTextRenderer->RenderText(StringToWString(valueText), {mInputContainer.x + textMargin, mInputContainer.y, mInputContainer.width - (textMargin * 2), mInputContainer.height}, mZ + 0.02f, (mType == TEXT || mType == PATH) ? Text::LEFT_MIDDLE : Text::CENTER_MIDDLE, mTextColour);
}


void InputField::DrawCheckbox() {
    const int margin = 1;
    if (mShowCheckbox) {
        rectPrim.Rect(mInputContainer.x, mInputContainer.y + margin, mInputContainer.height - (2 * margin),mInputContainer.height - (2 * margin), 5, mZ);
        rectPrim.SetColour(mBoolValue ? ((mState == ENABLED) ? mActiveColour:mActiveDisabledColour) : (mMouseInBounds ? ((mState == ENABLED) ? mHoverColour:mDisabledHoverColour) : ((mState == ENABLED) ? mDefaultColour:mDisabledColour)));
        rectPrim.Draw();
    }

    if (mBoolValue) {
        if (mCheckboxIconTrue) {
            mCheckboxIconTrue->DrawSprite(glm::vec2(mInputContainer.x - margin, mInputContainer.y), glm::vec2(mInputContainer.height, mInputContainer.height), mZ + 0.001f, 0.0f, glm::vec3((mState == ENABLED) ? 1.0f : 0.6f));
        }
    }
    else {
        if (mCheckboxIconFalse) {
            mCheckboxIconFalse->DrawSprite(glm::vec2(mInputContainer.x - margin, mInputContainer.y), glm::vec2(mInputContainer.height, mInputContainer.height), mZ + 0.001f, 0.0f, glm::vec3((mState == ENABLED) ? 1.0f : 0.6f));
        }
    }
}


void InputField::DrawOptions() {
    if (optionBtns.empty()) return;

    int totalOptions = static_cast<int>(optionBtns.size());
    int baseWidth = mInputContainer.width / totalOptions; // Base width for each button
    int remainder = mInputContainer.width % totalOptions; // Remainder to distribute
    int currentX = mInputContainer.x;

    for (int i = 0; i < totalOptions; ++i) {
        std::shared_ptr<Button> buttonPtr = optionBtns[i];
        if (buttonPtr) {

            int optionWidth = baseWidth + (i < remainder ? 1 : 0);  // Add one extra pixel to account for the remainder, distributed among the first few buttons
            buttonPtr->SetPos(currentX, mInputContainer.y, optionWidth, mInputContainer.height).SetZ(mZ + 0.002f);
            currentX += optionWidth; // Move to the next button position
            buttonPtr->setColour(mIntValue == i + mOptionsStartOffset ? mActiveColour : mDefaultColour);
            buttonPtr->setDColour(mIntValue == i + mOptionsStartOffset ? mActiveDisabledColour : mDisabledColour);
            buttonPtr->setHColour(mIntValue == i + mOptionsStartOffset ? mActiveColour : mHoverColour);
            buttonPtr->setDHColour(mIntValue == i + mOptionsStartOffset ? mActiveDisabledColour : mDisabledHoverColour);
            buttonPtr->setState(mState);
            buttonPtr->Draw();
        }
    }
}

InputField& InputField::SetOptions(
    const std::vector<std::string>& options,
    const std::vector<std::string>& return_options,
    std::optional<std::reference_wrapper<std::string>> stringVarToChange) {

    bool mOptionStringVar = false;
    if (!return_options.empty()) {
        mOptionStringVar = true;
    }

    if (mType == DROPDOWN) {
        optionBtns.clear();
        int i = 0;
        for (const auto& opt : options) {
            std::shared_ptr<Button> btn = std::make_shared<Button>(mUI, mTextRenderer, StringToWString(opt), Text::LEFT_MIDDLE, Boundary(0, 0, 20, 20), 5, mZ + 0.004f, 5, 0, 1.0f);
            if (mOptionStringVar) {
                btn->SetOnClickCallback([this, i, &return_options, stringVarToChange]() {
                    if (stringVarToChange.has_value()) {
                        stringVarToChange.value().get() = return_options[i];
                    }
                    this->SetValue(i);
                });
            }
            else {
                btn->SetOnClickCallback([this, i]() {
                    this->SetValue(i);
                });
            }
            optionBtns.push_back(btn);
            i++;
        }
        dropDownBtn->SetChildButtons(optionBtns);
    }
    if (mType == OPTIONS) {
        optionBtns.clear();
        int i = 0;
        for (std::string opt : options) {
            std::shared_ptr<Button> btn = std::make_shared<Button>(mUI, mTextRenderer, StringToWString(opt), Text::CENTER_MIDDLE, Boundary(0, 0, 20, 20), 5, mZ + 0.004f, 0, 0, 1.0f, mDefaultColour, mHoverColour, mDisabledColour, mDisabledHoverColour);
            btn->SetOnClickCallback([this, i]() {this->SetValue(i + mOptionsStartOffset);});
            btn->setCorners({i == 0, i == (options.size() - 1), i == (options.size() - 1), i == 0});
            optionBtns.push_back(btn);
            i++;
        }
    }
    return *this;
}

    