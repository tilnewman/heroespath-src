//
// testing-stage.cpp
//
#include "testing-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/real-utils.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/condition-image-manager.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/combat-image-manager.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/loop-manager.hpp"
#include "heroespath/game-data-file.hpp"
#include "heroespath/creature/titles.hpp"
#include "heroespath/creature/conditions.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/player/character.hpp"

#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>


namespace heroespath
{
namespace stage
{

    const std::size_t TestingStage::TEXT_LINES_COUNT_MAX_(100);


    TestingStage::TestingStage()
    :
        Stage              ("Testing"),
        SCREEN_WIDTH_      (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_     (sfml_util::Display::Instance()->GetWinHeight()),
        textureSVec_       (),
        ouroborosSPtr_     (),
        testingBlurbsVec_  (),
        sleepMilliseconds_ (0)
    {}


    TestingStage::~TestingStage()
    {}


    void TestingStage::Setup()
    {
        ouroborosSPtr_.reset( new Ouroboros("TestingStage's") );
        EntityAdd(ouroborosSPtr_);
    }


    void TestingStage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        Stage::Draw(target, states);

        auto const IMAGE_POS_TOP{ 1.0f };
        {
            std::size_t imageDrawCount{ 0 };
            auto posLeft{ SCREEN_WIDTH_ };
            for (std::size_t i(textureSVec_.size() - 1); i >= 0; --i)
            {
                if (i >= textureSVec_.size())
                    break;

                auto const NEXT_TEXTURE_SPTR{ textureSVec_.at(i) };
                if (NEXT_TEXTURE_SPTR.get() == nullptr)
                    continue;

                sf::Sprite sprite;
                sprite.setTexture( * NEXT_TEXTURE_SPTR);

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
                target.draw(sprite, states);
                ++imageDrawCount;

                if (posLeft < 0.0f)
                    break;
            }


            while (textureSVec_.size() > imageDrawCount)
            {
                textureSVec_.erase(textureSVec_.begin());
            }
        }

        {
            //find out how tall the text lines will be
            sf::Text testText("M", * sfml_util::FontManager::Instance()->Font_Default1(), sfml_util::FontManager::Instance()->Size_Normal());
            auto const TEXT_HEIGHT{ testText.getGlobalBounds().height + 10.0f };

            auto DO_NOT_PASS_TOP{ IMAGE_POS_TOP + 256.0 + TEXT_HEIGHT };
            auto posTop{ SCREEN_HEIGHT_ - (TEXT_HEIGHT * 2.0f) };

            for (std::size_t i(testingBlurbsVec_.size() - 1); i >= 0; --i)
            {
                if (i >= testingBlurbsVec_.size())
                    break;

                std::ostringstream ss;
                ss << testingBlurbsVec_.at(i).first;

                if (testingBlurbsVec_.at(i).second > 0)
                    ss << " " << testingBlurbsVec_.at(i).second;

                sf::Text text(sf::String(ss.str()),
                              * sfml_util::FontManager::Instance()->Font_Default1(),
                              sfml_util::FontManager::Instance()->Size_Normal());

                text.setPosition(1.0f, posTop);

                target.draw(text, states);

                posTop -= TEXT_HEIGHT;
                if (posTop < DO_NOT_PASS_TOP)
                    break;
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
            testingBlurbsVec_.erase(testingBlurbsVec_.begin());
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
            TestingStrAppend(S);
    }


    void TestingStage::TestingImageSet(const sfml_util::TextureSPtr_t & TEXTURE_SPTR)
    {
        textureSVec_.push_back(TEXTURE_SPTR);
    }


    void TestingStage::PerformNextTest()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds_));

        if (LoopManager::Instance()->IsFading())
            return;

