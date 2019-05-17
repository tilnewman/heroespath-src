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
#include "test/util/displayer-base.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <memory>
#include <vector>

namespace heroespath
{
namespace test
{

    struct DrawablesDisplayer : public DisplayerBase
    {
        DrawablesDisplayer();
        virtual ~DrawablesDisplayer() = default;

        void releaseAndFreeAll() override;
        void appendDrawable(std::unique_ptr<sf::Drawable> drawableUPtr) override;
        void draw(sf::RenderTarget & target, sf::RenderStates = sf::RenderStates()) const override;

    private:
        std::vector<std::unique_ptr<sf::Drawable>> m_drawables;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_DRAWABLES_DISPLAYER_HPP_INCLUDED
