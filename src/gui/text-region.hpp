// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXTREGION_HPP_INCLUDED
#define HEROESPATH_GUI_TEXTREGION_HPP_INCLUDED
//
// text-region.hpp
//  A class that encapsulates text drawn to a bounded region that may need a scrollbar.
//
#include "gui/box-entity-info.hpp"
#include "gui/callback.hpp"
#include "gui/entity.hpp"
#include "gui/margins.hpp"
#include "gui/sliderbar.hpp"
#include "gui/text-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace sf
{
class RenderTexture;
}

namespace heroespath
{

namespace stage
{
    struct IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
    using IStagePtrOpt_t = boost::optional<IStagePtr_t>;

} // namespace stage

namespace gui
{

    using RenderTextureUPtr_t = std::unique_ptr<sf::RenderTexture>;

    class BoxEntity;
    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

    // Encapsulates text drawn and bounded to an area that might need a scroll bar
    class TextRegion
        : public Entity
        , public SliderBar::Callback_t::IHandler_t
    {
    public:
        TextRegion(const TextRegion &) = delete;
        TextRegion(TextRegion &&) = delete;
        TextRegion & operator=(const TextRegion &) = delete;
        TextRegion & operator=(TextRegion &&) = delete;

        // if using this constructor, Setup() must be called before any other function
        explicit TextRegion(const std::string & NAME);

        // If this constructor is used then no scrollbar will be created,
        // even if the text height exceeds the REGION.
        TextRegion(
            const std::string & NAME,
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION = sf::FloatRect(),
            const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo(),
            const Margins & MARGINS = Margins());

        TextRegion(
            const std::string & NAME,
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const stage::IStagePtr_t ISTAGE_PTR,
            const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo(),
            const Margins & MARGINS = Margins());

        virtual ~TextRegion();

        // If this Setup() is used then no scrollbar will be created,
        // even if the text height exceeds the REGION
        void Setup(
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo(),
            const Margins & MARGINS = Margins());

        void Setup(
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT,
            const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo(),
            const Margins & MARGINS = Margins(),
            const bool WILL_ALLOW_SCROLLBAR = true);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        bool HandleCallback(const SliderBar::Callback_t::PacketPtr_t) override;

        const std::string GetText() const { return textInfoOrig_.text; }
        void SetText(const std::string &);

        void Append(const TextRegion &);

        const TextInfo GetTextInfo() const { return textInfoOrig_; }

        void ShrinkEntityRegionToFitText();

    protected:
        void HandleSliderBar(const SliderBarPtrOpt_t);

        void OnColorChange() override;

        const SliderBarPtr_t MakeSliderBar(const sf::FloatRect & REGION) const;

        void SetupEntityRegion(const sf::FloatRect & REGION_ORIG);

        void SetupBox(const BoxEntityInfo & BOX_INFO);

        const SliderBarPtrOpt_t RenderText(
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION,
            const Margins & MARGINS,
            const unsigned int SMALLER_FONT_SIZE);

        void SetEntityRegion(const sf::FloatRect & R) override { Entity::SetEntityRegion(R); }

    private:
        void EstablishWhichLinesToDraw(const float SCROLL_RATIO);

    public:
        static const unsigned int DEFAULT_NO_RESIZE_;

    protected:
        BoxEntityUPtr_t boxEntityUPtr_;
        SliderBarUPtr_t sliderBarUPtr_;
        stage::IStagePtrOpt_t stagePtrOpt_;
        std::size_t startLine_;
        std::size_t stopLine_;
        sf::FloatRect regionOrig_;
        TextInfo textInfoOrig_;
        unsigned int smallFontSizeOrig_;
        Margins marginsOrig_;
        bool allowScrollbarOrig_;
        RenderTextureUPtr_t renderTextureUPtr_;
        sf::Sprite sprite_;
        bool willDraw_;
    };

    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    using TextRegionUVec_t = std::vector<TextRegionUPtr_t>;
    using TextRegionSPtr_t = std::shared_ptr<TextRegion>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXTREGION_HPP_INCLUDED
