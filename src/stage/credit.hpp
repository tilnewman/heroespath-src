// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_CREDIT_HPP_INCLUDED
#define HEROESPATH_STAGE_CREDIT_HPP_INCLUDED
//
// credit.hpp
//
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/font-enum.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    } // namespace gui
} // namespace sfml_util

namespace stage
{

    struct MediaType
    {
        enum Enum
        {
            Text = 0,
            Image,
            Anim,
            Count
        };
    };

    // responsibe for drawing a credit on screen
    class Credit : public sf::Drawable
    {
    public:
        // use to create text only credits
        Credit(
            sf::FloatRect & trackingRect,
            const std::string & TITLE_TEXT,
            const std::string & CONTENT_TEXT);

        // use to create animation credits
        Credit(
            sf::FloatRect & trackingRect,
            const std::string & TITLE_TEXT,
            const std::string & CONTENT_TEXT,
            const sfml_util::Animations::Enum ANIM_ENUM,
            const float ANIM_SIZE_HORIZ,
            const float ANIM_FRAME_TIME_SEC);

        // use to create general image credits
        Credit(
            sf::FloatRect & trackingRect,
            const std::string & TITLE_TEXT,
            const std::string & CONTENT_TEXT,
            const std::string & IMAGE_PATH_KEY,
            const float IMAGE_SIZE_HORIZ);

        // specialization used to create font (image) credits
        Credit(
            sf::FloatRect & trackingRect,
            const std::string & TITLE_TEXT,
            const sfml_util::GuiFont::Enum FONT,
            const std::string & CONTENT_TEXT);

        // used to help construct all types of credits
        void Setup(
            sf::FloatRect & trackingRect,
            const std::string & TITLE_TEXT,
            const sfml_util::GuiFont::Enum TITLE_FONT,
            const unsigned int TITLE_FONT_SIZE,
            const std::string & CONTENT_TEXT,
            const MediaType::Enum MEDIA_TYPE,
            const float MEDIA_SIZE_HORIZ = 0.0f,
            const sfml_util::Animations::Enum ANIM_ENUM = sfml_util::Animations::Count,
            const float ANIM_FRAME_TIME_SEC = 0.0f);

        void draw(sf::RenderTarget &, sf::RenderStates) const override final;
        void UpdateTime(const float ELAPSED_TIME_SECONDS);
        void Move(const float ADJ_HORIZ, const float ADJ_VERT);
        bool IsVisible() const;

    private:
        sfml_util::gui::TextRegionUPtr_t titleTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t contentTextUPtr_;
        MediaType::Enum mediaType_;
        sfml_util::CachedTextureOpt_t cachedTextureOpt_;
        sf::Sprite sprite_;
        sfml_util::AnimationUPtr_t animUPtr_;
        float screenSizeVert_;
    };

    using CreditUPtr_t = std::unique_ptr<Credit>;
    using CreditUVec_t = std::vector<CreditUPtr_t>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_CREDIT_HPP_INCLUDED
