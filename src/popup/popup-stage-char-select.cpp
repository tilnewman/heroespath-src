// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-char-select.cpp
//
#include "popup-stage-char-select.hpp"

#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "gui/image-loaders.hpp"
#include "gui/sound-manager.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace popup
{

    PopupStageCharacterSelect::PopupStageCharacterSelect(const PopupInfo & POPUP_INFO)
        : PopupStageImageSelect(POPUP_INFO)
        , charDetailsTextRegionUPtr_()
    {}

    PopupStageCharacterSelect::~PopupStageCharacterSelect() = default;

    void PopupStageCharacterSelect::SetupCharDetailsText()
    {
        sf::FloatRect charDetailsTextRegion(ContentRegion());
        charDetailsTextRegion.top = sfutil::Bottom(imagesRect_);
        charDetailsTextRegion.height = 0.0f;

        auto charDetailsTextInfo { popupInfo_.TextInfo() };
        charDetailsTextInfo.text = " ";

        charDetailsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "PopupWindow's_CharacterSelection_DetailsText",
            charDetailsTextInfo,
            charDetailsTextRegion);
    }

    void PopupStageCharacterSelect::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        PopupStageImageSelect::draw(target, states);
        charDetailsTextRegionUPtr_->draw(target, states);
    }

    bool PopupStageCharacterSelect::HandleSelect()
    {
        // attempt to select the value most recently enqueued
        if (imageMoveQueue_.empty() == false)
        {
            if (popupInfo_.TextVec().at(imageMoveQueue_.back()).empty() == false)
            {
                PlayInvalidKeypressSoundEffect();
                return false;
            }
            else
            {
                selection_ = static_cast<int>(imageMoveQueue_.back());
            }
        }
        else
        {
            selection_ = static_cast<int>(imageIndex_);
        }

        if ((selection_ < 0) || (selection_ >= static_cast<int>(popupInfo_.TextVec().size())))
        {
            PlayInvalidKeypressSoundEffect();
            return false;
        }

        if (popupInfo_.TextVec().at(static_cast<std::size_t>(selection_)).empty() == false)
        {
            PlayInvalidKeypressSoundEffect();
            return false;
        }

        return PopupStageBase::HandleSelect();
    }

    void PopupStageCharacterSelect::SetupContentForNewImage(const bool WILL_ERASE)
    {
        SetupCharacterSelectDetailText(WILL_ERASE);
        SetupCharacterSelectionRejectImage(WILL_ERASE);
    }

    void PopupStageCharacterSelect::SetupCharacterSelectDetailText(const bool WILL_ERASE)
    {
        if (WILL_ERASE)
        {
            charDetailsTextRegionUPtr_->SetText(" ");
            return;
        }

        const auto CREATURE_PTR { game::Game::Instance()->State().Party().GetAtOrderPos(
            imageIndex_) };

        std::ostringstream ss;
        ss << CREATURE_PTR->Name() << "\n" << CREATURE_PTR->RaceName();

        if (CREATURE_PTR->IsBeast())
        {
            if (CREATURE_PTR->Race() != creature::race::Wolfen)
            {
                ss << ", " << CREATURE_PTR->RoleName();
            }

            ss << " " << CREATURE_PTR->RankClassName() << "\n";
        }
        else
        {
            ss << " " << CREATURE_PTR->RankClassName() << " " << CREATURE_PTR->RoleName() << "\n";
        }

        ss << "Health:  " << CREATURE_PTR->HealthCurrent() << "/" << CREATURE_PTR->HealthNormal()
           << "\n"
           << "Condition:  " << CREATURE_PTR->ConditionNames(3) << "\n";

        charDetailsTextRegionUPtr_->SetText(ss.str());
    }

    void PopupStageCharacterSelect::SetupCharacterSelectionRejectImage(const bool WILL_ERASE)
    {
        if (imageIndex_ >= popupInfo_.TextVec().size())
        {
            return;
        }

        const auto TEXT { popupInfo_.TextVec().at(imageIndex_) };

        if (TEXT.empty())
        {
            selection_ = static_cast<int>(imageIndex_);
        }
        else
        {
            selection_ = -1; // any negative value will work here
        }

        if (TEXT.empty() || WILL_ERASE)
        {
            willShowXImage_ = false;
            SetupCharacterSelectDetailText(false);
            charDetailsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color::Black);
        }
        else
        {
            willShowXImage_ = true;
            charDetailsTextRegionUPtr_->SetText(TEXT);
            charDetailsTextRegionUPtr_->SetEntityColorFgBoth(sf::Color(100, 32, 32));
        }
    }

} // namespace popup
} // namespace heroespath
