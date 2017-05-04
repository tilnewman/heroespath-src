//
// main-menu-title.cpp
//
#include "main-menu-title.hpp"
#include "game/game-data-file.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/loaders.hpp"


namespace game
{

    MainMenuTitle::MainMenuTitle(const std::string & TITLE_KEY_STR,
                                 const bool          WILL_INVERT_SYMBOL,
                                 const float         SYMBOL_SCALE_HORIZ,
                                 const float         SYMBOL_SCALE_VERT)
    :
        symbolTextureSPtr_(),
        symbolSprite_     (),
        titleTextureSPtr_ (),
        titleSprite_      ()
    {
        Setup(TITLE_KEY_STR, WILL_INVERT_SYMBOL, SYMBOL_SCALE_HORIZ, SYMBOL_SCALE_VERT);
    }


    MainMenuTitle::~MainMenuTitle()
    {}


    void MainMenuTitle::Setup(const std::string & TITLE_IMAGE_FILENAME,
                              const bool          WILL_INVERT_SYMBOL,
                              const float         SYMBOL_SCALE_HORIZ,
                              const float         SYMBOL_SCALE_VERT)
    {
        const std::string IMAGE_PATH_KEY((WILL_INVERT_SYMBOL) ? "media-images-gui-accents-symbol2-inv" : "media-images-gui-accents-symbol2");
        sfml_util::LoadImageOrTextureSPtr<sfml_util::TextureSPtr_t>(symbolTextureSPtr_, GameDataFile::Instance()->GetMediaPath(IMAGE_PATH_KEY));

        symbolTextureSPtr_->setSmooth(true);
        symbolSprite_.setTexture( * symbolTextureSPtr_);

        if (TITLE_IMAGE_FILENAME.empty() == false)
        {
            const std::string TITLE_IMAGE_PATH(std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append(TITLE_IMAGE_FILENAME));
            sfml_util::LoadImageOrTextureSPtr<sfml_util::TextureSPtr_t>(titleTextureSPtr_, TITLE_IMAGE_PATH);
            titleTextureSPtr_->setSmooth(true);
            titleSprite_.setTexture( * titleTextureSPtr_ );
        }

        SetPositionAndSize(SYMBOL_SCALE_HORIZ, SYMBOL_SCALE_VERT);
    }


    void MainMenuTitle::SetPositionAndSize(const float SYMBOL_SCALE_HORIZ,
                                           const float SYMBOL_SCALE_VERT)
    {
        const float SYMBOL_IMAGE_SCALE(sfml_util::MapByRes(0.6f, 2.5f));
        symbolSprite_.setScale(SYMBOL_IMAGE_SCALE * SYMBOL_SCALE_HORIZ, SYMBOL_IMAGE_SCALE * SYMBOL_SCALE_VERT);
        symbolSprite_.setPosition((sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (symbolSprite_.getGlobalBounds().width * 0.5f), sfml_util::MapByRes(10.0f, 30.0f));

        if (titleTextureSPtr_.get() != nullptr)
        {
            const float TITLE_IMAGE_SCALE(sfml_util::MapByRes(1.0f, 3.25f));
            titleSprite_.setScale(TITLE_IMAGE_SCALE, TITLE_IMAGE_SCALE);
            titleSprite_.setPosition((sfml_util::Display::Instance()->GetWinWidth() * 0.5f) - (titleSprite_.getGlobalBounds().width * 0.5f),
                                     (symbolSprite_.getPosition().y + (symbolSprite_.getGlobalBounds().height * 0.5f)) - (titleSprite_.getGlobalBounds().height * 0.5f));
        }
    }


    void MainMenuTitle::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(symbolSprite_, states);

        if (titleTextureSPtr_.get() != nullptr)
            target.draw(titleSprite_, states);
    }

}
