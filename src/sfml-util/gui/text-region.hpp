// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_TEXTREGION_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_TEXTREGION_HPP_INCLUDED
//
// text-region.hpp
//  A class that encapsulates text drawn to a bounded region that may need a scrollbar.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/text-rendering.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
    using IStagePtrOpt_t = boost::optional<IStagePtr_t>;

    namespace gui
    {
        namespace box
        {
            class Box;
            using BoxUPtr_t = std::unique_ptr<Box>;
        } // namespace box

        // Responsible for wrapping the line number and segment number combination of what text will
        // be drawn.
        struct TextSnipNum
        {
            TextSnipNum(const std::size_t LINE, const std::size_t SEGMENT)
                : line_num(LINE)
                , seg_num(SEGMENT)
            {}

            std::size_t line_num;
            std::size_t seg_num;
        };

        using TextSnipNumVec_t = std::vector<TextSnipNum>;

        // Encapsulates text drawn and bounded to an area that might need a scroll bar
        class TextRegion
            : public GuiEntity
            , public callback::ISliderBarCallbackHandler_t
        {
        public:
            TextRegion(const TextRegion &) = delete;
            TextRegion(TextRegion &&) = delete;
            TextRegion & operator=(const TextRegion &) = delete;
            TextRegion & operator=(TextRegion &&) = delete;

        public:
            // if using this constructor, Setup() must be called before any other function
            explicit TextRegion(const std::string & NAME);

            // If this constructor is used then no scrollbar will be created,
            // even if the text height exceeds the REGION.
            TextRegion(
                const std::string & NAME,
                const TextInfo & TEXT_INFO,
                const sf::FloatRect & REGION = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f),
                const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
                const box::Info & BOX_INFO = gui::box::Info(),
                const Margins & MARGINS = Margins());

            TextRegion(
                const std::string & NAME,
                const TextInfo & TEXT_INFO,
                const sf::FloatRect & REGION,
                const IStagePtr_t ISTAGE_PTR,
                const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
                const box::Info & BOX_INFO = gui::box::Info(),
                const Margins & MARGINS = Margins());

            virtual ~TextRegion();

            // If this Setup() is used then no scrollbar will be created,
            // even if the text height exceeds the REGION
            void Setup(
                const TextInfo & TEXT_INFO,
                const sf::FloatRect & REGION,
                const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
                const box::Info & BOX_INFO = box::Info(),
                const Margins & MARGINS = Margins());

            void Setup(
                const TextInfo & TEXT_INFO,
                const sf::FloatRect & REGION,
                const IStagePtrOpt_t ISTAGE_PTR_OPT,
                const unsigned int SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
                const box::Info & BOX_INFO = box::Info(),
                const Margins & MARGINS = Margins(),
                const bool WILL_ALLOW_SCROLLBAR = true);

            void HandleSliderBar(const sfml_util::gui::SliderBarPtrOpt_t);
            void HandleBox(const box::Info & BOX_INFO);

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            void SetEntityPos(const sf::Vector2f & V) override { SetEntityPos(V.x, V.y); }
            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;
            void MoveEntityPos(const sf::Vector2f & V) override { MoveEntityPos(V.x, V.y); }
            void MoveEntityPos(const float HORIZ, const float VERT) override;

            const std::string HandlerName() const override { return GetEntityName(); }
            bool HandleCallback(const callback::SliderBarCallbackPackage_t &) override;

            const std::string GetText() const { return text_; }
            void SetText(const std::string &);

            void Append(const TextRegion &);

            const TextInfo GetTextInfo() const { return textInfoOrig_; }

        protected:
            void OnClick(const sf::Vector2f &) override {}

            // Changes the font color by applying the current entity foreground
            // color to the sprite and textInfo_ member, but does not re-render the text.
            void OnColorChange() override;

        private:
            void ResetDrawCache();
            void EstablishWhichLinesToDraw(const float SCROLL_RATIO);
            void EstablishWhichLinesToDraw(const float SCROLL_RATIO, const float REGION_HEIGHT);

        public:
            static const unsigned int DEFAULT_NO_RESIZE_;

        protected:
            box::Info boxInfo_;
            box::BoxUPtr_t boxUPtr_;
            gui::SliderBarUPtr_t sliderBarUPtr_;
            IStagePtrOpt_t stagePtrOpt_;
            std::string text_;
            text_render::RenderedText renderedText_;
            std::size_t startLine_;
            sf::FloatRect regionOrig_;
            sfml_util::gui::TextInfo textInfoOrig_;
            unsigned int smallFontSizeOrig_;
            sfml_util::Margins marginsOrig_;
            bool allowScrollbarOrig_;
            TextSnipNumVec_t textSnipsToDrawVec_;
        };

        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
        using TextRegionUVec_t = std::vector<TextRegionUPtr_t>;
        using TextRegionSPtr_t = std::shared_ptr<TextRegion>;

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_TEXTREGION_HPP_INCLUDED
