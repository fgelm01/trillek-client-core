#ifndef MOUSE_LISTENER_H
#define MOUSE_LISTENER_H

namespace trillek
{

class mouse_listener
{
    public:
        mouse_listener() {}
        virtual ~mouse_listener() {}
        virtual void mouse_listen(float dx, float dy)=0;
    protected:
    private:
};

}

#endif // MOUSE_LISTENER_H
