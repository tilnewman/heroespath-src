// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// adventure-stage-char-list.cpp
//
#include "adventure-stage-char-list.hpp"

#include "creature/player-party.hpp"
#include "creature/race-enum.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/text-info.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/vertex.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

namespace heroespath
{
namespace stage
{

    AdventureCharacterList::AdventureCharacterList(const stage::IStagePtr_t ISTAGE_PTR)
        : gui::Entity("AdventureStage'sCharacterList", 0.0f, 0.0f)
        , ALPHA_FOR_LINES_(180)
        , ALPHA_FOR_TEXT_(160)
        , ALPHA_FOR_CHAR_IMAGES_(150)
        , ALPHA_FOR_COLORED_BARS_(150)
        , CELL_LINE_THICKNESS_(1.0f)
        , OUTER_SPACER_(sfutil::MapByRes(75.0f, 300.0f))
        , HEALTH_COLUMN_WIDTH_(sfutil::MapByRes(180.0f, 500.0f))
        , MANA_COLUMN_WIDTH_(HEALTH_COLUMN_WIDTH_)
        , CELL_TEXT_LEFT_SPACER_(sfutil::MapByRes(12.0f, 25.0f))
        , CHARLIST_SEP_SPACER_(sfutil::MapByRes(15.0f, 150.0f))
        , stagePtr_(ISTAGE_PTR)
        , namesButtonUVec_()
        , condsTextRegionsUVec_()
        , healthTextRegionsUVec_()
        , manaTextRegionsUVec_()
        , imageColumnRects_()
        , nameColumnRects_()
        , healthColumnRects_()
        , manaColumnRects_()
        , conditionColumnRects_()
        , lineVerts_()
        , colorGradientBars_()
        , charImages_()
        , charListSepLine_()
    {}

    AdventureCharacterList::~AdventureCharacterList() = default;

    void AdventureCharacterList::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(&lineVerts_[0], lineVerts_.size(), sf::Lines, states);

        for (const auto & COLORED_RECT : colorGradientBars_)
        {
            target.draw(COLORED_RECT, states);
        }

        target.draw(charListSepLine_, states);

        for (const auto & TEXT_REGION_UPTR : manaTextRegionsUVec_)
        {
            if (TEXT_REGION_UPTR->GetText() != "0/0")
            {
                target.draw(*TEXT_REGION_UPTR, states);
            }
        }

        for (const auto & IMAGE_VECTORMAP_PAIR : charImages_)
        {
            target.draw(IMAGE_VECTORMAP_PAIR.second.second, states);
        }
    }

    void AdventureCharacterList::SetEntityPos(const float LEFT, const float TOP)
    {
        Entity::SetEntityPos(LEFT, TOP);
        Setup();
    }

    void AdventureCharacterList::Setup()
    {
        lineVerts_.clear();
        colorGradientBars_.clear();

        SetupNameButtons();
        SetupHealthNumbersText();
        SetupManaNumbersText();

        SetupColumnRects_Image();
        SetupColumnRects_Name();
        SetupColumnRects_Health();
        SetupColumnRects_Mana();
        SetupColumnRects_Condition();

        SetupConditionsText();

        SetupPositions_NameButtons();
        SetupPositions_HealthBars();
        SetupPositions_ManaBars();
        SetupPositions_Conditions();

        SetupPositions_OverallRegion();
        SetupCellDividerLines();
        SetupMouseoverText();
        SetupCharacterImages();
    }

    void AdventureCharacterList::SetupNameButtons()
    {
        for (const auto & BUTTON_UPTR : namesButtonUVec_)
        {
            stagePtr_->EntityRemove(BUTTON_UPTR);
        }

        namesButtonUVec_.clear();

        for (const auto & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            const gui::TextInfo TEXT_INFO { CHARACTER_PTR->Name(),
                                            gui::GuiFont::Default,
                                            gui::FontManager::Instance()->Size_Large(),
                                            FadedDarkColor_Text() };

            namesButtonUVec_.emplace_back(std::make_unique<gui::ImageTextEntity>(
                "AdventureStage'sCharacterList'sNameButtonFor_" + CHARACTER_PTR->Name(),
                gui::MouseImageInfo(),
                TEXT_INFO,
                boost::none,
                gui::ImageTextEntity::MouseStateSync::Image));
        }

        for (const auto & BUTTON_UPTR : namesButtonUVec_)
        {
            stagePtr_->EntityAdd(BUTTON_UPTR);
        }
    }

