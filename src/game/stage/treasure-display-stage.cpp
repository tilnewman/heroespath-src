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
// treasure-display-stage.hpp
//
#include "treasure-display-stage.hpp"

#include "game/game-data-file.hpp"
#include "game/combat/encounter.hpp"
#include "game/stage/treasure-stage.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/player/character.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"

#include "misc/vectors.hpp"


namespace game
{
namespace stage
{
namespace treasure
{

    ListboxColors::ListboxColors()
    :
        image(sf::Color(255, 255, 255, 190)),
        line(sfml_util::FontManager::Color_GrayDark()),
        foreground(line),
        background(sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220)),
        title(sfml_util::FontManager::Color_Orange() - sf::Color(130, 130, 130, 0)),
        colorSet(foreground, background),
        backgroundInfo(background)
    {}


    DisplayMeasurements::DisplayMeasurements()
    :
        screenWidth(sfml_util::Display::Instance()->GetWinWidth()),
        screenHeight(sfml_util::Display::Instance()->GetWinHeight()),
        innerPad(sfml_util::MapByRes(10.0f, 40.0f)),
        innerRect(
            innerPad,
            innerPad,
            screenWidth - (2.0f * innerPad),
            screenWidth - (2.0f * innerPad)),
        creatureImageLeft(innerRect.left + sfml_util::MapByRes(35.0f, 100.0f)),
        creatureImageScale(sfml_util::MapByRes(0.75f, 3.25f)),
        creatureImageHeight(
            sfml_util::gui::CreatureImageManager::DimmensionMax() * creatureImageScale),
        listboxHeightReduction(sfml_util::MapByRes(100.0f, 400.0f)),
        listboxScreenEdgeMargin(sfml_util::MapByRes(35.0f, 100.0f)),
        listboxBetweenSpacer(sfml_util::MapByRes(65.0f, 200.0f)),
        listboxWidth(
            ((innerRect.width - (2.0f * listboxScreenEdgeMargin)) - listboxBetweenSpacer) * 0.5f),
        offScreenPos((listboxWidth + sfml_util::MapByRes(100.0f, 300.0f)) * -1.0f),
        treasureListboxLeft(innerRect.left + listboxScreenEdgeMargin),
        inventoryListboxLeft(treasureListboxLeft + listboxWidth + listboxBetweenSpacer),
        listboxTop(
            /* TODO CREATURE_IMAGE_POS_TOP_ +*/ creatureImageHeight + (listboxHeightReduction * 0.5f)),
        listboxHeight(
            (screenHeight - listboxTop) - listboxHeightReduction),
        listboxRegion(
            treasureListboxLeft,
            listboxTop,
            listboxWidth,
            listboxHeight)
    {}

}//end of namespace treasure


    TreasureDisplayStage::TreasureDisplayStage(TreasureStage * treasureStagePtr)
    :
        Stage("TreasureDisplay", false),
        treasureStagePtr_(treasureStagePtr),
        ouroborosUPtr_(),
        listboxMoverUPtr_(),
        backgroundTexture_(),
        backgroundSprite_(),
        corpseTexture_(),
        corpseSprite_(),
        treasureTexture_(),
        treasureSprite_(),
        coinsTexture_(),
        coinsSprite_(),
        willShowCoins_(false)
    {}


    void TreasureDisplayStage::Setup()
    {
        Stage::StageRegionSet(sf::FloatRect(
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight()));

        SetupInitial();
    }


