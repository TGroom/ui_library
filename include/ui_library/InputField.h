// Copyright (c) 2025 Thomas Groom

#pragma once

#include <glad/glad.h>
#include <cmath>
#include <vector>
#include <functional> // Required for std::function
#include <filesystem>
#include <future>
#include <variant>
#include <optional>
#include <ui_library/portable-file-dialogs.h>

#include "Utils.h"
#include "Text.h"
#include "Texture.h"
#include "TextField.h"
#include "Button.h"
#include "DropdownButton.h"
#include "ScrollBar.h"




class TabLayout {
public:
    TabLayout(UI* _ui) : mUI(_ui), mActiveTab(0) {}
    ~TabLayout() {}

    void AddTab(const std::wstring& label, std::shared_ptr<Texture2D> icon) {
        std::shared_ptr<Button> newTab = std::make_shared<Button>(mUI, UIText, label);
        newTab->setIcon(icon, 5, 5, 20, 20);
        newTab->setColour(HEADER_COLOUR);
        newTab->SetOnClickCallback([this, index = numOfTabs]() { mActiveTab = index; });
        tabBtns.push_back(newTab);
        numOfTabs++;
    }

    void Draw(Boundary _container, float _z) {
        mContainer = _container;
		mZ = _z;
        mPrim.Rect(mContainer.x, mContainer.y, mContainer.width, mContainer.height, 0, mZ + 0.02f);
        mPrim.SetColour(BACKDROP_COLOUR);
        mPrim.Draw();

        for (size_t i = 0; i < tabBtns.size(); ++i) {
            tabBtns[i]->SetPos(mContainer.x - 30, mContainer.y + static_cast<int>(i) * 33, 35, 30);
            tabBtns[i]->SetZ(mZ + 0.01f);
            tabBtns[i]->setColour(i == mActiveTab ? BACKDROP_COLOUR : HEADER_COLOUR);
            tabBtns[i]->Draw();
        }
    }

    int GetActiveTab() const { return mActiveTab; }

private:
    UI* mUI;
    Boundary mContainer = {0, 0, 100, 100};
    Primitive mPrim;
    std::shared_ptr<Text> UIText = std::make_shared<Text>(std::string(UI_LIBRARY_RESOURCES_DIR) + "/fonts/arial.ttf", 12);
    std::vector<std::shared_ptr<Button>> tabBtns;
    float mZ = 0.0f;
    int numOfTabs = 0;
    int mActiveTab;

};





// TODO: Merge this with InputField?
template <typename T>
class ListInput {
public:
    static_assert(std::is_base_of<Identifier, T>::value, "T must derive from Identifier");
    enum Type {
        LIST_ONLY,
        SIMPLE_ADD,
        DROPDOWN_ADD,
    };

    // In non-dropdown mode, the remove callback receives the currently selected option.
    ListInput(UI* _ui, const std::wstring& labelText, int fieldHeight,
              Type type = LIST_ONLY,
              std::function<void(std::shared_ptr<T>)> addCallback = nullptr,
              std::function<void(std::shared_ptr<T>)> removeCallback = nullptr)
        : mUI(_ui),
          mLabelText(labelText),
          mFieldHeight(fieldHeight),
          mZ(0.0f),
          mCurrentSelectedIndex(-1),
          mType(type),
          mAddCallback(addCallback),
          mRemoveCallback(removeCallback)
    {
        mScroll.SetZ(mZ);
        // Configure the add dropdown button.
        mAddDropdownButton.setIcon(ResourceManager::GetInstance().iconTextures.at("add"),
                                   0, 0, 20, 20)
                          .setCorners({ true, true, false, false })
                          .setColour(FIELD_COLOUR)
                          .setHColour(FIELD_HOVER_COLOUR)
                          .setDHColour(FIELD_COLOUR);

        if (mType == SIMPLE_ADD) {
            mAddDropdownButton.SetOnClickCallback([this]() { callAddCallback(); }); // This disables normal dropdown functionality making it act like a button
        }

        // Configure the remove button.
        mRemoveButton.setIcon(ResourceManager::GetInstance().iconTextures.at("remove"),
                              0, 0, 20, 20)
                     .setCorners({ false, false, true, true })
                     .setColour(FIELD_COLOUR)
                     .setHColour(FIELD_HOVER_COLOUR)
                     .setDHColour(FIELD_COLOUR);

        if (mType != LIST_ONLY) {
            mRemoveButton.SetOnClickCallback([this]() { callRemoveCallback(); });
        }
    }
    
