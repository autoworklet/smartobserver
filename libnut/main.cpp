#include "keypress.h"
#include "microsleep.h"
#include "MMBitmap.h"
#include "mouse.h"
#include "screen.h"
#include "screengrab.h"
#include "window_manager.h"
#include <iostream>
#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // For std::vector binding

namespace py = pybind11;

int mouseDelay = 10;
int keyboardDelay = 10;

/*
 __  __
|  \/  | ___  _   _ ___  ___
| |\/| |/ _ \| | | / __|/ _ \
| |  | | (_) | |_| \__ \  __/
|_|  |_|\___/ \__,_|___/\___|

*/

int CheckMouseButton(const std::string &buttonString, MMMouseButton *const button) {
    if (buttonString.empty())
        return -1;

    if (buttonString.compare("left") == 0) {
        *button = LEFT_BUTTON;
    } else if (buttonString.compare("right") == 0) {
        *button = RIGHT_BUTTON;
    } else if (buttonString.compare("middle") == 0) {
        *button = CENTER_BUTTON;
    } else {
        return -2;
    }

    return 0;
}

// Napi::Number _dragMouse(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     if (info.Length() < 2 || info.Length() > 3) {
//         throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     const size_t x = info[0].As<Napi::Number>().Int32Value();
//     const size_t y = info[1].As<Napi::Number>().Int32Value();
//     MMMouseButton button = LEFT_BUTTON;

//     if (info.Length() == 3) {
//         std::string buttonString = info[2].As<Napi::String>();

//         switch (CheckMouseButton(buttonString, &button)) {
//             case -1:
//                 throw Napi::Error::New(env, "Null pointer in mouse button code.");
//                 break;
//             case -2:
//                 throw Napi::Error::New(env, "Invalid mouse button specified.");
//         }
//     }

//     MMPoint point = MMPointMake(x, y);
//     dragMouse(point, button);
//     microsleep(mouseDelay);

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for dragMouse function
py::object py_dragMouse(int x, int y, const std::string& buttonString = "left") {
    MMMouseButton button = LEFT_BUTTON;
    if (CheckMouseButton(buttonString, &button) != 0) {
        throw std::runtime_error("Invalid mouse button specified.");
    }
    MMPoint point = MMPointMake(x, y);
    dragMouse(point, button);
    microsleep(mouseDelay);
    return py::cast(1); // Return 1 for success
}

// Napi::Number _moveMouse(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     if (info.Length() != 2) {
//         throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     size_t x = info[0].As<Napi::Number>().Int32Value();
//     size_t y = info[1].As<Napi::Number>().Int32Value();

//     MMPoint point;
//     point = MMPointMake(x, y);
//     moveMouse(point);
//     microsleep(mouseDelay);

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for moveMouse function
py::object py_moveMouse(int x, int y) {
    MMPoint point = MMPointMake(x, y);
    moveMouse(point);
    microsleep(mouseDelay);
    return py::cast(1); // Return 1 for success
}

// Napi::Object _getMousePos(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     MMPoint pos = getMousePos();

//     //Return object with .x and .y.
//     Napi::Object obj = Napi::Object::New(env);

//     obj.Set("x", (int) pos.x);
//     obj.Set("y", (int) pos.y);

//     return obj;
// }
// PyBind11 wrapper for getMousePos function
py::dict py_getMousePos() {
    MMPoint pos = getMousePos();
    py::dict obj;
    obj["x"] = pos.x;
    obj["y"] = pos.y;
    return obj;
}

// Napi::Number _mouseClick(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     MMMouseButton button = LEFT_BUTTON;
//     bool doubleC = false;

//     if (info.Length() > 0) {
//         std::string buttonString = info[0].As<Napi::String>();

//         switch (CheckMouseButton(buttonString, &button)) {
//             case -1:
//                 throw Napi::Error::New(env, "Null pointer in mouse button code.");
//                 break;
//             case -2:
//                 throw Napi::Error::New(env, "Invalid mouse button specified.");
//                 break;
//         }
//     }

//     if (info.Length() == 2) {
//         doubleC = info[1].As<Napi::Boolean>().Value();
//     } else if (info.Length() > 2) {
//         throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     if (!doubleC) {
//         clickMouse(button);
//     } else {
//         doubleClick(button);
//     }

//     microsleep(mouseDelay);

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for mouseClick function
py::object py_mouseClick(const std::string& buttonString = "left", bool doubleC = false) {
    MMMouseButton button = LEFT_BUTTON;
    if (CheckMouseButton(buttonString, &button) != 0) {
        throw std::runtime_error("Invalid mouse button specified.");
    }
    if (!doubleC) {
        clickMouse(button);
    } else {
        doubleClick(button);
    }
    microsleep(mouseDelay);
    return py::cast(1); // Return 1 for success
}

// Napi::Number _mouseToggle(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     MMMouseButton button = LEFT_BUTTON;
//     bool down = false;

