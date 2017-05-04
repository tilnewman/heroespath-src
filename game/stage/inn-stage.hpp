#ifndef GAME_INNSTAGE_INCLUDED
#define GAME_INNSTAGE_INCLUDED
//
// inn-stage.hpp
//  A Stage class that allows starting the game
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/gui/background-image.hpp"

#include "game/main-menu-buttons.hpp"
#include "game/i-popup-callback.hpp"
#include "game/horiz-symbol.hpp"

#include <memory>


namespace sfml_util
{
    class SingleTextureAnimation;
    using SingleTextureAnimationSPtr_t = std::shared_ptr<SingleTextureAnimation>;
}
namespace game
{
    class Ouroboros;
    using OuroborosSPtr_t = std::shared_ptr<Ouroboros>;

namespace stage
{

    //A Stage class that allows starting the game
    class InnStage
    :
        public sfml_util::Stage
    {
        //prevent copy construction
        InnStage(const InnStage &);

        //prevent copy assignment
        InnStage & operator=(const InnStage &);

    public:
        InnStage();
        virtual ~InnStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        sf::Texture titleSymbolTexture_;
        sf::Sprite  titleSymbolSprite_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sf::Texture candleTexture_;
        sf::Sprite  candleSprite_;
        sfml_util::SingleTextureAnimationSPtr_t candleAnimSPtr_;
        OuroborosSPtr_t ouroborosSPtr_;
        BottomSymbol bottomSymbol_;
    };

}
}
#endif //GAME_INNSTAGE_INCLUDED
