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
#include "log/log-macros.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include <algorithm>
#include <vector>

namespace heroespath
{
namespace stage
{

    AdventureCharacterList::AdventureCharacterList(const sfml_util::IStagePtr_t ISTAGE_PTR)
        : sfml_util::gui::GuiEntity("AdventureStage'sCharacterList", 0.0f, 0.0f)
        , ALPHA_FOR_LINES_(180)
        , ALPHA_FOR_TEXT_(160)
        , ALPHA_FOR_CHAR_IMAGES_(150)
        , ALPHA_FOR_COLORED_BARS_(150)
        , CELL_LINE_THICKNESS_(1.0f)
        , OUTER_SPACER_(sfml_util::MapByRes(75.0f, 300.0f))
        , HEALTH_COLUMN_WIDTH_(sfml_util::MapByRes(180.0f, 500.0f))
        , MANA_COLUMN_WIDTH_(HEALTH_COLUMN_WIDTH_)
        , CELL_TEXT_LEFT_SPACER_(sfml_util::MapByRes(12.0f, 25.0f))
        , CHARLIST_SEP_SPACER_(sfml_util::MapByRes(15.0f, 150.0f))
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
        , quadVerts_()
        , charImages_()
        , charListSepLine_(
              "AdventureStage_CharacterList_CharListSeparator_",
              sfml_util::Orientation::Horiz,
              sfml_util::Side::Top,
              true)
    {}

    AdventureCharacterList::~AdventureCharacterList() = default;

    bool AdventureCharacterList::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &)
    {
        return false;
    }

    void AdventureCharacterList::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(&lineVerts_[0], lineVerts_.size(), sf::Lines, states);
        target.draw(&quadVerts_[0], quadVerts_.size(), sf::Quads, states);
        target.draw(charListSepLine_, states);

        for (auto const & TEXT_REGION_UPTR : manaTextRegionsUVec_)
        {
            if (TEXT_REGION_UPTR->GetText() != "0/0")
            {
                target.draw(*TEXT_REGION_UPTR, states);
            }
        }

