//
// inn-stage.cpp
//
#include "inn-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "utilz/real.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/animation.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/popup-manager.hpp"

#include "game/ouroboros.hpp"
#include "game/game-data-file.hpp"
#include "game/state/game-state-factory.hpp"
#include "game/loop-manager.hpp"


namespace game
{
namespace stage
{

    InnStage::InnStage()
    :
        Stage              ("Inn"),
        SCREEN_WIDTH_      (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_     (sfml_util::Display::Instance()->GetWinHeight()),
        titleSymbolTexture_(),
        titleSymbolSprite_ (),
        backgroundImage_   ("media-images-backgrounds-tile-darkknot"),
        candleTexture_     (),
        candleSprite_      (),
        candleAnimSPtr_    (),
        ouroborosSPtr_     (),
        bottomSymbol_      ()
    {}


    InnStage::~InnStage()
    {}


    void InnStage::Setup()
    {
        //title symbol
        sfml_util::LoadImageOrTexture<sf::Texture>(titleSymbolTexture_, GameDataFile::Instance()->GetMediaPath("media-images-gui-accents-symbol2"));
        titleSymbolSprite_.setTexture(titleSymbolTexture_);
        titleSymbolSprite_.setScale(0.60f, 0.60f);
        titleSymbolSprite_.setPosition((SCREEN_WIDTH_ * 0.5f) - (titleSymbolSprite_.getGlobalBounds().width * 0.5f), 10.0f);

        //ouroboros
        ouroborosSPtr_.reset( new Ouroboros("InnStage's") );
        EntityAdd(ouroborosSPtr_);

        //candle
        sfml_util::LoadImageOrTexture<sf::Texture>(candleTexture_, GameDataFile::Instance()->GetMediaPath("media-images-candle"));
        candleSprite_.setTexture(candleTexture_);
        candleSprite_.setPosition(SCREEN_WIDTH_ - 200.0f, SCREEN_HEIGHT_ - 200.0f);

        //candle anim
        candleAnimSPtr_.reset( new sfml_util::SingleTextureAnimation("Candle",
                                                                     GameDataFile::Instance()->GetMediaPath("media-anim-images-candleflame"),
                                                                     SCREEN_WIDTH_ - 225.0f,
                                                                     SCREEN_HEIGHT_ - 290.0f,
                                                                     128,
                                                                     128,
                                                                     0.05f,
                                                                     64,
                                                                     sf::BlendAdd) );
        EntityAdd(candleAnimSPtr_);
    }


    void InnStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        bottomSymbol_.Draw(target, STATES);
        Stage::Draw(target, STATES);
    }

}
}
