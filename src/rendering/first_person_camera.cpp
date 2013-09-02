#include "rendering/first_person_camera.h"
#include "client.h"
#include <cmath>

namespace trillek
{

first_person_camera::first_person_camera()
    :   rot(0,0), pos(0,0,0)
{
    //ctor
}

first_person_camera::~first_person_camera()
{
    //dtor
}

void first_person_camera::mouse_listen(float dx, float dy)
{
    rot.y+=(dx*-1.0f)/10.0f;
    if(rot.y<0)
        rot.y+=360;
    if(rot.y>360)
        rot.y-=360;

    rot.x+=(dy*-1.0f)/10.0f;
        if(rot.x<-90)
            rot.x=-90;
        if(rot.x>90)
            rot.x=90;
}

void first_person_camera::render(graphics_service* service)
{
    input_service* i_s = service->get_client()->get_input_service();
    float conv=M_PI/180.0f;
    float speed=0.0125f;
    if(i_s->is_key_pressed((keyboard::key_code)'w'))
    {
        pos.x+=sin(rot.y*conv)*cos(rot.x*conv)*speed;
        pos.z+=cos(rot.y*conv)*cos(rot.x*conv)*speed;
        pos.y-=sin(rot.x*conv)*speed;
    }
    if(i_s->is_key_pressed((keyboard::key_code)'s'))
    {
        pos.x-=sin(rot.y*conv)*cos(rot.x*conv)*speed;
        pos.z-=cos(rot.y*conv)*cos(rot.x*conv)*speed;
        pos.y+=sin(rot.x*conv)*speed;
    }
    if(i_s->is_key_pressed((keyboard::key_code)'a'))
    {
        pos.x+=cos(rot.y*conv)*speed;
        pos.z-=sin(rot.y*conv)*speed;
    }
    if(i_s->is_key_pressed((keyboard::key_code)'d'))
    {
        pos.x-=cos(rot.y*conv)*speed;
        pos.z+=sin(rot.y*conv)*speed;
    }
    service->do_rotation_x(-rot.x);
    service->do_rotation_y(-rot.y);
    service->do_translation(pos);
}

}
