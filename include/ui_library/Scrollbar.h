// Copyright (c) 2025 Thomas Groom

#pragma once

#include <glad/glad.h>
#include <cmath>
#include "Utils.h"
#include "Button.h"
#include "ui_library/Config.h"

class Scrollbar : public MouseHandler
{
	public:
		Scrollbar(UI* _ui);
		void Draw(Boundary scrollContainer, int fullHeight, Colour col = BUTTON_COLOUR);
		void MouseCallback(int _mouseState) override;

		int scrollOffset = 0;

	private:
		UI* mUI;
		float normalizedScrollOffset = 0.0f;
		int mVertMargin = 3;
		int mScrollBoxWidth = 6;
		int mMaxScrollHeight = 0;
		std::shared_ptr<Text> UIText = std::make_shared<Text>(std::string(UI_LIBRARY_RESOURCES_DIR) + "/fonts/arial.ttf", 12);  // Technically not needed
		Button* mScrollBoxButton;
};

