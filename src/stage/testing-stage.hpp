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

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

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
        bool PerformTest_PopupManager();

        void DrawNormal(sf::RenderTarget &, sf::RenderStates) const;
        void DrawImageInspect(sf::RenderTarget &, sf::RenderStates) const;

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
        std::vector<gui::Text> waitingForKeyOrClick_ToDraw_Texts_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_TESTINGSTAGE_HPP_INCLUDED
