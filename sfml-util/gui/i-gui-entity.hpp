#ifndef SFMLUTIL_GUI_IGUIENTITY_INCLUDED
#define SFMLUTIL_GUI_IGUIENTITY_INCLUDED
//
// i-gui-entity.hpp
//  Common interface for all gui elements
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/mouse-state-enum.hpp"
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/gui/i-clickable.hpp"

#include <string>
#include <vector>
#include <set>
#include <list>


namespace sfml_util
{
namespace gui
{

    //Interface for GuiEntities
    class IGuiEntity
    :
        public sf::Drawable,
        public IClickable
    {
    public:
        IGuiEntity() {}
        virtual ~IGuiEntity() {}

        virtual bool GetEntityWillDraw() const = 0;
        virtual void SetEntityWillDraw(const bool WILL_DRAW) = 0;

        virtual const sf::Vector2f GetEntityPos() const = 0;
        virtual void SetEntityPos(const sf::Vector2f & V) = 0;
        virtual void SetEntityPos(const float LEFT, const float TOP) = 0;
        virtual void MoveEntityPos(const float HORIZ, const float VERT) = 0;

        virtual const std::string GetEntityName() const = 0;

        virtual const sf::FloatRect GetEntityRegion() const = 0;
        virtual void  SetEntityRegion(const sf::FloatRect & R) = 0;

        virtual bool  IsInEntityRegion(const sf::Vector2f & V) const = 0;

        virtual void SetEntityColors(const ColorSet & COLOR_SET) = 0;
        virtual void SetEntityColorFg(const sf::Color & FG_COLOR) = 0;
        virtual void SetEntityColorFgBoth(const sf::Color & FG_COLOR) = 0;
        virtual const ColorSet GetEntityColors() const = 0;
        virtual const sf::Color GetEntityColorForeground() const = 0;
        virtual const sf::Color GetEntityColorBackground() const = 0;

        virtual bool KeyPress(const sf::Event::KeyEvent &) = 0;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) = 0;
    protected:
        virtual void OnColorChange() = 0;

        //The only member equired by sf::Drawable.
    public:
        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const = 0;

        //IClickable
        virtual MouseState::Enum GetMouseState() const = 0;
        virtual void SetMouseState(const MouseState::Enum) = 0;
        //
        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V) = 0;
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V) = 0;
        virtual bool IsHeldDown() const = 0;
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) = 0;
        virtual bool UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION) = 0;
        virtual bool UpdateTime(const float ELAPSED_TIME_SEC) = 0;
        virtual bool HasFocus() const = 0;
        virtual bool SetHasFocus(const bool) = 0;
        virtual bool WillAcceptFocus() const = 0;
        virtual void SetWillAcceptFocus(const bool) = 0;
        virtual void FakeColorSetAsIfFocusIs(const bool) = 0;
        virtual const std::string GetMouseHoverText(const sf::Vector2f &) = 0;
        virtual void SetMouseHoverText(const std::string &) = 0;
    protected:
        virtual void OnClick(const sf::Vector2f &) = 0;
        virtual void OnDoubleClick(const sf::Vector2f &) = 0;
    };

    using IGuiEntityPtr_t   = IGuiEntity *;
    using IGuiEntitySPtr_t  = std::shared_ptr<IGuiEntity>;
    using IGuiEntitySVec_t  = std::vector<IGuiEntitySPtr_t>;
    using IGuiEntitySSet_t  = std::set<IGuiEntitySPtr_t>;
    using IGuiEntitySList_t = std::list<IGuiEntitySPtr_t>;

}
}
#endif //SFMLUTIL_GUI_IGUIENTITY_INCLUDED