//     if (info.Length() > 0) {
//         std::string directionString = info[0].As<Napi::String>();

//         if (directionString.compare("down") == 0) {
//             down = true;
//         } else if (directionString.compare("up") == 0) {
//             down = false;
//         } else {
//             throw Napi::Error::New(env, "Invalid mouse button state specified.");
//         }
//     }

//     if (info.Length() == 2) {
//         std::string buttonString = info[1].As<Napi::String>();

//         switch (CheckMouseButton(buttonString, &button)) {
//             case -1:
//                 throw Napi::Error::New(env, "Null pointer in mouse button code.");
//                 break;
//             case -2:
//                 throw Napi::Error::New(env, "Invalid mouse button specified.");
//                 break;
//         }
//     } else if (info.Length() > 2) {
//         throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     toggleMouse(down, button);
//     microsleep(mouseDelay);

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for mouseToggle function
py::object py_mouseToggle(bool down = false, const std::string& buttonString = "left") {
    MMMouseButton button = LEFT_BUTTON;
    if (CheckMouseButton(buttonString, &button) != 0) {
        throw std::runtime_error("Invalid mouse button specified.");
    }
    toggleMouse(down, button);
    microsleep(mouseDelay);
    return py::cast(1); // Return 1 for success
}

// Napi::Number _setMouseDelay(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     if (info.Length() != 1) {
//         throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     mouseDelay = info[0].As<Napi::Number>().Int32Value();

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for setMouseDelay function
py::object py_setMouseDelay(int delay) {
    mouseDelay = delay;
    return py::cast(1); // Return 1 for success
}

// Napi::Number _scrollMouse(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     if (info.Length() != 2) {
//         throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     int x = info[0].As<Napi::Number>().Int32Value();
//     int y = info[1].As<Napi::Number>().Int32Value();

//     scrollMouse(x, y);
//     microsleep(mouseDelay);

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for scrollMouse function
py::object py_scrollMouse(int x, int y) {
    scrollMouse(x, y);
    microsleep(mouseDelay);
    return py::cast(1); // Return 1 for success
}

/*
 _  __          _                         _
| |/ /___ _   _| |__   ___   __ _ _ __ __| |
| ' // _ \ | | | '_ \ / _ \ / _` | '__/ _` |
| . \  __/ |_| | |_) | (_) | (_| | | | (_| |
|_|\_\___|\__, |_.__/ \___/ \__,_|_|  \__,_|
          |___/
*/
struct KeyNames {
    const char *name;
    MMKeyCode key;
};

