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

#include "avatar/avatar-enum.hpp"
#include "combat/strategy-enums.hpp"
#include "combat/target-enum.hpp"
#include "creature/condition-holder.hpp"
#include "creature/creature.hpp"
#include "creature/name-info.hpp"
#include "creature/nonplayer-inventory-factory.hpp"
#include "creature/title-holder.hpp"
#include "game/game-data-file.hpp"
#include "interact/interaction-manager.hpp"
#include "item/armor-ratings.hpp"
#include "item/item-factory.hpp"
#include "item/item-profiles-reporter.hpp"
#include "log/log-macros.hpp"
#include "map/level-enum.hpp"
#include "map/map.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "misc/types.hpp"
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui-event-enum.hpp"
#include "sfml-util/gui/combat-image-loader.hpp"
#include "sfml-util/gui/condition-image-loader.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/gui-images.hpp"
#include "sfml-util/gui/item-image-loader.hpp"
#include "sfml-util/gui/song-image-loader.hpp"
#include "sfml-util/gui/spell-image-loader.hpp"
#include "sfml-util/gui/title-image-loader.hpp"
#include "sfml-util/image-loaders.hpp"
#include "sfml-util/image-option-enum.hpp"
#include "sfml-util/loop-state-enum.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-primitives.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"
#include "sfml-util/side-enum.hpp"
#include "sfml-util/sound-manager.hpp"
#include "song/song-holder.hpp"
#include "spell/spell-holder.hpp"

#include <boost/filesystem/path.hpp>

