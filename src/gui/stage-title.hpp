// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_STAGE_TITLE_HPP_INCLUDED
#define HEROESPATH_GUI_STAGE_TITLE_HPP_INCLUDED
//
// stage-title.hpp
//
#include "gui/cached-texture.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <string>

namespace heroespath
{
namespace gui
{

    // Responsible for drawing the stage title symbol and text title image at the top of a stage.
    class StageTitle : public sf::Drawable
    {
    public:
        StageTitle(const StageTitle &) = delete;
        StageTitle(StageTitle &&) = delete;
        StageTitle & operator=(const StageTitle &) = delete;
        StageTitle & operator=(StageTitle &&) = delete;

        explicit StageTitle(
            const std::string & TITLE_IMAGE_PATH_KEY = "",
            const bool WILL_INVERT_SYMBOL = false,
            const float SIZE_HORIZ = 0.0f,
            const float SIZE_VERT = 0.0f);

        void SetSizeAndReCenter(const float SIZE_HORIZ = 0.0f, const float SIZE_VERT = 0.0f);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        const sf::FloatRect Region() const { return symbolSprite_.getGlobalBounds(); }

        float DefaultBottomPad() const;

    private:
        static const float DEFAULT_IMAGE_WIDTH_AS_SCREEN_RATIO_;
        static const float TITLE_IMAGE_HEIGHT_RATIO_OF_SYMBOL_HEIGHT_;
        //
        gui::CachedTexture symbolCachedTexture_;
        sf::Sprite symbolSprite_;
        gui::CachedTextureOpt_t titleCachedTextureOpt_;
        sf::Sprite titleSprite_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_STAGE_TITLE_HPP_INCLUDED
