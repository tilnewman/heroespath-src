// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MOUSE_IMAGE_INFO_HPP_INCLUDED
#define HEROESPATH_GUI_MOUSE_IMAGE_INFO_HPP_INCLUDED
//
// mouse-image-info.hpp
//
#include "sfml-util/entity-image-info.hpp"
#include "sfml-util/mouse-state-enum.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <tuple>

namespace heroespath
{
namespace gui
{

    // Responsible for wrapping all the information needed by a Entity to store and display a
    // unique image for each of the three MouseStates.
    struct MouseImageInfo
    {
        explicit MouseImageInfo(
            const bool WILL_DRAW_UP_IF_MISSING = false,
            const EntityImageInfo & UP = EntityImageInfo(),
            const EntityImageInfo & DOWN = EntityImageInfo(),
            const EntityImageInfo & OVER = EntityImageInfo(),
            const EntityImageInfo & DISABLED = EntityImageInfo())
            : will_draw_up_if_missing(WILL_DRAW_UP_IF_MISSING)
            , up(UP)
            , down(DOWN)
            , over(OVER)
            , disabled(DISABLED)
        {}

        MouseImageInfo(const MouseImageInfo &) = default;
        MouseImageInfo(MouseImageInfo &&) = default;
        MouseImageInfo & operator=(const MouseImageInfo &) = default;
        MouseImageInfo & operator=(MouseImageInfo &&) = default;

        EntityImageInfo & FromMouseState(const MouseState::Enum MOUSE_STATE)
        {
            return const_cast<EntityImageInfo &>(
                static_cast<const MouseImageInfo &>(*this).FromMouseState(MOUSE_STATE));
        }

        const EntityImageInfo & FromMouseState(const MouseState::Enum MOUSE_STATE) const;

        const sf::FloatRect MinimallyEnclosingRegion() const;

        MouseState::Enum ToDraw(const MouseState::Enum MOUSE_STATE) const;

        const std::string ToString(
            const bool WILL_PREFIX = true,
            const misc::Wrap WILL_WRAP = misc::Wrap::Yes,
            const std::string & SEPARATOR = "_") const;

        void SetRegion(const sf::FloatRect & NEW_GLOBAL_BOUNDS);

        bool will_draw_up_if_missing;
        EntityImageInfo up;
        EntityImageInfo down;
        EntityImageInfo over;
        EntityImageInfo disabled;
    };

    inline bool operator==(const MouseImageInfo & L, const MouseImageInfo & R)
    {
        return std::tie(L.will_draw_up_if_missing, L.up, L.down, L.over, L.disabled)
            == std::tie(R.will_draw_up_if_missing, R.up, R.down, R.over, R.disabled);
    }

    inline bool operator!=(const MouseImageInfo & L, const MouseImageInfo & R) { return !(R == L); }

} // namespace gui

inline std::ostream & operator<<(std::ostream & os, const gui::MouseImageInfo & MII)
{
    os << MII.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_GUI_MOUSE_IMAGE_INFO_HPP_INCLUDED
