// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_HPP_INCLUDED
#define HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_HPP_INCLUDED
//
// single-image-displayer.hpp
//
#include "test/util/displayer-base.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace gui
{
    class CachedTexture;
}
namespace test
{

    struct SingleImageDisplayer : public DisplayerBase
    {
        SingleImageDisplayer();
        virtual ~SingleImageDisplayer();

        void releaseAndFreeAll() override;
        void appendImageToSeries(gui::CachedTexture cachedTexture) override;

        void draw(sf::RenderTarget &, sf::RenderStates = sf::RenderStates()) const override;

    private:
        void appendTexture(gui::CachedTexture cachedTexture);
        void appendSprite();

    private:
        std::vector<std::unique_ptr<gui::CachedTexture>> m_cachedTextureUPtrs;
        std::vector<sf::Sprite> m_sprites;
    };

} // namespace test
} // namespace heroespath

#endif // HEROESPATH_TESTUTIL_SINGLE_IMAGE_DISPLAYER_HPP_INCLUDED
