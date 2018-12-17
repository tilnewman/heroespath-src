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
#include "gui/menu-image-enum.hpp"

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
        explicit StageTitle(
            const MenuImage::Enum MENU_IMAGE = MenuImage::Count,
            const bool WILL_INVERT_SYMBOL_TO_WHITE = true,
            const float SYMBOL_HEIGHT_SCREEN_RATIO = DEFAULT_SYMBOL_HEIGHT_SCREEN_RATIO_,
            const float SYMBOL_TO_TITLE_HEIGHT_RATIO = DEFAULT_SYMBOL_TO_TITLE_HEIGHT_RATIO_,
            const float SYMBOL_VERT_POS_SCREEN_RATIO = DEFAULT_SYMBOL_VERT_POS_SCREEN_RATIO_);

        StageTitle(const StageTitle &) = delete;
        StageTitle(StageTitle &&) = delete;
        StageTitle & operator=(const StageTitle &) = delete;
        StageTitle & operator=(StageTitle &&) = delete;

        // for all floats: <=0 means "use default"
        void SetSizeAndReCenter(
            const float SYMBOL_HEIGHT_SCREEN_RATIO = DEFAULT_SYMBOL_HEIGHT_SCREEN_RATIO_,
            const float SYMBOL_TO_TITLE_HEIGHT_RATIO = DEFAULT_SYMBOL_TO_TITLE_HEIGHT_RATIO_,
            const float SYMBOL_VERT_POS_SCREEN_RATIO = DEFAULT_SYMBOL_VERT_POS_SCREEN_RATIO_);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;
        const sf::FloatRect Region() const { return symbolSprite_.getGlobalBounds(); }
        float DefaultBottomPad() const;
        float VisibleVertPosRatio() const { return VISIBLE_VERT_POS_RATIO_; }

    public:
        static const float DEFAULT_SYMBOL_HEIGHT_SCREEN_RATIO_;
        static const float DEFAULT_SYMBOL_TO_TITLE_HEIGHT_RATIO_;
        static const float DEFAULT_SYMBOL_VERT_POS_SCREEN_RATIO_;
        static const float DEFAULT_BOTTOM_PAD_AS_SYMBOL_HEIGHT_RATIO_;
        static const float VISIBLE_VERT_POS_RATIO_;

    private:
        gui::CachedTexture symbolCachedTexture_;
        sf::Sprite symbolSprite_;
        gui::CachedTextureOpt_t titleCachedTextureOpt_;
        sf::Sprite titleSprite_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_STAGE_TITLE_HPP_INCLUDED
