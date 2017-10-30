// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// adventure-stage-char-list.cpp
//
#include "adventure-stage-char-list.hpp"

#include "game/game.hpp"
#include "game/log-macros.hpp"
#include "game/state/game-state.hpp"
#include "game/player/party.hpp"
#include "game/creature/race-enum.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/text-info.hpp"

#include <vector>
#include <algorithm>


namespace game
{
namespace stage
{

    AdventureCharacterList::AdventureCharacterList(sfml_util::IStage * stagePtr)
    :
        sfml_util::gui::GuiEntity("AdventureStage'sCharacterList", 0.0f, 0.0f),
        LINE_COLOR_( sfml_util::FontManager::Instance()->Color_GrayDarker() ),
        OUTER_SPACER_( sfml_util::MapByRes(75.0f, 300.0f) ),
        HEALTH_COLUMN_WIDTH_( sfml_util::MapByRes(180.0f, 500.0f) ),
        MANA_COLUMN_WIDTH_( HEALTH_COLUMN_WIDTH_ ),
        CELL_TEXT_LEFT_SPACER_( sfml_util::MapByRes(12.0f, 25.0f) ),
        stagePtr_(stagePtr),
        namesButtonUVec_(),
        condsTextRegionsUVec_(),
        healthTextRegionsUVec_(),
        manaTextRegionsUVec_(),
        nameColumnRects_(),
        healthColumnRects_(),
        manaColumnRects_(),
        conditionColumnRects_(),
        lineVerts_(),
        quadVerts_(),
        innerShadeQuadVerts_()
    {}


    AdventureCharacterList::~AdventureCharacterList()
    {}


    bool AdventureCharacterList::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &)
    {
        return false;
    }


    void AdventureCharacterList::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw( & innerShadeQuadVerts_[0], innerShadeQuadVerts_.size(), sf::Quads, states);
        target.draw( & lineVerts_[0], lineVerts_.size(), sf::Lines, states);
        target.draw( & quadVerts_[0], quadVerts_.size(), sf::Quads, states);
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
        innerShadeQuadVerts_.clear();
        
        SetupNameButtons();
        SetupHealthNumbersText();
        SetupManaNumbersText();

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

