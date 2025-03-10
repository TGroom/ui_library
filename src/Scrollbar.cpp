// Copyright (c) 2025 Thomas Groom


#include "ui_library/Scrollbar.h"


Scrollbar::Scrollbar(UI* _ui) : mUI(_ui) {
    layer = 1;  // Layer 1 is used for scroll bars

	mScrollBoxButton = new Button(mUI, UIText, L"", Text::CENTER_MIDDLE, {0, 0, 6, 100}, 6 / 2, 0.0f, 0, 0, 1);
}

void Scrollbar::MouseCallback(int _mouseState) {
    if (mUI->G_SCROLL_TRIGGER) {
        normalizedScrollOffset += -(mUI->G_SCROLL_Y * 50.0f) / mMaxScrollHeight;
    }
}

void Scrollbar::Draw(Boundary scrollContainer, int fullHeight, Colour col) {
    isDrawn = true;
    mContainer = scrollContainer;

	// TODO: Drag scroll bar button functionality

    mMaxScrollHeight = fmaxf(fullHeight - mContainer.height, 0);
    normalizedScrollOffset = fminf(fmaxf(normalizedScrollOffset, 0.0f), 1.0f);
    scrollOffset = normalizedScrollOffset * mMaxScrollHeight;

    // Calculate button position and size in normalized terms
    if (mContainer.height < fullHeight) {
        mScrollBoxButton->setColour(col);
        mScrollBoxButton->setHColour(col * 1.2f);
        mScrollBoxButton->SetPos(
            mContainer.x + mContainer.width - mScrollBoxWidth - 1,
            mContainer.y + (mContainer.height * scrollOffset) / fullHeight + mVertMargin, mScrollBoxWidth,
            (mContainer.height * mContainer.height) / fullHeight - (mVertMargin * 2) + 1
        );
        mScrollBoxButton->SetZ(mZ);
        mScrollBoxButton->Draw();
    }
}