    // Called when the add button is clicked in non-dropdown mode.
    void callAddCallback() {
        if (mAddCallback && mType != DROPDOWN_ADD) {
            // Passing an empty shared_ptr<T> as a dummy.
            mAddCallback(std::shared_ptr<T>());
            mCurrentSelectedIndex = mList.size();
        }
    }

    // Called when the remove button is clicked.
    void callRemoveCallback() {
        if (mRemoveCallback &&
            mCurrentSelectedIndex >= 0 && 
            mCurrentSelectedIndex < static_cast<int>(mList.size()))
        {
            mRemoveCallback(mList[mCurrentSelectedIndex]);
        }
    }

    // Set the external lists for chosen options and available dropdown options.
    void setLists(const std::vector<std::shared_ptr<T>>& list,
                  const std::vector<std::shared_ptr<T>>& dropdownOptions) {
        mList = list;
        mDropdownOptions = dropdownOptions;
    }

    void setCallbacks(std::function<void(std::shared_ptr<T>)> addCallback,
                      std::function<void(std::shared_ptr<T>)> removeCallback) {
        mAddCallback = addCallback;
        mRemoveCallback = removeCallback;
    }

    // Update dropdown buttons for the DROPDOWN_ADD mode.
    void updateDropdownButtons() {
        if (mType != DROPDOWN_ADD) {
            return;
        }

        size_t desiredSize = mDropdownOptions.size();

        if (mDropdownButtons.size() > desiredSize) {
            mDropdownButtons.resize(desiredSize);
        } else {
            while (mDropdownButtons.size() < desiredSize) {
                mDropdownButtons.push_back(
                    std::make_shared<Button>(mUI, UIText, L"", Text::LEFT_MIDDLE,
                                             Boundary(0, 0, 10, 10), 5, mZ + 0.06f, 5));
            }
        }

        for (size_t i = 0; i < desiredSize; ++i) {
            std::shared_ptr<T> option = mDropdownOptions[i];
            mDropdownButtons[i]->setText(StringToWString(option->name));
            // Capture the current option by value.
            mDropdownButtons[i]->SetOnClickCallback([this, option]() { 
                if (mAddCallback) {
                    mAddCallback(option);
                }
            });
        }

        mAddDropdownButton.SetChildButtons(mDropdownButtons);
    }

    // Update the list buttons for the currently selected list.
    void updateList() {
        if (mList.empty()) {
            mListBtns.clear();
            return;
        }

        size_t desiredSize = mList.size();

        if (mListBtns.size() > desiredSize) {
            mListBtns.resize(desiredSize);
        } else {
            while (mListBtns.size() < desiredSize) {
                auto btn = std::make_shared<Button>(mUI, UIText, L"", Text::LEFT_MIDDLE,
                                                      Boundary(0, 0, 10, 10), 5, mZ + 0.06f, 5);
                mListBtns.push_back({ std::shared_ptr<T>(), btn });
            }
        }

        for (size_t i = 0; i < desiredSize; ++i) {
            std::shared_ptr<T> option = mList[i];
            mListBtns[i].first = option;
            mListBtns[i].second->setText(StringToWString(option->name));
            mListBtns[i].second->SetOnClickCallback([this, i]() { setSelectedOption(i); });
        }
    }

