// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// drawables-displayer.cpp
//
#include "drawables-displayer.hpp"

#include <SFML/Graphics.hpp>

namespace heroespath
{
namespace test
{

    DrawablesDisplayer::DrawablesDisplayer()
        : DisplayerBase("DrawablesDisplayer")
    {}

    void DrawablesDisplayer::releaseAndFreeAll() { m_drawables.clear(); }

    void DrawablesDisplayer::appendDrawable(std::unique_ptr<sf::Drawable> drawableUPtr)
    {
        m_drawables.emplace_back(std::move(drawableUPtr));
    }

    void DrawablesDisplayer::draw(sf::RenderTarget & target, sf::RenderStates) const
    {
        for (const auto & DRAWABLE_UPTR : m_drawables)
        {
            target.draw(*DRAWABLE_UPTR);
        }

        drawCommon(target);
    }

} // namespace test
} // namespace heroespath