#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace heroespath
{
namespace stage
{
    ImageInspectPacket::ImageInspectPacket(const sfml_util::CachedTextureOpt_t & CACHED_TEXTURE_OPT)
        : cached_texture_opt(CACHED_TEXTURE_OPT)
        , sprite()
        , text()
    {
        if (cached_texture_opt)
        {
            sprite.setTexture(cached_texture_opt.value().Get(), true);

            sfml_util::Fit(
                sprite,
                TestingStage::IMAGE_INSPECT_DIMMENSION_,
                TestingStage::IMAGE_INSPECT_DIMMENSION_);

            text = sfml_util::Text(
                misc::filesystem::Filename(cached_texture_opt.value().Path()),
                sfml_util::GuiFont::Default,
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
                sfml_util::MakeRectangleSolid(text.getGlobalBounds(), sf::Color::Black), states);

            target.draw(text, states);
        }
    }

    const float TestingStage::IMAGE_INSPECT_DIMMENSION_ { 256.0f };
    const std::size_t TestingStage::TEXT_LINES_COUNT_MAX_ { 100 };
    sfml_util::AnimationUPtr_t TestingStage::animUPtr_;

    TestingStage::TestingStage()
        : Stage(
              "Testing",
              {
                  sfml_util::GuiFont::Default,
                  sfml_util::GuiFont::DefaultBoldFlavor,
                  sfml_util::GuiFont::System,
                  sfml_util::GuiFont::Number,
                  sfml_util::GuiFont::SystemCondensed,
                  sfml_util::GuiFont::SignThinTallNarrow,
                  sfml_util::GuiFont::SignBoldShortWide,
                  sfml_util::GuiFont::Handwriting,
                  sfml_util::GuiFont::DialogModern,
                  sfml_util::GuiFont::DialogMedieval,
              },
              true)
        , textures_()
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("TestingStage's"))
        , testingBlurbsVec_()
        , sleepMilliseconds_(0)
        , animBackgroundCachedTexture_("media-images-backgrounds-tile-wood")
        , animBackgroundSprite_(animBackgroundCachedTexture_.Get())
        , imageInspectPackets_()
        , willInspectImages_(false)
        , isInspectingImages_(false)
        , imageInspectIndex_(0)
        , waitingForKeyOrClickId_(0)
        , waitingForKeyOrClickMaxTimeSec_(30.0f)
        , waitingForKeyOrClickElapsedTimeSec_(0.0f)
        , isWaitingForKeyOrClickPaused_(false)
        , waitingForKeyOrClick_CachedTextures_()
        , waitingForKeyOrClick_ToDraw_Sprites_()
        , waitingForKeyOrClick_ToDraw_GoldBars_()
        , waitingForKeyOrClick_ToDraw_Borders_()
        , waitingForKeyOrClick_ToDraw_RectangleShapes_()
        , waitingForKeyOrClick_ToDraw_VertexArrays_()
        , waitingForKeyOrClick_ToDraw_Texts_()
    {}

    TestingStage::~TestingStage() { Stage::ClearAllEntities(); }

    void TestingStage::Setup()
    {
        EntityAdd(ouroborosUPtr_.get());
        animBackgroundSprite_.setPosition(0.0f, 0.0f);
    }

    void TestingStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        sfml_util::Display::Instance()->ClearToBlack();

        if (waitingForKeyOrClickId_ > 0)
        {
            for (const auto & SPRITE : waitingForKeyOrClick_ToDraw_Sprites_)
            {
                target.draw(SPRITE, STATES);
            }

            for (const auto & GOLDBAR : waitingForKeyOrClick_ToDraw_GoldBars_)
            {
                target.draw(GOLDBAR, STATES);
            }

            for (const auto & BORDER : waitingForKeyOrClick_ToDraw_Borders_)
            {
                target.draw(BORDER, STATES);
            }

            for (const auto & RECTANGLE_SHAPE : waitingForKeyOrClick_ToDraw_RectangleShapes_)
            {
                target.draw(RECTANGLE_SHAPE, STATES);
            }

            for (const auto & VERTEX_ARRAY : waitingForKeyOrClick_ToDraw_VertexArrays_)
            {
                target.draw(VERTEX_ARRAY, STATES);
            }

            for (const auto & TEXT : waitingForKeyOrClick_ToDraw_Texts_)
            {
                target.draw(TEXT, STATES);
            }
        }
        else
        {
            Stage::Draw(target, STATES);

            if (isInspectingImages_)
            {
                DrawImageInspect(target, STATES);
            }
            else
            {
                DrawNormal(target, STATES);
            }
        }
    }

    void TestingStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

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
            auto const INSPECT_IMAGE_COUNT_HORIZ { static_cast<std::size_t>(
                StageRegionWidth() / IMAGE_INSPECT_DIMMENSION_) };

            auto const INSPECT_IMAGE_COUNT_VERT { static_cast<std::size_t>(
                StageRegionHeight() / IMAGE_INSPECT_DIMMENSION_) };

            auto const INSPECT_IMAGE_COUNT { INSPECT_IMAGE_COUNT_HORIZ * INSPECT_IMAGE_COUNT_VERT };

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
                game::LoopManager::Instance()->TransitionTo_Exit();
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
            auto const IMAGE_COUNT { imageInspectPackets_.size() };
            for (std::size_t i(imageInspectIndex_); i < IMAGE_COUNT; ++i)
            {
                auto const & PACKET { imageInspectPackets_[i] };
                if (PACKET.sprite.getGlobalBounds().contains(MOUSE_POS_V))
                {
                    auto const FILE_NAME { std::string(PACKET.text.getString()) };

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

        //#ifndef HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS
        //        // hack to get status messages on the screen BEFORE a test is finished
        //        sfml_util::Display::Instance()->ClearToBlack();
        //
        //        // okay, wow, if we pass "this" to Display::DrawStage(this) then the windows build
        //        seems to
        //        // slice the this pointer itself and crash hard, no real understanding here...
        //        auto const THIS_STAGE_PTR { this };
        //
        //        sfml_util::Display::Instance()->DrawStage(THIS_STAGE_PTR);
        //        sfml_util::Display::Instance()->DisplayFrameBuffer();
        //#endif
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

    void TestingStage::TestingImageSet(
        const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE)
    {
        sfml_util::CachedTexture cachedTexture { boost::filesystem::path(PATH_STR) };

        imageInspectPackets_.emplace_back(cachedTexture);
        textures_.emplace_back(cachedTexture);

        if (WILL_CHECK_FOR_OUTLINE && DoesImageHaveOutline(cachedTexture.Get()))
        {
            M_HP_LOG_ERR("Testing Stage found an image with an outline.  path=" << PATH_STR);
        }
    }

    void TestingStage::PerformNextTest()
    {
        static auto waitingForKeyOrClickCounter(0);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds_));

        if (game::LoopManager::Instance()->IsFading())
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

        // See below (function ReSaveWithBlackBorder) for a comment explaining why
        // this code is commented out.
        // ReSaveWithBlackBorder("media-images-creaturess-dir");
        // ReSaveWithBlackBorder("media-images-items-dir");

        M_TESTING_STAGE_TEST(GameDataFile);

        M_TESTING_STAGE_TEST_WAIT(SfTextLocalOffsetProblem);
        M_TESTING_STAGE_TEST_WAIT(GoldBar);
        M_TESTING_STAGE_TEST_WAIT(GoldBar2);
        M_TESTING_STAGE_TEST_WAIT(Border);

        M_TESTING_STAGE_TEST(Enums);
        M_TESTING_STAGE_TEST(Fonts);
        M_TESTING_STAGE_TEST(Maps);
        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(ItemFactory, item::ItemFactory);
        M_TESTING_STAGE_TEST(ItemProfileReport);
        M_TESTING_STAGE_TEST(InventoryFactory);
        M_TESTING_STAGE_TEST(ArmorRatings);
        M_TESTING_STAGE_TEST(IndividualImages);
        M_TESTING_STAGE_TEST(DirectoryImages);
        M_TESTING_STAGE_TEST(CharacterImageSet);
        M_TESTING_STAGE_TEST(Spells);

        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(SpellsImageLoader, sfml_util::SpellImageLoader);

        M_TESTING_STAGE_TEST(Songs);

        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(SongsImageLoader, sfml_util::SongImageLoader);

        M_TESTING_STAGE_TEST(Conditions);

        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(
            ConditionImageLoader, sfml_util::ConditionImageLoader);

        M_TESTING_STAGE_TEST(Titles);

        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(TitleImageLoader, sfml_util::TitleImageLoader);

        M_TESTING_STAGE_TEST(PopupManager);

        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(CombatImageLoader, sfml_util::CombatImageLoader);

        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(ItemImageLoader, sfml_util::ItemImageLoader);

        M_TESTING_STAGE_TEST_WITH_TYPE_AND_CALL(
            CreatureImageLoader, sfml_util::CreatureImageLoader);

        M_TESTING_STAGE_TEST(Animations);

        if (willInspectImages_)
        {
            isInspectingImages_ = true;
        }

        M_TESTING_STAGE_TEST(FontSizes);

        M_TESTING_STAGE_TEST(SoundManager);

        std::cout << "ALL TESTS PASSED" << std::endl;
        game::LoopManager::Instance()->TransitionTo_Exit();
    }

    bool TestingStage::PerformTest_Spells() { return spell::Holder::Test(); }

    bool TestingStage::PerformTest_Songs() { return song::Holder::Test(); }

    bool TestingStage::PerformTest_Conditions() { return creature::condition::Holder::Test(); }

    bool TestingStage::PerformTest_Titles() { return creature::title::Holder::Test(); }

    bool TestingStage::PerformTest_SoundManager()
    {
        return sfml_util::SoundManager::Instance()->Test();
    }

    bool TestingStage::PerformTest_PopupManager()
    {
        return popup::PopupManager::Instance()->Test();
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
            auto const FONT_SIZE { [sizeIndex]() {
                switch (sizeIndex)
                {
                    // clang-format off
                    case 0: { return sfml_util::FontManager::Instance()->Size_Normal(); }
                    case 1: { return sfml_util::FontManager::Instance()->Size_Largeish(); }
                    case 2: { return sfml_util::FontManager::Instance()->Size_Large(); }
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
                auto const FONT_ENUM { static_cast<sfml_util::GuiFont::Enum>(fontIndex) };

                auto const LENGTH { nameInfo.Length(sfml_util::TextInfo(
                    nameInfo.LargestName(),
                    FONT_ENUM,
                    FONT_SIZE)) };

                lengths.emplace_back(LENGTH);

                M_HP_LOG_DBG(
                    sfml_util::GuiFont::ToString(FONT_ENUM)
                    << "\t" << FONT_SIZE_STR << '\t' << LENGTH << '\t' << "(" << LENGTH << "/"
                    << sfml_util::Display::Instance()->GetWinWidth()
                    << ")=" << (LENGTH / sfml_util::Display::Instance()->GetWinWidth()));
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
        for (misc::EnumUnderlying_t i(0); i < creature::Traits::StatCount; ++i)
        {
            auto const NEXT_ENUM{ static_cast<creature::Traits::Enum>(i) };

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

        static std::vector<std::string> imagePathKeyVec {
            game::GameDataFile::Instance()->GetAllKeysPrefixedWith("media-images-")
        };

        imagePathKeyVec.erase(
            std::remove_if(
                std::begin(imagePathKeyVec),
                std::end(imagePathKeyVec),
                [](const auto & KEY_STR) { return boost::algorithm::icontains(KEY_STR, "-dir"); }),
            std::end(imagePathKeyVec));

        static std::size_t imageIndex { 0 };
        if (imageIndex < imagePathKeyVec.size())
        {
            std::ostringstream ss;
            ss << "PerformTest_IndividualImage: \"" << imagePathKeyVec[imageIndex] << "\"";
            TestingStrAppend(ss.str());

            auto const IMAGE_PATH_STR { game::GameDataFile::Instance()->GetMediaPath(
                imagePathKeyVec[imageIndex]) };

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
            game::GameDataFile::Instance()->GetAllKeysWith("-dir")
        };

        static std::vector<std::string> imagePathsVec;
        static std::size_t imageIndex { 0 };
        static std::size_t imageDirIndex { 0 };
        if (imageDirIndex < imageDirPathKeyVec.size())
        {
            if (imageIndex == 0)
            {
                const auto IMAGE_DIR_PATH_KEY { imageDirPathKeyVec[imageDirIndex] };

                const auto IMAGE_DIR_PATH { game::GameDataFile::Instance()->GetMediaPath(
                    IMAGE_DIR_PATH_KEY) };

                std::ostringstream ss;
                ss << "PerformTest_DirectoryImages path: \"" << IMAGE_DIR_PATH << "\"";
                TestingStrAppend(ss.str());

                imagePathsVec = misc::filesystem::FindFilesInDirectory(
                    IMAGE_DIR_PATH, "", ".png", misc::filesystem::FilenameText::TO_EXCLUDE_VEC_);
            }

            if (imageIndex < imagePathsVec.size())
            {
                const auto IMAGE_PATH_STR { imagePathsVec[imageIndex] };

                std::ostringstream ss;
                ss << "PerformTest_DirectoryImage: #" << imageIndex << "\t\"" << IMAGE_PATH_STR
                   << "\"";

                TestingStrAppend(ss.str());

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

        static misc::EnumUnderlying_t imageIndex { 0 };
        if (imageIndex < avatar::Avatar::Count)
        {
            auto const WHICH_AVATAR { static_cast<avatar::Avatar::Enum>(imageIndex) };

            std::ostringstream ss;
            ss << "PerformTest_CharacterImageSet() \"" << avatar::Avatar::ToString(WHICH_AVATAR)
               << "\"";
            TestingStrAppend(ss.str());

            auto const IMAGE_PATH_STR { avatar::Avatar::ImagePath(WHICH_AVATAR) };
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

        auto const ParseMap { [](const map::Level::Enum LEVEL_ENUM, const std::string & ERROR_MSG) {
            map::MapUPtr_t mapUPtr { std::make_unique<map::Map>(
                sf::FloatRect(0.0f, 0.0f, 128.0f, 256.0f), interactionManager) };

            try
            {
                mapUPtr->Load(LEVEL_ENUM, map::Level::Count, true);
            }
            catch (const std::exception & E)
            {
                M_HP_LOG_FAT(
                    "TestingStage::TestMaps() threw an exception while \""
                    << ERROR_MSG << "\" for map \"" << map::Level::ToString(LEVEL_ENUM) << "\"");

                throw E;
            }

            MapLevelVec_t entryLevels;
            MapLevelVec_t exitLevels;
            mapUPtr->EntryAndExitLevels(entryLevels, exitLevels);
            return MapAndTransitions(LEVEL_ENUM, entryLevels, exitLevels);
        } };

        static auto hasFirstPassLoad { false };
        if (false == hasFirstPassLoad)
        {
            static misc::EnumUnderlying_t mapIndex { 0 };
            if (mapIndex < map::Level::Count)
            {
                auto const WHICH_LEVEL { static_cast<map::Level::Enum>(mapIndex) };

                std::ostringstream ss;
                ss << "TestMaps() Testing \"" << map::Level::ToString(WHICH_LEVEL)
                   << "\" Map in First Pass Loading";
                TestingStrAppend(ss.str());

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
                auto const CURRENT_LEVEL { transitionIter->first };

                std::ostringstream ss;
                ss << "TestMaps() Testing \"" << map::Level::ToString(CURRENT_LEVEL)
                   << "\" Map's transitions";

                TestingStrAppend(ss.str());

                for (auto const ENTRY_LEVEL : transitionIter->second.entry_levels)
                {
                    auto const ENTRY_TRANSITIONS { transitions[ENTRY_LEVEL] };

                    auto const WAS_FOUND { std::find(
                                               std::begin(ENTRY_TRANSITIONS.exit_levels),
                                               std::end(ENTRY_TRANSITIONS.exit_levels),
                                               CURRENT_LEVEL)
                                           != std::end(ENTRY_TRANSITIONS.exit_levels) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (WAS_FOUND),
                        "TestingStage::TestMaps() found a map \""
                            << map::Level::ToString(CURRENT_LEVEL)
                            << "\" that had an entry level from map \""
                            << map::Level::ToString(ENTRY_LEVEL)
                            << "\", but that map did not have an exit level to match.");
                }

                for (auto const EXIT_LEVEL : transitionIter->second.exit_levels)
                {
                    auto const EXIT_TRANSITIONS { transitions[EXIT_LEVEL] };

                    auto const WAS_FOUND { std::find(
                                               std::begin(EXIT_TRANSITIONS.entry_levels),
                                               std::end(EXIT_TRANSITIONS.entry_levels),
                                               CURRENT_LEVEL)
                                           != std::end(EXIT_TRANSITIONS.entry_levels) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (WAS_FOUND),
                        "TestingStage::TestMaps() found a map \""
                            << map::Level::ToString(CURRENT_LEVEL)
                            << "\" that had an exit level to map \""
                            << map::Level::ToString(EXIT_LEVEL)
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
            = { "heroespath-stats-reduce-ratio",
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
                "heroespath-fight-chance-enemies-ignore-unconscious" };

        static std::size_t keyIndex { 0 };
        if (keyIndex < keyVec.size())
        {
            std::ostringstream ss;
            ss << "Testing GameDataFile Key() \"" << keyVec[keyIndex]
               << "\"=" << game::GameDataFile::Instance()->GetCopyFloat(keyVec[keyIndex]);

            TestingStrAppend(ss.str());
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

        const long ANIM_FRAME_SLEEP_MS { 0 };

        static std::size_t animIndex { 0 };

        if (animIndex < sfml_util::Animations::Count)
        {
            if (!animUPtr_)
            {
                auto const ENUM { static_cast<sfml_util::Animations::Enum>(animIndex) };

                std::ostringstream ss;
                ss << "PerformTest_Animations() \"" << sfml_util::Animations::ToString(ENUM)
                   << "\"";
                TestingStrAppend(ss.str());

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

        static misc::EnumUnderlying_t raceIndex { 0 };
        static misc::EnumUnderlying_t roleIndex { 0 };

        if (raceIndex < static_cast<int>(creature::race::Count))
        {
            auto const RACE_ENUM { static_cast<creature::race::Enum>(raceIndex) };
            auto const RACE_STR { creature::race::ToString(RACE_ENUM) };
            auto const ROLE_VEC { creature::race::Roles(RACE_ENUM) };

            if (roleIndex < static_cast<misc::EnumUnderlying_t>(ROLE_VEC.size()))
            {
                auto const ROLE_ENUM { ROLE_VEC[static_cast<std::size_t>(roleIndex)] };
                auto const ROLE_STR { creature::role::ToString(ROLE_ENUM) };

                const int RANK_BASE { 50 };
                const int RANK_MAX { [&]() {
                    if (RACE_ENUM == creature::race::Dragon)
                    {
                        return RANK_BASE
                            + game::GameDataFile::Instance()->GetCopyInt(
                                  "heroespath-creature-dragon-class-rank-min-Elder");
                    }
                    else if (RACE_ENUM == creature::race::Wolfen)
                    {
                        return RANK_BASE
                            + game::GameDataFile::Instance()->GetCopyInt(
                                  "heroespath-creature-wolfen-class-rank-min-Elder");
                    }
                    else
                    {
                        return RANK_BASE
                            + game::GameDataFile::Instance()->GetCopyInt(
                                  "heroespath-rankclass-Master-rankmax");
                    }
                }() };

                {
                    std::ostringstream ss;

                    ss << "InventoryFactory Testing: race=" << RACE_STR << " and role=" << ROLE_STR
                       << "...";

                    TestingStrAppend(ss.str());
                }

                for (int rankIndex(1); rankIndex <= RANK_MAX; ++rankIndex)
                {
                    std::ostringstream nameSS;
                    nameSS << "Name_" << RACE_STR << "_" << ROLE_STR << "_" << rankIndex;

                    creature::Creature character(
                        false,
                        nameSS.str(),
                        ((misc::random::Bool()) ? creature::sex::Female : creature::sex::Male),
                        RACE_ENUM,
                        ROLE_ENUM,
                        creature::StatSet(10_str, 10_acc, 10_cha, 10_lck, 10_spd, 10_int),
                        "",
                        10_health,
                        Rank_t(rankIndex),
                        Experience_t(rankIndex * 10000));

                    inventoryFactory.SetupCreatureInventory(&character);
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

        auto const WIDTH { TEXTURE.getSize().x };
        auto const HEIGHT { TEXTURE.getSize().y };

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
            auto const ENUM { static_cast<CornerSide>(cs) };

            auto wereAllColorsEqual { true };

            auto const FIRST_COLOR { [&]() {
                switch (ENUM)
                {
                    case TopLeftTop:
                    {
                        return image.getPixel(OFFSET, 0);
                    }
                    case TopLeftLeft:
                    {
                        return image.getPixel(0, OFFSET);
                    }
                    case TopRightTop:
                    {
                        return image.getPixel(((WIDTH - 1) - OFFSET) - RUN_LENGTH, 0);
                    }
                    case TopRightRight:
                    {
                        return image.getPixel((WIDTH - 1), 0);
                    }
                    case BotLeftBot:
                    {
                        return image.getPixel(OFFSET, (HEIGHT - 1));
                    }
                    case BotLeftLeft:
                    {
                        return image.getPixel(0, ((HEIGHT - 1) - OFFSET) - RUN_LENGTH);
                    }
                    case BotRightBot:
                    {
                        return image.getPixel(((WIDTH - 1) - OFFSET) - RUN_LENGTH, (HEIGHT - 1));
                    }
                    case BotRightRight:
                    {
                        return image.getPixel((WIDTH - 1), (HEIGHT - 1) - OFFSET);
                    }
                    case Count:
                    default:
                    {
                        return image.getPixel(0, 0);
                    }
                }
            }() };

            for (unsigned rl(0); rl < RUN_LENGTH; ++rl)
            {
                auto const NEXT_COLOR { [&]() {
                    switch (ENUM)
                    {
                        case TopLeftTop:
                        {
                            return image.getPixel(OFFSET + rl, 0);
                        }
                        case TopLeftLeft:
                        {
                            return image.getPixel(0, OFFSET + rl);
                        }
                        case TopRightTop:
                        {
                            return image.getPixel((((WIDTH - 1) - OFFSET) - RUN_LENGTH) + rl, 0);
                        }
                        case TopRightRight:
                        {
                            return image.getPixel((WIDTH - 1), 0 + rl);
                        }
                        case BotLeftBot:
                        {
                            return image.getPixel(OFFSET + rl, (HEIGHT - 1));
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
                        default:
                        {
                            return image.getPixel(0, 0);
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
            sfml_util::Loaders::Texture(texture, FILE_PATH_STR, false);
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

    bool TestingStage::PerformTest_Enums()
    {
        item::category::Test();
        item::material::Test();
        item::element_type::Test();
        item::misc_type::Test();
        item::set_type::Test();
        item::named_type::Test();
        item::weapon_type::Test();
        item::armor_type::Test();
        item::body_part::Test();
        item::name_material_type::Test();
        item::armor::shield_type::Test();
        item::armor::helm_type::Test();
        item::armor::base_type::Test();
        item::armor::cover_type::Test();
        item::weapon::sword_type::Test();
        item::weapon::axe_type::Test();
        item::weapon::club_type::Test();
        item::weapon::whip_type::Test();
        item::weapon::projectile_type::Test();
        item::weapon::bladedstaff_type::Test();
        map::Level::Test();
        map::LevelType::Test();
        creature::origin_type::Test();
        creature::race::Test();
        creature::role::Test();
        creature::EnchantmentType::Test();
        creature::AchievementType::Test();
        creature::Conditions::Test();
        creature::Titles::Test();
        creature::sex::Test();
        creature::dragon_class::Test();
        creature::wolfen_class::Test();
        creature::Traits::Test();
        // combat::strategy::SelectType::Test(); //bah, this takes too long...
        combat::strategy::RefineType::Test();
        combat::strategy::AdvanceType::Test();
        combat::strategy::RetreatType::Test();
        combat::strategy::FrequencyType::Test();
        combat::TargetType::Test();
        game::Phase::Test();
        creature::nonplayer::wealth_type::Test();
        creature::nonplayer::collector_type::Test();
        creature::nonplayer::owns_magic_type::Test();
        creature::nonplayer::complexity_type::Test();
        popup::ResponseTypes::Test();
        sfml_util::Brightness::Test();
        sfml_util::Corner::Test();
        sfml_util::GuiEvent::Test();
        sfml_util::Side::Test();
        sfml_util::Animations::Test();
        sfml_util::LoopState::TestHelper();
        sfml_util::Footstep::Test();
        sfml_util::music::Test();
        sfml_util::sound_effect::Test();
        sfml_util::sound_effect_set::Test();
        sfml_util::Footstep::Test();
        sfml_util::GuiFont::Test();
        sfml_util::ImageOpt::Test();
        sfml_util::Justified::Test();
        // sfml_util::text_render::engine::StopReason::Test();
        avatar::Avatar::Test();
        spell::Spells::Test();
        song::Songs::Test();
        return true;
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

        static misc::EnumUnderlying_t fontIndex { 0 };

        if (fontIndex < sfml_util::GuiFont::Count)
        {
            auto const FONT_ENUM { static_cast<sfml_util::GuiFont::Enum>(fontIndex) };
            sfml_util::Text text("asdf", FONT_ENUM, 30);
            ++fontIndex;
            return false;
        }
        else
        {
            TestingStrAppend("stage::TestingStage::PerformTest_Fonts() Finished.");
            return true;
        }
    }

    void TestingStage::DrawNormal(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (animUPtr_)
        {
            target.draw(animBackgroundSprite_, STATES);
            animUPtr_->draw(target, STATES);
        }

        auto const IMAGE_POS_TOP { 1.0f };
        {
            std::size_t imageDrawCount { 0 };
            auto posLeft { StageRegionWidth() };

            auto rItr(textures_.rbegin());
            for (; rItr != textures_.rend(); ++rItr)
            {
                sf::Sprite sprite;
                sprite.setTexture(rItr->Get());

                // reduce size if any dimmension is greater than 256
                auto const MAX_DIMMENSION { 256.0f };
                auto newHorizScale { 1.0f };
                if (sprite.getGlobalBounds().width > MAX_DIMMENSION)
                {
                    newHorizScale = MAX_DIMMENSION / sprite.getGlobalBounds().width;
                    sprite.setScale(newHorizScale, newHorizScale);
                }
                if (sprite.getGlobalBounds().height > MAX_DIMMENSION)
                {
                    auto const NEW_VERT_SCALE { MAX_DIMMENSION / sprite.getGlobalBounds().height };
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

            while (textures_.size() > imageDrawCount)
            {
                textures_.erase(textures_.begin());
            }
        }

        {
            // find out how tall the text lines will be
            sfml_util::Text testText(
                "M",
                sfml_util::GuiFont::Default,
                sfml_util::FontManager::Instance()->Size_Normal());

            // The extra +10 is added because testText's height is only an estimation.
            auto const TEXT_HEIGHT { testText.getGlobalBounds().height + 10.0f };

            // The + 256 is to make room for the images, so text is not drawn over them.
            auto DO_NOT_PASS_TOP { IMAGE_POS_TOP + 256.0f + TEXT_HEIGHT };

            // The extra * 2 is added because without it, the text at the bottom is cut off.
            auto posTop { StageRegionHeight() - (TEXT_HEIGHT * 2.0f) };

            StrSizePairVec_t::reverse_iterator rItr { testingBlurbsVec_.rbegin() };
            for (; rItr != testingBlurbsVec_.rend(); ++rItr)
            {
                std::ostringstream ss;
                ss << rItr->first;

                if (rItr->second > 0)
                {
                    ss << " " << rItr->second;
                }

                sfml_util::Text text(
                    ss.str(),
                    sfml_util::GuiFont::Default,
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

    void TestingStage::DrawImageInspect(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        auto willLowerText { false };
        auto posX { 0.0f };
        auto posY { 0.0f };
        auto imageIndex { imageInspectIndex_ };

        while (posY < (StageRegionHeight() - IMAGE_INSPECT_DIMMENSION_))
        {
            while (posX < (StageRegionWidth() - IMAGE_INSPECT_DIMMENSION_))
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

                target.draw(packet, STATES);

                posX += IMAGE_INSPECT_DIMMENSION_;
                ++imageIndex;
            }

            posX = 0.0f;
            posY += IMAGE_INSPECT_DIMMENSION_;
        }
    }

    void TestingStage::SetupWaitTest_GoldBar()
    {
        const auto SCREEN_EDGE_PAD { std::sqrt((StageRegionWidth() * StageRegionHeight()))
                                     * 0.092f };

        const auto BETWEEN_PAD { SCREEN_EDGE_PAD * 0.025f };

        auto createGoldBars = [&](const float LEFT,
                                  const float TOP,
                                  const sfml_util::Orientation::Enum ORIENTATION,
                                  const sfml_util::Side::Enum SIDE,
                                  const bool WILL_CAP_ENDS) {
            float posX { LEFT };
            float posY { TOP };

            auto length { 0.0f };
            while (((posX - LEFT) < SCREEN_EDGE_PAD) && ((posY - TOP) < SCREEN_EDGE_PAD))
            {
                waitingForKeyOrClick_ToDraw_GoldBars_.emplace_back(
                    sfml_util::GoldBar(posX, posY, length, ORIENTATION, SIDE, WILL_CAP_ENDS));

                const auto REGION { waitingForKeyOrClick_ToDraw_GoldBars_.back().OuterRegion() };

                waitingForKeyOrClick_ToDraw_RectangleShapes_.emplace_back(
                    sfml_util::MakeRectangleHollow(REGION, sf::Color::Red, 1.0f, true));

                if (ORIENTATION == sfml_util::Orientation::Horiz)
                {
                    posY += REGION.height + BETWEEN_PAD;
                }
                else
                {
                    posX += REGION.width + BETWEEN_PAD;
                }

                if (length < 3.0f)
                {
                    length += misc::random::Float();
                }
                else
                {
                    length += misc::random::Float(1.0f, length);
                }
            }

            return sf::Vector2f(posX, posY);
        };

        const auto BETWEEN_SERIES_PAD { BETWEEN_PAD * 2.0f };
        const auto BETWEEN_SET_PAD { BETWEEN_SERIES_PAD * 2.0f };

        auto createGoldBarsOfAllSides
            = [&](const sf::Vector2f & POS_V, const sfml_util::Orientation::Enum ORIENTATION) {
                  sf::Vector2f posV { POS_V };
                  misc::EnumUnderlying_t flag(1);
                  while (flag <= sfml_util::Side::Last)
                  {
                      posV = createGoldBars(
                          posV.x, posV.y, ORIENTATION, sfml_util::Side::Enum(flag), false);

                      flag <<= 1;

                      if (ORIENTATION == sfml_util::Orientation::Horiz)
                      {
                          posV.y += BETWEEN_SERIES_PAD;
                          posV.x = POS_V.x;
                      }
                      else
                      {
                          posV.x += BETWEEN_SERIES_PAD;
                          posV.y = POS_V.y;
                      }
                  }

                  if (ORIENTATION == sfml_util::Orientation::Horiz)
                  {
                      posV.y += BETWEEN_SET_PAD;
                      posV.x = POS_V.x;
                  }
                  else
                  {
                      posV.x += BETWEEN_SET_PAD;
                      posV.y = POS_V.y;
                  }

                  const auto START_POS_V { posV };

                  flag = 1;
                  while (flag <= sfml_util::Side::Last)
                  {
                      posV = createGoldBars(
                          posV.x, posV.y, ORIENTATION, sfml_util::Side::Enum(flag), true);

                      flag <<= 1;

                      if (ORIENTATION == sfml_util::Orientation::Horiz)
                      {
                          posV.y += BETWEEN_SERIES_PAD;
                          posV.x = START_POS_V.x;
                      }
                      else
                      {
                          posV.x += BETWEEN_SERIES_PAD;
                          posV.y = START_POS_V.y;
                      }
                  }
              };

        createGoldBarsOfAllSides(
            sf::Vector2f(SCREEN_EDGE_PAD, SCREEN_EDGE_PAD * 1.9f), sfml_util::Orientation::Horiz);

        createGoldBarsOfAllSides(
            sf::Vector2f(SCREEN_EDGE_PAD * 1.9f, SCREEN_EDGE_PAD), sfml_util::Orientation::Vert);

        waitingForKeyOrClick_CachedTextures_.emplace_back(
            sfml_util::CachedTexture(sfml_util::GuiImages::PathKey()));

        sf::Sprite sprite(waitingForKeyOrClick_CachedTextures_.back().Get());
        sfml_util::Center(sprite);

        waitingForKeyOrClick_ToDraw_Sprites_.emplace_back(sprite);
    }

    void TestingStage::SetupWaitTest_GoldBar2()
    {
        const auto SCREEN_EDGE_PAD { std::sqrt((StageRegionWidth() * StageRegionHeight()))
                                     * 0.12f };

        const auto BETWEEN_SPACER { SCREEN_EDGE_PAD * 0.025f };
        const auto BETWEEN_ROW_VERT_SPACER { BETWEEN_SPACER * 4.0f };

        auto makeHorizRowOfGoldBarBoxes = [&](const float LEFT,
                                              const float TOP,
                                              const bool WILL_MAKE_SMALL,
                                              const bool WILL_GROW_BORDER) {
            const auto GROWTH_MIN { 0.333f };
            const auto GROWTH_MAX { 10.0f };

            float posX { LEFT };
            float posY { TOP };

            auto width { 0.0f };
            auto height { 0.0f };

            auto horizGrowthPrev { GROWTH_MIN };
            auto vertGrowthPrev { GROWTH_MIN };
            while (posX < (StageRegionWidth() - SCREEN_EDGE_PAD))
            {
                const sf::FloatRect REGION_INITIAL(posX, posY, width, height);

                waitingForKeyOrClick_ToDraw_GoldBars_.emplace_back(sfml_util::GoldBar(
                    REGION_INITIAL, sfml_util::Orientation::Count, WILL_GROW_BORDER));

                const auto REGION_OUTER {
                    waitingForKeyOrClick_ToDraw_GoldBars_.back().OuterRegion()
                };

                waitingForKeyOrClick_ToDraw_RectangleShapes_.emplace_back(
                    sfml_util::MakeRectangleHollow(REGION_OUTER, sf::Color::Red, 1.0f, true));

                const auto REGION_INNER {
                    waitingForKeyOrClick_ToDraw_GoldBars_.back().InnerRegion()
                };

                waitingForKeyOrClick_ToDraw_RectangleShapes_.emplace_back(
                    sfml_util::MakeRectangleHollow(REGION_INNER, sf::Color::Green, 1.0f, true));

                posX += REGION_OUTER.width + BETWEEN_SPACER;

                if (WILL_MAKE_SMALL)
                {
                    const auto GROWTH_HORIZ { misc::random::Float(
                        GROWTH_MIN, std::max(GROWTH_MIN, (1.0f - horizGrowthPrev))) };

                    horizGrowthPrev = GROWTH_HORIZ;

                    width += GROWTH_HORIZ;

                    const auto GROWTH_VERT { misc::random::Float(
                        GROWTH_MIN, std::max(GROWTH_MIN, (1.0f - vertGrowthPrev))) };

                    vertGrowthPrev = GROWTH_VERT;

                    height += GROWTH_VERT;
                }
                else
                {
                    width += misc::random::Float(1.0f, GROWTH_MAX);
                    height += misc::random::Float(1.0f, GROWTH_MAX);
                }
            }

            const auto REGION_FINAL { waitingForKeyOrClick_ToDraw_GoldBars_.back().OuterRegion() };
            return TOP + REGION_FINAL.height;
        };

        const auto BOTTOM_OF_FIRST_ROW { makeHorizRowOfGoldBarBoxes(
            SCREEN_EDGE_PAD, SCREEN_EDGE_PAD, true, false) };

        const auto BOTTOM_OF_SECOND_ROW { makeHorizRowOfGoldBarBoxes(
            SCREEN_EDGE_PAD, BOTTOM_OF_FIRST_ROW + BETWEEN_ROW_VERT_SPACER, false, false) };

        const auto BOTTOM_OF_THIRD_ROW { makeHorizRowOfGoldBarBoxes(
            SCREEN_EDGE_PAD, BOTTOM_OF_SECOND_ROW + (BETWEEN_ROW_VERT_SPACER * 2.0f), true, true) };

        makeHorizRowOfGoldBarBoxes(
            SCREEN_EDGE_PAD, BOTTOM_OF_THIRD_ROW + BETWEEN_ROW_VERT_SPACER, false, true);
    }

    void TestingStage::SetupWaitTest_Border()
    {
        const auto SCREEN_EDGE_PAD { std::sqrt((StageRegionWidth() * StageRegionHeight()))
                                     * 0.12f };

        const auto BETWEEN_SPACER { SCREEN_EDGE_PAD * 0.025f };
        const auto BETWEEN_ROW_VERT_SPACER { BETWEEN_SPACER * 4.0f };

        auto makeHorizRowOfBorders = [&](const float LEFT,
                                         const float TOP,
                                         const bool WILL_DRAW_LINES,
                                         const bool WILL_MAKE_SMALL,
                                         const bool WILL_GROW_BORDER) {
            const auto GROWTH_MIN { 0.333f };
            const auto GROWTH_MAX { 10.0f };

            float posX { LEFT };
            float posY { TOP };

            auto width { 0.0f };
            auto height { 0.0f };

            auto horizGrowthPrev { GROWTH_MIN };
            auto vertGrowthPrev { GROWTH_MIN };
            while (posX < (StageRegionWidth() - SCREEN_EDGE_PAD))
            {
                const sf::FloatRect REGION_INITIAL(posX, posY, width, height);

                if (WILL_DRAW_LINES)
                {
                    waitingForKeyOrClick_ToDraw_Borders_.emplace_back(
                        sfml_util::Border(REGION_INITIAL, WILL_GROW_BORDER));
                }
                else
                {
                    waitingForKeyOrClick_ToDraw_Borders_.emplace_back(sfml_util::Border(
                        REGION_INITIAL,
                        1.0f,
                        sf::Color::White,
                        sf::Color::Transparent,
                        WILL_GROW_BORDER));
                }

                const auto REGION_OUTER {
                    waitingForKeyOrClick_ToDraw_Borders_.back().OuterRegion()
                };

                waitingForKeyOrClick_ToDraw_RectangleShapes_.emplace_back(
                    sfml_util::MakeRectangleHollow(REGION_OUTER, sf::Color::Red, 1.0f, true));

                const auto REGION_INNER {
                    waitingForKeyOrClick_ToDraw_Borders_.back().InnerRegion()
                };

                waitingForKeyOrClick_ToDraw_RectangleShapes_.emplace_back(
                    sfml_util::MakeRectangleHollow(REGION_INNER, sf::Color::Green, 1.0f, true));

                posX += REGION_OUTER.width + BETWEEN_SPACER;

                if (WILL_MAKE_SMALL)
                {
                    const auto GROWTH_HORIZ { misc::random::Float(
                        GROWTH_MIN, std::max(GROWTH_MIN, (1.0f - horizGrowthPrev))) };

                    horizGrowthPrev = GROWTH_HORIZ;

                    width += GROWTH_HORIZ;

                    const auto GROWTH_VERT { misc::random::Float(
                        GROWTH_MIN, std::max(GROWTH_MIN, (1.0f - vertGrowthPrev))) };

                    vertGrowthPrev = GROWTH_VERT;

                    height += GROWTH_VERT;
                }
                else
                {
                    width += misc::random::Float(1.0f, GROWTH_MAX);
                    height += misc::random::Float(1.0f, GROWTH_MAX);
                }
            }

            const auto REGION_FINAL { waitingForKeyOrClick_ToDraw_Borders_.back().OuterRegion() };
            return TOP + REGION_FINAL.height;
        };

        const auto BOTTOM_OF_FIRST_ROW { makeHorizRowOfBorders(
            SCREEN_EDGE_PAD, SCREEN_EDGE_PAD, true, true, false) };

        const auto BOTTOM_OF_SECOND_ROW { makeHorizRowOfBorders(
            SCREEN_EDGE_PAD, BOTTOM_OF_FIRST_ROW + BETWEEN_ROW_VERT_SPACER, true, false, false) };

        const auto BOTTOM_OF_THIRD_ROW { makeHorizRowOfBorders(
            SCREEN_EDGE_PAD,
            BOTTOM_OF_SECOND_ROW + (BETWEEN_ROW_VERT_SPACER * 2.0f),
            true,
            true,
            true) };

        const auto BOTTOM_OF_FOURTH_ROW { makeHorizRowOfBorders(
            SCREEN_EDGE_PAD, BOTTOM_OF_THIRD_ROW + BETWEEN_ROW_VERT_SPACER, false, true, false) };

        const auto BOTTOM_OF_FIFTH_ROW { makeHorizRowOfBorders(
            SCREEN_EDGE_PAD, BOTTOM_OF_FOURTH_ROW + BETWEEN_ROW_VERT_SPACER, false, false, false) };

        makeHorizRowOfBorders(
            SCREEN_EDGE_PAD, BOTTOM_OF_FIFTH_ROW + BETWEEN_ROW_VERT_SPACER, false, true, true);
    }

    void TestingStage::ResetWaitingForKeyOrClick()
    {
        waitingForKeyOrClickId_ = 0;
        waitingForKeyOrClickElapsedTimeSec_ = 0.0f;
        isWaitingForKeyOrClickPaused_ = false;

        waitingForKeyOrClick_CachedTextures_.clear();
        waitingForKeyOrClick_ToDraw_Sprites_.clear();
        waitingForKeyOrClick_ToDraw_GoldBars_.clear();
        waitingForKeyOrClick_ToDraw_Borders_.clear();
        waitingForKeyOrClick_ToDraw_RectangleShapes_.clear();
        waitingForKeyOrClick_ToDraw_VertexArrays_.clear();
        waitingForKeyOrClick_ToDraw_Texts_.clear();
    }

    void TestingStage::AppendWaitTestTitle(const std::string & TITLE_STR)
    {
        const sfml_util::TextInfo TEXT_INFO(
            TITLE_STR,
            sfml_util::GuiFont::Default,
            sfml_util::FontManager::Instance()->Size_Larger());

        sfml_util::Text text(TEXT_INFO);

        const sf::Vector2f POS_V((StageRegionWidth() * 0.5f) - text.getGlobalBounds().width, 50.0f);

        waitingForKeyOrClick_ToDraw_Texts_.emplace_back(text);
    }

    bool TestingStage::SetupWaitTest_SfTextLocalOffsetProblem()
    {
        auto setupTextTest
            = [&](const sf::FloatRect & REGION, const unsigned FONT_SIZE, const float SCALE) {
                  waitingForKeyOrClick_ToDraw_RectangleShapes_.emplace_back(
                      sfml_util::MakeRectangleHollow(REGION, sf::Color::Green));

                  const sfml_util::TextInfo TEXT_INFO(
                      "This is text.", sfml_util::GuiFont::SystemCondensed, FONT_SIZE);

                  sfml_util::Text text(TEXT_INFO);
                  text.setScale(sf::Vector2f(SCALE, SCALE));
                  text.setPosition(sfml_util::Position(REGION));

                  waitingForKeyOrClick_ToDraw_RectangleShapes_.emplace_back(
                      sfml_util::MakeRectangleHollow(text.getGlobalBounds(), sf::Color::Yellow));

                  waitingForKeyOrClick_ToDraw_Texts_.emplace_back(text);
              };

        const float OFFSET { 100.0f };
        const sf::Vector2f OFFSET_V(OFFSET, OFFSET);

        const float SIZE { 600.0f };
        const sf::Vector2f SIZE_V(SIZE, (SIZE * 0.5f));

        sf::FloatRect region(OFFSET_V, SIZE_V);
        setupTextTest(region, 100, 1.0f);

        region.left = sfml_util::ScreenRatioToPixelsHoriz(0.5f);
        setupTextTest(region, 50, 5.0f);

        return true;
    }

} // namespace stage
} // namespace heroespath