        static auto hasPromptStart{ false };
        if (false == hasPromptStart)
        {
            hasPromptStart = true;
            LoopManager::Instance()->TestingStrAppend("System Tests Starting...");
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

        static auto hasTestingCompleted_SoundManager{ false };
        if (false == hasTestingCompleted_SoundManager)
        {
            hasTestingCompleted_SoundManager = sfml_util::SoundManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_Title{ false };
        if (false == hasTestingCompleted_Title)
        {
            hasTestingCompleted_Title = creature::title::TitleWarehouse::Test();
            return;
        }

        static auto hasTestingCompleted_ItemImageManager{ false };
        if (false == hasTestingCompleted_ItemImageManager)
        {
            hasTestingCompleted_ItemImageManager = sfml_util::gui::ItemImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_CreatureImageManager { false };
        if (false == hasTestingCompleted_CreatureImageManager)
        {
            hasTestingCompleted_CreatureImageManager = sfml_util::gui::CreatureImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_SpellImageManager{ false };
        if (false == hasTestingCompleted_SpellImageManager)
        {
            hasTestingCompleted_SpellImageManager = sfml_util::gui::SpellImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_Condition{ false };
        if (false == hasTestingCompleted_Condition)
        {
            hasTestingCompleted_Condition = creature::condition::ConditionFactory::Test();
            return;
        }

        static auto hasTestingCompleted_ConditionImageManager{ false };
        if (false == hasTestingCompleted_ConditionImageManager)
        {
            hasTestingCompleted_ConditionImageManager = sfml_util::gui::ConditionImageManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_PopupManager{ false };
        if (false == hasTestingCompleted_PopupManager)
        {
            hasTestingCompleted_PopupManager = sfml_util::gui::PopupManager::Instance()->Test();
            return;
        }

        static auto hasTestingCompleted_CombatImageManager{ false };
        if (false == hasTestingCompleted_CombatImageManager)
        {
            hasTestingCompleted_CombatImageManager = sfml_util::gui::CombatImageManager::Instance()->Test();
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
        LoopManager::Instance()->TestingStrAppend("heroespath::stage::TestingStage::PerformStatsTests() Starting Tests...");

        const stats::StatSet STAT_SET_ZEROS(0, 0, 0, 0, 0, 0);
        stats::StatSet actualSet;
        TestStatSetsCurrentAndNormal("Empty Set Initial Values", actualSet, STAT_SET_ZEROS);

        const stats::StatSet STAT_SET_MOD1(0, -1, 2, -3, 4, -5);
        actualSet = STAT_SET_MOD1;
        TestStatSetsCurrentAndNormal("Mod1 Set Identity", actualSet, STAT_SET_MOD1);

        actualSet = STAT_SET_MOD1;
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
        TestStatSetsCurrentAndNormal("Base Set Mod1 INV Current (should be back to Base)", actualSet, expectedSet);
        
        actualSet = STAT_SET_BASE;
        for (int i(0); i < 10; ++i)
            actualSet.ModifyCurrent(STAT_SET_MOD1);
        const stats::StatSet STAT_SET_BASE_MOD1_X10(12, 2, 32, 0, 52, 0);
        expectedSet = STAT_SET_BASE;
        expectedSet.ResetCurrent(STAT_SET_BASE_MOD1_X10);
        TestStatSetsCurrentAndNormal("Base Set Mod1 x10 Current", actualSet, expectedSet);

        for (int i(0); i < 10; ++i)
            actualSet.ModifyCurrent(STAT_SET_MOD1.CreateInvertCopy());
        expectedSet = STAT_SET_BASE;
        TestStatSetsCurrentAndNormal("Base Set Mod1 INV x10 Current", actualSet, expectedSet);

        auto playerSPtr( std::make_shared<player::Character>("StatsTestingCreatureName",
                                                             creature::sex::Female,
                                                             creature::BodyType::Make_Wolfen(),
                                                             creature::race::Wolfen,
                                                             creature::role::Wolfen,
                                                             STAT_SET_BASE) );

        playerSPtr->Stats() = STAT_SET_BASE;
        playerSPtr->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::Dead));
        expectedSet = STAT_SET_BASE;
        expectedSet.ResetCurrent(STAT_SET_ZEROS);
        TestStatSetsCurrentAndNormal("Creature Base Set Dead", playerSPtr->Stats(), expectedSet);

        playerSPtr->ConditionAdd(creature::condition::ConditionFactory::Make(creature::condition::Dazed));
        TestStatSetsCurrentAndNormal("Creature Base Set Dead AND Dazed (still current is all zeros)", playerSPtr->Stats(), expectedSet);

        playerSPtr->ConditionRemove(creature::condition::Dead);
        expectedSet = STAT_SET_BASE;
        const stats::StatSet STAT_SET_BASE_DAZED(12, 2, 12, 12, 2, 2);
        expectedSet.ResetCurrent(STAT_SET_BASE_DAZED);
        TestStatSetsCurrentAndNormal("Creature Base Set Dazed", playerSPtr->Stats(), expectedSet);

        playerSPtr->ConditionRemove(creature::condition::Dazed);
        expectedSet = STAT_SET_BASE;
        TestStatSetsCurrentAndNormal("Creature Base Set No Conditions (should be back to Base)", playerSPtr->Stats(), expectedSet);

        LoopManager::Instance()->TestingStrAppend("heroespath::stage::TestingStage::PerformStatsTests()  All Tests PASSED.");
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
                isMismatchCurrent = true;
            
            if (ACTUAL.GetCopy(NEXT_ENUM).Normal() != EXPECTED.GetCopy(NEXT_ENUM).Normal())
                isMismatchNormal = true;
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

            ss << "\nExpected:" << EXPECTED.ToStringTesting(true) << "\nActual:" << ACTUAL.ToStringTesting(true);
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
            LoopManager::Instance()->TestingStrAppend("heroespath::stage::TestingStage::TestImageSet() ALL Tests Passed.");
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
            "media-images-combat-stone4"
        };

        static std::size_t imageIndex{ 0 };
        if (imageIndex < imagePathKeyVec.size())
        {
            std::ostringstream ss;
            ss << "TestImageSet() \"" << imagePathKeyVec[imageIndex] << "\"";
            LoopManager::Instance()->TestingStrAppend(ss.str());

            sfml_util::TextureSPtr_t textureSPtr;
            sfml_util::LoadImageOrTextureSPtr(textureSPtr, GameDataFile::Instance()->GetMediaPath(imagePathKeyVec[imageIndex]));
            TestingImageSet(textureSPtr);

            ++imageIndex;
            return false;
        }

        LoopManager::Instance()->TestingStrAppend("heroespath::stage::TestingStage::TestImageSet() ALL Tests Passed.");
        return true;
    }

}
}