    void TreasureDisplayStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundSprite_, STATES);
        target.draw(corpseSprite_, STATES);
        target.draw(treasureSprite_, STATES);

        if (willShowCoins_)
        {
            target.draw(coinsSprite_, STATES);
        }

        Stage::Draw(target, STATES);
    }


    void TreasureDisplayStage::SetupInitial()
    {   
        SetupInitial_BackgroundImage();
        SetupInitial_Ouroboros();
    }


    void TreasureDisplayStage::SetupAfterPleaseWait(const item::TreasureImage::Enum WHICH_IMAGE)
    {
        SetupAfterPleaseWait_CorpseImage();
        auto const TREASURE_IMAGE_POS_V{ SetupAfterPleaseWait_TreasureImage(WHICH_IMAGE) };
        SetupAfterPleaseWait_CoinsImage(TREASURE_IMAGE_POS_V);
    }


    void TreasureDisplayStage::SetupForCollection(const item::TreasureImage::Enum WHICH_IMAGE)
    {
        SetupForCollection_TreasureImage(WHICH_IMAGE);
        SetupForCollection_TreasureListbox();
        //TODO
    }


    void TreasureDisplayStage::SetupInitial_BackgroundImage()
    {
        sfml_util::LoadTexture(backgroundTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2b"));

        backgroundSprite_.setTexture(backgroundTexture_);
        backgroundSprite_.setPosition(0.0f, 0.0f);

        backgroundSprite_.setScale(
            sfml_util::Display::Instance()->GetWinWidth() / backgroundSprite_.getLocalBounds().width,
            sfml_util::Display::Instance()->GetWinHeight() / backgroundSprite_.getLocalBounds().height);
    }


    void TreasureDisplayStage::SetupInitial_Ouroboros()
    {
        ouroborosUPtr_ = std::make_unique<Ouroboros>("TreasureDisplayStage's", true);
        EntityAdd(ouroborosUPtr_.get());
    }


    void TreasureDisplayStage::SetupAfterPleaseWait_CorpseImage()
    {
        sfml_util::LoadTexture(corpseTexture_,
            GameDataFile::Instance()->GetMediaPath(CorpseImageKeyFromEnemyParty()));

        corpseSprite_.setTexture(corpseTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        auto const CORPSE_IMAGE_MAX_WIDTH{ (SCREEN_WIDTH * 0.75f) };
        auto const CORPSE_IMAGE_MAX_HEIGHT{ (SCREEN_HEIGHT * 0.5f) };

        auto const SCALE_HORIZ{ CORPSE_IMAGE_MAX_WIDTH /
            corpseSprite_.getLocalBounds().width };

        corpseSprite_.setScale(SCALE_HORIZ, SCALE_HORIZ);

        if (corpseSprite_.getGlobalBounds().height > CORPSE_IMAGE_MAX_HEIGHT)
        {
            auto const SCALE_VERT{ CORPSE_IMAGE_MAX_HEIGHT /
                corpseSprite_.getLocalBounds().height };

            corpseSprite_.setScale(SCALE_VERT, SCALE_VERT);
        }

        auto const CORPSE_IMAGE_LEFT{ (SCREEN_WIDTH * 0.5f) -
            (corpseSprite_.getGlobalBounds().width * 0.5f) };

        auto const CORPSE_IMAGE_TOP{ SCREEN_HEIGHT - (sfml_util::MapByRes(50.0f, 150.0f) +
            corpseSprite_.getGlobalBounds().height) };

        corpseSprite_.setPosition(CORPSE_IMAGE_LEFT, CORPSE_IMAGE_TOP);
        corpseSprite_.setColor(sf::Color(255, 255, 255, 50));
    }


    const sf::Vector2f TreasureDisplayStage::SetupAfterPleaseWait_TreasureImage(
        const item::TreasureImage::Enum WHICH_IMAGE)
    {
        auto const TREASURE_IMAGE_KEY{ item::TreasureImage::ToImageKey(WHICH_IMAGE) };

        auto const TREASURE_IMAGE_SCALE_NEED_REDUCTION{
            (TREASURE_IMAGE_KEY == "media-images-bones-bone-pile-1") ||
            (TREASURE_IMAGE_KEY == "media-images-bones-bone-pile-2") };

        sfml_util::LoadTexture(treasureTexture_,
            GameDataFile::Instance()->GetMediaPath(TREASURE_IMAGE_KEY));

        treasureSprite_.setTexture(treasureTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        //these values found by experiment to look good at various resolutions
        auto const TREASURE_IMAGE_SCALE_ADJ{
            ((TREASURE_IMAGE_SCALE_NEED_REDUCTION) ? 0.75f : 1.0f) };

        auto const TREASURE_IMAGE_MAX_WIDTH{
            (SCREEN_WIDTH * 0.5f * TREASURE_IMAGE_SCALE_ADJ) };
        
        auto const TREASURE_IMAGE_MAX_HEIGHT{
            (SCREEN_HEIGHT * 0.333f * TREASURE_IMAGE_SCALE_ADJ) };

        sfml_util::ScaleSpriteToFit(
            treasureSprite_,
            TREASURE_IMAGE_MAX_WIDTH,
            TREASURE_IMAGE_MAX_HEIGHT);

        //these values found by experiment to look good at various resolutions
        auto const TREASURE_IMAGE_LEFT{ sfml_util::MapByRes(100.0f, 300.0f) };

        auto const TREASURE_IMAGE_TOP{
            (sfml_util::MapByRes(50.0f, 150.0f) + (SCREEN_HEIGHT * 0.166f)) -
            (treasureSprite_.getGlobalBounds().height * 0.5f) };

        treasureSprite_.setPosition(TREASURE_IMAGE_LEFT, TREASURE_IMAGE_TOP);
        treasureSprite_.setColor(sf::Color(255, 255, 255, 192));

        return treasureSprite_.getPosition();
    }


    void TreasureDisplayStage::SetupAfterPleaseWait_CoinsImage(
        const sf::Vector2f & TREASURE_IMAGE_POS_V)
    {
        sfml_util::LoadTexture(coinsTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-coins"));

        coinsSprite_.setTexture(coinsTexture_);

        auto const COINS_IMAGE_WIDTH{ (sfml_util::Display::Instance()->GetWinWidth() * 0.125f) };
        auto const COINS_SCALE{ COINS_IMAGE_WIDTH / coinsSprite_.getLocalBounds().width };
        coinsSprite_.setScale(COINS_SCALE, COINS_SCALE);

        coinsSprite_.setColor(sf::Color(255, 255, 255, 192));

        //these values found by experiment to look good at various resolutions
        auto const COINS_LEFT{ TREASURE_IMAGE_POS_V.x +
            (treasureSprite_.getGlobalBounds().width * 0.80f) };

        auto const COINS_TOP{ TREASURE_IMAGE_POS_V.y +
            (treasureSprite_.getGlobalBounds().height * 0.75f) };

        coinsSprite_.setPosition(COINS_LEFT, COINS_TOP);
    }


    const std::string TreasureDisplayStage::CorpseImageKeyFromEnemyParty() const
    {
        auto const DEAD_ENEMY_CHARACTERS_PVEC{
            combat::Encounter::Instance()->DeadNonPlayerParty().Characters() };

        misc::StrVec_t corpseKeyStrVec;
        for (auto const NEXT_ENEMY_CHARACTER_PTR : DEAD_ENEMY_CHARACTERS_PVEC)
        {
            auto const CORPSE_KEY_STR_VEC{ creature::race::CorpseImageKeys(
                NEXT_ENEMY_CHARACTER_PTR->Race()) };

            std::copy(CORPSE_KEY_STR_VEC.begin(),
                CORPSE_KEY_STR_VEC.end(),
                std::back_inserter(corpseKeyStrVec));
        }

        if (DEAD_ENEMY_CHARACTERS_PVEC.empty() || corpseKeyStrVec.empty())
        {
            M_HP_LOG_ERR("stage::TreasureDisplayStage::GetCorpseImageKeyFromEnemyParty() "
                << "was unable to gather any key strings.  Using default image.  "
                << "DEAD_ENEMY_CHARACTERS_PVEC.size()=" << DEAD_ENEMY_CHARACTERS_PVEC.size()
                << ", corpseKeyStrVec.size()=" << corpseKeyStrVec.size());

            auto const DEFAULT_CORPSE_KEY_STR_VEC{
                creature::race::CorpseImageKeys(creature::race::Human) };

            return misc::Vector::SelectRandom(DEFAULT_CORPSE_KEY_STR_VEC);
        }
        else
        {
            //Allow duplicates in corpseKeyStrVec so that the more a race was faced
            //during combat means the more likely that corpse image is shown.
            return misc::Vector::SelectRandom(corpseKeyStrVec);
        }
    }


    void TreasureDisplayStage::SetupForCollection_TreasureImage(
        const item::TreasureImage::Enum WHICH_IMAGE)
    {
        if (item::TreasureImage::ChestOpen == WHICH_IMAGE)
        {
            sfml_util::LoadTexture(
                treasureTexture_,
                GameDataFile::Instance()->GetMediaPath("media-images-chest-open"));

            willShowCoins_ = true;
        }
        else if (item::TreasureImage::LockboxOpen == WHICH_IMAGE)
        {
            sfml_util::LoadTexture(
                treasureTexture_,
                GameDataFile::Instance()->GetMediaPath("media-images-lockbox-open"));

            willShowCoins_ = true;
        }
    }


    void TreasureDisplayStage::SetupForCollection_TreasureListbox()
    {
        //TODO
    }

}
}