    // Draw the list input control.
    void Draw(const Boundary& container) {
        updateList();

        mCurrentSelectedIndex = std::clamp(mCurrentSelectedIndex, 0, static_cast<int>(mList.size()) - 1);

        if (mType != LIST_ONLY) {
            Boundary labelContainer = { container.x, container.y,
                                          static_cast<int>((container.width * mRatio) - 5),
                                          mFieldHeight };

            Boundary inputContainer = { static_cast<int>(container.x + (container.width * mRatio) + 5),
                                          container.y,
                                          static_cast<int>((container.width * (1.0 - mRatio)) - 5),
                                          container.height };

            // Render label text.
            UIText->RenderText(mLabelText, labelContainer, mZ + 0.05f,
                               Text::RIGHT_MIDDLE, FIELD_TEXT_COLOUR);

            // Draw the input rectangle.
            mPrimitive.Rect(inputContainer.x + 30, inputContainer.y,
                              inputContainer.width - 30, inputContainer.height,
                              5, mZ + 0.03f)
                      .SetColour(HEADER_COLOUR)
                      .Draw();

            // Draw the scrollbar.
            mScroll.Draw(Boundary(inputContainer.x + 30, inputContainer.y,
                                  inputContainer.width - 30, inputContainer.height),
                         static_cast<int>(mListBtns.size() * (mFieldHeight + mYGap) + 10),
                         FIELD_COLOUR);

            // Set button states and draw the add and remove buttons.
            mAddDropdownButton.setState(
                mType == DROPDOWN_ADD ? 
                    (mAddDropdownButton.getNumOfOptions() == 0 ? DISABLED : ENABLED) : 
                    ENABLED);
            mAddDropdownButton.SetPos(inputContainer.x, inputContainer.y, 20, 20)
                              .SetZ(mZ + 0.05f)
                              .Draw();

            mRemoveButton.SetPos(inputContainer.x, inputContainer.y + 20, 20, 20)
                         .SetZ(mZ + 0.05f)
                         .Draw();

            // Set the scissor region.
            glScissor(inputContainer.x + 30, mUI->G_HEIGHT - (inputContainer.y + inputContainer.height),
                      inputContainer.width - 30, inputContainer.height);

            // Draw each list button.
            for (size_t i = 0; i < mListBtns.size(); ++i) {
                std::shared_ptr<T> option = mListBtns[i].first;
                mListBtns[i].second->setText(StringToWString(option->name));
                mListBtns[i].second->SetPos(inputContainer.x + 30 + 5,
                                            inputContainer.y + static_cast<int>(((mFieldHeight + mYGap) * i) + 5 - mScroll.scrollOffset),
                                            inputContainer.width - 10 - 30, mFieldHeight)
                           .SetZ(mZ + 0.04f)
                           .setColour(static_cast<int>(i) == mCurrentSelectedIndex ? FIELD_HOVER_COLOUR : BUTTON_COLOUR)
                           .Draw();
            }

            if (mType == DROPDOWN_ADD) {
                updateDropdownButtons();
            } else {
                mDropdownButtons.clear();
            }

        } else {
            // For LIST_ONLY type, draw the rectangle and buttons using the full container.
            mPrimitive.Rect(container.x, container.y,
                              container.width, container.height,
                              5, mZ + 0.03f)
                      .SetColour(HEADER_COLOUR)
                      .Draw();

            mScroll.Draw(container,
                         static_cast<int>(mListBtns.size() * (mFieldHeight + mYGap) + 10),
                         FIELD_COLOUR);

            for (size_t i = 0; i < mListBtns.size(); ++i) {
                std::shared_ptr<T> option = mListBtns[i].first;
                mListBtns[i].second->setText(StringToWString(option->name));
                mListBtns[i].second->SetPos(container.x + 5,
                                            container.y + static_cast<int>(((mFieldHeight + mYGap) * i) + 5 - mScroll.scrollOffset),
                                            container.width - 10, mFieldHeight)
                           .SetZ(mZ + 0.04f)
                           .setColour(static_cast<int>(i) == mCurrentSelectedIndex ? FIELD_HOVER_COLOUR : BUTTON_COLOUR)
                           .Draw();
            }
        }
    }

