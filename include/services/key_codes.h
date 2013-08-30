
#ifndef KEY_CODES_H
#define KEY_CODES_H

namespace trillek
{
namespace keyboard
{

    enum key_code
    {
        unknown=-1,
        // 0 to 127 are reserved for the ascii codes
        escape=128,
        left_control,
        left_shift,
        left_alt,
        left_system,
        right_control,
        right_escape,
        right_alt,
        right_system,
        menu,
        return_,
        backspace,
        tab,
        page_up,
        page_down,
        end_,
        home,
        insert_,
        delete_,
        left,
        right,
        up,
        down,
        numpad0,
        numpad1,
        numpad2,
        numpad3,
        numpad4,
        numpad5,
        numpad6,
        numpad7,
        numpad8,
        numpad9,
        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12,
        f13,
        f14,
        f15,
        pause,

        num_key_codes // Always last to know how many key_codes there are
    };

}
}
#endif // KEY_CODES_H
