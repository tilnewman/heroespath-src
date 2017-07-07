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
#include "misc/real.hpp"
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
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"

#include "game/log-macros.hpp"
#include "game/loop-manager.hpp"
#include "game/game-data-file.hpp"
#include "game/creature/title-warehouse.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/condition-warehouse.hpp"
#include "game/creature/creature.hpp"
#include "game/player/character.hpp"
#include "game/non-player/character.hpp"
#include "game/spell/spell-warehouse.hpp"
#include "game/song/song-warehouse.hpp"
#include "game/non-player/inventory-factory.hpp"

#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>


namespace game
{
namespace stage
{

    const std::size_t                       TestingStage::TEXT_LINES_COUNT_MAX_{ 100 };
    sfml_util::MultiTextureAnimationSPtr_t  TestingStage::multiTextureAnimSPtr_;
    sfml_util::SingleTextureAnimationSPtr_t TestingStage::singleTextureAnimSPtr_;


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
        animBGSprite_      ()
    {}


    TestingStage::~TestingStage()
    {
        ClearAllEntities();
    }


    void TestingStage::Setup()
    {
        ouroborosUPtr_ = std::make_unique<Ouroboros>("TestingStage's");
        EntityAdd(ouroborosUPtr_.get());

        sfml_util::LoadImageOrTexture(animBGTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-tile-wood"));

        animBGTexture_.setSmooth(true);
        animBGSprite_.setTexture(animBGTexture_);
        animBGSprite_.setPosition(0.0f, 0.0f);
    }


    void TestingStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        Stage::Draw(target, STATES);

        if (multiTextureAnimSPtr_.get() != nullptr)
        {
            target.draw(animBGSprite_, STATES);
            multiTextureAnimSPtr_->draw(target, STATES);
        }

        if (singleTextureAnimSPtr_.get() != nullptr)
        {
            target.draw(animBGSprite_, STATES);
            singleTextureAnimSPtr_->draw(target, STATES);
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

            auto const TEXT_HEIGHT{ testText.getGlobalBounds().height + 10.0f };

            auto DO_NOT_PASS_TOP{ IMAGE_POS_TOP + 256.0 + TEXT_HEIGHT };
            auto posTop{ SCREEN_HEIGHT_ - (TEXT_HEIGHT * 2.0f) };

            StrSizePairVec_t::reverse_iterator rItr(testingBlurbsVec_.rbegin());
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
        if (KE.code == sf::Keyboard::Escape)
        {
            sleepMilliseconds_ = 0;
        }
        else
        {
            auto const SPEED1{ 0 };
            auto const SPEED2{ 500 };
            auto const SPEED3{ 1'000 };
            auto const SPEED4{ 3'000 };
            if (sleepMilliseconds_ == SPEED1)
            {
                sleepMilliseconds_ = SPEED2;
                LoopManager::Instance()->TestingStrAppend("System Tests Speed = 2");
            }
            else if (sleepMilliseconds_ == SPEED2)
            {
                sleepMilliseconds_ = SPEED3;
                LoopManager::Instance()->TestingStrAppend("System Tests Speed = 3");
            }
            else if (sleepMilliseconds_ == SPEED3)
            {
                sleepMilliseconds_ = SPEED4;
                LoopManager::Instance()->TestingStrAppend("System Tests Speed = 4");
            }
            else
            {
                sleepMilliseconds_ = SPEED1;
                LoopManager::Instance()->TestingStrAppend("System Tests Speed = 1");
            }
        }

        return true;
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


    void TestingStage::TestingImageSet(const sf::Texture & TEXTURE)
    {
        textureList_.push_back(TEXTURE);
    }


    void TestingStage::PerformNextTest()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds_));

        if (LoopManager::Instance()->IsFading())
        {
            return;
        }

        static auto hasPromptStart{ false };
        if (false == hasPromptStart)
        {
            hasPromptStart = true;
            LoopManager::Instance()->TestingStrAppend("System Tests Starting...");
        }

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

        static auto hasTestingCompleted_Animations{ false };
        if (false == hasTestingCompleted_Animations)
        {
            hasTestingCompleted_Animations = TestAnimations();
            return;
        }

        static auto hasTestingCompleted_ImageSet{ false };
        if (false == hasTestingCompleted_ImageSet)
        {
            hasTestingCompleted_ImageSet = TestImageSet();
            return;
        }

        static auto hasTestingCompleted_Spells{ false };
        if (false == hasTestingCompleted_Spells)
        {
            hasTestingCompleted_Spells = game::spell::Warehouse::Test();
            return;
        }

        static auto hasTestingCompleted_SpellsImageManager{ false };
        if (false == hasTestingCompleted_SpellsImageManager)
        {
            hasTestingCompleted_SpellsImageManager =
                sfml_util::gui::SpellImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_Songs{ false };
        if (false == hasTestingCompleted_Songs)
        {
            hasTestingCompleted_Songs = game::song::Warehouse::Test();
            return;
        }

        static auto hasTestingCompleted_SongsImageManager{ false };
        if (false == hasTestingCompleted_SongsImageManager)
        {
            hasTestingCompleted_SongsImageManager =
                sfml_util::gui::SongImageManager::Instance()->Test();
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
                sfml_util::gui::ConditionImageManager::Instance()->Test();
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
                sfml_util::gui::TitleImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_SongImageManager{ false };
        if (false == hasTestingCompleted_SongImageManager)
        {
            hasTestingCompleted_SongImageManager =
                sfml_util::gui::SpellImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_SpellImageManager{ false };
        if (false == hasTestingCompleted_SpellImageManager)
        {
            hasTestingCompleted_SpellImageManager =
                sfml_util::gui::SpellImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_PopupManager{ false };
        if (false == hasTestingCompleted_PopupManager)
        {
            hasTestingCompleted_PopupManager =
                sfml_util::gui::PopupManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_CombatImageManager{ false };
        if (false == hasTestingCompleted_CombatImageManager)
        {
            hasTestingCompleted_CombatImageManager =
                sfml_util::gui::CombatImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_ItemImageManager{ false };
        if (false == hasTestingCompleted_ItemImageManager)
        {
            hasTestingCompleted_ItemImageManager =
                sfml_util::gui::ItemImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_SoundManager{ false };
        if (false == hasTestingCompleted_SoundManager)
        {
            hasTestingCompleted_SoundManager =
                sfml_util::SoundManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_InventoryFactory{ false };
        if (false == hasTestingCompleted_InventoryFactory)
        {
            hasTestingCompleted_InventoryFactory = TestInventoryFactory();;
            return;
        }

        static auto hasTestingCompleted_CreatureImageManager{ false };
        if (false == hasTestingCompleted_CreatureImageManager)
        {
            hasTestingCompleted_CreatureImageManager =
                sfml_util::gui::CreatureImageManager::Instance()->Test();
            return;
        }

        static auto hasFinalPrompt{ false };
        if (false == hasFinalPrompt)
        {
            hasFinalPrompt = true;
            LoopManager::Instance()->TestingStrAppend("ALL SYSTEM TESTS PASSED");
        }
    }


    void TestingStage::PerformStatsTests()
    {
        LoopManager::Instance()->TestingStrAppend(
            "game::stage::TestingStage::PerformStatsTests() Starting Tests...");

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
        actualSet.ModifyCurrent(STAT_SET_MOD1);
        const stats::StatSet STAT_SET_BASE_MOD1{12, 11, 14, 9, 16, 7};
        expectedSet = STAT_SET_BASE;
        expectedSet.ResetCurrent(STAT_SET_BASE_MOD1);
        TestStatSetsCurrentAndNormal("Base Set Mod1 Current", actualSet, expectedSet);

        actualSet.ModifyCurrent(STAT_SET_MOD1.CreateInvertCopy());
        expectedSet = STAT_SET_BASE;
        TestStatSetsCurrentAndNormal(
            "Base Set Mod1 INV Current (should be back to Base)", actualSet, expectedSet);

        actualSet = STAT_SET_BASE;
        for (int i(0); i < 10; ++i)
        {
            actualSet.ModifyCurrent(STAT_SET_MOD1);
        }
        const stats::StatSet STAT_SET_BASE_MOD1_X10(12, 2, 32, 0, 52, 0);
        expectedSet = STAT_SET_BASE;
        expectedSet.ResetCurrent(STAT_SET_BASE_MOD1_X10);
        TestStatSetsCurrentAndNormal("Base Set Mod1 x10 Current", actualSet, expectedSet);

        for (int i(0); i < 10; ++i)
        {
            actualSet.ModifyCurrent(STAT_SET_MOD1.CreateInvertCopy());
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

        LoopManager::Instance()->TestingStrAppend(
            "game::stage::TestingStage::PerformStatsTests()  All Tests PASSED.");
    }


    void TestingStage::TestStatSetsCurrentAndNormal(const std::string &    TEST_NAME,
                                                    const stats::StatSet & ACTUAL,
                                                    const stats::StatSet & EXPECTED)
    {
        static auto statTestCounter{ 0 };

        auto isMismatchCurrent{ false };
        auto isMismatchNormal{ false };
        for (int i(0); i < stats::stat::Count; ++i)
        {
            auto const NEXT_ENUM{ static_cast<stats::stat::Enum>(i) };

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
            LoopManager::Instance()->TestingStrAppend(ss.str());
        }
    }


    bool TestingStage::TestImageSet()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend(
                "game::stage::TestingStage::TestImageSet() Starting Tests...");
        }

        static std::vector<std::string> imagePathKeyVec =
        {
            "media-images-gui-elements",
            "media-images-title-paper",
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
            "media-images-gui-accents-symbol1-inv",
            "media-images-gui-accents-symbol2",
            "media-images-gui-accents-symbol2-inv",
            "media-images-gui-accents-symbol3",
            "media-images-gui-accents-ouroboros-white",
            "media-images-gui-accents-ouroboros-black",
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
            "media-images-misc-spark",
            "media-images-misc-cloud1",
            "media-images-misc-cloud2",
            "media-images-misc-cloud3",
            "media-images-misc-note1",
            "media-images-misc-note2",
            "media-images-misc-note3",
            "media-images-misc-note4",
            "media-images-misc-note5",
            "media-images-misc-note6",
            "media-images-misc-x"
        };

        static std::size_t imageIndex{ 0 };
        if (imageIndex < imagePathKeyVec.size())
        {
            std::ostringstream ss;
            ss << "TestImageSet() \"" << imagePathKeyVec[imageIndex] << "\"";
            LoopManager::Instance()->TestingStrAppend(ss.str());

            sf::Texture texture;
            sfml_util::LoadImageOrTexture(texture,
                GameDataFile::Instance()->GetMediaPath(imagePathKeyVec[imageIndex]));

            TestingImageSet(texture);

            ++imageIndex;
            return false;
        }

        LoopManager::Instance()->TestingStrAppend(
            "game::stage::TestingStage::TestImageSet() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::PerformGameDataFileTests()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend(
                "game::stage::TestingStage::PerformGameDataFileTests() Starting Tests...");
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
            "heroespath-fight-rank-damage-bonus-ratio"
        };

        static std::size_t keyIndex{ 0 };
        if (keyIndex < keyVec.size())
        {
            std::ostringstream ss;
            ss << "Testing GameDataFile Key() \"" << keyVec[keyIndex] << "\"="
                << GameDataFile::Instance()->GetCopyFloat(keyVec[keyIndex]);

            LoopManager::Instance()->TestingStrAppend(ss.str());
            ++keyIndex;
            return false;
        }

        LoopManager::Instance()->TestingStrAppend(
            "game::stage::TestingStage::PerformGameDataFileTests() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::TestAnimations()
    {
        static auto hasInitialPrompt{ false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            LoopManager::Instance()->TestingStrAppend(
                "game::stage::TestingStage::TestAnimations() Starting Tests...");
        }

        static std::vector<std::string> multiTexturedAnimPathKeyVec =
        {
            "media-anim-images-dir-dualcharge",
            "media-anim-images-dir-explosion",
            "media-anim-images-dir-firesmall",
            "media-anim-images-dir-inferno",
            "media-anim-images-dir-lightningball",
            "media-anim-images-dir-lightningbolt",
            "media-anim-images-dir-orbcharge",
            "media-anim-images-dir-orbshimmer",
            "media-anim-images-dir-shimmer",
            "media-anim-images-dir-smoke",
            "media-anim-images-dir-spiderflare",
            "media-anim-images-dir-symbolreduce",
            "media-anim-images-dir-whiteburst",
        };

        const long ANIM_FRAME_SLEEP_MS{ 0 };

        static std::size_t multiTexturedAnimIndex{ 0 };
        if (multiTexturedAnimIndex < multiTexturedAnimPathKeyVec.size())
        {
            static auto isNewAnimation{ true };

            sf::Color color{ sf::Color::White };
            if (0 == multiTexturedAnimIndex)       color = sf::Color::Red;
            else if (6 == multiTexturedAnimIndex)  color = sf::Color::Green;
            else if (7 == multiTexturedAnimIndex)  color = sf::Color::Yellow;
            else if (8 == multiTexturedAnimIndex)  color = sf::Color::Cyan;
            else if (11 == multiTexturedAnimIndex) color = sf::Color::Magenta;
            else if (12 == multiTexturedAnimIndex) color = sf::Color::Cyan;

            if (TestMultiTextureAnimation(multiTexturedAnimPathKeyVec[multiTexturedAnimIndex],
                                          isNewAnimation,
                                          color) == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(ANIM_FRAME_SLEEP_MS));
                isNewAnimation = false;
                return false;
            }
            else
            {
                multiTextureAnimSPtr_.reset();
                isNewAnimation = true;
                ++multiTexturedAnimIndex;
                return false;
            }
        }

        static auto isNewSingleTextureAnimation{ true };

        static auto hasAnimationBeenTested_CandleFlame{ false };
        if (false == hasAnimationBeenTested_CandleFlame)
        {
            if (TestSingleTextureAnimation("media-anim-images-candleflame",
                                           isNewSingleTextureAnimation,
                                           128,
                                           128,
                                           sf::BlendAlpha,
                                           sf::Color::White) == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(ANIM_FRAME_SLEEP_MS));
                isNewSingleTextureAnimation = false;
                return false;
            }
            else
            {
                singleTextureAnimSPtr_.reset();
                isNewSingleTextureAnimation = true;
                hasAnimationBeenTested_CandleFlame = true;
            }
        }

        static auto hasAnimationBeenTested_ExplosionSmall{ false };
        if (false == hasAnimationBeenTested_ExplosionSmall)
        {
            if (TestSingleTextureAnimation("media-anim-images-explosion-small",
                                           isNewSingleTextureAnimation,
                                           128,
                                           128,
                                           sf::BlendAlpha,
                                           sf::Color::White) == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(ANIM_FRAME_SLEEP_MS));
                isNewSingleTextureAnimation = false;
                return false;
            }
            else
            {
                singleTextureAnimSPtr_.reset();
                isNewSingleTextureAnimation = true;
                hasAnimationBeenTested_ExplosionSmall = true;
            }
        }

        static auto hasAnimationBeenTested_FireTorch{ false };
        if (false == hasAnimationBeenTested_FireTorch)
        {
            if (TestSingleTextureAnimation("media-anim-image-firetorch",
                                           isNewSingleTextureAnimation,
                                           128,
                                           256,
                                           sf::BlendAlpha,
                                           sf::Color::White) == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(ANIM_FRAME_SLEEP_MS));
                isNewSingleTextureAnimation = false;
                return false;
            }
            else
            {
                singleTextureAnimSPtr_.reset();
                isNewSingleTextureAnimation = true;
                hasAnimationBeenTested_FireTorch = true;
            }
        }

        static auto hasAnimationBeenTested_FlashSparkle{ false };
        if (false == hasAnimationBeenTested_FlashSparkle)
        {
            if (TestSingleTextureAnimation("media-anim-image-flash-sparkle",
                                           isNewSingleTextureAnimation,
                                           128,
                                           128,
                                           sf::BlendAdd,
                                           sf::Color::Cyan) == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(ANIM_FRAME_SLEEP_MS));
                isNewSingleTextureAnimation = false;
                return false;
            }
            else
            {
                singleTextureAnimSPtr_.reset();
                isNewSingleTextureAnimation = true;
                hasAnimationBeenTested_FlashSparkle = true;
            }
        }

        LoopManager::Instance()->TestingStrAppend(
            "game::stage::TestingStage::TestAnimations() ALL Tests Passed.");

        return true;
    }


    bool TestingStage::TestMultiTextureAnimation(
        const std::string & MEDIA_PATH_KEY_STR,
        const bool          WILL_REBUILD_ANIMATION_OBJECT,
        const sf::Color &   COLOR)
    {
        if (WILL_REBUILD_ANIMATION_OBJECT)
        {
            std::ostringstream ss;
            ss << "TestMultiTextureAnimation() \"" << MEDIA_PATH_KEY_STR << "\"";
            LoopManager::Instance()->TestingStrAppend(ss.str());

            multiTextureAnimSPtr_ = std::make_shared<sfml_util::MultiTextureAnimation>(
                "TestMultiTextureAnimation_" + MEDIA_PATH_KEY_STR,
                game::GameDataFile::Instance()->GetMediaPath(MEDIA_PATH_KEY_STR),
                0.0f,
                0.0f,
                0.06f);

            multiTextureAnimSPtr_->ColorTransition(sf::Color::White, COLOR);
            multiTextureAnimSPtr_->SetTargetSize( sf::Vector2f(512.0f, 512.0f) );
        }

        return multiTextureAnimSPtr_->UpdateTime(0.02f);
    }


    bool TestingStage::TestSingleTextureAnimation(
        const std::string &   MEDIA_PATH_KEY_STR,
        const bool            WILL_REBUILD_ANIMATION_OBJECT,
        const unsigned int    FRAME_WIDTH,
        const unsigned int    FRAME_HEIGHT,
        const sf::BlendMode & BLEND_MODE,
        const sf::Color &     COLOR)
    {
        if (WILL_REBUILD_ANIMATION_OBJECT)
        {
            std::ostringstream ss;
            ss << "TestSingleTextureAnimation() \"" << MEDIA_PATH_KEY_STR << "\"";
            LoopManager::Instance()->TestingStrAppend(ss.str());

            singleTextureAnimSPtr_ = std::make_shared<sfml_util::SingleTextureAnimation>(
                "TestSingleTextureAnimation_" + MEDIA_PATH_KEY_STR,
                game::GameDataFile::Instance()->GetMediaPath(MEDIA_PATH_KEY_STR),
                0.0f,
                0.0f,
                FRAME_WIDTH,
                FRAME_HEIGHT,
                0.06f,
                0,
                BLEND_MODE);

            singleTextureAnimSPtr_->ColorTransition(sf::Color::White, COLOR);
            singleTextureAnimSPtr_->SetTargetSize(sf::Vector2f(512.0f, 512.0f) );
        }

        return singleTextureAnimSPtr_->UpdateTime(0.02f);
    }


    bool TestingStage::TestInventoryFactory()
    {
        static auto didPostInitial{ false };
        if (false == didPostInitial)
        {
            didPostInitial = true;
            game::LoopManager::Instance()->TestingStrAppend(
                "game::stage::TestingStage::TestInventoryFactory() Starting Tests...");
        }

        static auto raceIndex{ 0 };
        static auto roleIndex{ 0 };

        if (raceIndex < static_cast<int>(game::creature::race::Count))
        {
            static int totalTestIndex{ 0 };

            auto const RACE_ENUM{ static_cast<game::creature::race::Enum>(raceIndex) };
            auto const RACE_STR{ game::creature::race::ToString(RACE_ENUM) };

            if (roleIndex < static_cast<int>(game::creature::role::Count))
            {
                auto const ROLE_ENUM{ static_cast<game::creature::role::Enum>(roleIndex) };
                auto const ROLE_STR{ game::creature::role::ToString(ROLE_ENUM) };

                if ((RACE_ENUM == creature::race::Dragon) &&
                    (ROLE_ENUM != creature::role::Sylavin) &&
                    (ROLE_ENUM != creature::role::Firebrand))
                {
                    ++roleIndex;
                    ++totalTestIndex;
                    return false;
                }

                if ((RACE_ENUM == creature::race::Wolfen) &&
                    (ROLE_ENUM != creature::role::Wolfen))
                {
                    ++roleIndex;
                    ++totalTestIndex;
                    return false;
                }

                static int totalTestCount{ static_cast<int>(
                    static_cast<float>(game::creature::race::Count) *
                    static_cast<float>(game::creature::role::Count)  *
                    1.0f) };

                const std::size_t RANK_BASE{ 10 };
                const std::size_t RANK_MAX{ [&]()
                    {
                        if (RACE_ENUM == creature::race::Dragon)
                        {
                            return RANK_BASE + GameDataFile::Instance()->GetCopySizet(
                                "heroespath-creature-dragon-class-rank-min-Elder");
                        }
                        else if (RACE_ENUM == creature::race::Wolfen)
                        {
                            return RANK_BASE + GameDataFile::Instance()->GetCopySizet(
                                "heroespath-creature-wolfen-class-rank-min-Elder");
                        }
                        else
                        {
                            return RANK_BASE + GameDataFile::Instance()->GetCopySizet(
                                "heroespath-rankclass-Master-rankmax");
                        }
                    }() };

                for(std::size_t rankIndex(1); rankIndex <= RANK_MAX; ++rankIndex)
                {
                    std::ostringstream ss;
                    ss << " InventoryFactory (" << totalTestCount - totalTestIndex
                        << ") Testing rank=" << rankIndex << " with race="
                        << RACE_STR << " and role=" << ROLE_STR;

                    M_HP_LOG_DBG(ss.str());

                    const stats::StatSet STATS(
                        10,  //str
                        10,  //acc
                        10,  //cha
                        10,  //lck
                        10,  //spd
                        10 );//int

                    std::ostringstream nameSS;
                    nameSS << "Name_" << RACE_STR << "_" << ROLE_STR << "_" << rankIndex;

                    auto characterUPtr = std::make_unique<non_player::Character>(
                        nameSS.str(),
                        creature::sex::Male,
                        creature::BodyType::Make_FromRaceAndRole(RACE_ENUM,
                                                                 ROLE_ENUM),
                        creature::Race(RACE_ENUM),
                        creature::Role(ROLE_ENUM),
                        STATS,
                        10,
                        rankIndex,
                        static_cast<stats::Exp_t>(rankIndex * 10'000) );

                    non_player::ownership::InventoryFactory::Instance()->
                        PopulateCreatureInventory(characterUPtr.get());

                    characterUPtr->SetCurrentWeaponsToBest();
                }

                std::ostringstream ss;
                ss << " InventoryFactory (" << totalTestCount - totalTestIndex << ") Tested "
                    << RANK_MAX << " ranks with race=" << RACE_STR << " role=" << ROLE_STR;

                game::LoopManager::Instance()->TestingStrAppend(ss.str());

                ++roleIndex;
                ++totalTestIndex;
                return false;
            }

            roleIndex = 0;
            ++raceIndex;
            ++totalTestIndex;
            return false;
        }

        game::LoopManager::Instance()->TestingStrAppend(
            "game::stage::TestingStage::TestInventoryFactory()  ALL TESTS PASSED.");

        return true;
    }

}
}