    void setZ(float z) { mZ = z; }

    std::shared_ptr<T> getSelectedOption() {
        if (mList.empty())
            return nullptr;
        return mList[mCurrentSelectedIndex];
    }

private:
    void setSelectedOption(size_t index) {
        mCurrentSelectedIndex = static_cast<int>(index);
    }

    // Shared text resource.
    std::shared_ptr<Text> UIText = std::make_shared<Text>(G_ResourcePath + "/fonts/arial.ttf", 12);

    // Data members.
    std::wstring mLabelText;
    Type mType;
    float mZ;
    int mFieldHeight;
    int mCurrentSelectedIndex = -1;
    const int mYGap = 1;
    const double mRatio = 0.4;

    // UI elements.
    UI* mUI;
    std::vector<std::pair<std::shared_ptr<T>, std::shared_ptr<Button>>> mListBtns;
    std::vector<std::shared_ptr<Button>> mDropdownButtons;  // Dropdown buttons
    DropdownButton mAddDropdownButton = DropdownButton(mUI, UIText, L"", Text::CENTER_MIDDLE, Boundary(0, 0, 20, 20), 5, mZ + 0.05f);
    Button mRemoveButton = Button(mUI, UIText, L"", Text::CENTER_MIDDLE, Boundary(0, 0, 20, 20), 5, mZ + 0.05f);
    Primitive mPrimitive;
    Scrollbar mScroll = Scrollbar(mUI); // TODO: What is preffered, initialising it here or in the constructor after the : I.E. : mScroll(mUI), ...

    // External data references.
    std::vector<std::shared_ptr<T>> mList;
    std::vector<std::shared_ptr<T>> mDropdownOptions;

    // Callback functions.
    std::function<void(std::shared_ptr<T>)> mAddCallback;
    std::function<void(std::shared_ptr<T>)> mRemoveCallback;
};













//class InputFieldGroup
//{
//	public:
//		InputFieldGroup() {}; //: isExpended(false)
//
//		void Add(InputField* inputField) {
//			mInputFields.push_back(inputField);
//		}
//
//		void Register() {
//			for (InputField* inputField : mInputFields) {
//				inputField->Register();
//			}
//		}
//
//		void Draw(int x, int y, int width, int height, std::optional<float> z) {
//			for (InputField* inputField : mInputFields) {
//				inputField->Draw(int x, int y, int width, int height, std::optional<float> z);
//			}
//		}
//
//	private:
//		std::vector<InputField*> mInputFields;
//};



class InputField : public MouseHandler {
public:
    enum InputType {
        DOUBLE,     // TODO: SHOULD HAVE VECTOR N VERSION?
        INT,        // TODO: SHOULD HAVE VECTOR N VERSION?
        DROPDOWN,
        BOOLEAN,    // TODO: SHOULD HAVE VECTOR N VERSION?
        TEXT,
        OPTIONS,
        PATH
    };


    InputField(UI* _ui,
               std::shared_ptr<Text> tr,
               std::string label, 
               InputType type, 
               Boundary container,
               float z,
               std::string units = "",
               bool percent = false,
               Text::Align labelAlign = Text::RIGHT_MIDDLE,
               Text::Align fieldAlign = Text::LEFT_MIDDLE);

    //std::optional<std::variant<double, int>> min = std::nullopt,
    //std::optional<std::variant<double, int>> max = std::nullopt,
    ~InputField();