static KeyNames key_names[] =
        {
                {"backspace",         K_BACKSPACE},
                {"delete",            K_DELETE},
                {"return",            K_RETURN},
                {"tab",               K_TAB},
                {"escape",            K_ESCAPE},

                {"up",                K_UP},
                {"down",              K_DOWN},
                {"right",             K_RIGHT},
                {"left",              K_LEFT},

                {"home",              K_HOME},
                {"end",               K_END},
                {"pageup",            K_PAGEUP},
                {"pagedown",          K_PAGEDOWN},

                {"0",                 K_0},
                {"1",                 K_1},
                {"2",                 K_2},
                {"3",                 K_3},
                {"4",                 K_4},
                {"5",                 K_5},
                {"6",                 K_6},
                {"7",                 K_7},
                {"8",                 K_8},
                {"9",                 K_9},

                {"a",                 K_A},
                {"b",                 K_B},
                {"c",                 K_C},
                {"d",                 K_D},
                {"e",                 K_E},
                {"f",                 K_F},
                {"g",                 K_G},
                {"h",                 K_H},
                {"i",                 K_I},
                {"j",                 K_J},
                {"k",                 K_K},
                {"l",                 K_L},
                {"m",                 K_M},
                {"n",                 K_N},
                {"o",                 K_O},
                {"p",                 K_P},
                {"q",                 K_Q},
                {"r",                 K_R},
                {"s",                 K_S},
                {"t",                 K_T},
                {"u",                 K_U},
                {"v",                 K_V},
                {"w",                 K_W},
                {"x",                 K_X},
                {"y",                 K_Y},
                {"z",                 K_Z},

                {",",                 K_COMMA},
                {".",                 K_PERIOD},
                {"/",                 K_SLASH},

                {";",                 K_SEMICOLON},
                {"'",                 K_QUOTE},
                {"[",                 K_LEFTBRACKET},
                {"]",                 K_RIGHTBRACKET},
                {"\\",                K_BACKSLASH},

                {"-",                 K_MINUS},
                {"=",                 K_EQUAL},

                {"`",                 K_GRAVE},

                {"f1",                K_F1},
                {"f2",                K_F2},
                {"f3",                K_F3},
                {"f4",                K_F4},
                {"f5",                K_F5},
                {"f6",                K_F6},
                {"f7",                K_F7},
                {"f8",                K_F8},
                {"f9",                K_F9},
                {"f10",               K_F10},
                {"f11",               K_F11},
                {"f12",               K_F12},
                {"f13",               K_F13},
                {"f14",               K_F14},
                {"f15",               K_F15},
                {"f16",               K_F16},
                {"f17",               K_F17},
                {"f18",               K_F18},
                {"f19",               K_F19},
                {"f20",               K_F20},
                {"f21",               K_F21},
                {"f22",               K_F22},
                {"f23",               K_F23},
                {"f24",               K_F24},

                {"meta",              K_META},
                {"right_meta",        K_RIGHTMETA},

                {"cmd",               K_CMD},
                {"right_cmd",         K_RIGHTCMD},

                {"win",               K_WIN},
                {"right_win",         K_RIGHTWIN},

                {"alt",               K_ALT},
                {"right_alt",         K_RIGHTALT},

                {"control",           K_CONTROL},
                {"right_control",     K_RIGHTCONTROL},

                {"shift",             K_SHIFT},
                {"right_shift",       K_RIGHTSHIFT},

                {"space",             K_SPACE},

                {"printscreen",       K_PRINTSCREEN},
                {"insert",            K_INSERT},
                {"menu",              K_MENU},
                {"fn",                K_FUNCTION},
                {"pause",             K_PAUSE},

                {"caps_lock",         K_CAPSLOCK},
                {"num_lock",          K_NUMLOCK},
                {"scroll_lock",       K_SCROLL_LOCK},

                {"audio_mute",        K_AUDIO_VOLUME_MUTE},
                {"audio_vol_down",    K_AUDIO_VOLUME_DOWN},
                {"audio_vol_up",      K_AUDIO_VOLUME_UP},
                {"audio_play",        K_AUDIO_PLAY},
                {"audio_stop",        K_AUDIO_STOP},
                {"audio_pause",       K_AUDIO_PAUSE},
                {"audio_prev",        K_AUDIO_PREV},
                {"audio_next",        K_AUDIO_NEXT},
                {"audio_rewind",      K_AUDIO_REWIND},
                {"audio_forward",     K_AUDIO_FORWARD},
                {"audio_repeat",      K_AUDIO_REPEAT},
                {"audio_random",      K_AUDIO_RANDOM},

                {"numpad_0",          K_NUMPAD_0},
                {"numpad_1",          K_NUMPAD_1},
                {"numpad_2",          K_NUMPAD_2},
                {"numpad_3",          K_NUMPAD_3},
                {"numpad_4",          K_NUMPAD_4},
                {"numpad_5",          K_NUMPAD_5},
                {"numpad_6",          K_NUMPAD_6},
                {"numpad_7",          K_NUMPAD_7},
                {"numpad_8",          K_NUMPAD_8},
                {"numpad_9",          K_NUMPAD_9},
                {"numpad_decimal",    K_NUMPAD_DECIMAL},
                {"enter",             K_ENTER},
                {"clear",             K_CLEAR},

                {"add",               K_ADD},
                {"subtract",          K_SUBTRACT},
                {"multiply",          K_MULTIPLY},
                {"divide",            K_DIVIDE},

                {"lights_mon_up",     K_LIGHTS_MON_UP},
                {"lights_mon_down",   K_LIGHTS_MON_DOWN},
                {"lights_kbd_toggle", K_LIGHTS_KBD_TOGGLE},
                {"lights_kbd_up",     K_LIGHTS_KBD_UP},
                {"lights_kbd_down",   K_LIGHTS_KBD_DOWN},
                {NULL,                K_NOT_A_KEY} /* end marker */
        };

int CheckKeyCodes(const std::string &keyName, MMKeyCode *key) {
    if (!key)
        return -1;

    if (keyName.length() == 1) {
        *key = keyCodeForChar(*keyName.c_str());
        return 0;
    }

    *key = K_NOT_A_KEY;

    KeyNames *kn = key_names;
    while (kn->name) {
        if (keyName.compare(kn->name) == 0) {
            *key = kn->key;
            break;
        }
        kn++;
    }

    if (*key == K_NOT_A_KEY) {
        return -2;
    }

    return 0;
}

int CheckKeyFlags(std::string &flagString, MMKeyFlags *flags) {
    if (!flags)
        return -1;

    if (flagString == "alt" || flagString == "right_alt") {
        *flags = MOD_ALT;
#if defined(IS_MACOSX)
    } else if (flagString == "command" || flagString == "cmd" || flagString == "right_cmd") {
        *flags = MOD_META;
#else
        } else if (flagString == "command" || flagString == "win" || flagString == "right_win") {
    *flags = MOD_META;
#endif
    } else if (flagString == "control" || flagString == "right_control") {
        *flags = MOD_CONTROL;
    } else if (flagString == "shift" || flagString == "right_shift") {
        *flags = MOD_SHIFT;
    } else if (flagString == "fn") {
        *flags = MOD_FN;
    } else if (flagString == "none" || flagString == "") {
        *flags = MOD_NONE;
    } else {
        return -2;
    }

    return 0;
}

// int GetFlagsFromString(Napi::Value value, MMKeyFlags *flags) {
//     std::string fstr = value.As<Napi::String>();
//     return CheckKeyFlags(fstr, flags);
// }
int GetFlagsFromString(const std::string &value, MMKeyFlags *flags) {
    std::string fstr = value;
    return CheckKeyFlags(fstr, flags);
}

// int GetFlagsFromValue(Napi::Value value, MMKeyFlags *flags) {
//     if (!flags)
//         return -1;

//     //Optionally allow an array of flag strings to be passed.
//     if (value.IsArray()) {
//         Napi::Array a = value.As<Napi::Array>();
//         for (uint32_t i = 0; i < a.Length(); ++i) {
//             Napi::Value v = a.Get(i);
//             if (!v.IsString())
//                 return -2;

//             MMKeyFlags f = MOD_NONE;
//             const int rv = GetFlagsFromString(v, &f);
//             if (rv)
//                 return rv;

//             *flags = (MMKeyFlags) (*flags | f);
//         }
//         return 0;
//     }

//     //If it's not an array, it should be a single string value.
//     return GetFlagsFromString(value, flags);
// }
int GetFlagsFromValue(const std::string &value, MMKeyFlags *flags) {
    if (!flags)
        return -1;

    //Optionally allow an array of flag strings to be passed.
    if (value.find(",") != std::string::npos) {
        std::vector<std::string> flagsList;
        std::string flag;
        std::stringstream ss(value);
        while (std::getline(ss, flag, ',')) {
            flagsList.push_back(flag);
        }
        for (const std::string& flag : flagsList) {
            MMKeyFlags f = MOD_NONE;
            const int rv = GetFlagsFromString(flag, &f);
            if (rv)
                return rv;
            *flags = (MMKeyFlags) (*flags | f);
        }
        return 0;
    }

    //If it's not an array, it should be a single string value.
    return GetFlagsFromString(value, flags);
}

// Napi::Number _keyTap(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     MMKeyFlags flags = MOD_NONE;
//     MMKeyCode key;

//     std::string keyName = info[0].As<Napi::String>();

//     switch (info.Length()) {
//         case 2:
//             switch (GetFlagsFromValue(info[1], &flags)) {
//                 case -1:
//                     throw Napi::Error::New(env, "Null pointer in key flag.");
//                     break;
//                 case -2:
//                     throw Napi::Error::New(env, "Invalid key flag specified.");
//                     break;
//             }
//             break;
//         case 1:
//             break;
//         default:
//             throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     switch (CheckKeyCodes(keyName, &key)) {
//         case -1:
//             throw Napi::Error::New(env, "Null pointer in key code.");
//             break;
//         case -2:
//             throw Napi::Error::New(env, "Invalid key code specified.");
//             break;
//         default:
//             tapKeyCode(key, flags);
//     }

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for keyTap function
py::object py_keyTap(const std::string& keyName, const std::string& flagString = "none") {
    MMKeyFlags flags = MOD_NONE;
    MMKeyCode key;
    if (GetFlagsFromValue(flagString, &flags) != 0) {
        throw std::runtime_error("Invalid key flag specified.");
    }
    if (CheckKeyCodes(keyName, &key) != 0) {
        throw std::runtime_error("Invalid key code specified.");
    }
    tapKeyCode(key, flags);
    return py::cast(1); // Return 1 for success
}

// Napi::Number _keyToggle(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     MMKeyFlags flags = MOD_NONE;
//     MMKeyCode key;

//     bool down = false;

//     //Get arguments from JavaScript.
//     std::string keyName = info[0].As<Napi::String>();

//     //Check and confirm number of arguments.
//     switch (info.Length()) {
//         case 3:
//             //Get key modifier.
//             switch (GetFlagsFromValue(info[2], &flags)) {
//                 case -1:
//                     throw Napi::Error::New(env, "Null pointer in key flag.");
//                     break;
//                 case -2:
//                     throw Napi::Error::New(env, "Invalid key flag specified.");
//                     break;
//             }
//             break;
//         case 2:
//             break;
//         default:
//             throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     //Get down value if provided.
//     if (info.Length() > 1) {
//         std::string directionString = info[1].As<Napi::String>();

//         if (directionString.compare("down") == 0) {
//             down = true;
//         } else if (directionString.compare("up") == 0) {
//             down = false;
//         } else {
//             throw Napi::Error::New(env, "Invalid key state specified.");
//         }
//     }

