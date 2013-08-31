#include "services\input_service.h"
#include "client.h"

namespace trillek
{

input_service::input_service(client* _client)
    :service(_client)
{
    this->keys_pressed.resize(keyboard::num_key_codes,false);
    trap_mouse=true;
}

input_service::~input_service()
{
    //dtor
}

void input_service::init()
{
    event_service* e_s=this->_client->get_event_service();
    e_s->register_for_event(event::event_type::key,this);
    e_s->register_for_event(event::event_type::mouse_move,this);
    e_s->register_for_event(event::event_type::mouse_button,this);
    e_s->register_for_event(event::event_type::mouse_wheel,this);
}

void input_service::receive_event(std::shared_ptr<event> e)
{
    auto type=e->get_type();
    if(type==event::key) {
        auto k_e=std::dynamic_pointer_cast<key_event>(e);
        this->keys_pressed[k_e->code]=k_e->pressed;
        if(k_e->pressed)
            std::cerr << "Key pressed: " << k_e->code << std::endl;
        else
            std::cerr << "Key released: " << k_e->code << std::endl;
        if(k_e->code==keyboard::key_code::escape)
            this->_client->get_window_service()->close();
    }else if(type==event::mouse_move) {
        auto mm_e=std::dynamic_pointer_cast<mouse_move_event>(e);
        this->mouse_move(mm_e);
    }else if(type==event::mouse_button) {
        auto mb_e=std::dynamic_pointer_cast<mouse_button_event>(e);
    }else if(type==event::mouse_wheel) {
        auto mw_e=std::dynamic_pointer_cast<mouse_wheel_event>(e);
    }
}

void input_service::mouse_move(std::shared_ptr<mouse_move_event> e)
{
    if(trap_mouse){
        window_service* w_s=this->_client->get_window_service();
        vector2d<unsigned int> s=w_s->get_size();
        int dx=e->x-s.x/2;
        int dy=e->y-s.y/2;
        if((dx!=0 ||dy!=0))
        {
            w_s->set_mouse_pos(0.5f,0.5f);
        }

        for(auto listener : mouse_listeners)
        {
            listener->mouse_listen(dx,dy);
        }
    }else
    {
    }
}

void input_service::register_mouse_listener(
                            std::shared_ptr<mouse_listener> listener)
{
    mouse_listeners.push_back(listener);
}

}
