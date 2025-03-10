// Copyright (c) 2025 Thomas Groom

#pragma once

#include <glad/glad.h>
#include <cmath>
#include <vector>
#include <functional> // Required for std::function
#include <filesystem>
#include <future>
#include <optional>

#include "Utils.h"
#include "Text.h"
#include "Texture.h"


class Button : public MouseHandler
{
	public:
		// TODO: Perhaps use glfwGetCursorPos(window, &G_MOUSE_X, &G_MOUSE_Y); in isMouseInBounds to avoid UI injection here
		Button(UI* _ui, std::shared_ptr<Text> tr, std::wstring _text, Text::Align align = Text::CENTER_MIDDLE,  
			Boundary container = {0, 0, 50, 20}, int r = 5, float z = 0.0f, int _textx = 0, int _texty = 0,
			Colour _Colour = BUTTON_COLOUR, Colour _HColour = BUTTON_HOVER_COLOUR, Colour _DColour = BUTTON_DISABLED_COLOUR,
			Colour _DHColour = BUTTON_DISABLED_HOVER_COLOUR);
		~Button() {};
		void MouseCallback(int _mouseState) override;
		//virtual void Register();
		virtual void Draw();
		Button& setIcon(std::shared_ptr<Texture2D> texture, int iconX = 5, int iconY = 0, int iconW = 20, int iconH = 20);
		Button& setIconPos(int iconX = 5, int iconY = 0, int iconW = 20, int iconH = 20) {
			mIconX = iconX;
			mIconY = iconY;
			mIconW = iconW;
			mIconH = iconH;
			return *this;
		};
		Button& SetPos(int x, int y);
		Button& SetPos(int x, int y, int width, int height);
		Button& Button::SetZ(float z);
		//void setTextColour(Colour col) { mTextCol = col; };
		virtual Button& setText(std::wstring _text) { mText = _text; return *this;};  // TODO: Replace all external mText use with this setter so that mText can be a private variable
		virtual Button& setColour(Colour col) { mMainColour = col; return *this;};
		virtual Button& setHColour(Colour col) { mHoverColour = col; return *this;};
		virtual Button& setDColour(Colour col) { mDisabledColour = col; return *this;};
		virtual Button& setDHColour(Colour col) { mDisabledHoverColour = col; return *this;};
		Button& setCorners(std::vector<bool> corners) { rectPrim.SetCorners(corners); return *this;};
		Button& setState(State state) { mActiveState = state; return *this;};  // TODO: Rename functions as setState and getState don't set and get the same thing!!!
		int getState() { return btnState; };
		Button& setTriggerEdge(int _triggerEdge) { mTriggerEdge = _triggerEdge; return *this;};

		Button& SetOnClickCallback(const std::function<void()>& callback) {
			if (!callback) {
				printf("Warning: Passed callback is empty.\n");
			}
			onClickCallback = callback;
			return *this;
		}

		void runCallback() {
			if (onClickCallback) onClickCallback();
		}

		UI* mUI;
		Colour mMainColour;
		Colour mHoverColour;
		Colour mDisabledColour;
		Colour mDisabledHoverColour;
		Colour mTextColour = FIELD_TEXT_COLOUR;
		std::wstring mText;
		int mTextx, mTexty, mIconX, mIconY, mIconW, mIconH, mRadius;  // TODO: Make these boundary structs isntead: (text x, y, w, h), (mIconX, mIconY, mIconW, mIconH)
		int mTextWidth = -1;
		int mTextHeight = -1;
		std::shared_ptr<Text> mTextRenderer;
	private:
		bool mInContainer = false;
		bool isBold = false;
		int btnState = 0;
		std::function<void()> onClickCallback;
		int mTriggerEdge = GLFW_PRESS;
		Text::Align mAlign;
		State mActiveState = ENABLED;
		bool flag = false;
		int m_textMargin = 2;

		std::filesystem::path mIconPath = L"";

		Primitive rectPrim;
		std::shared_ptr<Texture2D> mIconTexture;
};

