// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// testing-stage.cpp
//
#include "testing-stage.hpp"

#include "combat/strategy-enums.hpp"
#include "creature/condition-holder.hpp"
#include "creature/creature.hpp"
#include "creature/name-info.hpp"
#include "creature/nonplayer-inventory-factory.hpp"
#include "creature/title-holder.hpp"
#include "game/strong-types.hpp"
#include "gui/animation-factory.hpp"
#include "gui/creature-image-paths.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/gui-images.hpp"
#include "gui/image-loaders.hpp"
#include "gui/image-option-enum.hpp"
#include "gui/item-image-paths.hpp"
#include "gui/ouroboros.hpp"
#include "gui/sound-manager.hpp"
#include "interact/interaction-manager.hpp"
#include "item/armor-ratings.hpp"
#include "item/item-factory.hpp"
#include "item/item-profiles-reporter.hpp"
#include "map/map.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "sfutil/common.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/scale.hpp"
#include "song/song-holder.hpp"
#include "spell/spell-holder.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace heroespath
{
namespace stage
{
    ImageInspectPacket::ImageInspectPacket(const gui::CachedTextureOpt_t & CACHED_TEXTURE_OPT)
        : cached_texture_opt(CACHED_TEXTURE_OPT)
        , sprite()
        , text()
    {
        if (cached_texture_opt)
        {
            sprite.setTexture(cached_texture_opt.value().Get(), true);

            sfutil::Fit(
                sprite,
                TestingStage::IMAGE_INSPECT_DIMMENSION_,
                TestingStage::IMAGE_INSPECT_DIMMENSION_);

            text = gui::Text(
                misc::filesystem::Filename(cached_texture_opt.value().Path()),
                gui::GuiFont::Default,
                24);

            text.setFillColor(sf::Color::Red);
        }
    }

    void ImageInspectPacket::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (cached_texture_opt)
        {
            target.draw(sprite, states);

            target.draw(
                sfutil::MakeRectangleSolid(text.getGlobalBounds(), sf::Color::Black), states);

            target.draw(text, states);
        }
    }

    const float TestingStage::IMAGE_INSPECT_DIMMENSION_ { 256.0f };
    const std::size_t TestingStage::TEXT_LINES_COUNT_MAX_ { 100 };
    gui::AnimationUPtr_t TestingStage::animUPtr_;

    TestingStage::TestingStage()
        : StageBase(
              "Testing",
              {
                  gui::GuiFont::Default,
                  gui::GuiFont::DefaultBoldFlavor,
                  gui::GuiFont::System,
                  gui::GuiFont::Number,
                  gui::GuiFont::SystemCondensed,
                  gui::GuiFont::SignThinTallNarrow,
                  gui::GuiFont::SignBoldShortWide,
                  gui::GuiFont::Handwriting,
                  gui::GuiFont::DialogModern,
                  gui::GuiFont::DialogMedieval,
              })
        , textures_()
        , ouroborosUPtr_(std::make_unique<gui::Ouroboros>("TestingStage's"))
        , testingBlurbsVec_()
        , sleepMilliseconds_(0)
        , animBackgroundCachedTexture_("media-image-background-tile-wood")
        , animBackgroundSprite_(animBackgroundCachedTexture_.Get())
        , imageInspectPackets_()
        , willInspectImages_(false)
        , isInspectingImages_(false)
        , imageInspectIndex_(0)
        , waitingForKeyOrClickId_(0)
        , waitingForKeyOrClickMaxTimeSec_(2.0f)
        , waitingForKeyOrClickElapsedTimeSec_(0.0f)
        , isWaitingForKeyOrClickPaused_(false)
        , waitingForKeyOrClick_CachedTextures_()
        , waitingForKeyOrClick_ToDraw_Sprites_()
        , waitingForKeyOrClick_ToDraw_Texts_()
    {}

    TestingStage::~TestingStage() { StageBase::ClearAllEntities(); }

    void TestingStage::Setup()
    {
        EntityAdd(ouroborosUPtr_);
        animBackgroundSprite_.setPosition(0.0f, 0.0f);
    }

    void TestingStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (waitingForKeyOrClickId_ > 0)
        {
            for (const auto & SPRITE : waitingForKeyOrClick_ToDraw_Sprites_)
            {
                target.draw(SPRITE, states);
            }

            for (const auto & TEXT : waitingForKeyOrClick_ToDraw_Texts_)
            {
                target.draw(TEXT, states);
            }
        }
        else
        {
            StageBase::draw(target, states);

            if (isInspectingImages_)
            {
                DrawImageInspect(target, states);
            }
            else
            {
                DrawNormal(target, states);
            }
        }
    }

    void TestingStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

        if (!isWaitingForKeyOrClickPaused_ && (waitingForKeyOrClickId_ > 0))
        {
            waitingForKeyOrClickElapsedTimeSec_ += ELAPSED_TIME_SECONDS;
            if (waitingForKeyOrClickElapsedTimeSec_ > waitingForKeyOrClickMaxTimeSec_)
            {
                ResetWaitingForKeyOrClick();
            }
        }
    }

    bool TestingStage::KeyPress(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (isInspectingImages_)
        {
            const auto INSPECT_IMAGE_COUNT_HORIZ { static_cast<std::size_t>(
                StageRegion().width / IMAGE_INSPECT_DIMMENSION_) };

            const auto INSPECT_IMAGE_COUNT_VERT { static_cast<std::size_t>(
                StageRegion().height / IMAGE_INSPECT_DIMMENSION_) };

            const auto INSPECT_IMAGE_COUNT { INSPECT_IMAGE_COUNT_HORIZ * INSPECT_IMAGE_COUNT_VERT };

            if ((KEY_EVENT.code == sf::Keyboard::Right) || (KEY_EVENT.code == sf::Keyboard::Down))
            {
                imageInspectIndex_ += INSPECT_IMAGE_COUNT;
            }
            else if ((KEY_EVENT.code == sf::Keyboard::Left) || (KEY_EVENT.code == sf::Keyboard::Up))
            {
                if (imageInspectIndex_ < INSPECT_IMAGE_COUNT)
                {
                    imageInspectIndex_ = 0;
                }
                else
                {
                    imageInspectIndex_ -= INSPECT_IMAGE_COUNT;
                }
            }

            if (imageInspectIndex_ >= imageInspectPackets_.size())
            {
                TransitionTo(stage::Stage::Exit);
            }

            return true;
        }
        else
        {
            if (KEY_EVENT.code == sf::Keyboard::I)
            {
                willInspectImages_ = true;
            }
        }

        if (KEY_EVENT.code == sf::Keyboard::P)
        {
            isWaitingForKeyOrClickPaused_ = !isWaitingForKeyOrClickPaused_;
        }
        else
        {
            ResetWaitingForKeyOrClick();
        }

        waitingForKeyOrClickElapsedTimeSec_ = 0.0f;

        return false;
    }

    void TestingStage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (isInspectingImages_)
        {
            const auto IMAGE_COUNT { imageInspectPackets_.size() };
            for (std::size_t i(imageInspectIndex_); i < IMAGE_COUNT; ++i)
            {
                const auto & PACKET { imageInspectPackets_[i] };
                if (sfutil::Contains(PACKET.sprite, MOUSE_POS_V))
                {
                    const auto FILE_NAME { std::string(PACKET.text.getString()) };

                    M_HP_LOG_WRN("Image failed inspection: " << FILE_NAME);

                    auto image { PACKET.cached_texture_opt.value().Get().copyToImage() };
                    image.saveToFile(FILE_NAME);

                    break;
                }
            }
        }

        ResetWaitingForKeyOrClick();
    }

    void TestingStage::TestingStrAppend(const std::string & S)
    {
        M_HP_LOG(S);

        testingBlurbsVec_.emplace_back(std::make_pair(S, 0));

        if (testingBlurbsVec_.size() > TEXT_LINES_COUNT_MAX_)
        {
            testingBlurbsVec_.erase(testingBlurbsVec_.begin());
        }
    }

    void TestingStage::TestingStrIncrement(const std::string & S)
    {
        auto foundMatch { false };
        for (auto & nextStrCountPair : testingBlurbsVec_)
        {
            if (nextStrCountPair.first == S)
            {
                ++nextStrCountPair.second;
                foundMatch = true;
                M_HP_LOG(nextStrCountPair.first << " " << nextStrCountPair.second + 1);
            }
        }

        if (false == foundMatch)
        {
            TestingStrAppend(S);
        }
    }

    void TestingStage::TestingImageSet(const std::string & PATH_STR)
    {
        imageInspectPackets_.emplace_back(gui::CachedTexture(PathWrapper(PATH_STR)));
        textures_.emplace_back(gui::CachedTexture(PathWrapper(PATH_STR)));
    }

    void TestingStage::PerformNextTest()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds_));

        if (IsFading())
        {
            return;
        }

        static auto hasPromptStart { false };
        if (false == hasPromptStart)
        {
            hasPromptStart = true;
            TestingStrAppend("System Tests Starting...");
            return;
        }

        M_TESTING_STAGE_TEST(GameDataFile);

        M_TESTING_STAGE_TEST(Fonts);
        M_TESTING_STAGE_TEST(Maps);
        M_TESTING_STAGE_TEST_WITH_TYPE_AND_STAGECALL(ItemFactory, item::ItemFactory);
        M_TESTING_STAGE_TEST(ItemProfileReport);
        M_TESTING_STAGE_TEST(InventoryFactory);
        M_TESTING_STAGE_TEST(ArmorRatings);
        M_TESTING_STAGE_TEST(IndividualImages);
        M_TESTING_STAGE_TEST(DirectoryImages);
        M_TESTING_STAGE_TEST(CharacterImageSet);
        M_TESTING_STAGE_TEST(Spells);
        M_TESTING_STAGE_TEST(Songs);
        M_TESTING_STAGE_TEST(Conditions);
        M_TESTING_STAGE_TEST(Titles);
        M_TESTING_STAGE_TEST(PopupManager);

        M_TESTING_STAGE_TEST_WITH_STATIC_TYPE_AND_CALL(gui, ItemImagePaths);
        M_TESTING_STAGE_TEST_WITH_STATIC_TYPE_AND_CALL(gui, CreatureImagePaths);

        M_TESTING_STAGE_TEST(Animations);

        if (willInspectImages_)
        {
            isInspectingImages_ = true;
        }

        M_TESTING_STAGE_TEST(FontSizes);

        M_TESTING_STAGE_TEST(SoundManager);

        std::cout << "ALL TESTS PASSED" << std::endl;
        TransitionTo(stage::Stage::Exit);
    }

    bool TestingStage::PerformTest_Spells() { return spell::Holder::Test(misc::MakeNotNull(this)); }

    bool TestingStage::PerformTest_Songs() { return song::Holder::Test(misc::MakeNotNull(this)); }

    bool TestingStage::PerformTest_Conditions()
    {
        return creature::condition::Holder::Test(misc::MakeNotNull(this));
    }

    bool TestingStage::PerformTest_Titles()
    {
        return creature::title::Holder::Test(misc::MakeNotNull(this));
    }

    bool TestingStage::PerformTest_SoundManager()
    {
        return gui::SoundManager::Instance()->Test(misc::MakeNotNull(this));
    }

    bool TestingStage::PerformTest_PopupManager()
    {
        return popup::PopupManager::Instance()->Test(misc::MakeNotNull(this));
    }

    bool TestingStage::PerformTest_ItemProfileReport()
    {
        item::ItemProfilesReporter::LogReport();
        return true;
    }

    bool TestingStage::PerformTest_ArmorRatings()
    {
        item::ArmorRatings armorRatings;
        armorRatings.LogCommonArmorRatings();
        return true;
    }

    bool TestingStage::PerformTest_FontSizes()
    {
        /*
         * the NameInfo class changed and broke this logging function
         *
        creature::NameInfo nameInfo;

        for (int sizeIndex(0); sizeIndex <= 2; ++sizeIndex)
        {
            const auto FONT_SIZE { [sizeIndex]() {
                switch (sizeIndex)
                {
                    // clang-format off
                    case 0: { return gui::FontManager::Instance()->Size_Normal(); }
                    case 1: { return gui::FontManager::Instance()->Size_Largeish(); }
                    case 2: { return gui::FontManager::Instance()->Size_Large(); }
                    default: { return unsigned(0); }
                        // clang-format on
                }
            }() };

            const std::string FONT_SIZE_STR { [sizeIndex]() {
                switch (sizeIndex)
                {
                    // clang-format off
                    case 0: { return "Normal"; }
                    case 1: { return "Largeish"; }
                    case 2: { return "Large"; }
                    default: { return "ERROR"; }
                        // clang-format on
                }
            }() };

            std::vector<float> lengths;

            for (int fontIndex(0); fontIndex <= 3; ++fontIndex)
            {
                const auto FONT_ENUM { static_cast<gui::GuiFont::Enum>(fontIndex) };

                const auto LENGTH { nameInfo.Length(gui::TextInfo(
                    nameInfo.LargestName(),
                    FONT_ENUM,
                    FONT_SIZE)) };

                lengths.emplace_back(LENGTH);

                M_HP_LOG_DBG(
                    NAMEOF_ENUM(FONT_ENUM)
                    << "\t" << FONT_SIZE_STR << '\t' << LENGTH << '\t' << "(" << LENGTH << "/"
                    << gui::Display::Instance()->GetWinWidth()
                    << ")=" << (LENGTH / gui::Display::Instance()->GetWinWidth()));
            }

            misc::Vector::MinMaxAvgSum<float> mmas(lengths);

            M_HP_LOG_DBG(
                "[" << mmas.min << ", " << mmas.avg << ", " << mmas.max
                    << "]  std_dev=" << misc::Vector::StandardDeviation(lengths));
        }
        */
        return true;
    }

    void TestingStage::TestStatSetsCurrentAndNormal(
        const std::string &, const creature::StatSet &, const creature::StatSet &)
    {
        /*
        static auto statTestCounter{ 0 };

        auto isMismatchCurrent{ false };
        auto isMismatchNormal{ false };
        for (EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            const auto NEXT_ENUM{ static_cast<creature::Traits::Enum>(i) };

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
                ss << "(Current Value Incorrect)";
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
            TestingStrAppend(ss.str());
        }
        */
    }

    bool TestingStage::PerformTest_IndividualImages()
    {
        static auto hasIndividualStartPrompt { false };
        if (false == hasIndividualStartPrompt)
        {
            hasIndividualStartPrompt = true;
            TestingStrAppend(
                "stage::TestingStage::PerformTest_ImageSet() Starting Individual Image Tests...");
            return false;
        }

        static std::vector<std::string> imagePathKeyVec;

        if (imagePathKeyVec.empty())
        {
            imagePathKeyVec = misc::ConfigFile::Instance()->FindAllKeysWithPrefix("media-image-");

            imagePathKeyVec.erase(
                std::remove_if(
                    std::begin(imagePathKeyVec),
                    std::end(imagePathKeyVec),
                    [](const auto & KEY_STR) {
                        return (
                            boost::algorithm::ends_with(KEY_STR, "-dir")
                            || boost::algorithm::ends_with(KEY_STR, "-texture-rect"));
                    }),
                std::end(imagePathKeyVec));
        }

        static std::size_t imageIndex { 0 };
        if (imageIndex < imagePathKeyVec.size())
        {
            const auto IMAGE_PATH_STR { misc::ConfigFile::Instance()->GetMediaPath(
                imagePathKeyVec[imageIndex]) };

            std::string str;
            str.reserve(128);
            str += "PerformTest_IndividualImage: \"";
            str += imagePathKeyVec[imageIndex];
            str += "\", \"";
            str += IMAGE_PATH_STR;
            str += '\"';

            TestingStrAppend(str);

            TestingImageSet(IMAGE_PATH_STR);

            ++imageIndex;
            return false;
        }

        TestingStrAppend(
            "stage::TestingStage::PerformTest_ImageSet() All Individual Image Tests Passed...");

        return true;
    }

    bool TestingStage::PerformTest_DirectoryImages()
    {
        static auto hasDirectoryStartPrompt { false };
        if (false == hasDirectoryStartPrompt)
        {
            hasDirectoryStartPrompt = true;
            TestingStrAppend(
                "stage::TestingStage::PerformTest_ImageSet() Starting Directory Image Tests...");
            return false;
        }

        static std::vector<std::string> imageDirPathKeyVec {
            misc::ConfigFile::Instance()->FindAllKeysWithPostfix("-dir")
        };

        static std::vector<std::string> imagePathsVec;
        static std::size_t imageIndex { 0 };
        static std::size_t imageDirIndex { 0 };
        if (imageDirIndex < imageDirPathKeyVec.size())
        {
            if (imageIndex == 0)
            {
                const auto IMAGE_DIR_PATH_KEY { imageDirPathKeyVec[imageDirIndex] };

                const auto IMAGE_DIR_PATH { misc::ConfigFile::Instance()->GetMediaPath(
                    IMAGE_DIR_PATH_KEY) };

                TestingStrAppend("PerformTest_DirectoryImages path: \"" + IMAGE_DIR_PATH + "\"");

                imagePathsVec = misc::filesystem::FindFiles(
                    false,
                    IMAGE_DIR_PATH,
                    "",
                    ".png",
                    misc::filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_);
            }

            if (imageIndex < imagePathsVec.size())
            {
                const auto IMAGE_PATH_STR { imagePathsVec[imageIndex] };

                std::string str;
                str.reserve(128);
                ;
                str += "PerformTest_DirectoryImage: #";
                str += std::to_string(imageIndex);
                str += "\t\"";
                str += IMAGE_PATH_STR;
                str += '\"';
                TestingStrAppend(str);

                TestingImageSet(IMAGE_PATH_STR);
                ++imageIndex;
            }
            else
            {
                imageIndex = 0;
                ++imageDirIndex;
            }

            return false;
        }

        TestingStrAppend(
            "stage::TestingStage::PerformTest_ImageSet() All Directory Image Tests Passed...");

        return true;
    }

    bool TestingStage::PerformTest_CharacterImageSet()
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            TestingStrAppend(
                "stage::TestingStage::PerformTest_CharacterImageSet() Starting Tests...");

            return false;
        }

        static EnumUnderlying_t imageIndex { 0 };
        if (imageIndex < avatar::Avatar::Count)
        {
            const auto WHICH_AVATAR { static_cast<avatar::Avatar::Enum>(imageIndex) };

            TestingStrAppend(
                "PerformTest_CharacterImageSet() \"" + NAMEOF_ENUM_STR(WHICH_AVATAR) + "\"");

            const auto IMAGE_PATH_STR { avatar::Avatar::ImagePath(WHICH_AVATAR) };
            TestingImageSet(IMAGE_PATH_STR);

            ++imageIndex;
            return false;
        }

        TestingStrAppend("stage::TestingStage::PerformTest_CharacterImageSet() ALL Tests Passed.");
        return true;
    }

    bool TestingStage::PerformTest_Maps()
    {
        static interact::InteractionManager interactionManager;

        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            TestingStrAppend("stage::TestingStage::TestMaps() Starting Tests.  Please wait...");
            return false;
        }

        using MapLevelVec_t = std::vector<map::Level::Enum>;

        struct MapAndTransitions
        {
            explicit MapAndTransitions(
                const map::Level::Enum LEVEL = map::Level::Count,
                const MapLevelVec_t & ENTRY_LEVELS = MapLevelVec_t(),
                const MapLevelVec_t & EXIT_LEVELS = MapLevelVec_t())
                : level(LEVEL)
                , entry_levels(ENTRY_LEVELS)
                , exit_levels(EXIT_LEVELS)
            {}

            map::Level::Enum level;
            MapLevelVec_t entry_levels;
            MapLevelVec_t exit_levels;
        };

        static misc::VectorMap<map::Level::Enum, MapAndTransitions> transitions;

        const auto ParseMap { [](const map::Level::Enum LEVEL_ENUM, const std::string & ERROR_MSG) {
            map::MapUPtr_t mapUPtr { std::make_unique<map::Map>(
                sf::FloatRect(0.0f, 0.0f, 128.0f, 256.0f), interactionManager) };

            try
            {
                mapUPtr->Load(LEVEL_ENUM, map::Level::Count, true);
            }
            catch (const std::exception &)
            {
                M_HP_LOG_FAT(
                    "TestingStage::TestMaps() threw an exception while \""
                    << ERROR_MSG << "\" for map \"" << NAMEOF_ENUM(LEVEL_ENUM) << "\"");

                throw;
            }

            MapLevelVec_t entryLevels;
            MapLevelVec_t exitLevels;
            mapUPtr->EntryAndExitLevels(entryLevels, exitLevels);
            return MapAndTransitions(LEVEL_ENUM, entryLevels, exitLevels);
        } };

        static auto hasFirstPassLoad { false };
        if (false == hasFirstPassLoad)
        {
            static EnumUnderlying_t mapIndex { 0 };
            if (mapIndex < map::Level::Count)
            {
                const auto WHICH_LEVEL { static_cast<map::Level::Enum>(mapIndex) };

                TestingStrAppend(
                    "TestMaps() Testing \"" + NAMEOF_ENUM_STR(WHICH_LEVEL)
                    + "\" Map in First Pass Loading");

                transitions[WHICH_LEVEL] = ParseMap(WHICH_LEVEL, "initial parse");

                ++mapIndex;
            }
            else
            {
                hasFirstPassLoad = true;
            }

            return false;
        }

        static auto hasTestedTransitions { false };
        if (false == hasTestedTransitions)
        {
            static auto transitionIter { std::begin(transitions) };
            if (transitionIter == std::end(transitions))
            {
                hasTestedTransitions = true;
            }
            else
            {
                const auto CURRENT_LEVEL { transitionIter->first };

                TestingStrAppend(
                    "TestMaps() Testing \"" + NAMEOF_ENUM_STR(CURRENT_LEVEL)
                    + "\" Map's transitions");

                for (const auto ENTRY_LEVEL : transitionIter->second.entry_levels)
                {
                    const auto ENTRY_TRANSITIONS { transitions[ENTRY_LEVEL] };

                    const auto WAS_FOUND { std::find(
                                               std::begin(ENTRY_TRANSITIONS.exit_levels),
                                               std::end(ENTRY_TRANSITIONS.exit_levels),
                                               CURRENT_LEVEL)
                                           != std::end(ENTRY_TRANSITIONS.exit_levels) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (WAS_FOUND),
                        "TestingStage::TestMaps() found a map \""
                            << NAMEOF_ENUM(CURRENT_LEVEL)
                            << "\" that had an entry level from map \"" << NAMEOF_ENUM(ENTRY_LEVEL)
                            << "\", but that map did not have an exit level to match.");
                }

                for (const auto EXIT_LEVEL : transitionIter->second.exit_levels)
                {
                    const auto EXIT_TRANSITIONS { transitions[EXIT_LEVEL] };

                    const auto WAS_FOUND { std::find(
                                               std::begin(EXIT_TRANSITIONS.entry_levels),
                                               std::end(EXIT_TRANSITIONS.entry_levels),
                                               CURRENT_LEVEL)
                                           != std::end(EXIT_TRANSITIONS.entry_levels) };

                    M_HP_ASSERT_OR_LOG_AND_THROW(
                        (WAS_FOUND),
                        "TestingStage::TestMaps() found a map \""
                            << NAMEOF_ENUM(CURRENT_LEVEL) << "\" that had an exit level to map \""
                            << NAMEOF_ENUM(EXIT_LEVEL)
                            << "\", but that map did not have an entry level to match.");
                }

                ++transitionIter;
            }

            return false;
        }

        TestingStrAppend("stage::TestingStage::TestMaps() ALL Tests Passed.");

        return true;
    }

    bool TestingStage::PerformTest_GameDataFile()
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            TestingStrAppend("stage::TestingStage::PerformGameDataFileTests() Starting Tests...");
            return false;
        }

        static std::vector<std::string> keyVec
            = { "stats-reduce-ratio",
                "stats-race-bonus-base-adj-ratio",
                "stats-race-bonus-minor-adj-ratio",
                "stats-role-bonus-base-adj-ratio",
                "stats-role-bonus-minor-adj-ratio",
                "item-secondary-material-armor-adj-ratio",
                "fight-stats-luck-adj-ratio",
                "fight-stats-amazing-ratio",
                "fight-stats-base-high-val",
                "fight-stats-value-floor",
                "fight-stats-rank-bonus-ratio",
                "fight-hit-critical-chance-ratio",
                "fight-hit-power-chance-ratio",
                "fight-damage-strength-bonus-ratio",
                "fight-pixie-damage-floor",
                "fight-pixie-damage-adj-ratio",
                "fight-pixie-defend-speed-rank-bonus-ratio",
                "fight-block-defend-speed-bonus-ratio",
                "fight-archer-projectile-accuracy-bonus-ratio",
                "fight-archer-projectile-rank-bonus-ratio",
                "fight-chance-conditions-added-from-damage-ratio",
                "fight-rank-damage-bonus-ratio",
                "fight-chance-enemies-ignore-unconscious" };

        static std::size_t keyIndex { 0 };
        if (keyIndex < keyVec.size())
        {
            std::string str;
            str.reserve(128);

            str += "Testing GameDataFile Key() \"";
            str += keyVec[keyIndex];
            str += "\"=";

            str += std::to_string(
                misc::ConfigFile::Instance()->ValueOrDefault<float>(keyVec[keyIndex]));

            TestingStrAppend(str);

            ++keyIndex;
            return false;
        }

        TestingStrAppend("stage::TestingStage::PerformGameDataFileTests() ALL Tests Passed.");

        return true;
    }

    bool TestingStage::PerformTest_Animations()
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            TestingStrAppend("stage::TestingStage::PerformTest_Animations() Starting Tests...");
            return false;
        }

        const long ANIM_FRAME_SLEEP_MS { 1 };

        static std::size_t animIndex { 0 };

        if (animIndex < gui::Animations::Count)
        {
            if (!animUPtr_)
            {
                const auto ENUM { static_cast<gui::Animations::Enum>(animIndex) };

                TestingStrAppend("PerformTest_Animations() \"" + NAMEOF_ENUM_STR(ENUM) + "\"");

                animUPtr_ = gui::AnimationFactory::Make(
                    static_cast<gui::Animations::Enum>(animIndex),
                    sf::FloatRect(0.0f, 0.0f, 512.0f, 512.0f));
            }

            if (animUPtr_->UpdateTime(0.02f))
            {
                animUPtr_.reset();
                ++animIndex;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(ANIM_FRAME_SLEEP_MS));

            return false;
        }

        TestingStrAppend("stage::TestingStage::PerformTest_Animations() ALL Tests Passed.");

        return true;
    }

    bool TestingStage::PerformTest_InventoryFactory()
    {
        static auto didPostInitial { false };
        if (false == didPostInitial)
        {
            didPostInitial = true;
            TestingStrAppend(
                "stage::TestingStage::PerformTest_InventoryFactory() Starting Tests...");

            return false;
        }

        static creature::nonplayer::InventoryFactory inventoryFactory;

        static EnumUnderlying_t raceIndex { 0 };
        static EnumUnderlying_t roleIndex { 0 };

        if (raceIndex < static_cast<int>(creature::race::Count))
        {
            const auto RACE_ENUM { static_cast<creature::race::Enum>(raceIndex) };
            const auto RACE_STR { NAMEOF_ENUM(RACE_ENUM) };
            const auto ROLE_VEC { creature::race::Roles(RACE_ENUM) };

            if (roleIndex < static_cast<EnumUnderlying_t>(ROLE_VEC.size()))
            {
                const auto ROLE_ENUM { ROLE_VEC[static_cast<std::size_t>(roleIndex)] };
                const auto ROLE_STR { NAMEOF_ENUM(ROLE_ENUM) };

                const int RANK_BASE { 50 };
                const int RANK_MAX { [&]() {
                    if (RACE_ENUM == creature::race::Dragon)
                    {
                        return RANK_BASE
                            + misc::ConfigFile::Instance()->ValueOrDefault<int>(
                                  "creature-dragon-class-rank-min-Elder");
                    }
                    else if (RACE_ENUM == creature::race::Wolfen)
                    {
                        return RANK_BASE
                            + misc::ConfigFile::Instance()->ValueOrDefault<int>(
                                  "creature-wolfen-class-rank-min-Elder");
                    }
                    else
                    {
                        return RANK_BASE
                            + misc::ConfigFile::Instance()->ValueOrDefault<int>(
                                  "rankclass-Master-rankmax");
                    }
                }() };

                {
                    std::string str;
                    str.reserve(128);
                    str += "InventoryFactory Testing: race=";
                    str += RACE_STR;
                    str += " and role=";
                    str += ROLE_STR;
                    str += "...";
                    TestingStrAppend(str);
                }

                for (int rankIndex(1); rankIndex <= RANK_MAX; ++rankIndex)
                {
                    std::string str;
                    str.reserve(128);
                    str += "Name_";
                    str += RACE_STR;
                    str += '_';
                    str += ROLE_STR;
                    str += '_';
                    str += std::to_string(rankIndex);

                    const std::string NAME_STR(str);

                    creature::Creature character(
                        false,
                        NAME_STR,
                        ((misc::RandomBool()) ? creature::sex::Female : creature::sex::Male),
                        RACE_ENUM,
                        ROLE_ENUM,
                        creature::StatSet(10_str, 10_acc, 10_cha, 10_lck, 10_spd, 10_int),
                        "",
                        10_health,
                        Rank_t::Make(rankIndex),
                        Experience_t::Make(rankIndex * 10000));

                    inventoryFactory.SetupCreatureInventory(misc::MakeNotNull(&character));
                }

                ++roleIndex;
                return false;
            }

            roleIndex = 0;
            ++raceIndex;
            return false;
        }

        TestingStrAppend("stage::TestingStage::PerformTest_InventoryFactory()  ALL TESTS PASSED.");

        raceIndex = 0;
        roleIndex = 0;

        return true;
    }

    bool TestingStage::DoesImageHaveOutline(const sf::Texture & TEXTURE) const
    {
        const unsigned OFFSET { 10 };
        const unsigned RUN_LENGTH { 20 };

        if ((TEXTURE.getSize().x < (OFFSET + RUN_LENGTH + 1))
            || (TEXTURE.getSize().y < (OFFSET + RUN_LENGTH + 1)))
        {
            // in this case, the image is too small to detect outlines
            return false;
        }

        const auto WIDTH { TEXTURE.getSize().x };
        const auto HEIGHT { TEXTURE.getSize().y };

        sf::Image image { TEXTURE.copyToImage() };

        std::size_t outlineDetectedCount { 0 };

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

        for (int cs(0); cs < CornerSide::Count; ++cs)
        {
            const auto ENUM { static_cast<CornerSide>(cs) };

            auto wereAllColorsEqual { true };

            const auto FIRST_COLOR { [&]() {
                switch (ENUM)
                {
                    case TopLeftTop: { return image.getPixel(OFFSET, 0);
                    }
                    case TopLeftLeft: { return image.getPixel(0, OFFSET);
                    }
                    case TopRightTop:
                    {
                        return image.getPixel(((WIDTH - 1) - OFFSET) - RUN_LENGTH, 0);
                    }
                    case TopRightRight: { return image.getPixel((WIDTH - 1), 0);
                    }
                    case BotLeftBot: { return image.getPixel(OFFSET, (HEIGHT - 1));
                    }
                    case BotLeftLeft:
                    {
                        return image.getPixel(0, ((HEIGHT - 1) - OFFSET) - RUN_LENGTH);
                    }
                    case BotRightBot:
                    {
                        return image.getPixel(((WIDTH - 1) - OFFSET) - RUN_LENGTH, (HEIGHT - 1));
                    }
                    case BotRightRight: { return image.getPixel((WIDTH - 1), (HEIGHT - 1) - OFFSET);
                    }
                    case Count:
                    default: { return image.getPixel(0, 0);
                    }
                }
            }() };

            for (unsigned rl(0); rl < RUN_LENGTH; ++rl)
            {
                const auto NEXT_COLOR { [&]() {
                    switch (ENUM)
                    {
                        case TopLeftTop: { return image.getPixel(OFFSET + rl, 0);
                        }
                        case TopLeftLeft: { return image.getPixel(0, OFFSET + rl);
                        }
                        case TopRightTop:
                        {
                            return image.getPixel((((WIDTH - 1) - OFFSET) - RUN_LENGTH) + rl, 0);
                        }
                        case TopRightRight: { return image.getPixel((WIDTH - 1), 0 + rl);
                        }
                        case BotLeftBot: { return image.getPixel(OFFSET + rl, (HEIGHT - 1));
                        }
                        case BotLeftLeft:
                        {
                            return image.getPixel(0, (((HEIGHT - 1) - OFFSET) - RUN_LENGTH) + rl);
                        }
                        case BotRightBot:
                        {
                            return image.getPixel(
                                (((WIDTH - 1) - OFFSET) - RUN_LENGTH) + rl, (HEIGHT - 1));
                        }
                        case BotRightRight:
                        {
                            return image.getPixel(
                                (WIDTH - 1), (((HEIGHT - 1) - OFFSET) - RUN_LENGTH) + rl);
                        }
                        case Count:
                        default: { return image.getPixel(0, 0);
                        }
                    }
                }() };

                if ((NEXT_COLOR != FIRST_COLOR) || (NEXT_COLOR == sf::Color::White)
                    || (NEXT_COLOR == sf::Color::Black))
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

    bool TestingStage::PerformTest_Fonts()
    {
        static auto hasInitialPrompt { false };
        if (false == hasInitialPrompt)
        {
            hasInitialPrompt = true;
            TestingStrAppend("stage::TestingStage::PerformTest_Fonts() Starting Tests...");
            return false;
        }

        static EnumUnderlying_t fontIndex { 0 };

        if (fontIndex < gui::GuiFont::Count)
        {
            const auto FONT_ENUM { static_cast<gui::GuiFont::Enum>(fontIndex) };
            gui::Text text("asdf", FONT_ENUM, 30);
            ++fontIndex;
            return false;
        }
        else
        {
            TestingStrAppend("stage::TestingStage::PerformTest_Fonts() Finished.");
            return true;
        }
    }

    void TestingStage::DrawNormal(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (animUPtr_)
        {
            target.draw(animBackgroundSprite_, states);
            animUPtr_->draw(target, states);
        }

        const auto IMAGE_POS_TOP { 1.0f };
        {
            std::size_t imageDrawCount { 0 };
            auto posLeft { StageRegion().width };

            auto rItr(textures_.rbegin());
            for (; rItr != textures_.rend(); ++rItr)
            {
                sf::Sprite sprite;
                sprite.setTexture(rItr->Get());

                // reduce size if any dimension is greater than 256
                const auto MAX_DIMMENSION { 256.0f };
                auto newHorizScale { 1.0f };
                if (sprite.getGlobalBounds().width > MAX_DIMMENSION)
                {
                    newHorizScale = MAX_DIMMENSION / sprite.getGlobalBounds().width;
                    sprite.setScale(newHorizScale, newHorizScale);
                }
                if (sprite.getGlobalBounds().height > MAX_DIMMENSION)
                {
                    const auto NEW_VERT_SCALE { MAX_DIMMENSION / sprite.getGlobalBounds().height };
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
                {
                    break;
                }
            }

            while (textures_.size() > imageDrawCount)
            {
                textures_.erase(textures_.begin());
            }
        }

        {
            // find out how tall the text lines will be
            gui::Text testText(
                "M", gui::GuiFont::Default, gui::FontManager::Instance()->Size_Normal());

            // The extra +10 is added because testText's height is only an estimation.
            const auto TEXT_HEIGHT { testText.getGlobalBounds().height + 10.0f };

            // The + 256 is to make room for the images, so text is not drawn over them.
            auto DO_NOT_PASS_TOP { IMAGE_POS_TOP + 256.0f + TEXT_HEIGHT };

            // The extra * 2 is added because without it, the text at the bottom is cut off.
            auto posTop { StageRegion().height - (TEXT_HEIGHT * 2.0f) };

            StrSizePairVec_t::const_reverse_iterator rItr { testingBlurbsVec_.crbegin() };
            for (; rItr != testingBlurbsVec_.rend(); ++rItr)
            {
                std::string str;
                str.reserve(rItr->first.size() + rItr->second);

                str += (rItr->first);

                if (rItr->second > 0)
                {
                    str += ' ';
                    str += std::to_string(rItr->second);
                }

                gui::Text text(
                    str, gui::GuiFont::Default, gui::FontManager::Instance()->Size_Normal());

                text.setPosition(1.0f, posTop);

                target.draw(text, states);

                posTop -= TEXT_HEIGHT;

                if (posTop < DO_NOT_PASS_TOP)
                {
                    break;
                }
            }
        }
    }

    void TestingStage::DrawImageInspect(sf::RenderTarget & target, sf::RenderStates states) const
    {
        auto willLowerText { false };
        auto posX { 0.0f };
        auto posY { 0.0f };
        auto imageIndex { imageInspectIndex_ };

        while (posY < (StageRegion().height - IMAGE_INSPECT_DIMMENSION_))
        {
            while (posX < (StageRegion().width - IMAGE_INSPECT_DIMMENSION_))
            {
                if (imageIndex >= imageInspectPackets_.size())
                {
                    return;
                }

                auto & packet { imageInspectPackets_[imageIndex] };

                packet.sprite.setPosition(posX, posY);

                if (willLowerText)
                {
                    packet.text.setPosition(posX, posY + packet.text.getGlobalBounds().height);
                }
                else
                {
                    packet.text.setPosition(posX, posY);
                }

                willLowerText = !willLowerText;

                target.draw(packet, states);

                posX += IMAGE_INSPECT_DIMMENSION_;
                ++imageIndex;
            }

            posX = 0.0f;
            posY += IMAGE_INSPECT_DIMMENSION_;
        }
    }

    void TestingStage::ResetWaitingForKeyOrClick()
    {
        waitingForKeyOrClickId_ = 0;
        waitingForKeyOrClickElapsedTimeSec_ = 0.0f;
        isWaitingForKeyOrClickPaused_ = false;

        waitingForKeyOrClick_CachedTextures_.clear();
        waitingForKeyOrClick_ToDraw_Sprites_.clear();
        waitingForKeyOrClick_ToDraw_Texts_.clear();
    }

    void TestingStage::AppendWaitTestTitle(const std::string & TITLE_STR)
    {
        const gui::TextInfo TEXT_INFO(
            TITLE_STR, gui::GuiFont::Default, gui::FontManager::Instance()->Size_Larger());

        gui::Text text(TEXT_INFO);

        const sf::Vector2f POS_V(
            (StageRegion().width * 0.5f) - text.getGlobalBounds().width, 50.0f);

        waitingForKeyOrClick_ToDraw_Texts_.emplace_back(text);
    }

} // namespace stage
} // namespace heroespath
