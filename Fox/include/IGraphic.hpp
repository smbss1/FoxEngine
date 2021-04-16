

#ifndef FOX_IGRAPHIC_HPP_
#define FOX_IGRAPHIC_HPP_

namespace fox
{
    class Input;
    class IGraphic
    {
    public:
        virtual ~IGraphic() = 0;
        virtual void init() = 0;
        virtual void shutdown() = 0;
        virtual void poll_event(Input&) = 0;
        virtual void draw() = 0;
        virtual void set_vsync(bool value) = 0;
        virtual bool is_vsync() = 0;
        virtual bool quit_requested() = 0;
    };
} // namespace fox

#endif