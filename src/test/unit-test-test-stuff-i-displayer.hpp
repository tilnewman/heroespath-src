// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_UNIT_TEST_TEST_STUFF_I_DISPLAYER_INCLUDED
#define HEROESPATH_UNIT_TEST_TEST_STUFF_I_DISPLAYER_INCLUDED
//
// unit-test-test-stuff-i-displayer.hpp
//
#include <cstdlib>
#include <string>

#include <SFML/Graphics/Drawable.hpp>

namespace heroespath
{
namespace gui
{
    class CachedTexture;
}
} // namespace heroespath

struct IDisplayer : public sf::Drawable
{
    virtual ~IDisplayer() = default;

    virtual const std::string name() const = 0;

    virtual void setup(const sf::FloatRect & FULL_SCREEN_RECT) = 0;

    virtual void teardown() = 0;

    virtual void releaseAndFreeAllTextures() = 0;

    virtual void
        beginImageSeries(const std::string & TITLE_STR, const std::size_t EXPECTED_IMAGE_COUNT = 0)
        = 0;

    virtual void endImageSeries() = 0;

    virtual void appendImageToSeries(heroespath::gui::CachedTexture && cachedTexture) = 0;

    void draw(sf::RenderTarget &, sf::RenderStates = sf::RenderStates()) const override = 0;
};

#endif // HEROESPATH_UNIT_TEST_TEST_STUFF_I_DISPLAYER_INCLUDED
