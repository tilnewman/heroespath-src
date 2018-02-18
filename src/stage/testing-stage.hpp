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
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/ouroboros.hpp"

#include "game/loop-manager.hpp"
#include "stats/stat-set.hpp"

#include <boost/type_index.hpp>//for boost::typeindex::type_id<T>().pretty_name()

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <sstream>


namespace heroespath
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

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        virtual bool KeyPress(const sf::Event::KeyEvent &) override;
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        virtual void TestingStrAppend(const std::string &) override;
        virtual void TestingStrIncrement(const std::string &) override;

        virtual void TestingImageSet(
            const sf::Texture &,
            const bool WILL_CHECK_FOR_OUTLINE = false,
            const std::string & CATEGORY_NAME = "",
            const std::string & TYPE_NAME = "",
            const std::string & PATH = "") override;

        virtual void PerformNextTest() override;
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

        template<typename ManagerType_t, typename EnumType_t>
        bool TestImageManager()
        {
            std::ostringstream managerTypeNameSS;
            managerTypeNameSS
                << boost::typeindex::type_id<ManagerType_t>().pretty_name()
                << "<" << boost::typeindex::type_id<EnumType_t>().pretty_name()
                << ">";

            static auto hasInitialPrompt{ false };
            if (false == hasInitialPrompt)
            {
                hasInitialPrompt = true;

                game::LoopManager::Instance()->TestingStrAppend(
                    managerTypeNameSS.str() + "  Starting Tests...");
            }

            static auto willFlip{ false };
            static auto imageIndex{ 0 };
            if (imageIndex < EnumType_t::Count)
            {
                auto const ENUM{ static_cast<typename EnumType_t::Enum>(imageIndex) };
                sf::Texture texture;
                ManagerType_t::Instance()->Get(texture, ENUM, willFlip);

                game::LoopManager::Instance()->TestingImageSet(texture);

                game::LoopManager::Instance()->TestingStrAppend(
                    managerTypeNameSS.str() + " Tested "
                    + EnumType_t::ImageFilename(ENUM)
                    + ((willFlip) ? "HORIZ_FLIPPED" : ""));

                if (willFlip)
                {
                    ++imageIndex;
                }

                willFlip = ! willFlip;
                return false;
            }

            game::LoopManager::Instance()->TestingStrAppend(
                managerTypeNameSS.str() + "  All Test Passed");

            return true;
        }

        bool DoesImageHaveOutline(const sf::Texture & TEXTURE) const;

        //see comment in .cpp file
        //void ReSaveWithBlackBorder(const std::string & IMAGES_DIR_KEY_STR) const;

    public:
        static const std::size_t TEXT_LINES_COUNT_MAX_;
        static sfml_util::AnimationUPtr_t animUPtr_;
        static const int IMAGE_COUNT_MAX_;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        sfml_util::TextureLst_t textureList_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        StrSizePairVec_t testingBlurbsVec_;
        int sleepMilliseconds_;
        sf::Texture animBGTexture_;
        sf::Sprite animBGSprite_;
        int imageCount_;
        bool willImageCheck_;
    };

}
}

#endif //HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED

