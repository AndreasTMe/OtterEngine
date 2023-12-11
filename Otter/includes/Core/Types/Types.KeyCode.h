#ifndef OTTERENGINE_TYPES_KEYCODE_H
#define OTTERENGINE_TYPES_KEYCODE_H

#include "Core/Defines/Defines.Utilities.h"
#include "Core/Types/Types.Base.h"

namespace Otter
{
    /**
     * @brief The mouse button type. Its size is 1 byte and its values are the same as the Windows Virtual-Key Codes.
     */
    enum class KeyCode : UInt8
    {
        /// @brief No key.
        None = 0x00,

        /// @brief The backspace key.
        Backspace = 0x08,

        /// @brief The tab key.
        Tab = 0x09,

        /// @brief The clear key.
        Clear = 0x0C,

        /// @brief The enter key.
        Enter = 0x0D,

        /// @brief The shift key.
        Shift = 0x10,

        /// @brief The control key.
        Ctrl = 0x11,

        /// @brief The alt key.
        Alt = 0x12,

        /// @brief The pause key.
        Pause = 0x13,

        /// @brief The caps lock key.
        CapsLock = 0x14,

        /// @brief The escape key.
        Esc = 0x1B,

        /// @brief The space bar.
        Space = 0x20,

        /// @brief The page up key.
        PageUp = 0x21,

        /// @brief The page down key.
        PageDown = 0x22,

        /// @brief The end key.
        End = 0x23,

        /// @brief The home key.
        Home = 0x24,

        /// @brief The left arrow key.
        Left = 0x25,

        /// @brief The up arrow key.
        Up = 0x26,

        /// @brief The right arrow key.
        Right = 0x27,

        /// @brief The down arrow key.
        Down = 0x28,

        /// @brief The select key.
        Select = 0x29,

        /// @brief The print key.
        Print = 0x2A,

        /// @brief The execute key.
        Execute = 0x2B,

        /// @brief The print screen key.
        PrintScreen = 0x2C,

        /// @brief The insert key.
        Insert = 0x2D,

        /// @brief The delete key.
        Delete = 0x2E,

        /// @brief The help key.
        Help = 0x2F,

        /// @brief The 0 key.
        Num0 = 0x30,

        /// @brief The 1 key.
        Num1 = 0x31,

        /// @brief The 2 key.
        Num2 = 0x32,

        /// @brief The 3 key.
        Num3 = 0x33,

        /// @brief The 4 key.
        Num4 = 0x34,

        /// @brief The 5 key.
        Num5 = 0x35,

        /// @brief The 6 key.
        Num6 = 0x36,

        /// @brief The 7 key.
        Num7 = 0x37,

        /// @brief The 8 key.
        Num8 = 0x38,

        /// @brief The 9 key.
        Num9 = 0x39,

        /// @brief The A key.
        KeyA = 0x41,

        /// @brief The B key.
        KeyB = 0x42,

        /// @brief The C key.
        KeyC = 0x43,

        /// @brief The D key.
        KeyD = 0x44,

        /// @brief The E key.
        KeyE = 0x45,

        /// @brief The F key.
        KeyF = 0x46,

        /// @brief The G key.
        KeyG = 0x47,

        /// @brief The H key.
        KeyH = 0x48,

        /// @brief The I key.
        KeyI = 0x49,

        /// @brief The J key.
        KeyJ = 0x4A,

        /// @brief The K key.
        KeyK = 0x4B,

        /// @brief The L key.
        KeyL = 0x4C,

        /// @brief The M key.
        KeyM = 0x4D,

        /// @brief The N key.
        KeyN = 0x4E,

        /// @brief The O key.
        KeyO = 0x4F,

        /// @brief The P key.
        KeyP = 0x50,

        /// @brief The Q key.
        KeyQ = 0x51,

        /// @brief The R key.
        KeyR = 0x52,

        /// @brief The S key.
        KeyS = 0x53,

        /// @brief The T key.
        KeyT = 0x54,

        /// @brief The U key.
        KeyU = 0x55,

        /// @brief The V key.
        KeyV = 0x56,

        /// @brief The W key.
        KeyW = 0x57,

        /// @brief The X key.
        KeyX = 0x58,

        /// @brief The Y key.
        KeyY = 0x59,

        /// @brief The Z key.
        KeyZ = 0x5A,

        /// @brief The 0 key on the numpad.
        Numpad0 = 0x60,

        /// @brief The 1 key on the numpad.
        Numpad1 = 0x61,

        /// @brief The 2 key on the numpad.
        Numpad2 = 0x62,

        /// @brief The 3 key on the numpad.
        Numpad3 = 0x63,

        /// @brief The 4 key on the numpad.
        Numpad4 = 0x64,

        /// @brief The 5 key on the numpad.
        Numpad5 = 0x65,

        /// @brief The 6 key on the numpad.
        Numpad6 = 0x66,

        /// @brief The 7 key on the numpad.
        Numpad7 = 0x67,

        /// @brief The 8 key on the numpad.
        Numpad8 = 0x68,

        /// @brief The 9 key on the numpad.
        Numpad9 = 0x69,

        /// @brief The multiply key on the numpad.
        NumpadMultiply = 0x6A,

        /// @brief The add key on the numpad.
        NumpadAdd = 0x6B,

        /// @brief The separator key on the numpad.
        NumpadSeparator = 0x6C,

        /// @brief The subtract key on the numpad.
        NumpadSubtract = 0x6D,

        /// @brief The decimal key on the numpad.
        NumpadDecimal = 0x6E,

        /// @brief The divide key on the numpad.
        NumpadDivide = 0x6F,

        /// @brief The F1 key.
        F1 = 0x70,

        /// @brief The F2 key.
        F2 = 0x71,

        /// @brief The F3 key.
        F3 = 0x72,

        /// @brief The F4 key.
        F4 = 0x73,

        /// @brief The F5 key.
        F5 = 0x74,

        /// @brief The F6 key.
        F6 = 0x75,

        /// @brief The F7 key.
        F7 = 0x76,

        /// @brief The F8 key.
        F8 = 0x77,

        /// @brief The F9 key.
        F9 = 0x78,

        /// @brief The F10 key.
        F10 = 0x79,

        /// @brief The F11 key.
        F11 = 0x7A,

        /// @brief The F12 key.
        F12 = 0x7B,

        /// @brief The F13 key.
        F13 = 0x7C,

        /// @brief The F14 key.
        F14 = 0x7D,

        /// @brief The F15 key.
        F15 = 0x7E,

        /// @brief The F16 key.
        F16 = 0x7F,

        /// @brief The F17 key.
        F17 = 0x80,

        /// @brief The F18 key.
        F18 = 0x81,

        /// @brief The F19 key.
        F19 = 0x82,

        /// @brief The F20 key.
        F20 = 0x83,

        /// @brief The F21 key.
        F21 = 0x84,

        /// @brief The F22 key.
        F22 = 0x85,

        /// @brief The F23 key.
        F23 = 0x86,

        /// @brief The F24 key.
        F24 = 0x87,

        /// @brief The num lock key.
        NumLock = 0x90,

        /// @brief The scroll lock key.
        ScrollLock = 0x91,

        /// @brief The left shift key.
        LeftShift = 0xA0,

        /// @brief The right shift key.
        RightShift = 0xA1,

        /// @brief The left control key.
        LeftCtrl = 0xA2,

        /// @brief The right control key.
        RightCtrl = 0xA3,

        /// @brief The left alt key.
        LeftAlt = 0xA4,

        /// @brief The right alt key.
        RightAlt = 0xA5,

        /// @brief The semicolon key.
        Semicolon = 0xBA,

        /// @brief The equal key.
        Equal = 0xBB,

        /// @brief The comma key.
        Comma = 0xBC,

        /// @brief The minus key.
        Minus = 0xBD,

        /// @brief The period key.
        Period = 0xBE,

        /// @brief The forward slash key.
        ForwardSlash = 0xBF,

        /// @brief The grave key.
        Grave = 0xC0,

        /// @brief The left bracket key.
        LeftBracket = 0xDB,

        /// @brief The backslash key.
        Backslash = 0xDC,

        /// @brief The right bracket key.
        RightBracket = 0xDD,

        /// @brief The apostrophe key.
        Apostrophe = 0xDE,

        /// @brief The tilde key.
        Tilde = 0xE7,

        /// @brief The maximum value of the enum.
        Max = 0xFF
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::KeyCode& keyCode)
{
    switch (keyCode)
    {
        case Otter::KeyCode::None:
            os << OTR_NAME_OF(Otter::KeyCode::None);
            break;
        case Otter::KeyCode::Backspace:
            os << OTR_NAME_OF(Otter::KeyCode::Backspace);
            break;
        case Otter::KeyCode::Tab:
            os << OTR_NAME_OF(Otter::KeyCode::Tab);
            break;
        case Otter::KeyCode::Clear:
            os << OTR_NAME_OF(Otter::KeyCode::Clear);
            break;
        case Otter::KeyCode::Enter:
            os << OTR_NAME_OF(Otter::KeyCode::Enter);
            break;
        case Otter::KeyCode::Shift:
            os << OTR_NAME_OF(Otter::KeyCode::Shift);
            break;
        case Otter::KeyCode::Ctrl:
            os << OTR_NAME_OF(Otter::KeyCode::Ctrl);
            break;
        case Otter::KeyCode::Alt:
            os << OTR_NAME_OF(Otter::KeyCode::Alt);
            break;
        case Otter::KeyCode::Pause:
            os << OTR_NAME_OF(Otter::KeyCode::Pause);
            break;
        case Otter::KeyCode::CapsLock:
            os << OTR_NAME_OF(Otter::KeyCode::CapsLock);
            break;
        case Otter::KeyCode::Esc:
            os << OTR_NAME_OF(Otter::KeyCode::Esc);
            break;
        case Otter::KeyCode::Space:
            os << OTR_NAME_OF(Otter::KeyCode::Space);
            break;
        case Otter::KeyCode::PageUp:
            os << OTR_NAME_OF(Otter::KeyCode::PageUp);
            break;
        case Otter::KeyCode::PageDown:
            os << OTR_NAME_OF(Otter::KeyCode::PageDown);
            break;
        case Otter::KeyCode::End:
            os << OTR_NAME_OF(Otter::KeyCode::End);
            break;
        case Otter::KeyCode::Home:
            os << OTR_NAME_OF(Otter::KeyCode::Home);
            break;
        case Otter::KeyCode::Left:
            os << OTR_NAME_OF(Otter::KeyCode::Left);
            break;
        case Otter::KeyCode::Up:
            os << OTR_NAME_OF(Otter::KeyCode::Up);
            break;
        case Otter::KeyCode::Right:
            os << OTR_NAME_OF(Otter::KeyCode::Right);
            break;
        case Otter::KeyCode::Down:
            os << OTR_NAME_OF(Otter::KeyCode::Down);
            break;
        case Otter::KeyCode::Select:
            os << OTR_NAME_OF(Otter::KeyCode::Select);
            break;
        case Otter::KeyCode::Print:
            os << OTR_NAME_OF(Otter::KeyCode::Print);
            break;
        case Otter::KeyCode::Execute:
            os << OTR_NAME_OF(Otter::KeyCode::Execute);
            break;
        case Otter::KeyCode::PrintScreen:
            os << OTR_NAME_OF(Otter::KeyCode::PrintScreen);
            break;
        case Otter::KeyCode::Insert:
            os << OTR_NAME_OF(Otter::KeyCode::Insert);
            break;
        case Otter::KeyCode::Delete:
            os << OTR_NAME_OF(Otter::KeyCode::Delete);
            break;
        case Otter::KeyCode::Help:
            os << OTR_NAME_OF(Otter::KeyCode::Help);
            break;
        case Otter::KeyCode::Num0:
            os << OTR_NAME_OF(Otter::KeyCode::Num0);
            break;
        case Otter::KeyCode::Num1:
            os << OTR_NAME_OF(Otter::KeyCode::Num1);
            break;
        case Otter::KeyCode::Num2:
            os << OTR_NAME_OF(Otter::KeyCode::Num2);
            break;
        case Otter::KeyCode::Num3:
            os << OTR_NAME_OF(Otter::KeyCode::Num3);
            break;
        case Otter::KeyCode::Num4:
            os << OTR_NAME_OF(Otter::KeyCode::Num4);
            break;
        case Otter::KeyCode::Num5:
            os << OTR_NAME_OF(Otter::KeyCode::Num5);
            break;
        case Otter::KeyCode::Num6:
            os << OTR_NAME_OF(Otter::KeyCode::Num6);
            break;
        case Otter::KeyCode::Num7:
            os << OTR_NAME_OF(Otter::KeyCode::Num7);
            break;
        case Otter::KeyCode::Num8:
            os << OTR_NAME_OF(Otter::KeyCode::Num8);
            break;
        case Otter::KeyCode::Num9:
            os << OTR_NAME_OF(Otter::KeyCode::Num9);
            break;
        case Otter::KeyCode::KeyA:
            os << OTR_NAME_OF(Otter::KeyCode::KeyA);
            break;
        case Otter::KeyCode::KeyB:
            os << OTR_NAME_OF(Otter::KeyCode::KeyB);
            break;
        case Otter::KeyCode::KeyC:
            os << OTR_NAME_OF(Otter::KeyCode::KeyC);
            break;
        case Otter::KeyCode::KeyD:
            os << OTR_NAME_OF(Otter::KeyCode::KeyD);
            break;
        case Otter::KeyCode::KeyE:
            os << OTR_NAME_OF(Otter::KeyCode::KeyE);
            break;
        case Otter::KeyCode::KeyF:
            os << OTR_NAME_OF(Otter::KeyCode::KeyF);
            break;
        case Otter::KeyCode::KeyG:
            os << OTR_NAME_OF(Otter::KeyCode::KeyG);
            break;
        case Otter::KeyCode::KeyH:
            os << OTR_NAME_OF(Otter::KeyCode::KeyH);
            break;
        case Otter::KeyCode::KeyI:
            os << OTR_NAME_OF(Otter::KeyCode::KeyI);
            break;
        case Otter::KeyCode::KeyJ:
            os << OTR_NAME_OF(Otter::KeyCode::KeyJ);
            break;
        case Otter::KeyCode::KeyK:
            os << OTR_NAME_OF(Otter::KeyCode::KeyK);
            break;
        case Otter::KeyCode::KeyL:
            os << OTR_NAME_OF(Otter::KeyCode::KeyL);
            break;
        case Otter::KeyCode::KeyM:
            os << OTR_NAME_OF(Otter::KeyCode::KeyM);
            break;
        case Otter::KeyCode::KeyN:
            os << OTR_NAME_OF(Otter::KeyCode::KeyN);
            break;
        case Otter::KeyCode::KeyO:
            os << OTR_NAME_OF(Otter::KeyCode::KeyO);
            break;
        case Otter::KeyCode::KeyP:
            os << OTR_NAME_OF(Otter::KeyCode::KeyP);
            break;
        case Otter::KeyCode::KeyQ:
            os << OTR_NAME_OF(Otter::KeyCode::KeyQ);
            break;
        case Otter::KeyCode::KeyR:
            os << OTR_NAME_OF(Otter::KeyCode::KeyR);
            break;
        case Otter::KeyCode::KeyS:
            os << OTR_NAME_OF(Otter::KeyCode::KeyS);
            break;
        case Otter::KeyCode::KeyT:
            os << OTR_NAME_OF(Otter::KeyCode::KeyT);
            break;
        case Otter::KeyCode::KeyU:
            os << OTR_NAME_OF(Otter::KeyCode::KeyU);
            break;
        case Otter::KeyCode::KeyV:
            os << OTR_NAME_OF(Otter::KeyCode::KeyV);
            break;
        case Otter::KeyCode::KeyW:
            os << OTR_NAME_OF(Otter::KeyCode::KeyW);
            break;
        case Otter::KeyCode::KeyX:
            os << OTR_NAME_OF(Otter::KeyCode::KeyX);
            break;
        case Otter::KeyCode::KeyY:
            os << OTR_NAME_OF(Otter::KeyCode::KeyY);
            break;
        case Otter::KeyCode::KeyZ:
            os << OTR_NAME_OF(Otter::KeyCode::KeyZ);
            break;
        case Otter::KeyCode::Numpad0:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad0);
            break;
        case Otter::KeyCode::Numpad1:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad1);
            break;
        case Otter::KeyCode::Numpad2:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad2);
            break;
        case Otter::KeyCode::Numpad3:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad3);
            break;
        case Otter::KeyCode::Numpad4:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad4);
            break;
        case Otter::KeyCode::Numpad5:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad5);
            break;
        case Otter::KeyCode::Numpad6:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad6);
            break;
        case Otter::KeyCode::Numpad7:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad7);
            break;
        case Otter::KeyCode::Numpad8:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad8);
            break;
        case Otter::KeyCode::Numpad9:
            os << OTR_NAME_OF(Otter::KeyCode::Numpad9);
            break;
        case Otter::KeyCode::NumpadMultiply:
            os << OTR_NAME_OF(Otter::KeyCode::NumpadMultiply);
            break;
        case Otter::KeyCode::NumpadAdd:
            os << OTR_NAME_OF(Otter::KeyCode::NumpadAdd);
            break;
        case Otter::KeyCode::NumpadSeparator:
            os << OTR_NAME_OF(Otter::KeyCode::NumpadSeparator);
            break;
        case Otter::KeyCode::NumpadSubtract:
            os << OTR_NAME_OF(Otter::KeyCode::NumpadSubtract);
            break;
        case Otter::KeyCode::NumpadDecimal:
            os << OTR_NAME_OF(Otter::KeyCode::NumpadDecimal);
            break;
        case Otter::KeyCode::NumpadDivide:
            os << OTR_NAME_OF(Otter::KeyCode::NumpadDivide);
            break;
        case Otter::KeyCode::F1:
            os << OTR_NAME_OF(Otter::KeyCode::F1);
            break;
        case Otter::KeyCode::F2:
            os << OTR_NAME_OF(Otter::KeyCode::F2);
            break;
        case Otter::KeyCode::F3:
            os << OTR_NAME_OF(Otter::KeyCode::F3);
            break;
        case Otter::KeyCode::F4:
            os << OTR_NAME_OF(Otter::KeyCode::F4);
            break;
        case Otter::KeyCode::F5:
            os << OTR_NAME_OF(Otter::KeyCode::F5);
            break;
        case Otter::KeyCode::F6:
            os << OTR_NAME_OF(Otter::KeyCode::F6);
            break;
        case Otter::KeyCode::F7:
            os << OTR_NAME_OF(Otter::KeyCode::F7);
            break;
        case Otter::KeyCode::F8:
            os << OTR_NAME_OF(Otter::KeyCode::F8);
            break;
        case Otter::KeyCode::F9:
            os << OTR_NAME_OF(Otter::KeyCode::F9);
            break;
        case Otter::KeyCode::F10:
            os << OTR_NAME_OF(Otter::KeyCode::F10);
            break;
        case Otter::KeyCode::F11:
            os << OTR_NAME_OF(Otter::KeyCode::F11);
            break;
        case Otter::KeyCode::F12:
            os << OTR_NAME_OF(Otter::KeyCode::F12);
            break;
        case Otter::KeyCode::F13:
            os << OTR_NAME_OF(Otter::KeyCode::F13);
            break;
        case Otter::KeyCode::F14:
            os << OTR_NAME_OF(Otter::KeyCode::F14);
            break;
        case Otter::KeyCode::F15:
            os << OTR_NAME_OF(Otter::KeyCode::F15);
            break;
        case Otter::KeyCode::F16:
            os << OTR_NAME_OF(Otter::KeyCode::F16);
            break;
        case Otter::KeyCode::F17:
            os << OTR_NAME_OF(Otter::KeyCode::F17);
            break;
        case Otter::KeyCode::F18:
            os << OTR_NAME_OF(Otter::KeyCode::F18);
            break;
        case Otter::KeyCode::F19:
            os << OTR_NAME_OF(Otter::KeyCode::F19);
            break;
        case Otter::KeyCode::F20:
            os << OTR_NAME_OF(Otter::KeyCode::F20);
            break;
        case Otter::KeyCode::F21:
            os << OTR_NAME_OF(Otter::KeyCode::F21);
            break;
        case Otter::KeyCode::F22:
            os << OTR_NAME_OF(Otter::KeyCode::F22);
            break;
        case Otter::KeyCode::F23:
            os << OTR_NAME_OF(Otter::KeyCode::F23);
            break;
        case Otter::KeyCode::F24:
            os << OTR_NAME_OF(Otter::KeyCode::F24);
            break;
        case Otter::KeyCode::NumLock:
            os << OTR_NAME_OF(Otter::KeyCode::NumLock);
            break;
        case Otter::KeyCode::ScrollLock:
            os << OTR_NAME_OF(Otter::KeyCode::ScrollLock);
            break;
        case Otter::KeyCode::LeftShift:
            os << OTR_NAME_OF(Otter::KeyCode::LeftShift);
            break;
        case Otter::KeyCode::RightShift:
            os << OTR_NAME_OF(Otter::KeyCode::RightShift);
            break;
        case Otter::KeyCode::LeftCtrl:
            os << OTR_NAME_OF(Otter::KeyCode::LeftCtrl);
            break;
        case Otter::KeyCode::RightCtrl:
            os << OTR_NAME_OF(Otter::KeyCode::RightCtrl);
            break;
        case Otter::KeyCode::LeftAlt:
            os << OTR_NAME_OF(Otter::KeyCode::LeftAlt);
            break;
        case Otter::KeyCode::RightAlt:
            os << OTR_NAME_OF(Otter::KeyCode::RightAlt);
            break;
        case Otter::KeyCode::Semicolon:
            os << OTR_NAME_OF(Otter::KeyCode::Semicolon);
            break;
        case Otter::KeyCode::Equal:
            os << OTR_NAME_OF(Otter::KeyCode::Equal);
            break;
        case Otter::KeyCode::Comma:
            os << OTR_NAME_OF(Otter::KeyCode::Comma);
            break;
        case Otter::KeyCode::Minus:
            os << OTR_NAME_OF(Otter::KeyCode::Minus);
            break;
        case Otter::KeyCode::Period:
            os << OTR_NAME_OF(Otter::KeyCode::Period);
            break;
        case Otter::KeyCode::ForwardSlash:
            os << OTR_NAME_OF(Otter::KeyCode::ForwardSlash);
            break;
        case Otter::KeyCode::Grave:
            os << OTR_NAME_OF(Otter::KeyCode::Grave);
            break;
        case Otter::KeyCode::LeftBracket:
            os << OTR_NAME_OF(Otter::KeyCode::LeftBracket);
            break;
        case Otter::KeyCode::Backslash:
            os << OTR_NAME_OF(Otter::KeyCode::Backslash);
            break;
        case Otter::KeyCode::RightBracket:
            os << OTR_NAME_OF(Otter::KeyCode::RightBracket);
            break;
        case Otter::KeyCode::Apostrophe:
            os << OTR_NAME_OF(Otter::KeyCode::Apostrophe);
            break;
        case Otter::KeyCode::Tilde:
            os << OTR_NAME_OF(Otter::KeyCode::Tilde);
            break;
        case Otter::KeyCode::Max:
            os << OTR_NAME_OF(Otter::KeyCode::Max);
            break;
        default:
            os << "Unknown KeyCode";
    }

    return os;
}

#endif //OTTERENGINE_TYPES_KEYCODE_H
