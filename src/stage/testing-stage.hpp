// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED
//
// testing-stage.hpp
//  A Stage class that allows visualization of testing routines.
//
#include "creature/stat-set.hpp"
#include "gui/border.hpp"
#include "gui/cached-texture.hpp"
#include "gui/gold-bar.hpp"
#include "gui/text.hpp"
#include "sfutil/rectangle-shape.hpp"
#include "sfutil/vertex.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define M_TESTING_STAGE_TEST_WITH_FUNCTION(test_name, test_function_name)                          \
    static bool hasTestingCompleted_##test_name { false };                                         \
    if (false == hasTestingCompleted_##test_name)                                                  \
    {                                                                                              \
        hasTestingCompleted_##test_name = ##test_function_name();                                  \
        return;                                                                                    \
    }

#define M_TESTING_STAGE_TEST(test_name)                                                            \
    static bool hasTestingCompleted_##test_name { false };                                         \
    if (false == hasTestingCompleted_##test_name)                                                  \
    {                                                                                              \
        hasTestingCompleted_##test_name = PerformTest_##test_name();                               \
        return;                                                                                    \
    }

#define M_TESTING_STAGE_TEST_WITH_STATIC_TYPE_AND_CALL(space_name, test_name)                      \
    static bool hasTestingTypeAndCallCompleted_##test_name { false };                              \
    if (false == hasTestingTypeAndCallCompleted_##test_name)                                       \
    {                                                                                              \
        hasTestingTypeAndCallCompleted_##test_name                                                 \
            = space_name::test_name::Test(misc::MakeNotNull(this));                                \
                                                                                                   \
        return;                                                                                    \
    }

#define M_TESTING_STAGE_TEST_WITH_TYPE_AND_STAGECALL(test_name, type_name)                         \
    static bool hasTestingTypeAndCallCompleted_##test_name { false };                              \
    if (false == hasTestingTypeAndCallCompleted_##test_name)                                       \
    {                                                                                              \
        static type_name thing;                                                                    \
        hasTestingTypeAndCallCompleted_##test_name = thing.Test(misc::MakeNotNull(this));          \
        return;                                                                                    \
    }

#define M_TESTING_STRINGIFY(x) #x
#define M_TESTING_TOSTRING(x) M_TESTING_STRINGIFY(x)

#define M_TESTING_STAGE_TEST_WAIT(test_name)                                                       \
    static const auto testingWaitId_##test_name { ++waitingForKeyOrClickCounter };                 \
    static bool hasTestingWaitCompleted_##test_name { false };                                     \
    if (false == hasTestingWaitCompleted_##test_name)                                              \
    {                                                                                              \
        waitingForKeyOrClickId_ = testingWaitId_##test_name;                                       \
        hasTestingWaitCompleted_##test_name = true;                                                \
        SetupWaitTest_##test_name();                                                               \
        AppendWaitTestTitle(std::string(M_TESTING_TOSTRING(test_name)) + " Test");                 \
    }                                                                                              \
    if (testingWaitId_##test_name == waitingForKeyOrClickId_)                                      \
    {                                                                                              \
        return;                                                                                    \
    }

namespace heroespath
{
namespace gui
{
    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;
} // namespace gui
namespace stage
{

    using StrSizePair_t = std::pair<std::string, std::size_t>;
    using StrSizePairVec_t = std::vector<StrSizePair_t>;

    struct ImageInspectPacket : public sf::Drawable
    {
        explicit ImageInspectPacket(const gui::CachedTextureOpt_t & = boost::none);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        gui::CachedTextureOpt_t cached_texture_opt;
        sf::Sprite sprite;
        gui::Text text;
    };

    // A Stage class that allows visualizing testing info
    class TestingStage : public stage::StageBase
    {
    public:
        TestingStage(const TestingStage &) = delete;
        TestingStage(TestingStage &&) = delete;
        TestingStage & operator=(const TestingStage &) = delete;
        TestingStage & operator=(TestingStage &&) = delete;

        TestingStage();
        virtual ~TestingStage();

        void Setup() override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyPress(const sf::Event::KeyEvent &) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        void TestingStrAppend(const std::string &) override;
        void TestingStrIncrement(const std::string &) override;
        void TestingImageSet(const std::string & PATH_STR) override;

        void PerformNextTest() override;

    private:
        void TestStatSetsCurrentAndNormal(
            const std::string & TEST_NAME,
            const creature::StatSet & ACTUAL,
            const creature::StatSet & EXPECTED);

        bool PerformTest_IndividualImages();
        bool PerformTest_DirectoryImages();
        bool PerformTest_CharacterImageSet();
        bool PerformTest_Maps();
        bool PerformTest_GameDataFile();
        bool PerformTest_Animations();
        bool PerformTest_InventoryFactory();
        bool DoesImageHaveOutline(const sf::Texture & TEXTURE) const;
        bool PerformTest_Fonts();
        bool PerformTest_Spells();
        bool PerformTest_Songs();
        bool PerformTest_Conditions();
        bool PerformTest_Titles();
        bool PerformTest_PopupManager();
        bool PerformTest_SoundManager();
        bool PerformTest_FontSizes();
        bool PerformTest_ItemProfileReport();
        bool PerformTest_ArmorRatings();

        void DrawNormal(sf::RenderTarget &, sf::RenderStates) const;
        void DrawImageInspect(sf::RenderTarget &, sf::RenderStates) const;

        // see comment in .cpp file
        // void ReSaveWithBlackBorder(const std::string & IMAGES_DIR_KEY_STR) const;

        void SetupWaitTest_GoldBar();
        void SetupWaitTest_GoldBar2();
        void SetupWaitTest_Border();
        bool SetupWaitTest_SfTextLocalOffsetProblem();

        void ResetWaitingForKeyOrClick();

        void AppendWaitTestTitle(const std::string & TITLE_STR);

    public:
        static const std::size_t TEXT_LINES_COUNT_MAX_;
        static gui::AnimationUPtr_t animUPtr_;
        static const float IMAGE_INSPECT_DIMMENSION_;

    private:
        mutable std::vector<gui::CachedTexture> textures_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        StrSizePairVec_t testingBlurbsVec_;
        int sleepMilliseconds_;
        gui::CachedTexture animBackgroundCachedTexture_;
        sf::Sprite animBackgroundSprite_;
        mutable std::vector<ImageInspectPacket> imageInspectPackets_;
        bool willInspectImages_;
        bool isInspectingImages_;
        std::size_t imageInspectIndex_;
        int waitingForKeyOrClickId_;
        float waitingForKeyOrClickMaxTimeSec_;
        float waitingForKeyOrClickElapsedTimeSec_;
        bool isWaitingForKeyOrClickPaused_;

        // all these members are drawn during waitingForKeyOrClick tests
        gui::CachedTextureVec_t waitingForKeyOrClick_CachedTextures_;
        std::vector<sf::Sprite> waitingForKeyOrClick_ToDraw_Sprites_;
        std::vector<gui::GoldBar> waitingForKeyOrClick_ToDraw_GoldBars_;
        std::vector<gui::Border> waitingForKeyOrClick_ToDraw_Borders_;
        std::vector<sf::RectangleShape> waitingForKeyOrClick_ToDraw_RectangleShapes_;
        std::vector<sf::VertexArray> waitingForKeyOrClick_ToDraw_VertexArrays_;
        std::vector<gui::Text> waitingForKeyOrClick_ToDraw_Texts_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED
