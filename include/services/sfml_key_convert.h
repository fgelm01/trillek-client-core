
#ifndef SFML_KEY_CONVERT_H
#define SFML_KEY_CONVERT_H

#include "services\mouse_button_codes.h"
#include <SFML/Window.hpp>

namespace trillek
{

    keyboard::key_code sfml_key_convert(sf::Keyboard::Key input)
    {
        switch(input)
        {
            // Keycodes that have an ASCII representation
            case sf::Keyboard::A:
            case sf::Keyboard::B:
            case sf::Keyboard::C:
            case sf::Keyboard::D:
            case sf::Keyboard::E:
            case sf::Keyboard::F:
            case sf::Keyboard::G:
            case sf::Keyboard::H:
            case sf::Keyboard::I:
            case sf::Keyboard::J:
            case sf::Keyboard::K:
            case sf::Keyboard::L:
            case sf::Keyboard::M:
            case sf::Keyboard::N:
            case sf::Keyboard::O:
            case sf::Keyboard::P:
            case sf::Keyboard::Q:
            case sf::Keyboard::R:
            case sf::Keyboard::S:
            case sf::Keyboard::T:
            case sf::Keyboard::U:
            case sf::Keyboard::V:
            case sf::Keyboard::W:
            case sf::Keyboard::X:
            case sf::Keyboard::Y:
            case sf::Keyboard::Z:
                return (keyboard::key_code)((input-sf::Keyboard::A)+'a');
            break;
            case sf::Keyboard::Num0:
            case sf::Keyboard::Num1:
            case sf::Keyboard::Num2:
            case sf::Keyboard::Num3:
            case sf::Keyboard::Num4:
            case sf::Keyboard::Num5:
            case sf::Keyboard::Num6:
            case sf::Keyboard::Num7:
            case sf::Keyboard::Num8:
            case sf::Keyboard::Num9:
                return (keyboard::key_code)((input-sf::Keyboard::Num0)+'0');
            break;
            case sf::Keyboard::LBracket:
                return (keyboard::key_code)('[');
            break;
            case sf::Keyboard::RBracket:
                return (keyboard::key_code)(']');
            break;
            case sf::Keyboard::SemiColon:
                return (keyboard::key_code)(';');
            break;
            case sf::Keyboard::Comma:
                return (keyboard::key_code)(',');
            break;
            case sf::Keyboard::Period:
                return (keyboard::key_code)('.');
            break;
            case sf::Keyboard::Quote:
                return (keyboard::key_code)('\'');
            break;
            case sf::Keyboard::Divide:
            case sf::Keyboard::Slash:
                return (keyboard::key_code)('/');
            break;
            case sf::Keyboard::BackSlash:
                return (keyboard::key_code)('\\');
            break;
            case sf::Keyboard::Tilde:
                return (keyboard::key_code)('~');
            break;
            case sf::Keyboard::Equal:
                return (keyboard::key_code)('=');
            break;
            case sf::Keyboard::Subtract:
            case sf::Keyboard::Dash:
                return (keyboard::key_code)('-');
            break;
            case sf::Keyboard::Space:
                return (keyboard::key_code)(' ');
            break;
            case sf::Keyboard::Add:
                return (keyboard::key_code)('+');
            break;
            case sf::Keyboard::Multiply:
                return (keyboard::key_code)('*');
            break;
            // Keycodes that don't have an ASCII representation
            case sf::Keyboard::Escape:
                return keyboard::escape;
            break;
            case sf::Keyboard::LControl:
                return keyboard::left_control;
            break;
            case sf::Keyboard::LShift:
                return keyboard::left_shift;
            break;
            case sf::Keyboard::LAlt:
                return keyboard::left_alt;
            break;
            case sf::Keyboard::LSystem:
                return keyboard::left_system;
            break;
            case sf::Keyboard::RControl:
                return keyboard::right_control;
            break;
            case sf::Keyboard::RShift:
                return keyboard::right_escape;
            break;
            case sf::Keyboard::RAlt:
                return keyboard::right_alt;
            break;
            case sf::Keyboard::RSystem:
                return keyboard::right_system;
            break;
            case sf::Keyboard::Menu:
                return keyboard::menu;
            break;
            case sf::Keyboard::Return:
                return keyboard::return_;
            break;
            case sf::Keyboard::BackSpace:
                return keyboard::backspace;
            break;
            case sf::Keyboard::Tab:
                return keyboard::tab;
            break;
            case sf::Keyboard::PageUp:
                return keyboard::page_up;
            break;
            case sf::Keyboard::PageDown:
                return keyboard::page_down;
            break;
            case sf::Keyboard::End:
                return keyboard::end_;
            break;
            case sf::Keyboard::Home:
                return keyboard::home;
            break;
            case sf::Keyboard::Insert:
                return keyboard::insert_;
            break;
            case sf::Keyboard::Delete:
                return keyboard::delete_;
            break;
            case sf::Keyboard::Left:
                return keyboard::left;
            break;
            case sf::Keyboard::Right:
                return keyboard::right;
            break;
            case sf::Keyboard::Up:
                return keyboard::up;
            break;
            case sf::Keyboard::Down:
                return keyboard::down;
            break;
            case sf::Keyboard::Numpad0:
                return keyboard::numpad0;
            break;
            case sf::Keyboard::Numpad1:
                return keyboard::numpad1;
            break;
            case sf::Keyboard::Numpad2:
                return keyboard::numpad2;
            break;
            case sf::Keyboard::Numpad3:
                return keyboard::numpad3;
            break;
            case sf::Keyboard::Numpad4:
                return keyboard::numpad4;
            break;
            case sf::Keyboard::Numpad5:
                return keyboard::numpad5;
            break;
            case sf::Keyboard::Numpad6:
                return keyboard::numpad6;
            break;
            case sf::Keyboard::Numpad7:
                return keyboard::numpad7;
            break;
            case sf::Keyboard::Numpad8:
                return keyboard::numpad8;
            break;
            case sf::Keyboard::Numpad9:
                return keyboard::numpad9;
            break;
            case sf::Keyboard::F1:
                return keyboard::f1;
            break;
            case sf::Keyboard::F2:
                return keyboard::f2;
            break;
            case sf::Keyboard::F3:
                return keyboard::f3;
            break;
            case sf::Keyboard::F4:
                return keyboard::f4;
            break;
            case sf::Keyboard::F5:
                return keyboard::f5;
            break;
            case sf::Keyboard::F6:
                return keyboard::f6;
            break;
            case sf::Keyboard::F7:
                return keyboard::f7;
            break;
            case sf::Keyboard::F8:
                return keyboard::f8;
            break;
            case sf::Keyboard::F9:
                return keyboard::f9;
            break;
            case sf::Keyboard::F10:
                return keyboard::f10;
            break;
            case sf::Keyboard::F11:
                return keyboard::f11;
            break;
            case sf::Keyboard::F12:
                return keyboard::f12;
            break;
            case sf::Keyboard::F13:
                return keyboard::f13;
            break;
            case sf::Keyboard::F14:
                return keyboard::f14;
            break;
            case sf::Keyboard::F15:
                return keyboard::f14;
            break;
            case sf::Keyboard::Pause:
                return keyboard::pause;
            break;
            case sf::Keyboard::Unknown:
            default:
                return keyboard::unknown;
            break;
        };
    }

}
#endif // SFML_KEY_CONVERT_H