//     //Get the acutal key.
//     switch (CheckKeyCodes(keyName, &key)) {
//         case -1:
//             throw Napi::Error::New(env, "Null pointer in key code.");
//             break;
//         case -2:
//             throw Napi::Error::New(env, "Invalid key code specified.");
//             break;
//         default:
//             toggleKeyCode(key, down, flags);
//     }

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for keyToggle function
py::object py_keyToggle(const std::string& keyName, bool down = false, const std::string& flagString = "none") {
    MMKeyFlags flags = MOD_NONE;
    MMKeyCode key;
    if (GetFlagsFromValue(flagString, &flags) != 0) {
        throw std::runtime_error("Invalid key flag specified.");
    }
    if (CheckKeyCodes(keyName, &key) != 0) {
        throw std::runtime_error("Invalid key code specified.");
    }
    toggleKeyCode(key, down, flags);
    return py::cast(1); // Return 1 for success
}

py::object py_keyToggleRaw(int key, bool down = false, const std::string& flagString = "none") {
    MMKeyFlags flags = MOD_NONE;
    if (GetFlagsFromValue(flagString, &flags) != 0) {
        throw std::runtime_error("Invalid key flag specified.");
    }
    toggleKeyCode((MMKeyCode) key, down, flags);
    return py::cast(1); // Return 1 for success
}

// Napi::Number _typeString(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     std::string stringToType = info[0].As<Napi::String>();

//     typeString(stringToType.c_str());

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for typeString function
py::object py_typeString(const std::string& stringToType) {
    typeString(stringToType.c_str());
    return py::cast(1); // Return 1 for success
}

// Napi::Number _typeStringDelayed(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     std::string stringToType = info[0].As<Napi::String>();
//     size_t cpm = info[1].As<Napi::Number>().Int32Value();

//     typeStringDelayed(stringToType.c_str(), (unsigned int) cpm);

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for typeStringDelayed function
py::object py_typeStringDelayed(const std::string& stringToType, int cpm) {
    typeStringDelayed(stringToType.c_str(), (unsigned int) cpm);
    return py::cast(1); // Return 1 for success
}

// Napi::Number _setKeyboardDelay(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     if (info.Length() != 1) {
//         throw Napi::Error::New(env, "Invalid number of arguments.");
//     }

//     keyboardDelay = info[0].As<Napi::Number>().Int32Value();

//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for setKeyboardDelay function
py::object py_setKeyboardDelay(int delay) {
    keyboardDelay = delay;
    return py::cast(1); // Return 1 for success
}

/*
  ____
 / ___|  ___ _ __ ___  ___ _ __
 \___ \ / __| '__/ _ \/ _ \ '_ \
  ___) | (__| | |  __/  __/ | | |
 |____/ \___|_|  \___|\___|_| |_|

*/

// Napi::Object _getScreenSize(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     //Get display size.
//     MMSize displaySize = getMainDisplaySize();

//     //Create our return object.
//     Napi::Object obj = Napi::Object::New(env);
//     obj.Set(Napi::String::New(env, "width"), Napi::Number::New(env, (double) displaySize.width));
//     obj.Set(Napi::String::New(env, "height"), Napi::Number::New(env, (double) displaySize.height));

//     return obj;
// }
// PyBind11 wrapper for getScreenSize function
py::dict py_getScreenSize() {
    MMSize displaySize = getMainDisplaySize();
    py::dict obj;
    obj["width"] = displaySize.width;
    obj["height"] = displaySize.height;
    return obj;
}

// Napi::String _getXDisplayName(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

// #if defined(USE_X11)
//     std::string display = getXDisplay();
//     return Napi::String::New(env, display);
// #else
//     throw Napi::Error::New(env, "getXDisplayName is only supported on Linux");
// #endif
// }
// PyBind11 wrapper for getXDisplayName function
py::str py_getXDisplayName() {
#if defined(USE_X11)
    std::string display = getXDisplay();
    return py::str(display);
#else
    throw std::runtime_error("getXDisplayName is only supported on Linux");
#endif
}

// Napi::Number _setXDisplayName(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

// #if defined(USE_X11)
//     std::string displayName = info[0].As<Napi::String>();
//     setXDisplay(displayName.c_str());
//     return Napi::Number::New(env, 1);
// #else
//     throw Napi::Error::New(env, "setXDisplayName is only supported on Linux");
// #endif
// }
// PyBind11 wrapper for setXDisplayName function
py::object py_setXDisplayName(const std::string& displayName) {
#if defined(USE_X11)
    setXDisplay(displayName.c_str());
    return py::cast(1); // Return 1 for success
#else
    throw std::runtime_error("setXDisplayName is only supported on Linux");
#endif
}

// Napi::Number _highlight(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();
//     int32_t x;
//     int32_t y;
//     int32_t width;
//     int32_t height;
//     long duration;
//     float opacity;

//     if (info.Length() == 6) {
//         x = info[0].As<Napi::Number>().Int32Value();
//         y = info[1].As<Napi::Number>().Int32Value();
//         width = info[2].As<Napi::Number>().Int32Value();
//         height = info[3].As<Napi::Number>().Int32Value();
//         duration = (int) info[4].As<Napi::Number>().Int64Value();
//         opacity = info[5].As<Napi::Number>().FloatValue();

