// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INNSTAGE_HPP_INCLUDED
#define HEROESPATH_INNSTAGE_HPP_INCLUDED
//
// inn-stage.hpp
//  A Stage class that allows starting the game
//
#include "sfml-util/box-entity.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/stage-title.hpp"
#include "stage/stage-base.hpp"

#include <memory>

namespace heroespath
{
namespace sfml_util
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;

    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;
} // namespace sfml_util

namespace stage
{

    // A Stage class that allows starting the game
    class InnStage : public stage::StageBase
    {
    public:
        InnStage(const InnStage &) = delete;
        InnStage(InnStage &&) = delete;
        InnStage & operator=(const InnStage &) = delete;
        InnStage & operator=(InnStage &&) = delete;

        InnStage();
        virtual ~InnStage();

        void Setup() final;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) final;

    private:
        sfml_util::StageTitle stageTitle_;
        sfml_util::BoxEntity backgroundBox_;
        sfml_util::CachedTexture candleCachedTexture_;
        sf::Sprite candleSprite_;
        sfml_util::AnimationUPtr_t candleAnimUPtr_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_INNSTAGE_HPP_INCLUDED
