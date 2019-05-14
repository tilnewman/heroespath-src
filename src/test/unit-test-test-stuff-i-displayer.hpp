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
#include <memory>
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

    virtual const sf::FloatRect contentRegion() const = 0;

    virtual void incrememntProgress() {}

    virtual void setProgress(const std::size_t) {}

    virtual void setup(const sf::FloatRect & FULL_SCREEN_RECT) = 0;
    virtual void teardown() = 0;

    virtual void releaseAndFreeAll() = 0;

    virtual void beginImageSeries(const std::string &, const std::size_t = 0) {}
    virtual void endImageSeries() {}
    virtual void appendImageToSeries(heroespath::gui::CachedTexture &&) {}

    virtual void beginDrawablesSet(const std::string &) {}
    virtual void endDrawablesSet() {}
    virtual void appendDrawable(std::unique_ptr<sf::Drawable> &&) {}

    void draw(sf::RenderTarget &, sf::RenderStates = sf::RenderStates()) const override = 0;
};

#endif // HEROESPATH_UNIT_TEST_TEST_STUFF_I_DISPLAYER_INCLUDED
