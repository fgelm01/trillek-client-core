#include "services\sfml_window_service.h"
#include "services\settings_service.h"
#include "services\sfml_key_convert.h"
#include "services\sfml_mouse_button_convert.h"
#include "services\event_service_events.h"
#include "client.h"

namespace trillek
{

sfml_window_service::sfml_window_service(client* _client)
    : window_service(_client)
{

}

sfml_window_service::~sfml_window_service()
{
    //dtor
}

bool sfml_window_service::is_open()
{
    return sfml_window.isOpen();
}

void sfml_window_service::open()
{
    settings_service* settings=_client->get_settings_service();
    int width=settings->get("window")->get("width")->to_int(800);
    int height=settings->get("window")->get("height")->to_int(600);
    int depth=settings->get("window")->get("depth")->to_int(32);
    std::string title=settings->get("title")->to_string("Trillek");

    sf::VideoMode vm(width,height,depth);
    sfml_window.create(vm,title);
    sfml_window.setKeyRepeatEnabled(false);
    sfml_window.setMouseCursorVisible(false);

    event_service* e_s=this->_client->get_event_service();
    e_s->send_event(std::make_shared<window_resized_event>(
                                    width,
                                    height));
}

void sfml_window_service::close()
{
    sfml_window.close();
}

void sfml_window_service::activate()
{
    sfml_window.setActive(true);
}

void sfml_window_service::finish_frame()
{
    sfml_window.display();
}

vector2d<unsigned int> sfml_window_service::get_size()
{
    sf::Vector2u s=sfml_window.getSize();
    return vector2d<unsigned int>(s.x,s.y);
}

void sfml_window_service::set_mouse_pos(float x, float y)
{
    sf::Vector2u s=sfml_window.getSize();
    sf::Mouse::setPosition(sf::Vector2i(s.x*x,s.y*y),sfml_window);
}

void sfml_window_service::set_mouse_pos(int x, int y)
{
    sf::Mouse::setPosition(sf::Vector2i(x,y),sfml_window);
}

void sfml_window_service::process()
{
    sf::Event event;
    event_service* e_s=this->_client->get_event_service();
    while(sfml_window.pollEvent(event))
    {
        switch(event.type)
        {
            // Window Events
            case sf::Event::EventType::Closed:
                this->sfml_window.close();
            break;
            case sf::Event::EventType::Resized:
                e_s->send_event(std::make_shared<window_resized_event>(
                                            event.size.width,
                                            event.size.height));
            break;
            // Keyboard Events
            case sf::Event::EventType::KeyPressed:
                e_s->send_event(std::make_shared<key_event>(
                                            true,
                                            sfml_key_convert(event.key.code)));
            break;
            case sf::Event::EventType::KeyReleased:
                e_s->send_event(std::make_shared<key_event>(
                                            false,
                                            sfml_key_convert(event.key.code)));
            break;
            // Mouse Events
            case sf::Event::EventType::MouseButtonPressed:
                e_s->send_event(std::make_shared<mouse_button_event>(
                                true,
                                sfml_mouse_button_convert(event.mouseButton.button)));
            break;
            case sf::Event::EventType::MouseButtonReleased:
                e_s->send_event(std::make_shared<mouse_button_event>(
                                false,
                                sfml_mouse_button_convert(event.mouseButton.button)));
            break;
            case sf::Event::EventType::MouseMoved:
                e_s->send_event(std::make_shared<mouse_move_event>(
                                            event.mouseMove.x,
                                            event.mouseMove.y));
            break;
            case sf::Event::EventType::MouseWheelMoved:
                e_s->send_event(std::make_shared<mouse_wheel_event>(
                                            event.mouseWheel.delta));
            break;
            /*
                -------- Unused --------------
                    TODO: Use them all :)
            */
            case sf::Event::EventType::JoystickButtonPressed:
            break;
            case sf::Event::EventType::JoystickButtonReleased:
            break;
            case sf::Event::EventType::JoystickConnected:
            break;
            case sf::Event::EventType::JoystickDisconnected:
            break;
            case sf::Event::EventType::JoystickMoved:
            break;
            case sf::Event::EventType::TextEntered:
            break;
            case sf::Event::EventType::GainedFocus:
            break;
            case sf::Event::EventType::LostFocus:
            break;
            case sf::Event::EventType::MouseEntered:
            break;
            case sf::Event::EventType::MouseLeft:
            break;
            default:
                std::cerr << "Unknown EventType encountered: " << event.type << std::endl;
            break;
        }
    }
}

}