    InputField& setHoverColour(Colour _hoverColour) {mHoverColour = _hoverColour; return *this;};
    InputField& setDefaultColour(Colour _defaultColour) {mDefaultColour = _defaultColour; return *this;};
    InputField& setEditColour(Colour _editColour) {mEditColour = _editColour; return *this;};
    InputField& setEditDisabledColour(Colour _editDisabledColour) {mEditDisabledColour = _editDisabledColour; return *this;};
    InputField& setActiveColour(Colour _activeColour) {mActiveColour = _activeColour; return *this;};
    InputField& setActiveDisabledColour(Colour _activeDisabledColour) {mActiveDisabledColour = _activeDisabledColour; return *this;};
    InputField& setDisabledColour(Colour _disabledColour) {mDisabledColour = _disabledColour; return *this;};
    InputField& setDisabledHoverColour(Colour _disabledHoverColour) {mDisabledHoverColour = _disabledHoverColour; return *this;};
    InputField& setOptionsStartOffset(int _optionsStartOffset) {mOptionsStartOffset = _optionsStartOffset; return *this;};

    // Whether to render the checkbox primitive
    InputField& showCheckbox(bool _showCheckbox) {
        mShowCheckbox = _showCheckbox;
        return *this;
    }
    InputField& checkboxIcons(std::shared_ptr<Texture2D> _checkboxIconTrue, std::shared_ptr<Texture2D> _checkboxIconFalse) {
        mCheckboxIconTrue = _checkboxIconTrue;
        mCheckboxIconFalse = _checkboxIconFalse;
        return *this;
    }

    // isOptional only valid for double at the moment
    InputField& isOptional(bool isOptional) { mIsOptional = isOptional; return *this; }
    InputField& SetValue(double value) { mDoubleValue = value; WriteVar(); return *this; }
    InputField& SetValue(int value) { mIntValue = value; mBoolValue = value; WriteVar(); return *this; }
    InputField& SetValue(const std::string& text) { mTextValue = text; WriteVar(); return *this; }
    InputField& SetValue(const std::wstring& text) { mTextValue = WStringToString(text); WriteVar(); return *this; }  // TODO: Not needed?
    
    InputField& SetOptions(
        const std::vector<std::string>& options,
        const std::vector<std::string>& return_options = {},
        std::optional<std::reference_wrapper<std::string>> stringVarToChange = std::nullopt);
    InputField& SetState(State state) { mState = state; return *this; };
    State GetState() { return mState; };

    InputField& setMin(std::variant<double, int> min);
    InputField& setMax(std::variant<double, int> max);

    InputField& setRatio(double _ratio) { mRatio = _ratio; return *this; };

    using VarType = std::variant<std::reference_wrapper<double>, 
                                 std::reference_wrapper<int>, 
                                 std::reference_wrapper<bool>, 
                                 std::reference_wrapper<std::string>>;
       
    InputField& SetVar(VarType var) {
        mVar = std::make_shared<VarType>(std::move(var));
        std::visit([this](auto&& ref) {
            this->SetValue(ref.get()); // Call the appropriate overload of SetValue
        }, *mVar);
        return *this;
    }


    void MouseCallback(int _mouseState) override;
    void Draw(int x, int y, int width, int height, std::optional<float> z = std::nullopt);

    void SetPos(int x, int y, int width, int height, std::optional<float> z = std::nullopt);

    
    static void drawField(InputField &field,
                          int baseX,
                          int yOffset,
                          int fieldWidths,
                          int fieldHeight,
                          int yGap,
                          float zDraw,
                          int &row)
    {
        // Draw the field at (baseX, yOffset + row*(fieldHeight+yGap)) then increment row.
        field.Draw(baseX, yOffset + (fieldHeight + yGap) * row++, fieldWidths, fieldHeight, zDraw);
    }