//         highlight(x, y, width, height, duration, opacity);
//         return Napi::Number::New(env, 1);
//     }
//     return Napi::Number::New(env, 1);
// }
// PyBind11 wrapper for highlight function
py::object py_highlight(int x, int y, int width, int height, long duration, float opacity) {
    highlight(x, y, width, height, duration, opacity);
    return py::cast(1); // Return 1 for success
}

// Napi::Number _getActiveWindow(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     WindowHandle windowHandle = getActiveWindow();
//     return Napi::Number::New(env, (double) windowHandle);
// }
// PyBind11 wrapper for getActiveWindow function
py::int_ py_getActiveWindow() {
    WindowHandle windowHandle = getActiveWindow();
    return py::int_(windowHandle);
}

// Napi::Array _getWindows(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     std::vector<WindowHandle> windowHandles = getWindows();
//     auto arr = Napi::Array::New(env, windowHandles.size());

//     for (size_t idx = 0; idx < windowHandles.size(); ++idx) {
//         arr[(uint32_t) idx] = windowHandles[idx];
//     }

//     return arr;
// }
// PyBind11 wrapper for getWindows function
py::list py_getWindows() {
    std::vector<WindowHandle> windowHandles = getWindows();
    py::list arr;
    for (size_t idx = 0; idx < windowHandles.size(); ++idx) {
        arr.append(windowHandles[idx]);
    }
    return arr;
}

// Napi::Object _getWindowRect(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     WindowHandle windowHandle = info[0].As<Napi::Number>().Int64Value();
//     MMRect windowRect = getWindowRect(windowHandle);

//     Napi::Object obj = Napi::Object::New(env);
//     obj.Set(Napi::String::New(env, "x"), Napi::Number::New(env, (double) windowRect.origin.x));
//     obj.Set(Napi::String::New(env, "y"), Napi::Number::New(env, (double) windowRect.origin.y));
//     obj.Set(Napi::String::New(env, "width"), Napi::Number::New(env, (double) windowRect.size.width));
//     obj.Set(Napi::String::New(env, "height"), Napi::Number::New(env, (double) windowRect.size.height));

//     return obj;
// }
// PyBind11 wrapper for getWindowRect function
py::dict py_getWindowRect(WindowHandle windowHandle) {
    MMRect windowRect = getWindowRect(windowHandle);
    py::dict obj;
    obj["x"] = windowRect.origin.x;
    obj["y"] = windowRect.origin.y;
    obj["width"] = windowRect.size.width;
    obj["height"] = windowRect.size.height;
    return obj;
}

// Napi::String _getWindowTitle(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     WindowHandle windowHandle = info[0].As<Napi::Number>().Int64Value();
//     return Napi::String::New(env, getWindowTitle(windowHandle));
// }
// PyBind11 wrapper for getWindowTitle function
py::str py_getWindowTitle(WindowHandle windowHandle) {
    return py::str(getWindowTitle(windowHandle));
}

// Napi::Boolean _focusWindow(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     WindowHandle windowHandle = info[0].As<Napi::Number>().Int64Value();

//     bool result = focusWindow(windowHandle);

//     return Napi::Boolean::New(env, result);
// }
// PyBind11 wrapper for focusWindow function
py::object py_focusWindow(WindowHandle windowHandle) {
    bool result = focusWindow(windowHandle);
    return py::cast(result);
}

// Napi::Boolean _resizeWindow(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsObject()) {
//         throw Napi::TypeError::New(env, "Invalid arguments. Expected handle (number) and rect (object).");
//     }

//     WindowHandle windowHandle = info[0].As<Napi::Number>().Int64Value();
//     auto sizeObject = info[1].As<Napi::Object>();

//     if (!sizeObject.Has("width") || !sizeObject.Has("height")) {
//         throw Napi::TypeError::New(env,
//                                    "Invalid second parameter. Expected object of shape {width: number, height: number}");
//     }

//     auto width = sizeObject.Get("width").As<Napi::Number>().Int64Value();
//     auto height = sizeObject.Get("height").As<Napi::Number>().Int64Value();

//     auto newSize = MMSizeMake(width, height);

//     auto resizeResult = resizeWindow(windowHandle, newSize);

//     return Napi::Boolean::New(env, resizeResult);
// }
// PyBind11 wrapper for resizeWindow function
py::object py_resizeWindow(WindowHandle windowHandle, int width, int height) {
    MMSize newSize = MMSizeMake(width, height);
    bool resizeResult = resizeWindow(windowHandle, newSize);
    return py::cast(resizeResult);
}

// Napi::Boolean _moveWindow(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsObject()) {
//         throw Napi::TypeError::New(env, "Invalid arguments. Expected handle (number) and point (object).");
//     }

//     WindowHandle windowHandle = info[0].As<Napi::Number>().Int64Value();

//     auto rectObj = info[1].As<Napi::Object>();
//     if (!rectObj.Has("x") || !rectObj.Has("y")) {
//         throw Napi::TypeError::New(env, "Invalid second parameter. Expected object of shape {x: number, y: number}");
//     }