        SetupMouseoverText();
    }


    void AdventureCharacterList::SetupNameButtons()
    {
        for (auto const & BUTTON_UPTR : namesButtonUVec_)
        {
            stagePtr_->EntityRemove(BUTTON_UPTR.get());
        }

        namesButtonUVec_.clear();

        for (auto const CHARACTER_PTR : Game::Instance()->State().Party().Characters())
        {
            const sfml_util::gui::TextInfo TEXT_INFO{
                CHARACTER_PTR->Name(),
                sfml_util::FontManager::Instance()->Font_Default1(),
                sfml_util::FontManager::Instance()->Size_Large(),
                sfml_util::FontManager::Instance()->Color_GrayDarker() };

            const sfml_util::gui::MouseTextInfo MOUSE_TEXT_INFO{ TEXT_INFO };

            namesButtonUVec_.push_back( std::make_unique<sfml_util::gui::FourStateButton>(
                "AdventureStage'sCharacterList'sNameButtonFor_" + CHARACTER_PTR->Name(),
                0.0f,//position set in SetupPositions_NameButtons()
                0.0f,
                sf::Texture(),
                false,
                sf::Texture(),
                false,
                sf::Texture(),
                false,
                sf::Texture(),
                false,
                MOUSE_TEXT_INFO) );
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

        for (auto const CHARACTER_PTR : Game::Instance()->State().Party().Characters())
        {
            std::ostringstream ss;
            ss << CHARACTER_PTR->HealthCurrent() << "/" << CHARACTER_PTR->HealthNormal();

            const sfml_util::gui::TextInfo TEXT_INFO{
                ss.str(),
                sfml_util::FontManager::Instance()->Font_NumbersDefault1(),
                sfml_util::FontManager::Instance()->Size_Smallish(),
                sfml_util::FontManager::Instance()->Color_GrayDarker() };

            const sf::FloatRect RECT(
                0.0f,//actual position will be set by SetupPositions_Conditions()
                0.0f,
                0.0f,
                0.0f);

            healthTextRegionsUVec_.push_back(std::make_unique<sfml_util::gui::TextRegion>(
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
        for (auto const & TEXT_UPTR : manaTextRegionsUVec_)
        {
            stagePtr_->EntityRemove(TEXT_UPTR.get());
        }

        manaTextRegionsUVec_.clear();

        for (auto const CHARACTER_PTR : Game::Instance()->State().Party().Characters())
        {
            std::ostringstream ss;
            ss << CHARACTER_PTR->Mana() << "/" << CHARACTER_PTR->ManaNormal();

            const sfml_util::gui::TextInfo TEXT_INFO{
                ss.str(),
                sfml_util::FontManager::Instance()->Font_NumbersDefault1(),
                sfml_util::FontManager::Instance()->Size_Smallish(),
                sfml_util::FontManager::Instance()->Color_GrayDarker() };

            const sf::FloatRect RECT(
                0.0f,//actual position will be set by SetupPositions_Conditions()
                0.0f,
                0.0f,
                0.0f);

            manaTextRegionsUVec_.push_back(std::make_unique<sfml_util::gui::TextRegion>(
                "AdventureStage'sCharacterList'sManaTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT));
        }

        for (auto const & TEXT_UPTR : manaTextRegionsUVec_)
        {
            stagePtr_->EntityAdd(TEXT_UPTR.get());
        }
    }


    void AdventureCharacterList::SetupConditionsText()
    {
        for (auto const & TEXTREGION_UPTR : condsTextRegionsUVec_)
        {
            stagePtr_->EntityRemove(TEXTREGION_UPTR.get());
        }

        condsTextRegionsUVec_.clear();

        auto const CHARACTER_PVEC{ Game::Instance()->State().Party().Characters() };
        auto const NUM_CHARACTERS{ CHARACTER_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            auto const CHARACTER_PTR{ CHARACTER_PVEC[i] };
            
            const sfml_util::gui::TextInfo TEXT_INFO{
                CHARACTER_PTR->ConditionNames(),
                sfml_util::FontManager::Instance()->Font_Default1(),
                sfml_util::FontManager::Instance()->Size_Normal(),
                sfml_util::FontManager::Instance()->Color_GrayDarker() };

            const sf::FloatRect RECT(
                0.0f,//actual position will be set by SetupPositions_Conditions()
                0.0f,
                conditionColumnRects_[i].width - (CELL_TEXT_LEFT_SPACER_ * 2.0f),
                conditionColumnRects_[i].height);

            condsTextRegionsUVec_.push_back( std::make_unique<sfml_util::gui::TextRegion>(
                "AdventureStageCharacterList'sConditionTextFor_" + CHARACTER_PTR->Name(),
                TEXT_INFO,
                RECT) );
        }

        for (auto const & TEXTREGION_UPTR : condsTextRegionsUVec_)
        {
            stagePtr_->EntityAdd(TEXTREGION_UPTR.get());
        }
    }


    void AdventureCharacterList::SetupColumnRects_Name()
    {
        nameColumnRects_.clear();

        auto const NAME_CELL_PAD_HORIZ{ sfml_util::MapByRes(32.0f, 64.0f) };
        auto const NAME_CELL_PAD_VERT{ sfml_util::MapByRes(16.0f, 32.0f) };

        auto const LONGEST_NAME_BUTTON_WIDTH{ NAME_CELL_PAD_HORIZ + ( * std::max_element(
            namesButtonUVec_.begin(),
            namesButtonUVec_.end(),
            [](auto & A_UPTR, auto & B_UPTR)
            {
                return (A_UPTR->GetEntityRegion().width < B_UPTR->GetEntityRegion().width);
            }))->GetEntityRegion().width };

        auto const TALLEST_NAME_BUTTON_HEIGHT{ NAME_CELL_PAD_VERT + ( * std::max_element(
            namesButtonUVec_.begin(),
            namesButtonUVec_.end(),
            [](auto & A_UPTR, auto & B_UPTR)
            {
                return (A_UPTR->GetEntityRegion().height < B_UPTR->GetEntityRegion().height);
            }))->GetEntityRegion().height };

        auto const NUM_BUTTONS{ namesButtonUVec_.size() };
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            auto const I_FLOAT{ static_cast<float>(i) };

            const sf::FloatRect NAME_RECT(
                GetEntityPos().x,
                GetEntityRegion().top + (I_FLOAT * TALLEST_NAME_BUTTON_HEIGHT) + I_FLOAT,
                LONGEST_NAME_BUTTON_WIDTH,
                TALLEST_NAME_BUTTON_HEIGHT);

            nameColumnRects_.push_back(NAME_RECT);
            sfml_util::DrawRectangleWithLineVerts(NAME_RECT, LINE_COLOR_, lineVerts_);
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

            healthColumnRects_.push_back(HEALTH_RECT);
            sfml_util::DrawRectangleWithLineVerts(HEALTH_RECT, LINE_COLOR_, lineVerts_);
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

            manaColumnRects_.push_back(MANA_RECT);
            sfml_util::DrawRectangleWithLineVerts(MANA_RECT, LINE_COLOR_, lineVerts_);
        }
    }


    void AdventureCharacterList::SetupColumnRects_Condition()
    {
        conditionColumnRects_.clear();

        auto const OUTER_PAD_HORIZ{ 2.0f * OUTER_SPACER_ };
        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };

        auto const NON_COND_COLUMN_WIDTHS{
            nameColumnRects_[0].width +
            healthColumnRects_[0].width +
            manaColumnRects_[0].width };

        auto const COND_COLUMN_WIDTH{ (SCREEN_WIDTH - OUTER_PAD_HORIZ) - NON_COND_COLUMN_WIDTHS };

        for (auto const & MANA_RECT : manaColumnRects_)
        {
            const sf::FloatRect COND_RECT(
                MANA_RECT.left + MANA_RECT.width + 1.0f,
                MANA_RECT.top,
                COND_COLUMN_WIDTH,
                MANA_RECT.height);

            conditionColumnRects_.push_back(COND_RECT);
            sfml_util::DrawRectangleWithLineVerts(COND_RECT, LINE_COLOR_, lineVerts_);
        }
    }


    void AdventureCharacterList::SetupPositions_NameButtons()
    {
        auto const NUM_BUTTONS{ namesButtonUVec_.size() };
        for (std::size_t i(0); i<NUM_BUTTONS; ++i)
        {
            auto const LEFT{ nameColumnRects_[0].left + CELL_TEXT_LEFT_SPACER_ };

            auto const CELL_MID_VERT{
                nameColumnRects_[i].top + (nameColumnRects_[0].height * 0.5f) };

            auto const TOP{
                CELL_MID_VERT - (namesButtonUVec_[i]->GetEntityRegion().height * 0.5f) };
            
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
        auto const NUM_TEXTREGIONS{ condsTextRegionsUVec_.size() };
        for (std::size_t i(0); i<NUM_TEXTREGIONS; ++i)
        {
            auto const LEFT{ conditionColumnRects_[0].left + CELL_TEXT_LEFT_SPACER_ };

            auto const TOP{
                (conditionColumnRects_[i].top + (conditionColumnRects_[i].height * 0.5f)) -
                    (condsTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f) +
                    sfml_util::MapByRes(10.0f, 20.0f)};

            condsTextRegionsUVec_[i]->SetEntityPos(LEFT, TOP);
        }
    }
    

    void AdventureCharacterList::SetupPositions_OverallRegion()
    {
        auto const OVERALL_RECT_WIDTH{
            nameColumnRects_[0].width +
            healthColumnRects_[0].width +
            manaColumnRects_[0].width +
            conditionColumnRects_[0].width };

        auto const LAST_INDEX{ nameColumnRects_.size() - 1 };

        auto const OVERALL_RECT_HEIGHT{
            (nameColumnRects_[LAST_INDEX].top + nameColumnRects_[LAST_INDEX].height) -
                nameColumnRects_[0].top };

        const sf::FloatRect OVERALL_RECT(
            nameColumnRects_[0].left,
            nameColumnRects_[0].top,
            OVERALL_RECT_WIDTH,
            OVERALL_RECT_HEIGHT);

        SetEntityRegion(OVERALL_RECT);

        for (float i(0.0f); i < 3.0f; i += 1.0f)
        {
            sfml_util::DrawRectangleWithLineVerts(
                sf::FloatRect(
                    OVERALL_RECT.left - i,
                    OVERALL_RECT.top - i,
                    (OVERALL_RECT.width + 3.0f) + (i * 2.0f),
                    OVERALL_RECT.height + (i * 2.0f)),
                LINE_COLOR_ - sf::Color(0, 0, 0, static_cast<sf::Uint8>(i) * 50),
                lineVerts_);
        }

        const sf::Uint8 INNER_SHADOW_ALPHA{ 18 };
        auto const INNER_SHADOW_COLOR{ sf::Color(0, 0, 0, INNER_SHADOW_ALPHA) };

        sfml_util::DrawQuad(
            sf::FloatRect(
                OVERALL_RECT.left - 1.0f,
                OVERALL_RECT.top - 1.0f,
                (OVERALL_RECT.width + 3.0f) - 1.0f,
                OVERALL_RECT.height - 1.0f),
            INNER_SHADOW_COLOR,
            INNER_SHADOW_COLOR,
            innerShadeQuadVerts_);
    }


    void AdventureCharacterList::SetupPositions_Bars(const WhichBar WHICH_BAR)
    {
        auto const NUM_RECTS{
            ((WHICH_BAR == WhichBar::Health) ?
                healthColumnRects_.size() :
                manaColumnRects_.size()) };

        for (std::size_t i(0); i < NUM_RECTS; ++i)
        {
            //determine if mana bar is drawn at all
            auto const CHARACTER_PTR{ Game::Instance()->State().Party().Characters()[i] };

            if (WHICH_BAR == WhichBar::Mana && (CHARACTER_PTR->ManaNormal().IsZero()))
            {
                continue;
            }

            auto const BOUNDS{
                ((WHICH_BAR == WhichBar::Health) ?
                    healthColumnRects_[i] :
                    manaColumnRects_[i]) };

            //eastblish the outer line (bounding box) coordinates
            auto const WIDTH{ BOUNDS.width * 0.75f };

            auto const HEIGHT{ BOUNDS.height * 0.5f };
            auto const LEFT{ BOUNDS.left + ((BOUNDS.width - WIDTH) * 0.5f) };

            auto const TOP{
                (BOUNDS.top + (BOUNDS.height * 0.5f)) - (HEIGHT * 0.5f) };

            sfml_util::DrawRectangleWithLineVerts(
                sf::FloatRect(LEFT, TOP, WIDTH, HEIGHT),
                LINE_COLOR_ - sf::Color(0, 0, 0, 127),
                lineVerts_);

            //establish the inner line (bounding box) coordinates
            sfml_util::DrawRectangleWithLineVerts(
                sf::FloatRect(LEFT + 1.0f, TOP + 1.0f, WIDTH - 2.0f, HEIGHT - 2.0f),
                LINE_COLOR_,
                lineVerts_);

            //establish the inner colored bar coordinates
            auto const BAR_RATIO{
                ((WHICH_BAR == WhichBar::Health) ?
                CHARACTER_PTR->HealthRatio() :
                    CHARACTER_PTR->ManaRatio()) };

            auto const BAR_WIDTH{ (WIDTH - 4.0f) * BAR_RATIO };

            //establish the inner colored bar colors
            const sf::Uint8 COLOR_BASE{ 64 };
            auto const COLOR_BASE_F{ static_cast<float>(COLOR_BASE) };
            const sf::Uint8 COLOR_MAX{ 255 };
            const sf::Uint8 COLOR_HIGH{ 192 };
            auto const COLOR_HIGH_F{ static_cast<float>(COLOR_HIGH) };
            auto const COLOR_BAR_ALPHA{ COLOR_HIGH };

            auto const COLOR_BAR_LEFT{ sf::Color(
                ((WHICH_BAR == WhichBar::Health) ? COLOR_MAX : COLOR_BASE),
                COLOR_BASE,
                ((WHICH_BAR == WhichBar::Health) ? COLOR_BASE : COLOR_MAX)) };

            auto const COLOR_BAR_RIGHT_WHITE_VAL_F{
                COLOR_BASE_F + ((COLOR_HIGH_F - COLOR_BASE_F) * BAR_RATIO) };

            auto const COLOR_BAR_RIGHT_WHITE_VAL{
                static_cast<sf::Uint8>(COLOR_BAR_RIGHT_WHITE_VAL_F) };

            auto const COLOR_BAR_RIGHT{
                sf::Color(
                    ((WHICH_BAR == WhichBar::Health) ? COLOR_MAX : COLOR_BAR_RIGHT_WHITE_VAL),
                    COLOR_BAR_RIGHT_WHITE_VAL,
                    ((WHICH_BAR == WhichBar::Health) ? COLOR_BAR_RIGHT_WHITE_VAL : COLOR_MAX),
                    COLOR_BAR_ALPHA) };

            //draw inner colored bar
            sfml_util::DrawQuad(
                sf::FloatRect(LEFT + 2.0f, TOP + 2.0f, BAR_WIDTH, HEIGHT - 4.0f),
                COLOR_BAR_LEFT,
                COLOR_BAR_RIGHT,
                quadVerts_);

            auto const NUMBER_TEXT_TOP_SPACER{ sfml_util::MapByRes(0.0f, 10.0f) };

            if (WHICH_BAR == WhichBar::Health)
            {
                healthTextRegionsUVec_[i]->SetEntityPos(
                    (LEFT + (BAR_WIDTH * 0.5f)) - (healthTextRegionsUVec_[i]->GetEntityRegion().width * 0.5f),
                    (TOP + (HEIGHT * 0.5f)) - (healthTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f) + NUMBER_TEXT_TOP_SPACER);
            }
            else
            {
                manaTextRegionsUVec_[i]->SetEntityPos(
                    (LEFT + (BAR_WIDTH * 0.5f)) - (manaTextRegionsUVec_[i]->GetEntityRegion().width * 0.5f),
                    (TOP + (HEIGHT * 0.5f)) - (manaTextRegionsUVec_[i]->GetEntityRegion().height * 0.5f) + NUMBER_TEXT_TOP_SPACER);
            }
        }
    }


    void AdventureCharacterList::SetupMouseoverText()
    {
        auto const CHARACTER_PVEC{ Game::Instance()->State().Party().Characters() };
        auto const NUM_CHARACTERS{ CHARACTER_PVEC.size() };
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            auto const CHARACTER_PTR{ Game::Instance()->State().Party().Characters()[i] };
            namesButtonUVec_[i]->SetMouseHoverText( NameButtonMouseoverText(CHARACTER_PTR) );
            condsTextRegionsUVec_[i]->SetMouseHoverText(CHARACTER_PTR->ConditionNames());
        }
    }


    const std::string AdventureCharacterList::NameButtonMouseoverText(
        const player::CharacterPtr_t CHARACTER_PTR)
    {
        auto const NAME_STR{ CHARACTER_PTR->Name() };
        auto const RACE_STR{ CHARACTER_PTR->RaceName() };

        std::ostringstream ss;
        ss << NAME_STR << " the " << CHARACTER_PTR->RankClassName();

        if (NAME_STR != RACE_STR)
        {
            ss << " "
                << creature::race::RaceRoleName(CHARACTER_PTR->Race(), CHARACTER_PTR->Role());
        }

        ss << " of Rank " << CHARACTER_PTR->Rank();
        return ss.str();
    }

}
}
