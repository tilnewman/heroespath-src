// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_DRAWABLES_DISPLAYER_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_DRAWABLES_DISPLAYER_HPP_INCLUDED
//
// drawables-displayer.hpp
//
#include "testutil/displayer-base.hpp"

namespace heroespath
{
namespace test
{

    struct DrawablesDisplayer : public DisplayerBase
    {
        DrawablesDisplayer()
            : DisplayerBase("DrawablesDisplayer")
        {}

        virtual ~DrawablesDisplayer() = default;

        void releaseAndFreeAll() override { m_drawables.clear(); }

        void appendDrawable(std::unique_ptr<sf::Drawable> && drawableUPtr) override
        {
            m_drawables.emplace_back(std::move(drawableUPtr));
        }

        void draw(sf::RenderTarget & target, sf::RenderStates = sf::RenderStates()) const override
        {
            for (const auto & DRAWABLE_UPTR : m_drawables)
            {
                target.draw(*DRAWABLE_UPTR);
            }

            drawCommon(target);
        }

    private:
        std::vector<std::unique_ptr<sf::Drawable>> m_drawables;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_DRAWABLES_DISPLAYER_HPP_INCLUDED
