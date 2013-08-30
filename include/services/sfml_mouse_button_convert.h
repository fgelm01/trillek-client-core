
#ifndef SFML_MOUSE_BUTTON_CONVERT_H
#define SFML_MOUSE_BUTTON_CONVERT_H

#include "services\mouse_button_codes.h"
#include <SFML/Window.hpp>

namespace trillek
{

    mouse::mouse_button_code sfml_mouse_button_convert(sf::Mouse::Button input)
    {
        switch(input)
        {
            case sf::Mouse::Button::Left:
                return mouse::left;
            break;
            case sf::Mouse::Button::Right:
                return mouse::right;
            break;
            case sf::Mouse::Button::Middle:
                return mouse::middle;
            break;
            case sf::Mouse::Button::XButton1:
                return mouse::extra_button_1;
            break;
            case sf::Mouse::Button::XButton2:
                return mouse::extra_button_2;
            break;
            default:
                return mouse::unknown;
            break;
        };
    }

}
#endif // SFML_MOUSE_BUTTON_CONVERT_H
