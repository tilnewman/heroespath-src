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
#include "game/loop-manager.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

#include <boost/type_index.hpp>

#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace stage
{

    using StrSizePair_t = std::pair<std::string, std::size_t>;
    using StrSizePairVec_t = std::vector<StrSizePair_t>;

    // A Stage class that allows visualizing testing info
    class TestingStage : public sfml_util::Stage
    {
    public:
        TestingStage(const TestingStage &) = delete;
        TestingStage(TestingStage &&) = delete;
        TestingStage & operator=(const TestingStage &) = delete;
        TestingStage & operator=(TestingStage &&) = delete;

    public:
        TestingStage();
        virtual ~TestingStage();

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyPress(const sf::Event::KeyEvent &) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        void TestingStrAppend(const std::string &) override;
        void TestingStrIncrement(const std::string &) override;

        void TestingImageSet(
            const sf::Texture &,
            const bool WILL_CHECK_FOR_OUTLINE = false,
            const std::string & CATEGORY_NAME = "",
            const std::string & TYPE_NAME = "",
            const std::string & PATH = "") override;

        void PerformNextTest() override;
        void PerformStatsTests();

    private:
        void TestStatSetsCurrentAndNormal(
            const std::string & TEST_NAME,
            const creature::StatSet & ACTUAL,
            const creature::StatSet & EXPECTED);

        bool TestImageSet();
        bool TestCharacterImageSet();
        bool TestMaps();
        bool PerformGameDataFileTests();
        bool TestAnimations();
        bool TestInventoryFactory();
        bool DoesImageHaveOutline(const sf::Texture & TEXTURE) const;
        bool PerformEnumTests();
        bool TestFonts();

        // see comment in .cpp file
        // void ReSaveWithBlackBorder(const std::string & IMAGES_DIR_KEY_STR) const;

    public:
        static const std::size_t TEXT_LINES_COUNT_MAX_;
        static sfml_util::AnimationUPtr_t animUPtr_;
        static const int IMAGE_COUNT_MAX_;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;

        // given how large sf::Textures are and how this class uses them, a std::list is a huge
        // speed improvement over std::vector
        std::list<sf::Texture> textureList_;

        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        StrSizePairVec_t testingBlurbsVec_;
        int sleepMilliseconds_;
        sf::Texture animBGTexture_;
        sf::Sprite animBGSprite_;
        int imageCount_;
        bool willImageCheck_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED
