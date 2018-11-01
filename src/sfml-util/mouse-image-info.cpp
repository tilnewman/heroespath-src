// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// mouse-image-info.cpp
//
#include "mouse-image-info.hpp"

#include "sfutil/size-and-scale.hpp"

namespace heroespath
{
namespace gui
{

    const EntityImageInfo & MouseImageInfo::FromMouseState(const MouseState::Enum MOUSE_STATE) const
    {
        if (MOUSE_STATE == MouseState::Disabled)
        {
            return disabled;
        }
        else if (MOUSE_STATE == MouseState::Over)
        {
            return over;
        }
        else if (MOUSE_STATE == MouseState::Down)
        {
            return down;
        }
        else
        {
            return up;
        }
    }

    const sf::FloatRect MouseImageInfo::MinimallyEnclosingRegion() const
    {
        sf::FloatRect region(0.0f, 0.0f, 0.0f, 0.0f);

        MouseState::Enum mouseStateToUse { MouseState::Count };
        if (up.HasTexture())
        {
            mouseStateToUse = MouseState::Up;
        }
        else if (over.HasTexture())
        {
            mouseStateToUse = MouseState::Over;
        }
        else if (down.HasTexture())
        {
            mouseStateToUse = MouseState::Down;
        }
        else if (disabled.HasTexture())
        {
            mouseStateToUse = MouseState::Disabled;
        }

        if (MouseState::Count != mouseStateToUse)
        {
            region = FromMouseState(mouseStateToUse).sprite.getGlobalBounds();

            if ((mouseStateToUse != MouseState::Over) && over.HasTexture())
            {
                region = sfutil::MinimallyEnclosing(region, over.sprite.getGlobalBounds());
            }
            else if ((mouseStateToUse != MouseState::Down) && down.HasTexture())
            {
                region = sfutil::MinimallyEnclosing(region, down.sprite.getGlobalBounds());
            }
            else if ((mouseStateToUse != MouseState::Up) && up.HasTexture())
            {
                region = sfutil::MinimallyEnclosing(region, up.sprite.getGlobalBounds());
            }
            else if ((mouseStateToUse != MouseState::Disabled) && disabled.HasTexture())
            {
                region = sfutil::MinimallyEnclosing(region, up.sprite.getGlobalBounds());
            }
        }
        else
        {
            const sf::Vector2f ZERO_V(0.0f, 0.0f);

            if (up.sprite.getPosition() != ZERO_V)
            {
                region = sf::FloatRect(up.sprite.getPosition(), ZERO_V);
            }
            else if (over.sprite.getPosition() != ZERO_V)
            {
                region = sf::FloatRect(over.sprite.getPosition(), ZERO_V);
            }
            else if (disabled.sprite.getPosition() != ZERO_V)
            {
                region = sf::FloatRect(disabled.sprite.getPosition(), ZERO_V);
            }
            else
            {
                region = sf::FloatRect(down.sprite.getPosition(), ZERO_V);
            }
        }

        return region;
    }

    MouseState::Enum MouseImageInfo::ToDraw(const MouseState::Enum MOUSE_STATE) const
    {
        if ((MOUSE_STATE == MouseState::Disabled) && disabled.HasTexture())
        {
            return MouseState::Disabled;
        }
        else if ((MOUSE_STATE == MouseState::Over) && over.HasTexture())
        {
            return MouseState::Over;
        }
        else if ((MOUSE_STATE == MouseState::Down) && down.HasTexture())
        {
            return MouseState::Down;
        }
        else if (((MOUSE_STATE == MouseState::Up) || will_draw_up_if_missing) && up.HasTexture())
        {
            return MouseState::Up;
        }
        else
        {
            return MouseState::Count;
        }
    }

    const std::string MouseImageInfo::ToString(
        const bool WILL_PREFIX, const misc::Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::ostringstream ss;

        ss << "Up=" << up.ToString(false, misc::Wrap::Yes) << SEPARATOR
           << "Down=" << down.ToString(false, misc::Wrap::Yes) << SEPARATOR
           << "Over=" << over.ToString(false, misc::Wrap::Yes) << SEPARATOR
           << "Disabled=" << disabled.ToString(false, misc::Wrap::Yes) << SEPARATOR
           << "will_draw_up_if_missing=" << std::boolalpha << will_draw_up_if_missing;

        const auto PARTS_STR { (
            (WILL_WRAP == misc::Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

        if (WILL_PREFIX)
        {
            return std::string("MouseImageInfo").append((WILL_WRAP == misc::Wrap::Yes) ? "" : "=")
                + PARTS_STR;
        }
        else
        {
            return PARTS_STR;
        }
    }

    void MouseImageInfo::SetRegion(const sf::FloatRect & NEW_GLOBAL_BOUNDS)
    {
        up.SetRegion(NEW_GLOBAL_BOUNDS);
        down.SetRegion(NEW_GLOBAL_BOUNDS);
        over.SetRegion(NEW_GLOBAL_BOUNDS);
        disabled.SetRegion(NEW_GLOBAL_BOUNDS);
    }

} // namespace gui
} // namespace heroespath
