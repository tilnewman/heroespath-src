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
#ifndef GAME_TESTINGSTAGE_INCLUDED
#define GAME_TESTINGSTAGE_INCLUDED
//
// testing-stage.hpp
//  A Stage class that allows visualization of testing routines.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/animation-factory.hpp"

#include "game/ouroboros.hpp"
#include "game/stats/stat-set.hpp"

#include <memory>
#include <string>
#include <vector>
#include <utility>


namespace game
{
namespace stage
{

    using StrSizePair_t     = std::pair<std::string, std::size_t>;
    using StrSizePairVec_t  = std::vector<StrSizePair_t>;


    //A Stage class that allows visualizing testing info
    class TestingStage : public sfml_util::Stage
    {
        TestingStage(const TestingStage &) =delete;
        TestingStage & operator=(const TestingStage &) =delete;

    public:
        TestingStage();
        virtual ~TestingStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual bool KeyPress(const sf::Event::KeyEvent &);
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);

        void TestingStrAppend(const std::string &);
        void TestingStrIncrement(const std::string &);
        void TestingImageSet(const sf::Texture &);

        void PerformNextTest();
        void PerformStatsTests();

    private:
        void TestStatSetsCurrentAndNormal(const std::string &    TEST_NAME,
                                          const stats::StatSet & ACTUAL,
                                          const stats::StatSet & EXPECTED);

        bool TestImageSet();
        bool PerformGameDataFileTests();
        bool TestAnimations();
        bool TestInventoryFactory();

    public:
        static const std::size_t TEXT_LINES_COUNT_MAX_;
        static sfml_util::AnimationUPtr_t animUPtr_;
        static const int IMAGE_COUNT_MAX_;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        sfml_util::TextureLst_t textureList_;
        OuroborosUPtr_t ouroborosUPtr_;
        StrSizePairVec_t testingBlurbsVec_;
        int sleepMilliseconds_;
        sf::Texture animBGTexture_;
        sf::Sprite animBGSprite_;
        int imageCount_;
        bool willImageCheck_;
    };

}
}
#endif //GAME_TESTINGSTAGE_INCLUDED

