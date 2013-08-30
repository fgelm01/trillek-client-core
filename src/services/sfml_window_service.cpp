#include "services\sfml_window_service.h"
#include "services\settings_service.h"
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

    event_service* e_s=this->_client->get_event_service();
    e_s->send_event(std::make_shared<window_resized_event>(
                                    width,
                                    height));
}

void sfml_window_service::activate()
{
    sfml_window.setActive(true);
}

void sfml_window_service::finish_frame()
{
    sfml_window.display();
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
                /*e_s->send_event(std::make_shared<window_resized>(
                                            event.size.width,
                                            event.size.height));*/
            break;
            // Keyboard Events
            case sf::Event::EventType::KeyPressed:
                /*e_s->send_event(std::make_shared<key_pressed>(
                                            event.key.code));*/
            break;
            case sf::Event::EventType::KeyReleased:
            break;
            // Mouse Events
            case sf::Event::EventType::MouseButtonPressed:
            break;
            case sf::Event::EventType::MouseButtonReleased:
            break;
            case sf::Event::EventType::MouseEntered:
            break;
            case sf::Event::EventType::MouseLeft:
            break;
            case sf::Event::EventType::MouseMoved:
            break;
            case sf::Event::EventType::MouseWheelMoved:
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
            default:
                std::cerr << "Unknown EventType encountered: " << event.type << std::endl;
            break;
        }
    }
}

}
