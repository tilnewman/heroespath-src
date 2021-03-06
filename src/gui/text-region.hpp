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
#include "gui/entity.hpp"
#include "gui/margins.hpp"
#include "gui/sliderbar.hpp"
#include "gui/text-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/callback.hpp"
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
        // scrollbars will only be created if a valid IStagePtr is provided
        explicit TextRegion(
            const std::string & NAME,
            const TextInfo & TEXT_INFO = TextInfo(),
            const sf::FloatRect & REGION = sf::FloatRect(),
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo(),
            const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT = boost::none);

        virtual ~TextRegion();

        TextRegion(const TextRegion &) = delete;
        TextRegion(TextRegion &&) = delete;
        TextRegion & operator=(const TextRegion &) = delete;
        TextRegion & operator=(TextRegion &&) = delete;

        const std::string HandleCallback(
            const SliderBar::Callback_t::Packet_t &,
            const std::string & PACKET_DESCRIPTION_IGNORED) override;

        // default values will be ignored/replaced by the original values set by the constructor
        void Setup(
            const TextInfo & TEXT_INFO,
            const sf::FloatRect & REGION = sf::FloatRect(),
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo(),
            const stage::IStagePtrOpt_t & ISTAGE_PTR_OPT = boost::none);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
        void MoveEntityPos(const float HORIZ, const float VERT) override;

        const std::string GetText() const { return textInfo_.text; }
        void SetText(const std::string &);

        const TextInfo GetTextInfo() const { return textInfo_; }

        const sf::FloatRect ActualTextRegion() const;

    protected:
        void SetupRender(const TextInfo & TEXT_INFO, const sf::FloatRect & REGION);
        void SetupSliderbar(const sf::FloatRect & REGION);
        void SetupEntityRegion(const sf::FloatRect & REGION_ORIG);
        void SetupBox(const BoxEntityInfo & BOX_INFO, const sf::FloatRect & REGION);

        void OnColorChange() override;

    protected:
        TextInfo textInfo_;
        BoxEntityUPtr_t boxEntityUPtr_;
        SliderBarUPtr_t sliderBarUPtr_;
        stage::IStagePtrOpt_t stagePtrOpt_;
        RenderTextureUPtr_t renderTextureUPtr_;
        sf::Sprite sprite_;
        bool willDraw_;

        // This class is often constructed with REGION width/height <= 0.0f in order to guide how
        // the text rendering works, and after rendering these original width/height values are lost
        // when the entity and sprite are setup with actual region sizes, so this member saves those
        // original invalid values so that future re-renderings work the same way.
        sf::Vector2f origSize_;
    };

    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    using TextRegionUVec_t = std::vector<TextRegionUPtr_t>;
    using TextRegionSPtr_t = std::shared_ptr<TextRegion>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXTREGION_HPP_INCLUDED
