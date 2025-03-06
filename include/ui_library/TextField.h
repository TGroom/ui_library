// Copyright (c) 2025 Thomas Groom

#ifndef TEXTFIELD_H
#define TEXTFIELD_H

#include <string>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <codecvt>
#include <locale>
#include "Utils.h"



class TextField {
public:
    std::wstring content;
    int caretPos = 0;
    int selStart = 0;
    int selEnd = 0;
    bool isActive = false;
    int caretVisibilityCounter = 0;
    int caretBlinkRate = 30;

    // The constructor takes a GLFWwindow pointer.
    TextField(UI* _ui) : mUI(_ui) {}

    void handleInput() {
        if (isActive) {
            // --- OS Clipboard Functionality using GLFW ---
            // Ctrl+C: Copy selected text to OS clipboard.
            if (mUI->G_CTRL_C_PRESS) {
                if (selStart != selEnd) {
                    if (selStart > selEnd) std::swap(selStart, selEnd);
                    std::wstring selected = content.substr(selStart, selEnd - selStart);
                    std::string selected_utf8 = wstring_to_utf8(selected);
                    glfwSetClipboardString(G_WINDOW, selected_utf8.c_str());
                }
            }

            // Ctrl+X: Cut selected text (copy to clipboard then delete selection).
            if (mUI->G_CTRL_X_PRESS) {
                if (selStart != selEnd) {
                    if (selStart > selEnd) std::swap(selStart, selEnd);
                    std::wstring selected = content.substr(selStart, selEnd - selStart);
                    std::string selected_utf8 = wstring_to_utf8(selected);
                    glfwSetClipboardString(G_WINDOW, selected_utf8.c_str());
                    deleteSelection();
                }
            }

            // Ctrl+V: Paste clipboard text (replace any selected text).
            if (mUI->G_CTRL_V_PRESS) {
                const char* clipboard_text = glfwGetClipboardString(G_WINDOW);
                if (clipboard_text) {
                    std::wstring paste_text = utf8_to_wstring(std::string(clipboard_text));
                    if (selStart != selEnd) {
                        deleteSelection();
                    }
                    content.insert(caretPos, paste_text);
                    caretPos += static_cast<int>(paste_text.size());
                    resetSelection();
                }
            }
            // ------------------------------------------------

            // Character input.
            if (mUI->G_CHAR_CALLBACK_FLAG) {
                if (selStart != selEnd) deleteSelection();
                content.insert(content.begin() + caretPos, static_cast<wchar_t>(mUI->G_CHAR_INPUT));
                caretPos++;
                resetSelection();
            }

            // Backspace and Delete.
            if (mUI->G_BACKSPACE_PRESS || mUI->G_DELETE_PRESS) {
                if (selStart != selEnd) {
                    deleteSelection();
                } else if (mUI->G_BACKSPACE_PRESS && caretPos > 0) {
                    content.erase(caretPos - 1, 1);
                    caretPos--;
                } else if (mUI->G_DELETE_PRESS && caretPos < content.size()) {
                    content.erase(caretPos, 1);
                }
                resetSelection();
            }

            // Arrow keys.
            if (mUI->G_LEFT_ARROW_PRESS) caretPos = std::max(0, caretPos - 1);
            if (mUI->G_RIGHT_ARROW_PRESS) caretPos = std::min(static_cast<int>(content.size()), caretPos + 1);

            // Update selection with Shift.
            if (mUI->G_SHIFT_PRESS && selStart == selEnd) {
                selStart = caretPos;
            }
            if (mUI->G_SHIFT_DOWN) {
                selEnd = caretPos;
            } else {
                if (mUI->G_LEFT_ARROW_PRESS || mUI->G_RIGHT_ARROW_PRESS || mUI->G_HOME_PRESS || mUI->G_END_PRESS) {
                    resetSelection();
                }
            }

            // Home and End keys.
            if (mUI->G_HOME_PRESS) caretPos = 0;
            if (mUI->G_END_PRESS) caretPos = static_cast<int>(content.size());

            // Blink cursor.
            caretVisibilityCounter = (caretVisibilityCounter + 1) % (caretBlinkRate * 2);
        } else {
            caretVisibilityCounter = 0;
            resetSelection();
        }
    }

    void deleteSelection() {
        if (selStart > selEnd) std::swap(selStart, selEnd);
        content.erase(selStart, selEnd - selStart);
        caretPos = selStart;
        resetSelection();
    }

    void resetSelection() {
        selStart = caretPos;
        selEnd = caretPos;
    }
private:
    UI* mUI;
};

#endif // TEXTFIELD_H