//     auto x = rectObj.Get("x").As<Napi::Number>().Int64Value();
//     auto y = rectObj.Get("y").As<Napi::Number>().Int64Value();

//     auto newOrigin = MMPointMake(x, y);

//     auto moveResult = moveWindow(windowHandle, newOrigin);

//     return Napi::Boolean::New(env, moveResult);
// }
// PyBind11 wrapper for moveWindow function
py::object py_moveWindow(WindowHandle windowHandle, int x, int y) {
    MMPoint newOrigin = MMPointMake(x, y);
    bool moveResult = moveWindow(windowHandle, newOrigin);
    return py::cast(moveResult);
}

// Napi::Object _captureScreen(const Napi::CallbackInfo &info) {
//     Napi::Env env = info.Env();

//     int64_t x;
//     int64_t y;
//     int64_t w;
//     int64_t h;

//     MMSize displaySize = getMainDisplaySize();
//     //If user has provided screen coords, use them!
//     if (info.Length() == 4) {
//         x = info[0].As<Napi::Number>().Int64Value();
//         y = info[1].As<Napi::Number>().Int64Value();
//         w = info[2].As<Napi::Number>().Int64Value();
//         h = info[3].As<Napi::Number>().Int64Value();

//         if (!(x >= 0 && x <= displaySize.width)) {
//             throw Napi::Error::New(env, "Error: x coordinate outside of display");
//         }
//         if (!(y >= 0 && y <= displaySize.height)) {
//             throw Napi::Error::New(env, "Error: y coordinate outside of display");
//         }
//         if (!((x + w) >= 0 && (x + w) <= displaySize.width)) {
//             throw Napi::Error::New(env, "Error: Given width exceeds display dimensions");
//         }
//         if (!((y + h) >= 0 && (y + h) <= displaySize.height)) {
//             throw Napi::Error::New(env, "Error: Given height exceeds display dimensions");
//         }
//     } else {
//         //We're getting the full screen.
//         x = 0;
//         y = 0;

//         //Get screen size.
//         w = displaySize.width;
//         h = displaySize.height;
//     }

//     MMBitmapRef bitmap = copyMMBitmapFromDisplayInRect(MMRectMake(x, y, w, h));

//     if (bitmap == NULL) {
//         throw Napi::Error::New(env, "Error: Failed to capture screen");
//     }

//     uint64_t bufferSize = bitmap->bytewidth * bitmap->height;
//     Napi::Buffer<char> buffer = Napi::Buffer<char>::Copy(env, (char *) bitmap->imageBuffer, bufferSize);

//     Napi::Object obj = Napi::Object::New(env);
//     obj.Set(Napi::String::New(env, "width"), Napi::Number::New(env, (double) bitmap->width));
//     obj.Set(Napi::String::New(env, "height"), Napi::Number::New(env, (double) bitmap->height));
//     obj.Set(Napi::String::New(env, "byteWidth"), Napi::Number::New(env, (double) bitmap->bytewidth));
//     obj.Set(Napi::String::New(env, "bitsPerPixel"), Napi::Number::New(env, bitmap->bitsPerPixel));
//     obj.Set(Napi::String::New(env, "bytesPerPixel"), Napi::Number::New(env, bitmap->bytesPerPixel));
//     obj.Set(Napi::String::New(env, "image"), buffer);

//     destroyMMBitmap(bitmap);

//     return obj;
// }
// PyBind11 wrapper for captureScreen function
py::dict py_captureScreen(int x = 0, int y = 0, int w = 0, int h = 0) {
    MMSize displaySize = getMainDisplaySize();
    if (w == 0 || h == 0) {
        w = displaySize.width;
        h = displaySize.height;
    }
    if (!(x >= 0 && x <= displaySize.width)) {
        throw std::runtime_error("Error: x coordinate outside of display");
    }
    if (!(y >= 0 && y <= displaySize.height)) {
        throw std::runtime_error("Error: y coordinate outside of display");
    }
    if (!((x + w) >= 0 && (x + w) <= displaySize.width)) {
        throw std::runtime_error("Error: Given width exceeds display dimensions");
    }
    if (!((y + h) >= 0 && (y + h) <= displaySize.height)) {
        throw std::runtime_error("Error: Given height exceeds display dimensions");
    }
    captureScreenAreaToJPEG(MMRectMake(x, y, w, h), "/Users/surat/autoworklet/autoworkletv1/test.jpeg", 0.8);
    MMBitmapRef bitmap = copyMMBitmapFromDisplayInRect(MMRectMake(x, y, w, h));
    if (bitmap == NULL) {
        throw std::runtime_error("Error: Failed to capture screen");
    }
    uint64_t bufferSize = bitmap->bytewidth * bitmap->height;
    py::bytes buffer((char *) bitmap->imageBuffer, bufferSize);
    py::dict obj;
    obj["width"] = bitmap->width;
    obj["height"] = bitmap->height;
    obj["byteWidth"] = bitmap->bytewidth;
    obj["bitsPerPixel"] = bitmap->bitsPerPixel;
    obj["bytesPerPixel"] = bitmap->bytesPerPixel;
    obj["image"] = buffer;
    destroyMMBitmap(bitmap);
    return obj;
}

