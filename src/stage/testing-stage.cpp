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
// testing-stage.cpp
//
#include "testing-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/song-image-manager.hpp"
#include "sfml-util/gui/condition-image-manager.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"

#include "popup/popup-manager.hpp"

#include "log/log-macros.hpp"

#include "misc/types.hpp"
#include "misc/real.hpp"
#include "game/game-data-file.hpp"
#include "creature/title-warehouse.hpp"
#include "creature/conditions.hpp"
#include "creature/condition-warehouse.hpp"
#include "creature/creature.hpp"
#include "player/character.hpp"
#include "non-player/character.hpp"
#include "spell/spell-warehouse.hpp"
#include "song/song-warehouse.hpp"
#include "non-player/inventory-factory.hpp"
#include "item/item-factory.hpp"
#include "avatar/avatar-enum.hpp"
#include "avatar/i-view.hpp"
#include "map/level-enum.hpp"
#include "map/map.hpp"
#include "map/map-display.hpp"


#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>


namespace heroespath
{
namespace stage
{

    const std::size_t          TestingStage::TEXT_LINES_COUNT_MAX_{ 100 };
    sfml_util::AnimationUPtr_t TestingStage::animUPtr_;
    const int                  TestingStage::IMAGE_COUNT_MAX_{ 11 };


    TestingStage::TestingStage()
    :
        Stage              ("Testing"),
        SCREEN_WIDTH_      (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_     (sfml_util::Display::Instance()->GetWinHeight()),
        textureList_       (),
        ouroborosUPtr_     (),
        testingBlurbsVec_  (),
        sleepMilliseconds_ (0),
        animBGTexture_     (),
        animBGSprite_      (),
        imageCount_        (0),
        willImageCheck_    (false)
    {}


    TestingStage::~TestingStage()
    {
        ClearAllEntities();
    }


    void TestingStage::Setup()
    {
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("TestingStage's");
        EntityAdd(ouroborosUPtr_.get());

        sfml_util::LoadTexture(animBGTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-wood"));

        animBGSprite_.setTexture(animBGTexture_);
        animBGSprite_.setPosition(0.0f, 0.0f);
    }


    void TestingStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        Stage::Draw(target, STATES);

        if (animUPtr_.get() != nullptr)
        {
            target.draw(animBGSprite_, STATES);
            animUPtr_->draw(target, STATES);
        }

        auto const IMAGE_POS_TOP{ 1.0f };
        {
            std::size_t imageDrawCount{ 0 };
            auto posLeft{ SCREEN_WIDTH_ };

            sfml_util::TextureLst_t::reverse_iterator rItr(textureList_.rbegin());
            for (; rItr != textureList_.rend(); ++rItr)
            {
                sf::Sprite sprite;
                sprite.setTexture( * rItr );

                //reduce size if any dimmension is greater than 256
                auto const MAX_DIMMENSION{ 256.0f };
                auto newHorizScale{ 1.0f };
                if (sprite.getGlobalBounds().width > MAX_DIMMENSION)
                {
                    newHorizScale = MAX_DIMMENSION / sprite.getGlobalBounds().width;
                    sprite.setScale(newHorizScale, newHorizScale);
                }
                if (sprite.getGlobalBounds().height > MAX_DIMMENSION)
                {
                    auto const NEW_VERT_SCALE{ MAX_DIMMENSION / sprite.getGlobalBounds().height };
                    if (NEW_VERT_SCALE < newHorizScale)
                    {
                        sprite.setScale(NEW_VERT_SCALE, NEW_VERT_SCALE);
                    }
                }

                posLeft -= (sprite.getGlobalBounds().width + 5.0f);
                sprite.setPosition(posLeft, IMAGE_POS_TOP);
                target.draw(sprite, STATES);
                ++imageDrawCount;

                if (posLeft < 0.0f)
                {
                    break;
                }
            }

            while (textureList_.size() > imageDrawCount)
            {
                textureList_.erase(textureList_.begin());
            }
        }

        {
            //find out how tall the text lines will be
            sf::Text testText("M",
                              * sfml_util::FontManager::Instance()->Font_Default1(),
                              sfml_util::FontManager::Instance()->Size_Normal());

            //The extra +10 is added because testText's height is only an estimation.
            auto const TEXT_HEIGHT{ testText.getGlobalBounds().height + 10.0f };

            //The + 256 is to make room for the images, so text is not drawn over them.
            auto DO_NOT_PASS_TOP{ IMAGE_POS_TOP + 256.0f + TEXT_HEIGHT };

            //The extra * 2 is added because without it, the text at the bottom is cut off.
            auto posTop{ SCREEN_HEIGHT_ - (TEXT_HEIGHT * 2.0f) };

            StrSizePairVec_t::reverse_iterator rItr{ testingBlurbsVec_.rbegin() };
            for (; rItr != testingBlurbsVec_.rend(); ++rItr)
            {
                std::ostringstream ss;
                ss << rItr->first;

                if (rItr->second > 0)
                {
                    ss << " " << rItr->second;
                }

                sf::Text text(sf::String(ss.str()),
                              * sfml_util::FontManager::Instance()->Font_Default1(),
                              sfml_util::FontManager::Instance()->Size_Normal());

                text.setPosition(1.0f, posTop);

                target.draw(text, STATES);

                posTop -= TEXT_HEIGHT;

                if (posTop < DO_NOT_PASS_TOP)
                {
                    break;
                }
            }
        }
    }


    void TestingStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);
    }