        for (auto const & IMAGE_VECTORMAP_PAIR : charImages_)
        {
            target.draw(IMAGE_VECTORMAP_PAIR.second.second, states);
        }
    }

    void AdventureCharacterList::SetEntityPos(const float LEFT, const float TOP)
    {
        GuiEntity::SetEntityPos(LEFT, TOP);
        Setup();
    }

    void AdventureCharacterList::Setup()
    {
        lineVerts_.clear();
        quadVerts_.clear();

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
        for (auto const & BUTTON_UPTR : namesButtonUVec_)
        {
            stagePtr_->EntityRemove(BUTTON_UPTR.get());
        }

        namesButtonUVec_.clear();

        for (auto const & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            const sfml_util::gui::TextInfo TEXT_INFO {
                CHARACTER_PTR->Name(),
                sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Default),
                sfml_util::FontManager::Instance()->Size_Large(),
                FadedDarkColor_Text()
            };

            const sfml_util::gui::MouseTextInfo MOUSE_TEXT_INFO { TEXT_INFO };

            namesButtonUVec_.emplace_back(std::make_unique<sfml_util::gui::FourStateButton>(
                "AdventureStage'sCharacterList'sNameButtonFor_" + CHARACTER_PTR->Name(),
                sf::Vector2f(0.0f, 0.0f), // position set in SetupPositions_NameButtons()
                sfml_util::gui::ButtonStateImageKeys(),
                MOUSE_TEXT_INFO));
        }

        for (auto const & BUTTON_UPTR : namesButtonUVec_)
        {
            stagePtr_->EntityAdd(BUTTON_UPTR.get());
        }
    }

    void AdventureCharacterList::SetupHealthNumbersText()
    {
        for (auto const & TEXT_UPTR : healthTextRegionsUVec_)
        {
            stagePtr_->EntityRemove(TEXT_UPTR.get());
        }

        healthTextRegionsUVec_.clear();

        for (auto const & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            std::ostringstream ss;
            ss << CHARACTER_PTR->HealthCurrent() << "/" << CHARACTER_PTR->HealthNormal();

            const sfml_util::gui::TextInfo TEXT_INFO {
                ss.str(),
                sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Number),
                sfml_util::FontManager::Instance()->Size_Smallish(),
                FadedDarkColor_Text()
            };

            const sf::FloatRect RECT(
                0.0f, // actual position will be set by SetupPositions_Conditions()
                0.0f,
                0.0f,
                0.0f);

            healthTextRegionsUVec_.emplace_back(std::make_unique<sfml_util::gui::TextRegion>(
                "AdventureStage'sCharacterList'sHealthTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT));
        }

        for (auto const & TEXT_UPTR : healthTextRegionsUVec_)
        {
            stagePtr_->EntityAdd(TEXT_UPTR.get());
        }
    }

    void AdventureCharacterList::SetupManaNumbersText()
    {
        // the mana TextRegions are not controlled/drawn by the stage

        manaTextRegionsUVec_.clear();

        for (auto const & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            std::ostringstream ss;
            ss << CHARACTER_PTR->Mana() << "/" << CHARACTER_PTR->ManaNormal();

            const sfml_util::gui::TextInfo TEXT_INFO {
                ss.str(),
                sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Number),
                sfml_util::FontManager::Instance()->Size_Smallish(),
                FadedDarkColor_Text()
            };

            const sf::FloatRect RECT(
                0.0f, // actual position will be set by SetupPositions_Conditions()
                0.0f,
                0.0f,
                0.0f);

            manaTextRegionsUVec_.emplace_back(std::make_unique<sfml_util::gui::TextRegion>(
                "AdventureStage'sCharacterList'sManaTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT));
        }
    }

    void AdventureCharacterList::SetupConditionsText()
    {
        for (auto const & TEXTREGION_UPTR : condsTextRegionsUVec_)
        {
            stagePtr_->EntityRemove(TEXTREGION_UPTR.get());
        }

        condsTextRegionsUVec_.clear();

        auto const CHARACTER_PVEC { game::Game::Instance()->State().Party().Characters() };
        auto const NUM_CHARACTERS { CHARACTER_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            auto const CHARACTER_PTR { CHARACTER_PVEC[i] };

            const sfml_util::gui::TextInfo TEXT_INFO {
                CHARACTER_PTR->ConditionNames(),
                sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::Default),
                sfml_util::FontManager::Instance()->Size_Normal(),
                FadedDarkColor_Text()
            };

            const sf::FloatRect RECT(
                0.0f, // actual position will be set by SetupPositions_Conditions()
                0.0f,
                conditionColumnRects_[i].width - (CELL_TEXT_LEFT_SPACER_ * 2.0f),
                conditionColumnRects_[i].height);

            condsTextRegionsUVec_.emplace_back(std::make_unique<sfml_util::gui::TextRegion>(
                "AdventureStageCharacterList'sConditionTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT));
        }

        for (auto const & TEXTREGION_UPTR : condsTextRegionsUVec_)
        {
            stagePtr_->EntityAdd(TEXTREGION_UPTR.get());
        }
    }

    void AdventureCharacterList::SetupColumnRects_Image()
    {
        imageColumnRects_.clear();

        auto const NAME_CELL_PAD_VERT { sfml_util::MapByRes(6.0f, 12.0f) };

        auto const TALLEST_NAME_BUTTON_HEIGHT {
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

        auto const NUM_ROWS { namesButtonUVec_.size() };
        for (std::size_t i(0); i < NUM_ROWS; ++i)
        {
            auto const I_FLOAT { static_cast<float>(i) };

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

        auto const NAME_CELL_PAD_HORIZ { sfml_util::MapByRes(12.0f, 24.0f) };

        auto const LONGEST_NAME_BUTTON_WIDTH {
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

        for (auto const & IMAGE_RECT : imageColumnRects_)
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

        for (auto const & NAME_RECT : nameColumnRects_)
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

        for (auto const & HEALTH_RECT : healthColumnRects_)
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

        auto const OUTER_PAD_HORIZ { 2.0f * OUTER_SPACER_ };
        auto const SCREEN_WIDTH { sfml_util::Display::Instance()->GetWinWidth() };

        auto const NON_COND_COLUMN_WIDTHS { imageColumnRects_[0].width + nameColumnRects_[0].width
                                            + healthColumnRects_[0].width
                                            + manaColumnRects_[0].width };

        auto const COND_COLUMN_WIDTH { (SCREEN_WIDTH - OUTER_PAD_HORIZ) - NON_COND_COLUMN_WIDTHS };

        for (auto const & MANA_RECT : manaColumnRects_)
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
        auto const NUM_BUTTONS { namesButtonUVec_.size() };
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            auto const LEFT { nameColumnRects_[0].left + CELL_TEXT_LEFT_SPACER_ };

            auto const CELL_MID_VERT { nameColumnRects_[i].top
                                       + (nameColumnRects_[0].height * 0.5f) };

            auto const TOP { CELL_MID_VERT
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
        auto const NUM_TEXTREGIONS { condsTextRegionsUVec_.size() };
        for (std::size_t i(0); i < NUM_TEXTREGIONS; ++i)
        {
            auto const LEFT { conditionColumnRects_[0].left + CELL_TEXT_LEFT_SPACER_ };

            auto const TOP { (conditionColumnRects_[i].top
                              + (conditionColumnRects_[i].height * 0.5f))
                             - (condsTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f)
                             + sfml_util::MapByRes(10.0f, 20.0f) };

            condsTextRegionsUVec_[i]->SetEntityPos(LEFT, TOP);
        }
    }

    void AdventureCharacterList::SetupPositions_OverallRegion()
    {
        auto const OVERALL_RECT_WIDTH { imageColumnRects_[0].width + nameColumnRects_[0].width
                                        + healthColumnRects_[0].width + manaColumnRects_[0].width
                                        + conditionColumnRects_[0].width };

        auto const LAST_INDEX { nameColumnRects_.size() - 1 };

        auto const OVERALL_RECT_HEIGHT { (nameColumnRects_[LAST_INDEX].top
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
            auto const LEFT { CHARLIST_SEP_SPACER_ };
            auto const TOP { imageColumnRects_[0].top - 10.0f };

            auto const LENGTH { sfml_util::Display::Instance()->GetWinWidth()
                                - (CHARLIST_SEP_SPACER_ * 2.0f) };

            charListSepLine_.Setup(LEFT, TOP, static_cast<std::size_t>(LENGTH));
        }

        {
            auto const HORIZ_LINE_LEFT { GetEntityRegion().left };
            auto const HORIZ_LINE_RIGHT { GetEntityRegion().left + GetEntityRegion().width + 4.0f };

            auto const NUM_ROWS { nameColumnRects_.size() };
            for (std::size_t i(0); i < NUM_ROWS; ++i)
            {
                auto const CELL_LINE_THICKNESS_INT { static_cast<int>(CELL_LINE_THICKNESS_) };
                for (int t(0); t < CELL_LINE_THICKNESS_INT; ++t)
                {
                    auto const HORIZ_LINE_TOP { imageColumnRects_[i].top
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
        auto const NUM_RECTS {
            ((WHICH_BAR == WhichBar::Health) ? healthColumnRects_.size() : manaColumnRects_.size())
        };

        for (std::size_t i(0); i < NUM_RECTS; ++i)
        {
            // determine if mana bar is drawn at all
            auto const CHARACTER_PTR { game::Game::Instance()->State().Party().Characters()[i] };

            if (WHICH_BAR == WhichBar::Mana && (CHARACTER_PTR->ManaNormal().IsZero()))
            {
                continue;
            }

            auto const BOUNDS { (
                (WHICH_BAR == WhichBar::Health) ? healthColumnRects_[i] : manaColumnRects_[i]) };

            // eastblish the outer line (bounding box) coordinates
            auto const WIDTH { BOUNDS.width * 0.75f };

            auto const HEIGHT { BOUNDS.height * 0.5f };
            auto const LEFT { BOUNDS.left + ((BOUNDS.width - WIDTH) * 0.5f) };

            auto const TOP { (BOUNDS.top + (BOUNDS.height * 0.5f)) - (HEIGHT * 0.5f) };

            sfml_util::DrawRectangleWithLineVerts(
                sf::FloatRect(LEFT, TOP, WIDTH, HEIGHT),
                FadedDarkColor_Line() - sf::Color(0, 0, 0, 50),
                lineVerts_);

            // establish the inner line (bounding box) coordinates
            sfml_util::DrawRectangleWithLineVerts(
                sf::FloatRect(LEFT + 1.0f, TOP + 1.0f, WIDTH - 2.0f, HEIGHT - 2.0f),
                FadedDarkColor_Line(),
                lineVerts_);

            // establish the inner colored bar coordinates
            auto const BAR_RATIO { (
                (WHICH_BAR == WhichBar::Health) ? CHARACTER_PTR->HealthRatio()
                                                : CHARACTER_PTR->ManaRatio()) };

            auto const BAR_WIDTH { (WIDTH - 4.0f) * BAR_RATIO };

            // establish the inner colored bar colors
            const sf::Uint8 COLOR_BASE { 64 };
            auto const COLOR_BASE_F { static_cast<float>(COLOR_BASE) };
            const sf::Uint8 COLOR_MAX { 255 };
            const sf::Uint8 COLOR_HIGH { 192 };
            auto const COLOR_HIGH_F { static_cast<float>(COLOR_HIGH) };
            auto const COLOR_BAR_ALPHA { ALPHA_FOR_COLORED_BARS_ };

            auto const COLOR_BAR_LEFT { sf::Color(
                ((WHICH_BAR == WhichBar::Health) ? COLOR_MAX : COLOR_BASE),
                COLOR_BASE,
                ((WHICH_BAR == WhichBar::Health) ? COLOR_BASE : COLOR_MAX)) };

            auto const COLOR_BAR_RIGHT_WHITE_VAL_F {
                COLOR_BASE_F + ((COLOR_HIGH_F - COLOR_BASE_F) * BAR_RATIO)
            };

            auto const COLOR_BAR_RIGHT_WHITE_VAL { static_cast<sf::Uint8>(
                COLOR_BAR_RIGHT_WHITE_VAL_F) };

            auto const COLOR_BAR_RIGHT { sf::Color(
                ((WHICH_BAR == WhichBar::Health) ? COLOR_MAX : COLOR_BAR_RIGHT_WHITE_VAL),
                COLOR_BAR_RIGHT_WHITE_VAL,
                ((WHICH_BAR == WhichBar::Health) ? COLOR_BAR_RIGHT_WHITE_VAL : COLOR_MAX),
                COLOR_BAR_ALPHA) };

            // draw inner colored bar
            sfml_util::DrawQuad(
                sf::FloatRect(LEFT + 2.0f, TOP + 2.0f, BAR_WIDTH, HEIGHT - 4.0f),
                COLOR_BAR_LEFT,
                COLOR_BAR_RIGHT,
                quadVerts_);

            auto const NUMBER_TEXT_TOP_SPACER { sfml_util::MapByRes(0.0f, 10.0f) };

            if (WHICH_BAR == WhichBar::Health)
            {
                auto const HEALTH_TEXTREGION_LEFT {
                    (LEFT + (BAR_WIDTH * 0.5f))
                    - (healthTextRegionsUVec_[i]->GetEntityRegion().width * 0.5f)
                };

                auto const HEALTH_TEXTREGION_TOP {
                    (TOP + (HEIGHT * 0.5f))
                    - (healthTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f)
                    + NUMBER_TEXT_TOP_SPACER
                };

                healthTextRegionsUVec_[i]->SetEntityPos(
                    HEALTH_TEXTREGION_LEFT, HEALTH_TEXTREGION_TOP);
            }
            else
            {
                auto const MANA_TEXTREGION_LEFT {
                    (LEFT + (BAR_WIDTH * 0.5f))
                    - (manaTextRegionsUVec_[i]->GetEntityRegion().width * 0.5f)
                };

                auto const MANA_TEXTREGION_TOP {
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
        auto const CHARACTER_PVEC { game::Game::Instance()->State().Party().Characters() };
        auto const NUM_CHARACTERS { CHARACTER_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            auto const CHARACTER_PTR { game::Game::Instance()->State().Party().Characters()[i] };
            namesButtonUVec_[i]->SetMouseHoverText(NameButtonMouseoverText(CHARACTER_PTR));
        }
    }

    void AdventureCharacterList::SetupCharacterImages()
    {
        charImages_.Clear();

        sfml_util::gui::CreatureImageLoader creatureImageLoader;

        auto const CHARACTER_VEC { game::Game::Instance()->State().Party().Characters() };
        auto const NUM_CHARACTERS { CHARACTER_VEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            auto const CHARACTER_PTR { CHARACTER_VEC[i] };

            auto & imagePair { charImages_[CHARACTER_PTR] };

            creatureImageLoader.Load(imagePair.first, CHARACTER_PTR);

            sfml_util::Invert(imagePair.first);
            sfml_util::Mask(imagePair.first, sf::Color::White);
            imagePair.second.setTexture(imagePair.first, true);
            imagePair.second.setColor(FadedDarkColor_CharacterImages());

            auto const RESIZE_RATIO { 0.9f };

            auto const SCALE_HORIZ { (imageColumnRects_[i].width
                                      / imagePair.second.getLocalBounds().width)
                                     * RESIZE_RATIO };

            auto const SCALE_VERT { (imageColumnRects_[i].height
                                     / imagePair.second.getLocalBounds().height)
                                    * RESIZE_RATIO };

            auto const FINAL_SCALE { std::min(SCALE_HORIZ, SCALE_VERT) };

            imagePair.second.setScale(FINAL_SCALE, FINAL_SCALE);

            auto const LEFT { (imageColumnRects_[i].left + (imageColumnRects_[i].width * 0.5f))
                              - (imagePair.second.getGlobalBounds().width * 0.5f) };

            auto const TOP { (imageColumnRects_[i].top + (imageColumnRects_[i].height * 0.5f))
                             - (imagePair.second.getGlobalBounds().height * 0.5f) };

            imagePair.second.setPosition(LEFT, TOP);
        }
    }

    const std::string
        AdventureCharacterList::NameButtonMouseoverText(const creature::CreaturePtr_t CHARACTER_PTR)
    {
        auto const NAME_STR { CHARACTER_PTR->Name() };
        auto const RACE_STR { CHARACTER_PTR->RaceName() };

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
        auto darkColor { sfml_util::Colors::GrayDarker };
        darkColor.a = ALPHA_FOR_LINES_;
        return darkColor;
    }

    const sf::Color AdventureCharacterList::FadedDarkColor_Text() const
    {
        auto darkColor { sfml_util::Colors::GrayDarker };
        darkColor.a = ALPHA_FOR_TEXT_;
        return darkColor;
    }

    const sf::Color AdventureCharacterList::FadedDarkColor_CharacterImages() const
    {
        auto darkColor { sfml_util::Colors::GrayDarker };
        darkColor.a = ALPHA_FOR_CHAR_IMAGES_;
        return darkColor;
    }

} // namespace stage
} // namespace heroespath
