// Copyright (c) 2025 Thomas Groom


#include "ui_library/Button.h"


Button::Button(UI* _ui, std::shared_ptr<Text> tr, std::wstring _text, Text::Align align, Boundary container,
    int r, float z, int textx, int texty, Colour _Colour, Colour _HColour, Colour _DColour, Colour _DHColour)
    : mUI(_ui), mTextRenderer(tr), mMainColour(_Colour), mHoverColour(_HColour), mDisabledColour(_DColour), mDisabledHoverColour(_DHColour), mAlign(align), mRadius(r), mTextx(textx), mTexty(texty), mText(_text)
    {
    mZ = z;
    mContainer = container;

	rectPrim.Rect(mContainer.x, mContainer.y, mContainer.width, mContainer.height, r, mZ);  // TODO: Rect should use a boundary as input
}
  

Button& Button::setIcon(std::shared_ptr<Texture2D> texture, int iconX, int iconY, int iconW, int iconH) {
    mIconX = iconX;
    mIconY = iconY;
    mIconW = iconW;
    mIconH = iconH;
    mIconTexture = texture;
    return *this;
}


void Button::MouseCallback(int _mouseState) {
    mInContainer = true;
    if(mActiveState != DISABLED) {
        if (_mouseState == mTriggerEdge && isMouseInBounds(mUI, &mContainer)) {
            if (onClickCallback) onClickCallback(); // TODO: Perhaps move this to the draw call so that only buttons which are drawn are actually functional
            btnState = 4;  // Button has been clicked
            return;
        }
        btnState = 3;  // Mouse is over the button but not yet clicked (hover state)
        return;
    }
	btnState = 2; // Mouse is over the button, but it's disabled (disabled state)
}


void Button::Draw(){
    isDrawn = true;

    if (!mInContainer){
        // 0: Mouse is far away from the button (inactive or idle state)
	    // 1: Mouse is outside the button but not far enough to be considered idle
        btnState = isMouseInBounds(mUI, &mContainer, -100) ? 1 : 0;
    }

    if (btnState >= 2) {
        isBold = true;
    }

    rectPrim.Rect(mContainer.x, mContainer.y, mContainer.width, mContainer.height, mRadius, mZ);
    if (mActiveState == ENABLED) {
        rectPrim.SetColour(isBold ? mHoverColour : mMainColour);
    } else {
        rectPrim.SetColour(isBold ? mDisabledHoverColour : mDisabledColour);
    }
	rectPrim.Draw();

	glm::ivec2 boundingBox = mTextRenderer->boundingBox(mText);
	glm::ivec2 centerOfButton = {mContainer.x + mContainer.width / 2, mContainer.y + mContainer.height / 2};

    mTextColour = mActiveState != ENABLED ? FIELD_TEXT_DISABLED_COLOUR : FIELD_TEXT_COLOUR;
    int textWidth = mTextWidth > -1 ? mTextWidth : mContainer.width - (m_textMargin * 2) - mTextx;  // Optional text width & height
    int textHeight = mTextHeight > -1 ? mTextHeight : mContainer.height - mTexty;
    mTextRenderer->RenderText(mText, {mContainer.x + mTextx + m_textMargin, mContainer.y + mTexty, textWidth, textHeight}, mZ + 0.002f, mAlign, mTextColour);

    
	// Draw the button icon if texture is available
    if (mIconTexture) {
        mIconTexture->DrawSprite(glm::vec2(mContainer.x + mIconX, mContainer.y + mIconY), glm::vec2(mIconW, mIconH), mZ + 0.001f);
    }
	isBold = false;
    mInContainer = false;
}


Button& Button::SetPos(int x, int y) {
    mContainer.x = x;
    mContainer.y = y;
    return *this;
}


Button& Button::SetPos(int x, int y, int width, int height) {
    mContainer.x = x;
    mContainer.y = y;
    mContainer.width = width;
    mContainer.height = height;
    return *this;
}


Button& Button::SetZ(float z) {
    mZ = z;
    return *this;
}