    bool TestingStage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        if (false == willImageCheck_)
        {
            if (KE.code == sf::Keyboard::F1)
            {
                sleepMilliseconds_ = 0;
            }
            else
            {
                auto const SPEED1{ 0 };
                auto const SPEED2{ 500 };
                auto const SPEED3{ 1000 };
                auto const SPEED4{ 3000 };
                if (sleepMilliseconds_ == SPEED1)
                {
                    sleepMilliseconds_ = SPEED2;
                    game::LoopManager::Instance()->TestingStrAppend("System Tests Speed = 2");
                }
                else if (sleepMilliseconds_ == SPEED2)
                {
                    sleepMilliseconds_ = SPEED3;
                    game::LoopManager::Instance()->TestingStrAppend("System Tests Speed = 3");
                }
                else if (sleepMilliseconds_ == SPEED3)
                {
                    sleepMilliseconds_ = SPEED4;
                    game::LoopManager::Instance()->TestingStrAppend("System Tests Speed = 4");
                }
                else
                {
                    sleepMilliseconds_ = SPEED1;
                    game::LoopManager::Instance()->TestingStrAppend("System Tests Speed = 1");
                }
            }
        }

        return true;
    }


    void TestingStage::UpdateMouseDown(const sf::Vector2f &)
    {
        imageCount_ = 0;
    }


    void TestingStage::TestingStrAppend(const std::string & S)
    {
        M_HP_LOG(S);

        testingBlurbsVec_.push_back( std::make_pair(S, 0) );

        if (testingBlurbsVec_.size() > TEXT_LINES_COUNT_MAX_)
        {
            testingBlurbsVec_.erase(testingBlurbsVec_.begin());
        }
    }


    void TestingStage::TestingStrIncrement(const std::string & S)
    {
        auto foundMatch{ false };
        for (auto & nextStrCountPair : testingBlurbsVec_)
        {
            if (nextStrCountPair.first == S)
            {
                ++nextStrCountPair.second;
                foundMatch = true;

                M_HP_LOG(nextStrCountPair.first << " " << nextStrCountPair.second);
            }
        }

        if (false == foundMatch)
        {
            TestingStrAppend(S);
        }
    }


    void TestingStage::TestingImageSet(
        const sf::Texture & TEXTURE,
        const bool WILL_CHECK_FOR_OUTLINE,
        const std::string & CATEGORY_NAME,
        const std::string & TYPE_NAME,
        const std::string & PATH)
    {
        textureList_.push_back(TEXTURE);
        ++imageCount_;

        if (WILL_CHECK_FOR_OUTLINE && DoesImageHaveOutline(TEXTURE))
        {
            M_HP_LOG_ERR("Testing Stage found an image with an outline:  "
                << "category=" << CATEGORY_NAME
                << ", type=" << TYPE_NAME
                << ", path=" << PATH);
        }
    }


    void TestingStage::PerformNextTest()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds_));

        if (game::LoopManager::Instance()->IsFading())
        {
            return;
        }

        if (willImageCheck_ && (IMAGE_COUNT_MAX_ <= imageCount_))
        {
            return;
        }

        static auto hasPromptStart{ false };
        if (false == hasPromptStart)
        {
            hasPromptStart = true;
            game::LoopManager::Instance()->TestingStrAppend("System Tests Starting...");
        }

        //See below (function ReSaveWithBlackBorder) for a comment explaining why
        //this code is commented out.
        //ReSaveWithBlackBorder("media-images-creaturess-dir");
        //ReSaveWithBlackBorder("media-images-items-dir");

        static auto hasTestingCompleted_GameDataFile{ false };
        if (false == hasTestingCompleted_GameDataFile)
        {
            hasTestingCompleted_GameDataFile = PerformGameDataFileTests();;
            return;
        }

        static auto hasTestingCompleted_Stats{ false };
        if (false == hasTestingCompleted_Stats)
        {
            PerformStatsTests();
            hasTestingCompleted_Stats = true;
            return;
        }

        static auto hasTestingCompleted_ImageSet{ false };
        if (false == hasTestingCompleted_ImageSet)
        {
            hasTestingCompleted_ImageSet = TestImageSet();
            return;
        }

        static auto hasTestingCompleted_CharacterImageSet{ false };
        if (false == hasTestingCompleted_CharacterImageSet)
        {
            hasTestingCompleted_CharacterImageSet = TestCharacterImageSet();
            return;
        }

        static auto hasTestingCompleted_Maps{ false };
        if (false == hasTestingCompleted_Maps)
        {
            hasTestingCompleted_Maps = TestMaps();
            return;
        }

        static auto hasTestingCompleted_Spells{ false };
        if (false == hasTestingCompleted_Spells)
        {
            hasTestingCompleted_Spells = spell::Warehouse::Test();
            return;
        }

        static auto hasTestingCompleted_SpellsImageManager{ false };
        if (false == hasTestingCompleted_SpellsImageManager)
        {
            hasTestingCompleted_SpellsImageManager =
                TestImageManager<sfml_util::gui::SpellImageManager, spell::Spells>();

            return;
        }

        static auto hasTestingCompleted_Songs{ false };
        if (false == hasTestingCompleted_Songs)
        {
            hasTestingCompleted_Songs = song::Warehouse::Test();
            return;
        }

        static auto hasTestingCompleted_SongsImageManager{ false };
        if (false == hasTestingCompleted_SongsImageManager)
        {
            hasTestingCompleted_SongsImageManager =
                TestImageManager<sfml_util::gui::SongImageManager, song::Songs>();

            return;
        }

        static auto hasTestingCompleted_Condition{ false };
        if (false == hasTestingCompleted_Condition)
        {
            hasTestingCompleted_Condition = creature::condition::Warehouse::Test();
            return;
        }

        static auto hasTestingCompleted_ConditionImageManager{ false };
        if (false == hasTestingCompleted_ConditionImageManager)
        {
            hasTestingCompleted_ConditionImageManager =
                TestImageManager<sfml_util::gui::ConditionImageManager, creature::Conditions>();

            return;
        }

        static auto hasTestingCompleted_Title{ false };
        if (false == hasTestingCompleted_Title)
        {
            hasTestingCompleted_Title = creature::title::Warehouse::Test();
            return;
        }

        static auto hasTestingCompleted_TitleImageManager{ false };
        if (false == hasTestingCompleted_TitleImageManager)
        {
            hasTestingCompleted_TitleImageManager =
                TestImageManager<sfml_util::gui::TitleImageManager, creature::Titles>();

            return;
        }

        static auto hasTestingCompleted_PopupManager{ false };
        if (false == hasTestingCompleted_PopupManager)
        {
            hasTestingCompleted_PopupManager =
                popup::PopupManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_CombatImageManager{ false };
        if (false == hasTestingCompleted_CombatImageManager)
        {
            hasTestingCompleted_CombatImageManager = TestImageManager<
                sfml_util::gui::CombatImageManager, sfml_util::gui::CombatImageType>();

            return;
        }

        static auto hasTestingCompleted_ItemImageManager{ false };
        if (false == hasTestingCompleted_ItemImageManager)
        {
            hasTestingCompleted_ItemImageManager =
                sfml_util::gui::ItemImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_ItemFactory{ false };
        if (false == hasTestingCompleted_ItemFactory)
        {
            hasTestingCompleted_ItemFactory = item::ItemFactory::Test();
            return;
        }

        static auto hasTestingCompleted_InventoryFactory{ false };
        if (false == hasTestingCompleted_InventoryFactory)
        {
            hasTestingCompleted_InventoryFactory = TestInventoryFactory();
            return;
        }

        static auto hasTestingCompleted_CreatureImageManager{ false };
        if (false == hasTestingCompleted_CreatureImageManager)
        {
            hasTestingCompleted_CreatureImageManager =
                sfml_util::gui::CreatureImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_Animations{ false };
        if (false == hasTestingCompleted_Animations)
        {
            hasTestingCompleted_Animations = TestAnimations();
            return;
        }

        static auto hasTestingCompleted_SoundManager{ false };
        if (false == hasTestingCompleted_SoundManager)
        {
            hasTestingCompleted_SoundManager =
                sfml_util::SoundManager::Instance()->Test();
            return;
        }

        static auto hasFinalPrompt{ false };
        if (false == hasFinalPrompt)
        {
            hasFinalPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend("ALL SYSTEM TESTS PASSED");
        }
    }


    void TestingStage::PerformStatsTests()
    {
        /*
        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::PerformStatsTests() Starting Tests...");

        const stats::StatSet STAT_SET_ZEROS(0, 0, 0, 0, 0, 0);
        stats::StatSet actualSet;
        TestStatSetsCurrentAndNormal("Empty Set Initial Values", actualSet, STAT_SET_ZEROS);

        const stats::StatSet STAT_SET_MOD1(0, -1, 2, -3, 4, -5);
        actualSet = STAT_SET_MOD1;
        TestStatSetsCurrentAndNormal("Mod1 Set Identity", actualSet, STAT_SET_MOD1);

        actualSet = STAT_SET_MOD1.CreateInvertCopy();
        const stats::StatSet STAT_SET_MOD1_INV(0, 1, -2, 3, -4, 5);
        TestStatSetsCurrentAndNormal("Mod1 Set CreateInvertCopy()", actualSet, STAT_SET_MOD1_INV);

        actualSet = STAT_SET_MOD1;
        actualSet.Invert();
        TestStatSetsCurrentAndNormal("Mod1 Set Invert()", actualSet, STAT_SET_MOD1_INV);

        actualSet = STAT_SET_MOD1;
        actualSet.ForceValidNormal();
        stats::StatSet expectedSet(STAT_SET_MOD1);
        const stats::StatSet STAT_SET_MOD1_VALID(0, 0, 2, 0, 4, 0);
        expectedSet.ResetNormal(STAT_SET_MOD1_VALID);
        TestStatSetsCurrentAndNormal("Mod1 Set ForceValidNormal()", actualSet, expectedSet);

        actualSet = STAT_SET_MOD1;
        actualSet.ForceValidCurrent();
        expectedSet = STAT_SET_MOD1;
        expectedSet.ResetCurrent(STAT_SET_MOD1_VALID);
        TestStatSetsCurrentAndNormal("Mod1 Set ForceValidCurrent()", actualSet, expectedSet);

        actualSet = STAT_SET_MOD1;
        actualSet.ForceValidAll();
        expectedSet = STAT_SET_MOD1_VALID;
        TestStatSetsCurrentAndNormal("Mod1 Set ForceValidAll()", actualSet, expectedSet);

        const stats::StatSet STAT_SET_BASE(12, 12, 12, 12, 12, 12);
        actualSet = STAT_SET_BASE;
        expectedSet = STAT_SET_BASE;
        TestStatSetsCurrentAndNormal("Base Set Identity", actualSet, expectedSet);

        actualSet = STAT_SET_BASE;
        actualSet.ModifyCurrentAndActual(STAT_SET_MOD1);
        const stats::StatSet STAT_SET_BASE_MOD1{12, 11, 14, 9, 16, 7};
        expectedSet = STAT_SET_BASE;
        expectedSet.ResetCurrent(STAT_SET_BASE_MOD1);
        TestStatSetsCurrentAndNormal("Base Set Mod1 Current", actualSet, expectedSet);

        actualSet.ModifyCurrentAndActual(STAT_SET_MOD1.CreateInvertCopy());
        expectedSet = STAT_SET_BASE;
        TestStatSetsCurrentAndNormal(
            "Base Set Mod1 INV Current (should be back to Base)", actualSet, expectedSet);

        actualSet = STAT_SET_BASE;
        for (int i(0); i < 10; ++i)
        {
            actualSet.ModifyCurrentAndActual(STAT_SET_MOD1);
        }
        const stats::StatSet STAT_SET_BASE_MOD1_X10(12, 2, 32, 0, 52, 0);
        expectedSet = STAT_SET_BASE;
        expectedSet.ResetCurrent(STAT_SET_BASE_MOD1_X10);
        TestStatSetsCurrentAndNormal("Base Set Mod1 x10 Current", actualSet, expectedSet);

        for (int i(0); i < 10; ++i)
        {
            actualSet.ModifyCurrentAndActual(STAT_SET_MOD1.CreateInvertCopy());
        }
        expectedSet = STAT_SET_BASE;
        TestStatSetsCurrentAndNormal("Base Set Mod1 INV x10 Current", actualSet, expectedSet);

        const stats::StatSet STAT_SET_CHAR_BASE{ 1000, 1000, 1000, 1000, 1000, 1000 };
        auto playerSPtr( std::make_shared<player::Character>(
            "StatsTestingCreatureName",
            creature::sex::Female,
            creature::BodyType::Make_Wolfen(),
            creature::Race(creature::race::Wolfen),
            creature::Role(creature::role::Wolfen),
            STAT_SET_CHAR_BASE) );

        playerSPtr->Stats() = STAT_SET_CHAR_BASE;
        expectedSet = STAT_SET_CHAR_BASE;
        TestStatSetsCurrentAndNormal("Creature Base Set", playerSPtr->Stats(), expectedSet);

        playerSPtr->ConditionAdd(creature::Conditions::Panic);
        expectedSet = STAT_SET_CHAR_BASE;
        const stats::StatSet STAT_SET_CHAR_FRIGHTENED(1000, 500, 500, 1000, 750, 500);
        expectedSet.ResetCurrent(STAT_SET_CHAR_FRIGHTENED);

        TestStatSetsCurrentAndNormal("Creature Base Set Panicked",
                                     playerSPtr->Stats(),
                                     expectedSet);

        playerSPtr->ConditionAdd(creature::Conditions::Dazed);
        expectedSet = STAT_SET_CHAR_BASE;
        const stats::StatSet STAT_SET_CHAR_FRIGHTENED_AND_DAZED(500, 166, 500, 1000, 250, 250);
        expectedSet.ResetCurrent(STAT_SET_CHAR_FRIGHTENED_AND_DAZED);

        TestStatSetsCurrentAndNormal("Creature Base Set Panicked and Dazed",
                                     playerSPtr->Stats(),
                                     expectedSet);

        playerSPtr->ConditionAdd(creature::Conditions::Dead);
        expectedSet = STAT_SET_CHAR_BASE;
        expectedSet.ResetCurrent(STAT_SET_ZEROS);

        TestStatSetsCurrentAndNormal("Creature Base Set Panicked and Dazed and Dead",
                                     playerSPtr->Stats(),
                                     expectedSet);

        playerSPtr->ConditionRemove(creature::Conditions::Panic);
        expectedSet = STAT_SET_CHAR_BASE;
        expectedSet.ResetCurrent(STAT_SET_ZEROS);

        TestStatSetsCurrentAndNormal("Creature Base Set Dazed and Dead",
                                     playerSPtr->Stats(),
                                     expectedSet);

        playerSPtr->ConditionRemove(creature::Conditions::Dead);
        expectedSet = STAT_SET_CHAR_BASE;
        const stats::StatSet STAT_SET_CHAR_DAZED(500, 333, 1000, 1000, 333, 500);
        expectedSet.ResetCurrent(STAT_SET_CHAR_DAZED);

        TestStatSetsCurrentAndNormal("Creature Base Set Dazed",
                                     playerSPtr->Stats(),
                                     expectedSet);

        playerSPtr->ConditionRemove(creature::Conditions::Dazed);
        expectedSet = STAT_SET_CHAR_BASE;

        TestStatSetsCurrentAndNormal("Creature Base Set (back to char base)",
                                     playerSPtr->Stats(),
                                     expectedSet);

        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::PerformStatsTests()  All Tests PASSED.");
            */
    }


    void TestingStage::TestStatSetsCurrentAndNormal(const std::string &,
                                                    const stats::StatSet &,
                                                    const stats::StatSet &)
    {
        /*
        static auto statTestCounter{ 0 };

        auto isMismatchCurrent{ false };
        auto isMismatchNormal{ false };
        for (int i(0); i < stats::Traits::StatCount; ++i)
        {
            auto const NEXT_ENUM{ static_cast<stats::Traits::Enum>(i) };

            if (ACTUAL.GetCopy(NEXT_ENUM).Current() != EXPECTED.GetCopy(NEXT_ENUM).Current())
            {
                isMismatchCurrent = true;
            }

            if (ACTUAL.GetCopy(NEXT_ENUM).Normal() != EXPECTED.GetCopy(NEXT_ENUM).Normal())
            {
                isMismatchNormal = true;
            }
        }

        std::ostringstream ss;
        ss << "Stats Test #" << ++statTestCounter << ":\t \"" << TEST_NAME << "\"  ";

        if (isMismatchCurrent || isMismatchNormal)
        {
            ss << "FAIL  ";

            if (isMismatchCurrent)
            {
                ss << "(Currrent Value Incorrect)";
            }

            if (isMismatchNormal)
            {
                ss << "(Normal Value Incorrect)";
            }

            ss << "\nExpected:" << EXPECTED.ToStringTesting(true)
                << "\nActual:" << ACTUAL.ToStringTesting(true);

            M_HP_LOG(ss.str());
            throw std::runtime_error(ss.str());
        }
        else
        {
            ss << "PASS";
            game::LoopManager::Instance()->TestingStrAppend(ss.str());
        }
        */
    }


    bool TestingStage::TestImageSet()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "stage::TestingStage::TestImageSet() Starting Tests...");
        }

        static std::vector<std::string> imagePathKeyVec =
            {
                "media-images-gui-elements",
                "media-images-title-intro",
                "media-images-title-blacksymbol",
                "media-images-backgrounds-tile-darkknot",
                "media-images-backgrounds-tile-runes",
                "media-images-backgrounds-tile-wood",
                "media-images-backgrounds-tile-darkpaper",
                "media-images-backgrounds-paper-2",
                "media-images-logos-sfml",
                "media-images-logos-tiled",
                "media-images-logos-terrain",
                "media-images-logos-sound",
                "media-images-logos-avalontrees",
                "media-images-logos-renderedtrees",
                "media-images-logos-manaworldtrees",
                "media-images-logos-darkwoodpaper",
                "media-images-logos-openfontlicense",
                "media-images-gui-accents-symbol1",
                "media-images-gui-accents-symbol2",
                "media-images-gui-accents-symbol3",
                "media-images-gui-accents-ouroboros",
                "media-images-campfire",
                "media-images-combat-dart",
                "media-images-combat-arrow1",
                "media-images-combat-arrow2",
                "media-images-combat-arrow3",
                "media-images-combat-arrow4",
                "media-images-combat-stone1",
                "media-images-combat-stone2",
                "media-images-combat-stone3",
                "media-images-combat-stone4",
                "media-images-combat-crossbones",
                "media-images-combat-crossswords",
                "media-images-combat-run",
                "media-images-misc-spark1",
                "media-images-misc-spark2",
                "media-images-misc-spark3",
                "media-images-misc-cloud1",
                "media-images-misc-cloud2",
                "media-images-misc-cloud3",
                "media-images-misc-note1",
                "media-images-misc-note2",
                "media-images-misc-note3",
                "media-images-misc-note4",
                "media-images-misc-note5",
                "media-images-misc-note6",
                "media-images-misc-x",
                "media-images-misc-error",
                "media-images-misc-money-bag",
                "media-images-misc-abc",
                "media-images-misc-weight",
                "media-images-chest-closed",
                "media-images-chest-open",
                "media-images-coins",
                "media-images-lockbox-closed",
                "media-images-lockbox-open",
                "media-images-bones-bat",
                "media-images-bones-beetle",
                "media-images-bones-bone-pile-1",
                "media-images-bones-bone-pile-2",
                "media-images-bones-bone-pile-3",
                "media-images-bones-cat",
                "media-images-bones-cave-crawler",
                "media-images-bones-griffin",
                "media-images-bones-harpy",
                "media-images-bones-skull-animal-1",
                "media-images-bones-skull-animal-2",
                "media-images-bones-skull-animal-3",
                "media-images-bones-skull-bog",
                "media-images-bones-skull-demon",
                "media-images-bones-skull-dragon-1",
                "media-images-bones-skull-dragon-2",
                "media-images-bones-skull-dragon-3",
                "media-images-bones-skull-dragon-4",
                "media-images-bones-skull-giant",
                "media-images-bones-skull-goblin",
                "media-images-bones-skull-humaniod-pile-1",
                "media-images-bones-skull-humaniod-pile-2",
                "media-images-bones-skull-humaniod-pile-3",
                "media-images-bones-skull-humaniod",
                "media-images-bones-skull-minotaur",
                "media-images-bones-skull-orc",
                "media-images-bones-skull-snake",
                "media-images-bones-three-headed-hound",
                "media-images-bones-wolfen",
                "media-images-trap",
                "media-images-character-shadow"
            };

        static std::size_t imageIndex{ 0 };
        if (imageIndex < imagePathKeyVec.size())
        {
            std::ostringstream ss;
            ss << "TestImageSet() \"" << imagePathKeyVec[imageIndex] << "\"";
            game::LoopManager::Instance()->TestingStrAppend(ss.str());

            sf::Texture texture;
            sfml_util::LoadTexture(texture,
                game::GameDataFile::Instance()->GetMediaPath(imagePathKeyVec[imageIndex]));

            TestingImageSet(texture);

            ++imageIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::TestImageSet() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::TestCharacterImageSet()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "stage::TestingStage::TestCharacterImageSet() Starting Tests...");
        }

        static auto imageIndex{ 0 };
        if (imageIndex < avatar::Avatar::Count)
        {
            auto const WHICH_AVATAR{ static_cast<avatar::Avatar::Enum>(imageIndex) };

            std::ostringstream ss;
            ss << "TestCharacterImageSet() \"" << avatar::Avatar::ToString(WHICH_AVATAR) << "\"";
            game::LoopManager::Instance()->TestingStrAppend(ss.str());

            sf::Texture texture;
            sfml_util::LoadTexture(texture, avatar::Avatar::ImagePath(WHICH_AVATAR));
            TestingImageSet(texture);

            ++imageIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::TestCharacterImageSet() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::TestMaps()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "stage::TestingStage::TestMaps() Starting Tests.  Please wait...");
        }

        static auto mapIndex{ 0 };
        if (mapIndex < map::Level::Count)
        {
            auto const WHICH_LEVEL{ static_cast<map::Level::Enum>(mapIndex) };

            std::ostringstream ss;
            ss << "TestMaps() Testing \"" << map::Level::ToString(WHICH_LEVEL) << "\" Map";
            game::LoopManager::Instance()->TestingStrAppend(ss.str());

            auto const ParseMap{
                [](
                    map::Map & map,
                    const map::Level::Enum LEVEL_ENUM,
                    std::vector<map::Level::Enum> & entryLevels,
                    std::vector<map::Level::Enum> & exitLevels,
                    const std::string & ERROR_MSG)
                {
                    try
                    {
                        map.Load(LEVEL_ENUM, map::Level::Count, true);
                    }
                    catch (const std::exception & E)
                    {
                        M_HP_LOG_FAT("TestingStage::TestMaps() threw an exception while \""
                            << ERROR_MSG << "\" for map \"" << map::Level::ToString(LEVEL_ENUM)
                            << "\"");

                        throw E;
                    }

                    map.EntryAndExitLevels(entryLevels, exitLevels);
                } };


            auto const MAP_POS_V{ sf::Vector2f(0.0f, 0.0f) };
            auto const MAP_SIZE_V{ sf::Vector2f(256.0f, 512.0f) };

            map::MapUPtr_t mapUPtr{ std::make_unique<map::Map>(MAP_POS_V, MAP_SIZE_V) };

            std::vector<map::Level::Enum> entryLevels;
            std::vector<map::Level::Enum> exitLevels;

            ParseMap(* mapUPtr, WHICH_LEVEL, entryLevels, exitLevels, "initial parse");

            for (auto const NEXT_ENTRY_LEVEL : entryLevels)
            {
                map::MapUPtr_t nextMapUPtr{ std::make_unique<map::Map>(MAP_POS_V, MAP_SIZE_V) };

                std::vector<map::Level::Enum> ignored;
                std::vector<map::Level::Enum> nextExitLevels;

                ParseMap(
                    * nextMapUPtr,
                    NEXT_ENTRY_LEVEL,
                    ignored,
                    nextExitLevels,
                    "secondary parse looking for exit");

                auto const FOUND_ITER{
                    std::find(
                        std::begin(nextExitLevels),
                        std::end(nextExitLevels),
                        WHICH_LEVEL) };

                M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER != std::end(nextExitLevels)),
                    "TestingStage::TestMaps() found a map \"" << map::Level::ToString(WHICH_LEVEL)
                    << "\" that had an entry level from map \""
                    << map::Level::ToString(NEXT_ENTRY_LEVEL)
                    << "\", but that map did not have an exit level to match.");
            }

            for (auto const NEXT_EXIT_LEVEL : exitLevels)
            {
                map::MapUPtr_t nextMapUPtr{ std::make_unique<map::Map>(MAP_POS_V, MAP_SIZE_V) };

                std::vector<map::Level::Enum> nextEntryLevels;
                std::vector<map::Level::Enum> ignored;

                ParseMap(
                    * nextMapUPtr,
                    NEXT_EXIT_LEVEL,
                    nextEntryLevels,
                    ignored,
                    "secondary parse looking for entry");

                auto const FOUND_ITER{
                    std::find(
                        std::begin(nextEntryLevels),
                        std::end(nextEntryLevels),
                        WHICH_LEVEL) };

                M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER != std::end(nextEntryLevels)),
                    "TestingStage::TestMaps() found a map \"" << map::Level::ToString(WHICH_LEVEL)
                    << "\" that had an exit level to map \""
                    << map::Level::ToString(NEXT_EXIT_LEVEL)
                    << "\", but that map did not have an entry level to match.");
            }

            ++mapIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::TestMaps() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::PerformGameDataFileTests()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "stage::TestingStage::PerformGameDataFileTests() Starting Tests...");
        }

        static std::vector<std::string> keyVec =
        {
            "heroespath-stats-reduce-ratio",
            "heroespath-stats-race-bonus-base-adj-ratio",
            "heroespath-stats-race-bonus-minor-adj-ratio",
            "heroespath-stats-role-bonus-base-adj-ratio",
            "heroespath-stats-role-bonus-minor-adj-ratio",

            "heroespath-item-secondary-material-armor-adj-ratio",

            "heroespath-fight-stats-luck-adj-ratio",
            "heroespath-fight-stats-amazing-ratio",
            "heroespath-fight-stats-base-high-val",
            "heroespath-fight-stats-value-floor",
            "heroespath-fight-stats-rank-bonus-ratio",
            "heroespath-fight-hit-critical-chance-ratio",
            "heroespath-fight-hit-power-chance-ratio",
            "heroespath-fight-damage-strength-bonus-ratio",
            "heroespath-fight-pixie-damage-floor",
            "heroespath-fight-pixie-damage-adj-ratio",
            "heroespath-fight-pixie-defend-speed-rank-bonus-ratio",
            "heroespath-fight-block-defend-speed-bonus-ratio",
            "heroespath-fight-archer-projectile-accuracy-bonus-ratio",
            "heroespath-fight-archer-projectile-rank-bonus-ratio",
            "heroespath-fight-chance-conditions-added-from-damage-ratio",
            "heroespath-fight-rank-damage-bonus-ratio",
            "heroespath-fight-chance-enemies-ignore-unconscious"
        };

        static std::size_t keyIndex{ 0 };
        if (keyIndex < keyVec.size())
        {
            std::ostringstream ss;
            ss << "Testing GameDataFile Key() \"" << keyVec[keyIndex] << "\"="
                << game::GameDataFile::Instance()->GetCopyFloat(keyVec[keyIndex]);

            game::LoopManager::Instance()->TestingStrAppend(ss.str());
            ++keyIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::PerformGameDataFileTests() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::TestAnimations()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "stage::TestingStage::TestAnimations() Starting Tests...");
        }

        const long ANIM_FRAME_SLEEP_MS{ 0 };

        static std::size_t animIndex{ 0 };

        if (animIndex < sfml_util::Animations::Count)
        {
            if (animUPtr_.get() == nullptr)
            {
                auto const ENUM{ static_cast<sfml_util::Animations::Enum>(animIndex) };

                std::ostringstream ss;
                ss << "TestAnimations() \"" << sfml_util::Animations::ToString(ENUM) << "\"";
                game::LoopManager::Instance()->TestingStrAppend(ss.str());

                animUPtr_ = sfml_util::AnimationFactory::Make(
                    static_cast<sfml_util::Animations::Enum>(animIndex),
                    sf::FloatRect(0.0f, 0.0f, 512.0f, 512.0f),
                    0.05f,
                    sf::Color::White,
                    ((ENUM == sfml_util::Animations::Smoke) ? sf::BlendAlpha : sf::BlendAdd));
            }

            if (animUPtr_->UpdateTime(0.02f))
            {
                animUPtr_.reset();
                ++animIndex;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(ANIM_FRAME_SLEEP_MS));

            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::TestAnimations() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::TestInventoryFactory()
    {
        static auto didPostInitial{ false };
        if (false == didPostInitial)
        {
            didPostInitial = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "stage::TestingStage::TestInventoryFactory() Starting Tests...");
        }

        static auto raceIndex{ 0 };
        static auto roleIndex{ 0 };

        if (raceIndex < static_cast<int>(creature::race::Count))
        {
            auto const RACE_ENUM{ static_cast<creature::race::Enum>(raceIndex) };
            auto const RACE_STR{ creature::race::ToString(RACE_ENUM) };
            auto const ROLE_VEC{ creature::race::Roles(RACE_ENUM) };

            if (roleIndex < static_cast<int>(ROLE_VEC.size()))
            {
                auto const ROLE_ENUM{ ROLE_VEC[static_cast<std::size_t>(roleIndex)] };
                auto const ROLE_STR{ creature::role::ToString(ROLE_ENUM) };

                const int RANK_BASE{ 50 };
                const int RANK_MAX{ [&]()
                    {
                        if (RACE_ENUM == creature::race::Dragon)
                        {
                            return RANK_BASE + game::GameDataFile::Instance()->GetCopyInt(
                                "heroespath-creature-dragon-class-rank-min-Elder");
                        }
                        else if (RACE_ENUM == creature::race::Wolfen)
                        {
                            return RANK_BASE + game::GameDataFile::Instance()->GetCopyInt(
                                "heroespath-creature-wolfen-class-rank-min-Elder");
                        }
                        else
                        {
                            return RANK_BASE + game::GameDataFile::Instance()->GetCopyInt(
                                "heroespath-rankclass-Master-rankmax");
                        }
                    }() };

                for(int rankIndex(1); rankIndex <= RANK_MAX; ++rankIndex)
                {
                    std::ostringstream ss;
                    ss << " InventoryFactory Testing rank=" << rankIndex << " with race="
                        << RACE_STR << " and role=" << ROLE_STR;

                    M_HP_LOG_DBG(ss.str());

                    const stats::StatSet STATS(
                        10_str,
                        10_acc,
                        10_cha,
                        10_lck,
                        10_spd,
                        10_int );

                    std::ostringstream nameSS;
                    nameSS << "Name_" << RACE_STR << "_" << ROLE_STR << "_" << rankIndex;

                    auto characterUPtr = std::make_unique<non_player::Character>(
                        nameSS.str(),
                        creature::sex::Male,
                        creature::BodyType::Make_FromRaceAndRole(RACE_ENUM,
                                                                 ROLE_ENUM),
                        RACE_ENUM,
                        ROLE_ENUM,
                        STATS,
                        10_health,
                        Rank_t(rankIndex),
                        Experience_t(rankIndex * 10000) );

                    non_player::ownership::InventoryFactory::Instance()->SetupCreatureInventory(
                        characterUPtr.get());
                }

                std::ostringstream ss;
                ss << "InventoryFactory Tested " << RANK_MAX
                    << " ranks with race=" << RACE_STR
                    << " role=" << ROLE_STR;

                game::LoopManager::Instance()->TestingStrAppend(ss.str());

                ++roleIndex;
                return false;
            }

            roleIndex = 0;
            ++raceIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "stage::TestingStage::TestInventoryFactory()  ALL TESTS PASSED.");

        return true;
    }


    bool TestingStage::DoesImageHaveOutline(const sf::Texture & TEXTURE) const
    {
        const unsigned OFFSET{ 10 };
        const unsigned RUN_LENGTH{ 20 };

        if ((TEXTURE.getSize().x < (OFFSET + RUN_LENGTH + 1)) ||
            (TEXTURE.getSize().x < (OFFSET + RUN_LENGTH + 1)))
        {
            //in this case, the image is too small to detect outlines
            return false;
        }

        auto const WIDTH{ TEXTURE.getSize().x };
        auto const HEIGHT{ TEXTURE.getSize().y };

        sf::Image image{ TEXTURE.copyToImage() };

        std::size_t outlineDetectedCount{ 0 };

        enum CornerSide
        {
            TopLeftTop,
            TopLeftLeft,
            TopRightTop,
            TopRightRight,
            BotLeftBot,
            BotLeftLeft,
            BotRightBot,
            BotRightRight,
            Count
        };

        for(int cs(0); cs < CornerSide::Count; ++cs)
        {
            auto const ENUM{ static_cast<CornerSide>(cs) };

            auto wereAllColorsEqual{ true };

            auto const FIRST_COLOR{
                [&]()
                {
                    switch (ENUM)
                    {
                        case TopLeftTop:   { return image.getPixel(OFFSET, 0); }
                        case TopLeftLeft:  { return image.getPixel(0, OFFSET); }
                        case TopRightTop:  { return image.getPixel(((WIDTH-1)-OFFSET)-RUN_LENGTH, 0); }
                        case TopRightRight:{ return image.getPixel((WIDTH-1), 0); }
                        case BotLeftBot:   { return image.getPixel(OFFSET, (HEIGHT-1)); }
                        case BotLeftLeft:  { return image.getPixel(0, ((HEIGHT-1)-OFFSET)-RUN_LENGTH); }
                        case BotRightBot:  { return image.getPixel(((WIDTH-1)-OFFSET)-RUN_LENGTH, (HEIGHT-1)); }
                        case BotRightRight:{ return image.getPixel((WIDTH-1), (HEIGHT-1)-OFFSET); }
                        case Count:
                        default:            { return image.getPixel(0, 0); }
                    }
                }() };

            for (unsigned rl(0); rl < RUN_LENGTH; ++rl)
            {
                auto const NEXT_COLOR{
                    [&]()
                    {
                        switch (ENUM)
                        {
                            case TopLeftTop:   { return image.getPixel(OFFSET+rl, 0); }
                            case TopLeftLeft:  { return image.getPixel(0, OFFSET+rl); }
                            case TopRightTop:  { return image.getPixel((((WIDTH-1)-OFFSET)-RUN_LENGTH)+rl, 0); }
                            case TopRightRight:{ return image.getPixel((WIDTH-1), 0+rl); }
                            case BotLeftBot:   { return image.getPixel(OFFSET+rl, (HEIGHT-1)); }
                            case BotLeftLeft:  { return image.getPixel(0, (((HEIGHT-1)-OFFSET)-RUN_LENGTH)+rl); }
                            case BotRightBot:  { return image.getPixel((((WIDTH-1)-OFFSET)-RUN_LENGTH)+rl, (HEIGHT-1)); }
                            case BotRightRight:{ return image.getPixel((WIDTH-1), (((HEIGHT-1)-OFFSET)-RUN_LENGTH)+rl); }
                            case Count:
                            default:            { return image.getPixel(0, 0); }
                        }
                    }()
                };

                if ((NEXT_COLOR != FIRST_COLOR) ||
                    (NEXT_COLOR == sf::Color::White) ||
                    (NEXT_COLOR == sf::Color::Black))
                {
                    wereAllColorsEqual = false;
                    break;
                }
            }

            if (wereAllColorsEqual)
            {
                ++outlineDetectedCount;
            }
        }

        return (outlineDetectedCount >= 4);
    }


    /*
     * There was a problem where many creature and item images had slightly off-black borders
     * so this code was used to programatically force a single-pixel black border.  This code
     * probably won't be needed again, but I'm keeping it here just in case.
     *
    void TestingStage::ReSaveWithBlackBorder(const std::string & IMAGES_DIR_KEY_STR) const
    {
        namespace bfs = boost::filesystem;
        
        auto const IMAGES_PATH_STR{
            game::GameDataFile::Instance()->GetMediaPath(IMAGES_DIR_KEY_STR) };

        auto const DIR_PATH{ bfs::system_complete(bfs::path(IMAGES_PATH_STR).normalize()) };
        
        auto const DIR_PATH_STR{ DIR_PATH.string() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::exists(DIR_PATH),
            "TestingStage::ReSaveWithBlackBorder()(\"" << DIR_PATH_STR
                << "\") failed because that path does not exist.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            bfs::is_directory(DIR_PATH),
            "TestingStage::ReSaveWithBlackBorder()(\"" << DIR_PATH_STR
                << "\") failed because that is not a directory.");

        bfs::directory_iterator endIter;
        for (bfs::directory_iterator dirIter(DIR_PATH); endIter != dirIter; ++dirIter)
        {
            auto const FILE_PATH_STR{ dirIter->path().string() };

            if (bfs::is_regular_file(dirIter->status()) == false)
            {
                continue;
            }

            if (boost::algorithm::iends_with(FILE_PATH_STR, ".png") == false)
            {
                continue;
            }
        
            sf::Texture texture;
            sfml_util::LoadTexture(texture, FILE_PATH_STR, false);
            sf::Image image{ texture.copyToImage() };
            
            auto const WIDTH{ texture.getSize().x };
            auto const HEIGHT{ texture.getSize().y };

            for (unsigned i(0); i < WIDTH; ++i) image.setPixel(i, 0, sf::Color::Black);
            for (unsigned i(0); i < WIDTH; ++i) image.setPixel(i, HEIGHT-1, sf::Color::Black);
            for (unsigned i(0); i < HEIGHT; ++i)image.setPixel(0, i, sf::Color::Black);
            for (unsigned i(0); i < HEIGHT; ++i)image.setPixel(WIDTH-1, i, sf::Color::Black);

            image.saveToFile(FILE_PATH_STR);

            M_HP_LOG_DBG("\tRe-saved image with black border:  \"" << FILE_PATH_STR << "\"");
        }
        exit(1);
    }
    */

}
}