    void AdventureCharacterList::SetupHealthNumbersText()
    {
        for (const auto & TEXT_UPTR : healthTextRegionsUVec_)
        {
            stagePtr_->EntityRemove(TEXT_UPTR);
        }

        healthTextRegionsUVec_.clear();

        for (const auto & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            const gui::TextInfo TEXT_INFO { (CHARACTER_PTR->HealthCurrent().ToString() + "/"
                                             + CHARACTER_PTR->HealthNormal().ToString()),
                                            gui::GuiFont::Number,
                                            gui::FontManager::Instance()->Size_Smallish(),
                                            FadedDarkColor_Text() };

            const sf::FloatRect RECT(
                0.0f, // actual position will be set by SetupPositions_Conditions()
                0.0f,
                0.0f,
                0.0f);

            healthTextRegionsUVec_.emplace_back(std::make_unique<gui::TextRegion>(
                "AdventureStage'sCharacterList'sHealthTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT));
        }

        for (const auto & TEXT_UPTR : healthTextRegionsUVec_)
        {
            stagePtr_->EntityAdd(TEXT_UPTR);
        }
    }

    void AdventureCharacterList::SetupManaNumbersText()
    {
        // the mana TextRegions are not controlled/drawn by the stage

        manaTextRegionsUVec_.clear();

        for (const auto & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            const gui::TextInfo TEXT_INFO { (CHARACTER_PTR->Mana().ToString() + "/"
                                             + CHARACTER_PTR->ManaNormal().ToString()),
                                            gui::GuiFont::Number,
                                            gui::FontManager::Instance()->Size_Smallish(),
                                            FadedDarkColor_Text() };

            const sf::FloatRect RECT(
                0.0f, // actual position will be set by SetupPositions_Conditions()
                0.0f,
                0.0f,
                0.0f);

            manaTextRegionsUVec_.emplace_back(std::make_unique<gui::TextRegion>(
                "AdventureStage'sCharacterList'sManaTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT));
        }
    }

    void AdventureCharacterList::SetupConditionsText()
    {
        for (const auto & TEXTREGION_UPTR : condsTextRegionsUVec_)
        {
            stagePtr_->EntityRemove(TEXTREGION_UPTR);
        }

        condsTextRegionsUVec_.clear();

        const auto CHARACTER_PVEC { game::Game::Instance()->State().Party().Characters() };
        const auto NUM_CHARACTERS { CHARACTER_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            const auto CHARACTER_PTR { CHARACTER_PVEC[i] };

            const gui::TextInfo TEXT_INFO { CHARACTER_PTR->ConditionNames(),
                                            gui::GuiFont::Default,
                                            gui::FontManager::Instance()->Size_Normal(),
                                            FadedDarkColor_Text() };

            const sf::FloatRect RECT(
                0.0f, // actual position will be set by SetupPositions_Conditions()
                0.0f,
                conditionColumnRects_[i].width - (CELL_TEXT_LEFT_SPACER_ * 2.0f),
                conditionColumnRects_[i].height);

            condsTextRegionsUVec_.emplace_back(std::make_unique<gui::TextRegion>(
                "AdventureStageCharacterList'sConditionTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT));
        }

        for (const auto & TEXTREGION_UPTR : condsTextRegionsUVec_)
        {
            stagePtr_->EntityAdd(TEXTREGION_UPTR);
        }
    }

    void AdventureCharacterList::SetupColumnRects_Image()
    {
        imageColumnRects_.clear();

        const auto NAME_CELL_PAD_VERT { sfutil::MapByRes(6.0f, 12.0f) };

        const auto TALLEST_NAME_BUTTON_HEIGHT {
            (NAME_CELL_PAD_VERT * 2.0f)
            + (*std::max_element(
                   namesButtonUVec_.begin(),
                   namesButtonUVec_.end(),
                   [](auto & A_UPTR, auto & B_UPTR) {
                       return (A_UPTR->GetEntityRegion().height < B_UPTR->GetEntityRegion().height);
                   }))
                  ->GetEntityRegion()
                  .height
        };

        const auto NUM_ROWS { namesButtonUVec_.size() };
        for (std::size_t i(0); i < NUM_ROWS; ++i)
        {
            const auto I_FLOAT { static_cast<float>(i) };

            const sf::FloatRect IMAGE_RECT(
                GetEntityPos().x,
                GetEntityRegion().top + (I_FLOAT * TALLEST_NAME_BUTTON_HEIGHT) + I_FLOAT,
                TALLEST_NAME_BUTTON_HEIGHT,
                TALLEST_NAME_BUTTON_HEIGHT);

            imageColumnRects_.emplace_back(IMAGE_RECT);
        }
    }

    void AdventureCharacterList::SetupColumnRects_Name()
    {
        nameColumnRects_.clear();

        const auto NAME_CELL_PAD_HORIZ { sfutil::MapByRes(12.0f, 24.0f) };

        const auto LONGEST_NAME_BUTTON_WIDTH {
            (NAME_CELL_PAD_HORIZ * 2.0f)
            + (*std::max_element(
                   namesButtonUVec_.begin(),
                   namesButtonUVec_.end(),
                   [](auto & A_UPTR, auto & B_UPTR) {
                       return (A_UPTR->GetEntityRegion().width < B_UPTR->GetEntityRegion().width);
                   }))
                  ->GetEntityRegion()
                  .width
        };

        for (const auto & IMAGE_RECT : imageColumnRects_)
        {
            const sf::FloatRect HEALTH_RECT(
                IMAGE_RECT.left + IMAGE_RECT.width + 1.0f,
                IMAGE_RECT.top,
                LONGEST_NAME_BUTTON_WIDTH,
                IMAGE_RECT.height);

            nameColumnRects_.emplace_back(HEALTH_RECT);
        }
    }

    void AdventureCharacterList::SetupColumnRects_Health()
    {
        healthColumnRects_.clear();

        for (const auto & NAME_RECT : nameColumnRects_)
        {
            const sf::FloatRect HEALTH_RECT(
                NAME_RECT.left + NAME_RECT.width + 1.0f,
                NAME_RECT.top,
                HEALTH_COLUMN_WIDTH_,
                NAME_RECT.height);

            healthColumnRects_.emplace_back(HEALTH_RECT);
        }
    }

    void AdventureCharacterList::SetupColumnRects_Mana()
    {
        manaColumnRects_.clear();

        for (const auto & HEALTH_RECT : healthColumnRects_)
        {
            const sf::FloatRect MANA_RECT(
                HEALTH_RECT.left + HEALTH_RECT.width + 1.0f,
                HEALTH_RECT.top,
                MANA_COLUMN_WIDTH_,
                HEALTH_RECT.height);

            manaColumnRects_.emplace_back(MANA_RECT);
        }
    }

    void AdventureCharacterList::SetupColumnRects_Condition()
    {
        conditionColumnRects_.clear();

        const auto OUTER_PAD_HORIZ { 2.0f * OUTER_SPACER_ };
        const auto SCREEN_WIDTH { gui::Display::Instance()->GetWinWidth() };

        const auto NON_COND_COLUMN_WIDTHS { imageColumnRects_[0].width + nameColumnRects_[0].width
                                            + healthColumnRects_[0].width
                                            + manaColumnRects_[0].width };

        const auto COND_COLUMN_WIDTH { (SCREEN_WIDTH - OUTER_PAD_HORIZ) - NON_COND_COLUMN_WIDTHS };

        for (const auto & MANA_RECT : manaColumnRects_)
        {
            const sf::FloatRect COND_RECT(
                MANA_RECT.left + MANA_RECT.width + 1.0f,
                MANA_RECT.top,
                COND_COLUMN_WIDTH,
                MANA_RECT.height);

            conditionColumnRects_.emplace_back(COND_RECT);
        }
    }

    void AdventureCharacterList::SetupPositions_NameButtons()
    {
        const auto NUM_BUTTONS { namesButtonUVec_.size() };
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            const auto LEFT { nameColumnRects_[0].left + CELL_TEXT_LEFT_SPACER_ };

            const auto CELL_MID_VERT { nameColumnRects_[i].top
                                       + (nameColumnRects_[0].height * 0.5f) };

            const auto TOP { CELL_MID_VERT
                             - (namesButtonUVec_[i]->GetEntityRegion().height * 0.5f) };

            namesButtonUVec_[i]->SetEntityPos(LEFT, TOP);
        }
    }

    void AdventureCharacterList::SetupPositions_HealthBars()
    {
        SetupPositions_Bars(WhichBar::Mana);
    }

    void AdventureCharacterList::SetupPositions_ManaBars()
    {
        SetupPositions_Bars(WhichBar::Health);
    }

    void AdventureCharacterList::SetupPositions_Conditions()
    {
        const auto NUM_TEXTREGIONS { condsTextRegionsUVec_.size() };
        for (std::size_t i(0); i < NUM_TEXTREGIONS; ++i)
        {
            const auto LEFT { conditionColumnRects_[0].left + CELL_TEXT_LEFT_SPACER_ };

            const auto TOP { (conditionColumnRects_[i].top
                              + (conditionColumnRects_[i].height * 0.5f))
                             - (condsTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f)
                             + sfutil::MapByRes(10.0f, 20.0f) };

            condsTextRegionsUVec_[i]->SetEntityPos(LEFT, TOP);
        }
    }

    void AdventureCharacterList::SetupPositions_OverallRegion()
    {
        const auto OVERALL_RECT_WIDTH { imageColumnRects_[0].width + nameColumnRects_[0].width
                                        + healthColumnRects_[0].width + manaColumnRects_[0].width
                                        + conditionColumnRects_[0].width };

        const auto LAST_INDEX { nameColumnRects_.size() - 1 };

        const auto OVERALL_RECT_HEIGHT { (nameColumnRects_[LAST_INDEX].top
                                          + nameColumnRects_[LAST_INDEX].height)
                                         - nameColumnRects_[0].top };

        const sf::FloatRect OVERALL_RECT(
            imageColumnRects_[0].left,
            imageColumnRects_[0].top,
            OVERALL_RECT_WIDTH,
            OVERALL_RECT_HEIGHT);

        SetEntityRegion(OVERALL_RECT);
    }

    void AdventureCharacterList::SetupCellDividerLines()
    {
        {
            const auto LEFT { CHARLIST_SEP_SPACER_ };
            const auto TOP { imageColumnRects_[0].top - 10.0f };

            const auto LENGTH { gui::Display::Instance()->GetWinWidth()
                                - (CHARLIST_SEP_SPACER_ * 2.0f) };

            charListSepLine_.Setup(
                LEFT, TOP, LENGTH, gui::Orientation::Horiz, gui::Side::Top, true);
        }

        {
            const auto HORIZ_LINE_LEFT { GetEntityRegion().left };
            const auto HORIZ_LINE_RIGHT { GetEntityRegion().left + GetEntityRegion().width + 4.0f };

            const auto NUM_ROWS { nameColumnRects_.size() };
            for (std::size_t i(0); i < NUM_ROWS; ++i)
            {
                const auto CELL_LINE_THICKNESS_INT { static_cast<int>(CELL_LINE_THICKNESS_) };
                for (int t(0); t < CELL_LINE_THICKNESS_INT; ++t)
                {
                    const auto HORIZ_LINE_TOP { imageColumnRects_[i].top
                                                + nameColumnRects_[i].height
                                                + static_cast<float>(t) };

                    lineVerts_.emplace_back(sf::Vertex(
                        sf::Vector2f(HORIZ_LINE_LEFT, HORIZ_LINE_TOP), FadedDarkColor_Line()));

                    lineVerts_.emplace_back(sf::Vertex(
                        sf::Vector2f(HORIZ_LINE_RIGHT, HORIZ_LINE_TOP), FadedDarkColor_Line()));
                }
            }
        }
    }

    void AdventureCharacterList::SetupPositions_Bars(const WhichBar WHICH_BAR)
    {
        const auto NUM_RECTS {
            ((WHICH_BAR == WhichBar::Health) ? healthColumnRects_.size() : manaColumnRects_.size())
        };

        for (std::size_t i(0); i < NUM_RECTS; ++i)
        {
            // determine if mana bar is drawn at all
            const auto CHARACTER_PTR { game::Game::Instance()->State().Party().Characters()[i] };

            if (WHICH_BAR == WhichBar::Mana && (CHARACTER_PTR->ManaNormal().IsZero()))
            {
                continue;
            }

            const auto BOUNDS { (
                (WHICH_BAR == WhichBar::Health) ? healthColumnRects_[i] : manaColumnRects_[i]) };

            // establish the outer line (bounding box) coordinates
            const auto WIDTH { BOUNDS.width * 0.75f };

            const auto HEIGHT { BOUNDS.height * 0.5f };
            const auto LEFT { BOUNDS.left + ((BOUNDS.width - WIDTH) * 0.5f) };

            const auto TOP { (BOUNDS.top + (BOUNDS.height * 0.5f)) - (HEIGHT * 0.5f) };

            sfutil::AppendVertexesForQuad(
                lineVerts_,
                sf::FloatRect(LEFT, TOP, WIDTH, HEIGHT),
                FadedDarkColor_Line() - sf::Color(0, 0, 0, 50));

            // establish the inner line (bounding box) coordinates
            sfutil::AppendVertexesForQuad(
                lineVerts_,
                sf::FloatRect(LEFT + 1.0f, TOP + 1.0f, WIDTH - 2.0f, HEIGHT - 2.0f),
                FadedDarkColor_Line());

            // establish the inner colored bar coordinates
            const auto BAR_RATIO { (
                (WHICH_BAR == WhichBar::Health) ? CHARACTER_PTR->HealthRatio()
                                                : CHARACTER_PTR->ManaRatio()) };

            const auto BAR_WIDTH { (WIDTH - 4.0f) * BAR_RATIO };

            // establish the inner colored bar colors
            const sf::Uint8 COLOR_BASE { 64 };
            const auto COLOR_BASE_F { static_cast<float>(COLOR_BASE) };
            const sf::Uint8 COLOR_MAX { 255 };
            const sf::Uint8 COLOR_HIGH { 192 };
            const auto COLOR_HIGH_F { static_cast<float>(COLOR_HIGH) };
            const auto COLOR_BAR_ALPHA { ALPHA_FOR_COLORED_BARS_ };

            const auto COLOR_BAR_LEFT { sf::Color(
                ((WHICH_BAR == WhichBar::Health) ? COLOR_MAX : COLOR_BASE),
                COLOR_BASE,
                ((WHICH_BAR == WhichBar::Health) ? COLOR_BASE : COLOR_MAX)) };

            const auto COLOR_BAR_RIGHT_WHITE_VAL_F {
                COLOR_BASE_F + ((COLOR_HIGH_F - COLOR_BASE_F) * BAR_RATIO)
            };

            const auto COLOR_BAR_RIGHT_WHITE_VAL { static_cast<sf::Uint8>(
                COLOR_BAR_RIGHT_WHITE_VAL_F) };

            const auto COLOR_BAR_RIGHT { sf::Color(
                ((WHICH_BAR == WhichBar::Health) ? COLOR_MAX : COLOR_BAR_RIGHT_WHITE_VAL),
                COLOR_BAR_RIGHT_WHITE_VAL,
                ((WHICH_BAR == WhichBar::Health) ? COLOR_BAR_RIGHT_WHITE_VAL : COLOR_MAX),
                COLOR_BAR_ALPHA) };

            // draw inner colored bar
            colorGradientBars_.emplace_back(gui::ColoredRect(
                sf::FloatRect(LEFT + 2.0f, TOP + 2.0f, BAR_WIDTH, HEIGHT - 4.0f),
                COLOR_BAR_LEFT,
                COLOR_BAR_RIGHT,
                gui::Side::Left));

            const auto NUMBER_TEXT_TOP_SPACER { sfutil::MapByRes(0.0f, 10.0f) };

            if (WHICH_BAR == WhichBar::Health)
            {
                const auto HEALTH_TEXTREGION_LEFT {
                    (LEFT + (BAR_WIDTH * 0.5f))
                    - (healthTextRegionsUVec_[i]->GetEntityRegion().width * 0.5f)
                };

                const auto HEALTH_TEXTREGION_TOP {
                    (TOP + (HEIGHT * 0.5f))
                    - (healthTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f)
                    + NUMBER_TEXT_TOP_SPACER
                };

                healthTextRegionsUVec_[i]->SetEntityPos(
                    HEALTH_TEXTREGION_LEFT, HEALTH_TEXTREGION_TOP);
            }
            else
            {
                const auto MANA_TEXTREGION_LEFT {
                    (LEFT + (BAR_WIDTH * 0.5f))
                    - (manaTextRegionsUVec_[i]->GetEntityRegion().width * 0.5f)
                };

                const auto MANA_TEXTREGION_TOP {
                    (TOP + (HEIGHT * 0.5f))
                    - (manaTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f)
                    + NUMBER_TEXT_TOP_SPACER
                };

                manaTextRegionsUVec_[i]->SetEntityPos(MANA_TEXTREGION_LEFT, MANA_TEXTREGION_TOP);
            }
        }
    }

    void AdventureCharacterList::SetupMouseoverText()
    {
        const auto CHARACTER_PVEC { game::Game::Instance()->State().Party().Characters() };
        const auto NUM_CHARACTERS { CHARACTER_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            const auto CHARACTER_PTR { game::Game::Instance()->State().Party().Characters()[i] };
            namesButtonUVec_[i]->SetMouseHoverText(NameButtonMouseoverText(CHARACTER_PTR));
        }
    }

    void AdventureCharacterList::SetupCharacterImages()
    {
        charImages_.Clear();

        const auto CHARACTER_VEC { game::Game::Instance()->State().Party().Characters() };
        const auto NUM_CHARACTERS { CHARACTER_VEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            const auto CHARACTER_PTR { CHARACTER_VEC[i] };

            ImagePair_t imagePair(
                gui::LoadAndCacheImage(
                    CHARACTER_PTR, gui::ImageOptions::InvertedCharacterOptions()),
                sf::Sprite());

            imagePair.second.setTexture(imagePair.first.Get(), true);
            imagePair.second.setColor(FadedDarkColor_CharacterImages());

            const auto RESIZE_RATIO { 0.9f };

            const auto SCALE_HORIZ { (imageColumnRects_[i].width
                                      / imagePair.second.getLocalBounds().width)
                                     * RESIZE_RATIO };

            const auto SCALE_VERT { (imageColumnRects_[i].height
                                     / imagePair.second.getLocalBounds().height)
                                    * RESIZE_RATIO };

            const auto FINAL_SCALE { misc::Min(SCALE_HORIZ, SCALE_VERT) };

            imagePair.second.setScale(FINAL_SCALE, FINAL_SCALE);

            const auto LEFT { (imageColumnRects_[i].left + (imageColumnRects_[i].width * 0.5f))
                              - (imagePair.second.getGlobalBounds().width * 0.5f) };

            const auto TOP { (imageColumnRects_[i].top + (imageColumnRects_[i].height * 0.5f))
                             - (imagePair.second.getGlobalBounds().height * 0.5f) };

            imagePair.second.setPosition(LEFT, TOP);

            charImages_.Append(CHARACTER_PTR, imagePair);
        }
    }

    const std::string
        AdventureCharacterList::NameButtonMouseoverText(const creature::CreaturePtr_t CHARACTER_PTR)
    {
        const auto NAME_STR { CHARACTER_PTR->Name() };
        const auto RACE_STR { CHARACTER_PTR->RaceName() };

        std::ostringstream ss;
        ss << NAME_STR << " the " << CHARACTER_PTR->RankClassName();

        if (NAME_STR != RACE_STR)
        {
            ss << " " << creature::race::RaceRoleName(CHARACTER_PTR->Race(), CHARACTER_PTR->Role());
        }

        ss << " of Rank " << CHARACTER_PTR->Rank();
        return ss.str();
    }

    const sf::Color AdventureCharacterList::FadedDarkColor_Line() const
    {
        auto darkColor { sfutil::color::GrayDarker };
        darkColor.a = ALPHA_FOR_LINES_;
        return darkColor;
    }

    const sf::Color AdventureCharacterList::FadedDarkColor_Text() const
    {
        auto darkColor { sfutil::color::GrayDarker };
        darkColor.a = ALPHA_FOR_TEXT_;
        return darkColor;
    }

    const sf::Color AdventureCharacterList::FadedDarkColor_CharacterImages() const
    {
        auto darkColor { sfutil::color::GrayDarker };
        darkColor.a = ALPHA_FOR_CHAR_IMAGES_;
        return darkColor;
    }

} // namespace stage
} // namespace heroespath