py::dict py_captureScreenToFile(const std::string &imagePath, int x = 0, int y = 0, int w = 0, int h = 0) {
    MMSize displaySize = getMainDisplaySize();
    if (w == 0 || h == 0) {
        w = displaySize.width;
        h = displaySize.height;
    }
    if (!(x >= 0 && x <= displaySize.width)) {
        throw std::runtime_error("Error: x coordinate outside of display");
    }
    if (!(y >= 0 && y <= displaySize.height)) {
        throw std::runtime_error("Error: y coordinate outside of display");
    }
    if (!((x + w) >= 0 && (x + w) <= displaySize.width)) {
        throw std::runtime_error("Error: Given width exceeds display dimensions");
    }
    if (!((y + h) >= 0 && (y + h) <= displaySize.height)) {
        throw std::runtime_error("Error: Given height exceeds display dimensions");
    }
    captureAndResizeScreenAreaToJPEG(MMRectMake(x, y, w, h), imagePath.c_str(), 0.8);
    py::dict obj;
    obj["imagePath"] = imagePath;
    return obj;
}

PYBIND11_MODULE(pylibnut, m) {
    m.doc() = "Python bindings for libnut";

    // Mouse functions
    m.def("dragMouse", &py_dragMouse, py::arg("x"), py::arg("y"), py::arg("button") = "left");
    m.def("moveMouse", &py_moveMouse, py::arg("x"), py::arg("y"));
    m.def("getMousePos", &py_getMousePos);
    m.def("mouseClick", &py_mouseClick, py::arg("button") = "left", py::arg("double_click") = false);
    m.def("mouseToggle", &py_mouseToggle, py::arg("down"), py::arg("button") = "left");
    m.def("setMouseDelay", &py_setMouseDelay, py::arg("delay"));
    m.def("scrollMouse", &py_scrollMouse, py::arg("x"), py::arg("y"));

    // Keyboard functions
    m.def("keyTap", &py_keyTap, py::arg("key_name"), py::arg("flags") = "none");
    m.def("keyToggle", &py_keyToggle, py::arg("key_name"), py::arg("down"), py::arg("flags") = "none");
    m.def("keyToggleRaw", &py_keyToggleRaw, py::arg("key"), py::arg("down"), py::arg("flags") = "none");
    m.def("typeString", &py_typeString, py::arg("string"));
    m.def("typeStringDelayed", &py_typeStringDelayed, py::arg("string"), py::arg("cpm"));
    m.def("setKeyboardDelay", &py_setKeyboardDelay, py::arg("delay"));

    // Screen and Window functions
    m.def("getScreenSize", &py_getScreenSize);
    m.def("getXDisplayName", &py_getXDisplayName);
    m.def("setXDisplayName", &py_setXDisplayName, py::arg("display_name"));
    m.def("highlight", &py_highlight, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"), py::arg("duration"), py::arg("opacity"));
    m.def("getActiveWindow", &py_getActiveWindow);
    m.def("getWindows", &py_getWindows);
    m.def("getWindowRect", &py_getWindowRect, py::arg("window_handle"));
    m.def("getWindowTitle", &py_getWindowTitle, py::arg("window_handle"));
    m.def("focusWindow", &py_focusWindow, py::arg("window_handle"));
    m.def("resizeWindow", &py_resizeWindow, py::arg("window_handle"), py::arg("width"), py::arg("height"));
    m.def("moveWindow", &py_moveWindow, py::arg("window_handle"), py::arg("x"), py::arg("y"));
    m.def("captureScreen", &py_captureScreen, py::arg("x") = 0, py::arg("y") = 0, py::arg("width") = 0, py::arg("height") = 0);
    m.def("captureScreenToFile", &py_captureScreenToFile, py::arg("image_path"), py::arg("x") = 0, py::arg("y") = 0, py::arg("width") = 0, py::arg("height") = 0);

    // microsleep
    m.def("microsleep", &microsleep, py::arg("microseconds"));

    // Bind MMPoint and MMSize structures
    py::class_<MMPoint>(m, "MMPoint")
        .def(py::init<int, int>())
        .def_readwrite("x", &MMPoint::x)
        .def_readwrite("y", &MMPoint::y);

    py::class_<MMSize>(m, "MMSize")
        .def(py::init<int, int>())
        .def_readwrite("width", &MMSize::width)
        .def_readwrite("height", &MMSize::height);

    // Bind MMRect structure
    py::class_<MMRect>(m, "MMRect")
        .def(py::init<MMPoint, MMSize>())
        .def_readwrite("origin", &MMRect::origin)
        .def_readwrite("size", &MMRect::size);

}