    static void drawTripleFields(InputField &field1,
                                 InputField &field2,
                                 InputField &field3,
                                 int baseX,
                                 int yOffset,
                                 int fieldWidths,
                                 int fieldHeight,
                                 int yGap,
                                 float zDraw,
                                 int &row)
    {
        // Calculate a triple–field layout.
        const float tripleWidth = fieldWidths * 0.6f / 3.0f;
        int tripleBaseX = baseX + static_cast<int>(0.4f * fieldWidths) + 5;
        field1.Draw(tripleBaseX,                   yOffset + (fieldHeight + yGap) * row, tripleWidth,       fieldHeight, zDraw);
        field2.Draw(tripleBaseX + tripleWidth,     yOffset + (fieldHeight + yGap) * row, tripleWidth,       fieldHeight, zDraw);
        field3.Draw(tripleBaseX + 2 * tripleWidth, yOffset + (fieldHeight + yGap) * row, tripleWidth,       fieldHeight, zDraw);
        ++row;
    }

    void setPathSettings(std::string pathTitle, std::string pathDefault = "", std::vector<std::string> pathFilters = { "All Files", "*" }, bool pathAllowMultiples = false) {
        mPathTitle = pathTitle;
        mPathDefault = pathDefault;
        mPathFilters = pathFilters;
        mPathAllowMultiples = pathAllowMultiples;
    }



private:
    void WriteVar();
    void ReadVar();
    void HandleInput();
    void HandleMouseDragForNumericInput();
    void DrawCheckbox();
    void DrawOptions();
    void DrawEditableText();
    void DrawNumericText();

    UI* mUI;
    std::shared_ptr<Text> mTextRenderer;
    std::string mLabel;
    InputType mType;
    Boundary mLabelContainer;
    Boundary mInputContainer;

    Colour mDefaultColour = FIELD_COLOUR;
    Colour mHoverColour = FIELD_HOVER_COLOUR; 
    Colour mEditColour = FIELD_EDIT_COLOUR;
    Colour mEditDisabledColour = FIELD_EDIT_DISABLED_COLOUR;
    Colour mActiveColour = FIELD_ACTIVE_COLOUR;
    Colour mActiveDisabledColour = FIELD_ACTIVE_DISABLED_COLOUR;
    Colour mDisabledColour = FIELD_DISABLED_COLOUR;
    Colour mDisabledHoverColour = FIELD_DISABLED_HOVER_COLOUR;

    Colour mTextColour = FIELD_TEXT_COLOUR;
    State mState = ENABLED;
    bool mDropdownExpanded = false;
    bool mBoolValue = false;
    double mDoubleValue = 0.0;
    int mIntValue = 0;
    std::string mTextValue;
    std::string mUnits;
    Text::Align mLabelAlign;
    Text::Align mFieldAlign;    // TODO: Not currently implemented
    int mTextMargin = 10;
    Primitive rectPrim;
    int r = 5;                  // TODO: Don't make this hard coded?
    std::variant<double, int> mMin, mMax;
    std::shared_ptr<VarType> mVar = nullptr;  // External variable
    int mTextMarginX = 5;
    bool mShowPercent;
    double mRatio = 0.4;
    bool mShowCheckbox = true;
    std::shared_ptr<Texture2D> mCheckboxIconTrue = nullptr;
    std::shared_ptr<Texture2D> mCheckboxIconFalse = nullptr;
    bool mMouseInBounds = false;
    int mOptionsStartOffset = 0;  // E.G. Allows the options to start at 1 rather than 0
    bool mDialogOpen = false;
    bool mResultReady = false;
    std::unique_ptr<pfd::open_file> mDialog;
    std::string mPathTitle = "Open File";
    std::string mPathDefault = "";
    std::vector<std::string> mPathFilters = { "All Files", "*" };
    bool mPathAllowMultiples = false;
    std::optional<std::reference_wrapper<std::string>> mStringVarToChange;  // Used for dropdowns
    const std::vector<std::string>* mReturnOptions = nullptr;   // Used for dropdowns

    std::shared_ptr<TextField> mTextField = std::make_shared<TextField>(mUI);

    bool mouseDrag = false;
    bool mIsOptional = false;

    DropdownButton* dropDownBtn;
    std::vector<std::shared_ptr<Button>> optionBtns;
};

