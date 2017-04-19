//
// text-region.cpp
//
#include "text-region.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/box-info.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{

    const unsigned int TextRegion::DEFAULT_NO_RESIZE_(0);


    TextRegion::TextRegion(const std::string & NAME)
    :
        GuiEntity          (std::string(NAME).append("_TextRegion"), 0.0f, 0.0f),
        boxInfo_           (),
        boxSPtr_           (),
        sliderBarSPtr_     (),
        stagePtr_          (nullptr),
        text_              (""),
        renderedText_      (),
        startLine_         (0),
        regionOrig_        (),
        textInfoOrig_      (),
        smallFontSizeOrig_ (),
        marginsOrig_       (),
        allowScrollbarOrig_()
    {}


    TextRegion::TextRegion(const std::string &   NAME,
                           const TextInfo &      TEXT_INFO,
                           const sf::FloatRect & REGION,
                           const unsigned int    SMALLER_FONT_SIZE,
                           const box::Info &     BOX_INFO,
                           const Margins &       MARGINS)
    :
        GuiEntity          (std::string(NAME).append("_TextRegion"), REGION),
        boxInfo_           (),
        boxSPtr_           (),
        sliderBarSPtr_     (),
        stagePtr_          (nullptr),
        text_              (""),
        renderedText_      (),
        startLine_         (0),
        regionOrig_        (),
        textInfoOrig_      (),
        smallFontSizeOrig_ (),
        marginsOrig_       (),
        allowScrollbarOrig_()
    {
        Setup(TEXT_INFO,
              REGION,
              SMALLER_FONT_SIZE,
              BOX_INFO,
              MARGINS);
    }


    TextRegion::TextRegion(const std::string &   NAME,
                           const TextInfo &      TEXT_INFO,
                           const sf::FloatRect & REGION,
                           IStage * const        stagePtr,
                           const unsigned int    SMALLER_FONT_SIZE,
                           const box::Info &     BOX_INFO,
                           const Margins &       MARGINS)
    :
        GuiEntity          (std::string(NAME).append("_TextRegion"), REGION),
        boxInfo_           (),
        boxSPtr_           (),
        sliderBarSPtr_     (),
        stagePtr_          (stagePtr),
        text_              (""),
        renderedText_      (),
        startLine_         (0),
        regionOrig_        (),
        textInfoOrig_      (),
        smallFontSizeOrig_ (),
        marginsOrig_       (),
        allowScrollbarOrig_()
    {
        Setup(TEXT_INFO,
              REGION,
              stagePtr_,
              SMALLER_FONT_SIZE,
              BOX_INFO,
              MARGINS,
              true);
    }


    TextRegion::~TextRegion()
    {}


    void TextRegion::Setup(const TextInfo &      TEXT_INFO,
                           const sf::FloatRect & REGION,
                           const unsigned int    SMALLER_FONT_SIZE,
                           const box::Info &     BOX_INFO,
                           const Margins &       MARGINS)
    {
        Setup(TEXT_INFO,
              REGION,
              stagePtr_,
              SMALLER_FONT_SIZE,
              BOX_INFO,
              MARGINS,
              false);
    }


    void TextRegion::Setup(const TextInfo &      TEXT_INFO,
                           const sf::FloatRect & REGION,
                           IStage * const        stagePtrParam,
                           const unsigned int    SMALLER_FONT_SIZE,
                           const box::Info &     BOX_INFO,
                           const Margins &       MARGINS,
                           const bool            WILL_ALLOW_SCROLLBAR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == TEXT_INFO.text.empty()), GetEntityName() << " TextRegion::Setup() was given a TEXT_INFO.up.text string that was empty.");
        M_ASSERT_OR_LOGANDTHROW_SS((TEXT_INFO.fontSPtr.get() != nullptr), GetEntityName() << " TextRegion::Setup(\"...\") was given an upTextInfo with a null font pointer.");

        regionOrig_ = REGION;
        textInfoOrig_ = TEXT_INFO;
        smallFontSizeOrig_ = SMALLER_FONT_SIZE;
        marginsOrig_ = MARGINS;
        allowScrollbarOrig_ = WILL_ALLOW_SCROLLBAR;

        //prevent color changes when clicking on text or on the scrollbar
        SetWillAcceptFocus(false);

        text_ = TEXT_INFO.text;
        stagePtr_ = stagePtrParam;

        sfml_util::gui::SliderBarSPtr_t tempSliderBarSPtr;
        text_render::RenderToArea(GetEntityName(),
                                  TEXT_INFO,
                                  tempSliderBarSPtr,
                                  renderedText_,
                                  REGION,
                                  SMALLER_FONT_SIZE,
                                  MARGINS,
                                  WILL_ALLOW_SCROLLBAR);

        HandleSliderBar(tempSliderBarSPtr);

        //position the text for on-screen coordinates
        const std::size_t NUM_LINES(renderedText_.vec_vec.size());
        for (std::size_t l(0); l < NUM_LINES; ++l)
        {
            const std::size_t NUM_SNIPPETS(renderedText_.vec_vec[l].size());
            for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                renderedText_.vec_vec[l][s].sf_text.move(REGION.left, REGION.top);
        }

        //establish the region
        float heightToUse(renderedText_.total_height);
        if (REGION.height > 1.0f)
            heightToUse = REGION.height;

        const sf::FloatRect OUTLINE_RECT(REGION.left,
                                         REGION.top,
                                         std::max(REGION.width, renderedText_.longest_line),
                                         heightToUse);

        SetEntityRegion(OUTLINE_RECT);
        HandleBox(BOX_INFO);
        EstablishWhichLinesToDraw(0.0f);
    }


    void TextRegion::HandleSliderBar(sfml_util::gui::SliderBarSPtr_t & tempSliderBarSPtr)
    {
        if (stagePtr_ != nullptr)
        {
            if (sliderBarSPtr_.get() != nullptr)
            {
                stagePtr_->EntityRemove(sliderBarSPtr_);
                sliderBarSPtr_.reset();
            }

            if (tempSliderBarSPtr.get() != nullptr)
            {
                sliderBarSPtr_ = tempSliderBarSPtr;
                sliderBarSPtr_->SetOnChangeHandler(this);
                sliderBarSPtr_->SetCurrentValue(0.0f);
                stagePtr_->EntityAdd(sliderBarSPtr_);
            }
        }
    }


    void TextRegion::HandleBox(const box::Info & BOX_INFO)
    {
        //deal with the Box that may need to be created or destroyed
        if ((stagePtr_ != nullptr) &&
            (boxInfo_ != BOX_INFO) &&
            (renderedText_.vec_vec.empty() == false) &&
            (renderedText_.vec_vec[0].empty() == false))
        {
            boxInfo_ = BOX_INFO;

            sf::FloatRect newRegion(entityRegion_);

            const float FIRST_LINE_HEIGHT(static_cast<float>(renderedText_.vec_vec[0][0].sf_text.getCharacterSize()));
            newRegion.top = entityRegion_.top - FIRST_LINE_HEIGHT;
            newRegion.height = entityRegion_.height + FIRST_LINE_HEIGHT;

            if (sliderBarSPtr_.get() != nullptr)
                sliderBarSPtr_->MoveEntityPos(25.0f, 0.0f);

            boxInfo_.SetBoxAndBackgroundRegion(newRegion);

            if (boxSPtr_.get() != nullptr)
                stagePtr_->EntityRemove(boxSPtr_);

            boxSPtr_.reset( new box::Box("TextRegion's", boxInfo_) );

            //Q: Why add the box to Stage's entitySSet?  Why not keep it as a class member and draw it ourselves?  ztn 2016-10-8
            //A: Because it is easier to let the Stage and its entitySSet_ handle all the mouse events that keep the focus and other features working.
            //Q: So what to do when the stage draws the box AFTER and on top of the text, such as the box's gradient covering the text?
            //A: Have whatever stage owns this TextRegion draw this TextRegion last.
            stagePtr_->EntityAdd(boxSPtr_);
        }
    }

    void TextRegion::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        //Note:  Don't draw the Box or SliderBar here.  They are being drawn by the stage.
        if (entityWillDraw_)
        {
            //don't draw farther down than the region extends, keep track with posY
            float posY(0.0f);
            const std::size_t NUM_LINES(renderedText_.vec_vec.size());
            for (std::size_t l(startLine_); l < NUM_LINES; ++l)
            {
                if (renderedText_.vec_vec[l].empty())
                    continue;

                //remove extra space created by newline characters
                sf::Text t(renderedText_.vec_vec[l][0].sf_text);
                t.setString( boost::algorithm::replace_all_copy(std::string(t.getString()), "\n", "") );

                //there should be no empty lines, instead choose a value that fits
                if (std::string(t.getString()).empty())
                {
                    t.setString("\n");
                    posY += t.getGlobalBounds().height * 0.8f;//this magic number found by experiment
                }
                else
                    posY += t.getGlobalBounds().height;

                const float SCROLL_PAD((sliderBarSPtr_.get() == nullptr) ? 0.0f : static_cast<float>(t.getCharacterSize()));
                if (posY > (entityRegion_.height - SCROLL_PAD))
                    break;

                const std::size_t NUM_SNIPPETS(renderedText_.vec_vec[l].size());
                for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                    target.draw(renderedText_.vec_vec[l][s].sf_text, states);
            }

            //sfml_util::DrawRectangle(target, states, entityRegion_, sf::Color::Red);
        }
    }


    void TextRegion::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);

        const std::size_t NUM_LINES(renderedText_.vec_vec.size());
        for (std::size_t l(0); l < NUM_LINES; ++l)
        {
            const std::size_t NUM_SNIPPETS(renderedText_.vec_vec[l].size());
            for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                renderedText_.vec_vec[l][s].sf_text.move(HORIZ, VERT);
        }

        if (boxSPtr_.get() != nullptr)
            boxSPtr_->MoveEntityPos(HORIZ, VERT);

        if (sliderBarSPtr_.get() != nullptr)
            sliderBarSPtr_->MoveEntityPos(HORIZ, VERT);
    }


    void TextRegion::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        const float OFFSET_X(POS_LEFT - entityRegion_.left);
        const float OFFSET_Y(POS_TOP - entityRegion_.top);
        MoveEntityPos(OFFSET_X, OFFSET_Y);
    }


    bool TextRegion::HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if (nullptr != sliderBarSPtr_.get())
        {
            EstablishWhichLinesToDraw(PACKAGE.PTR_->GetCurrentValue());
            return true;
        }
        else
            return false;
    }


    void TextRegion::Append(const TextRegion & TEXT_REGION)
    {
        //add the new lines of text
        const std::size_t NUM_LINES(TEXT_REGION.renderedText_.vec_vec.size());
        for (std::size_t l(0); l < NUM_LINES; ++l)
        {
            //adjust vertical positions
            sfml_util::text_render::TextSnippetVec_t nextSnippetVec(TEXT_REGION.renderedText_.vec_vec[l]);
            const std::size_t NUM_SNIPPETS(nextSnippetVec.size());
            for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                nextSnippetVec[s].sf_text.move(0.0f, renderedText_.total_height);

            renderedText_.vec_vec.push_back(nextSnippetVec);
        }

        //adjust total height
        renderedText_.total_height += TEXT_REGION.renderedText_.total_height;

        //adjust longest line if needed
        if (renderedText_.longest_line < TEXT_REGION.renderedText_.longest_line)
            renderedText_.longest_line = TEXT_REGION.renderedText_.longest_line;

        //
        sfml_util::gui::SliderBarSPtr_t tempSliderBarSPtr;
        text_render::ApplyToArea(GetEntityName(),
                                 tempSliderBarSPtr,
                                 renderedText_,
                                 GetEntityRegion());

        HandleSliderBar(tempSliderBarSPtr);

        //establish the new region
        const sf::FloatRect OUTLINE_RECT(entityRegion_.left,
                                         entityRegion_.top,
                                         renderedText_.longest_line,
                                         entityRegion_.height);
        SetEntityRegion(OUTLINE_RECT);

        //keep the text_ member accurate
        text_ += TEXT_REGION.text_;
    }


    void TextRegion::OnColorChange()
    {
        const std::size_t NUM_LINES(renderedText_.vec_vec.size());
        for (std::size_t l(0); l < NUM_LINES; ++l)
        {
            const std::size_t NUM_SNIPPETS(renderedText_.vec_vec[l].size());
            for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                sfml_util::SetTextColor(renderedText_.vec_vec[l][s].sf_text, entityFgColor_);
        }

        if (boxSPtr_.get() != nullptr)
            boxSPtr_->SetEntityColors(entityColorSet_);
    }


    void TextRegion::SetText(const std::string & NEW_TEXT)
    {
        textInfoOrig_.text = NEW_TEXT;

        Setup(textInfoOrig_,
              regionOrig_,
              stagePtr_,
              smallFontSizeOrig_,
              boxInfo_,
              marginsOrig_,
              allowScrollbarOrig_);
    }


    void TextRegion::EstablishWhichLinesToDraw(const float SCROLL_RATIO)
    {
        EstablishWhichLinesToDraw(SCROLL_RATIO, entityRegion_.height);
    }


    void TextRegion::EstablishWhichLinesToDraw(const float SCROLL_RATIO, const float REGION_HEIGHT)
    {
        if ((renderedText_.vec_vec.empty()) || (renderedText_.vec_vec[0].empty()))
            return;

        //establish how far down the user has scrolled
        const float SCROLL_POS(SCROLL_RATIO * (renderedText_.total_height - (REGION_HEIGHT * 0.8f)));
        const float FIRST_LINE_HEIGHT( static_cast<float>(renderedText_.vec_vec[0][0].sf_text.getCharacterSize()) );

        //establish which line to start drawing
        float startPosY(SCROLL_POS / FIRST_LINE_HEIGHT);
        if (startPosY < 0.0f)
            startPosY = 0.0f;

        startLine_ = static_cast<std::size_t>(startPosY);

        //move the text to compensate
        const std::size_t NUM_LINES(renderedText_.vec_vec.size());
        for (std::size_t l(0); l < NUM_LINES; ++l)
        {
            const std::size_t NUM_SNIPPETS(renderedText_.vec_vec[l].size());
            for (std::size_t s(0); s < NUM_SNIPPETS; ++s)
                renderedText_.vec_vec[l][s].sf_text.setPosition(renderedText_.vec_vec[l][s].sf_text.getPosition().x,
                                                                (entityRegion_.top + (l * FIRST_LINE_HEIGHT)) - SCROLL_POS);
        }
    }

}
}
