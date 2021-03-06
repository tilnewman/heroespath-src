// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

#include "sfutil/scale.hpp"

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
        const bool WILL_PREFIX, const Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::string str;
        str.reserve(1024);

        if (WILL_PREFIX)
        {
            str += std::string("MouseImageInfo").append((WILL_WRAP == Wrap::Yes) ? "" : "=");
        }

        if (WILL_WRAP == Wrap::Yes)
        {
            str += '(';
        }

        str += "Up=";
        str += up.ToString(false, Wrap::Yes);
        str += SEPARATOR;
        str += "Down=";
        str += down.ToString(false, Wrap::Yes);
        str += SEPARATOR;
        str += "Over=";
        str += over.ToString(false, Wrap::Yes);
        str += SEPARATOR;
        str += "Disabled=";
        str += disabled.ToString(false, Wrap::Yes);
        str += SEPARATOR;
        str += "will_draw_up_if_missing=";
        str += misc::ToString(will_draw_up_if_missing);

        if (WILL_WRAP == Wrap::Yes)
        {
            str += ')';
        }

        return str;
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
