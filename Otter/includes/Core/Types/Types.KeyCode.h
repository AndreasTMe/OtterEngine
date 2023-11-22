#ifndef OTTERENGINE_TYPES_KEYCODE_H
#define OTTERENGINE_TYPES_KEYCODE_H

#include "Core/Types/Types.Base.h"

// HELP: Using Windows Virtual-Key Codes
#define KEYCODE_LIST                    \
    REPLACE_WITH(Backspace,0x08)        \
    REPLACE_WITH(Tab,0x09)              \
    REPLACE_WITH(Clear,0x0C)            \
    REPLACE_WITH(Enter,0x0D)            \
    REPLACE_WITH(Shift,0x10)            \
    REPLACE_WITH(Ctrl,0x11)             \
    REPLACE_WITH(Alt,0x12)              \
    REPLACE_WITH(Pause,0x13)            \
    REPLACE_WITH(CapsLock,0x14)         \
    REPLACE_WITH(Esc,0x1B)              \
    REPLACE_WITH(Space,0x20)            \
    REPLACE_WITH(PageUp,0x21)           \
    REPLACE_WITH(PageDown,0x22)         \
    REPLACE_WITH(End,0x23)              \
    REPLACE_WITH(Home,0x24)             \
                                        \
    /*  Arrow Keys  */                  \
    REPLACE_WITH(Left,0x25)             \
    REPLACE_WITH(Up,0x26)               \
    REPLACE_WITH(Right,0x27)            \
    REPLACE_WITH(Down,0x28)             \
                                        \
    REPLACE_WITH(Select,0x29)           \
    REPLACE_WITH(Print,0x2A)            \
    REPLACE_WITH(Execute,0x2B)          \
    REPLACE_WITH(PrintScreen,0x2C)      \
    REPLACE_WITH(Insert,0x2D)           \
    REPLACE_WITH(Delete,0x2E)           \
    REPLACE_WITH(Help,0x2F)             \
                                        \
    /*  Number Keys  */                 \
    REPLACE_WITH(Num0,0x30)             \
    REPLACE_WITH(Num1,0x31)             \
    REPLACE_WITH(Num2,0x32)             \
    REPLACE_WITH(Num3,0x33)             \
    REPLACE_WITH(Num4,0x34)             \
    REPLACE_WITH(Num5,0x35)             \
    REPLACE_WITH(Num6,0x36)             \
    REPLACE_WITH(Num7,0x37)             \
    REPLACE_WITH(Num8,0x38)             \
    REPLACE_WITH(Num9,0x39)             \
                                        \
    /*  Letter Keys  */                 \
    REPLACE_WITH(KeyA,0x41)             \
    REPLACE_WITH(KeyB,0x42)             \
    REPLACE_WITH(KeyC,0x43)             \
    REPLACE_WITH(KeyD,0x44)             \
    REPLACE_WITH(KeyE,0x45)             \
    REPLACE_WITH(KeyF,0x46)             \
    REPLACE_WITH(KeyG,0x47)             \
    REPLACE_WITH(KeyH,0x48)             \
    REPLACE_WITH(KeyI,0x49)             \
    REPLACE_WITH(KeyJ,0x4A)             \
    REPLACE_WITH(KeyK,0x4B)             \
    REPLACE_WITH(KeyL,0x4C)             \
    REPLACE_WITH(KeyM,0x4D)             \
    REPLACE_WITH(KeyN,0x4E)             \
    REPLACE_WITH(KeyO,0x4F)             \
    REPLACE_WITH(KeyP,0x50)             \
    REPLACE_WITH(KeyQ,0x51)             \
    REPLACE_WITH(KeyR,0x52)             \
    REPLACE_WITH(KeyS,0x53)             \
    REPLACE_WITH(KeyT,0x54)             \
    REPLACE_WITH(KeyU,0x55)             \
    REPLACE_WITH(KeyV,0x56)             \
    REPLACE_WITH(KeyW,0x57)             \
    REPLACE_WITH(KeyX,0x58)             \
    REPLACE_WITH(KeyY,0x59)             \
    REPLACE_WITH(KeyZ,0x5A)             \
                                        \
    /*  NumPad Keys  */                 \
    REPLACE_WITH(Numpad0,0x60)          \
    REPLACE_WITH(Numpad1,0x61)          \
    REPLACE_WITH(Numpad2,0x62)          \
    REPLACE_WITH(Numpad3,0x63)          \
    REPLACE_WITH(Numpad4,0x64)          \
    REPLACE_WITH(Numpad5,0x65)          \
    REPLACE_WITH(Numpad6,0x66)          \
    REPLACE_WITH(Numpad7,0x67)          \
    REPLACE_WITH(Numpad8,0x68)          \
    REPLACE_WITH(Numpad9,0x69)          \
    REPLACE_WITH(NumpadMultiply,0x6A)   \
    REPLACE_WITH(NumpadAdd,0x6B)        \
    REPLACE_WITH(NumpadSeparator,0x6C)  \
    REPLACE_WITH(NumpadSubtract,0x6D)   \
    REPLACE_WITH(NumpadDecimal,0x6E)    \
    REPLACE_WITH(NumpadDivide,0x6F)     \
                                        \
    /*  F1-F24 Keys  */                 \
    REPLACE_WITH(F1,0x70)               \
    REPLACE_WITH(F2,0x71)               \
    REPLACE_WITH(F3,0x72)               \
    REPLACE_WITH(F4,0x73)               \
    REPLACE_WITH(F5,0x74)               \
    REPLACE_WITH(F6,0x75)               \
    REPLACE_WITH(F7,0x76)               \
    REPLACE_WITH(F8,0x77)               \
    REPLACE_WITH(F9,0x78)               \
    REPLACE_WITH(F10,0x79)              \
    REPLACE_WITH(F11,0x7A)              \
    REPLACE_WITH(F12,0x7B)              \
    REPLACE_WITH(F13,0x7C)              \
    REPLACE_WITH(F14,0x7D)              \
    REPLACE_WITH(F15,0x7E)              \
    REPLACE_WITH(F16,0x7F)              \
    REPLACE_WITH(F17,0x80)              \
    REPLACE_WITH(F18,0x81)              \
    REPLACE_WITH(F19,0x82)              \
    REPLACE_WITH(F20,0x83)              \
    REPLACE_WITH(F21,0x84)              \
    REPLACE_WITH(F22,0x85)              \
    REPLACE_WITH(F23,0x86)              \
    REPLACE_WITH(F24,0x87)              \
                                        \
    REPLACE_WITH(NumLock,0x90)          \
    REPLACE_WITH(ScrollLock,0x91)       \
                                        \
    /*  Shift/Ctrl/Alt  */              \
    REPLACE_WITH(LeftShift,0xA0)        \
    REPLACE_WITH(RightShift,0xA1)       \
    REPLACE_WITH(LeftCtrl,0xA2)         \
    REPLACE_WITH(RightCtrl,0xA3)        \
    REPLACE_WITH(LeftAlt,0xA4)          \
    REPLACE_WITH(RightAlt,0xA5)         \
                                        \
    /*  Symbol Keys  */                 \
    REPLACE_WITH(Semicolon,0xBA)        \
    REPLACE_WITH(Equal,0xBB)            \
    REPLACE_WITH(Comma,0xBC)            \
    REPLACE_WITH(Minus,0xBD)            \
    REPLACE_WITH(Period,0xBE)           \
    REPLACE_WITH(ForwardSlash,0xBF)     \
    REPLACE_WITH(Grave,0xC0)            \
    REPLACE_WITH(LeftBracket,0xDB)      \
    REPLACE_WITH(Backslash,0xDC)        \
    REPLACE_WITH(RightBracket,0xDD)     \
    REPLACE_WITH(Apostrophe,0xDE)       \
    REPLACE_WITH(Tilde,0xE7)

namespace Otter
{
    enum class KeyCode : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        KEYCODE_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::KeyCode& keyCode)
{
    switch (keyCode)
    {
#define REPLACE_WITH(Item, Value) case Otter::KeyCode::Item: os << "KeyCode::" << #Item; break;
        KEYCODE_LIST
#undef REPLACE_WITH
        default:
            os << "KeyCode[Unknown]";
    }

    return os;
}
#undef KEYCODE_LIST

#endif //OTTERENGINE_TYPES_KEYCODE_H
