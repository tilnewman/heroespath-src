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
#ifndef HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED
//
// testing-stage.hpp
//  A Stage class that allows visualization of testing routines.
//
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

#include "game/loop-manager.hpp"
#include "stats/stat-set.hpp"

#include <boost/type_index.hpp> //for boost::typeindex::type_id<T>().pretty_name()

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
            const stats::StatSet & ACTUAL,
            const stats::StatSet & EXPECTED);

        bool TestImageSet();
        bool TestCharacterImageSet();
        bool TestMaps();
        bool PerformGameDataFileTests();
        bool TestAnimations();
        bool TestInventoryFactory();
        bool DoesImageHaveOutline(const sf::Texture & TEXTURE) const;

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
