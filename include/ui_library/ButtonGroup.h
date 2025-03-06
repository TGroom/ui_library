// Copyright (c) 2025 Thomas Groom

#pragma once

#include <glad/glad.h>
#include <cmath>
#include <vector>
#include "Utils.h"
#include "Text.h"
#include "Button.h"


class ButtonGroup
{
	public:
		ButtonGroup() {}; //: isExpended(false)

		void AddButton(std::shared_ptr<Button> button) {
			mChildButtons.push_back(button);
		}

		void Draw() {
			for (std::shared_ptr<Button> button : mChildButtons) {
				button->Draw();
			}

			//  [ ] TODO: Make this entierly optional to change the functionality of dropdowns to work together. I.E. Make this do the Blender thing
			//// --- Handle Clicks ---
			//bool leftAOE = true;
			//
			//for (size_t i = 0; i < mChildButtons.size(); ++i) {
			//	Button* button = mChildButtons[i];
			//	int buttonState = button->btnState;
			//	if (buttonState >= 3 && G_LEFT_MOUSE_STATE == GLFW_PRESS) {
			//		isExpended = true;
			//		firstClick = true;
			//	}
			//	if (buttonState > 0) leftAOE = false;
			//}
			//if (leftAOE) isExpended = false;
			//
			//if (isExpended) {
			//	for (size_t i = 0; i < mChildButtons.size(); ++i) {
			//		Button* button = mChildButtons[i];
			//		int buttonState = button->btnState;
			//		if (buttonState >= 3) {
			//			for (Button* button : mChildButtons) button->mExpanded = Button::SIMPLE;
			//			button->mExpanded = Button::DROPDOWN;
			//		}
			//	}
			//} else {
			//	for (Button* button : mChildButtons) button->mExpanded = Button::SIMPLE;
			//}
			//
			//// TODO Personal Preference?: Take into account the dropdown buttons and which one is currently open. If clicking outside the dropdown area then GLFW_PRESS closes isExpanded,els GLFW_RELEASE closes isExpanded
			//if (isExpended && G_LEFT_MOUSE_STATE == GLFW_RELEASE) {
			//	if (!firstClick) isExpended = false;
			//	firstClick = false;
			//}
		}

	private:
		//bool isExpended;
		//bool firstClick = false;
		std::vector<std::shared_ptr<Button>> mChildButtons;
};

