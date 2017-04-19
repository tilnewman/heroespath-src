#ifndef HEROESPATH_STAGE_CREDIT_INCLUDED
#define HEROESPATH_STAGE_CREDIT_INCLUDED
//
// credit.hpp
//
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/animation.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
    }
}
namespace heroespath
{
namespace stage
{

    struct credit_media_type
    {
        enum Enum
        {
            None = 0,
            Image,
            AnimSingleTexture,
            AnimMultiTexture,
            Count
        };
    };


    //responsibe for drawing a credit on screen
    class Credit
    {
    public:
        Credit(sf::FloatRect &               creditsRegion,
               const std::string &           TITLE,
               const std::string &           CONTENT_TEXT,
               const credit_media_type::Enum MEDIA_TYPE            = credit_media_type::None,
               const std::string &           MEDIA_PATH            = "",
               const float                   MEDIA_SCALE           = 1.0f,
               const sf::Vector2f &          MEDIA_POS_ADJUSTMENT  = sf::Vector2f(0.0f, 0.0f),
               const float                   ANIM_FRAME_TIME_SEC   = 0.0f,
               const unsigned int            ANIM_FRAME_COUNT      = 0,
               const unsigned int            ANIM_FRAME_SIZE_HORIZ = 0,
               const unsigned int            ANIM_FRAME_SIZE_VERT  = 0);

        Credit(sf::FloatRect &               creditsRegion,
               const std::string &           TITLE,
               const sfml_util::FontSPtr_t & FONT_SPTR,
               const std::string &           CONTENT_TEXT,
               const credit_media_type::Enum MEDIA_TYPE     = credit_media_type::Image,
               const std::string &           MEDIA_PATH     = "media-images-logos-openfontlicense",
               const float                   MEDIA_SCALE    = sfml_util::MapByRes(1.5f, 5.75f));

        void Setup(sf::FloatRect &                  creditsRegion,
                   const std::string &              TITLE,
                   const sfml_util::FontSPtr_t &    TITLE_FONT,
                   const unsigned int               TITLE_FONT_SIZE,
                   const std::string &              CONTENT_TEXT,
                   const credit_media_type::Enum    MEDIA_TYPE,
                   const std::string &              MEDIA_PATH,
                   const float                      MEDIA_SCALE,
                   const sf::Vector2f &             MEDIA_POS_ADJUSTMENT,
                   const float                      ANIM_FRAME_TIME_SEC,
                   const unsigned int               ANIM_FRAME_COUNT,
                   const unsigned int               ANIM_FRAME_SIZE_HORIZ,
                   const unsigned int               ANIM_FRAME_SIZE_VERT);

        void Draw(sf::RenderTarget & target, sf::RenderStates states);

        void UpdateTime(const float ELAPSED_TIME_SECONDS);

        void Move(const float ADJ_HORIZ, const float ADJ_VERT);

        const sf::Vector2f PositionMedia(const sf::Vector2f &  MEDIA_SIZE,
                                         const sf::FloatRect & CREDITS_REGION,
                                         const sf::Vector2f &  POS_ADJUSTMENTS);

    private:
        sfml_util::gui::TextRegionSPtr_t titleTextRegionSPtr_;
        sfml_util::gui::TextRegionSPtr_t contentTextRegionSPtr_;
        credit_media_type::Enum          contentType_;
        std::string                      path_;
        sf::Vector2f                     posAdj_;
        sf::Texture                      texture_;
        sf::Sprite                       sprite_;
        sfml_util::MultiTextureAnimationSPtr_t multiTextureAnimSPtr_;
        sfml_util::SingleTextureAnimationSPtr_t singleTextureAnimSPtr_;
    };

    using CreditSPtr_t = std::shared_ptr<Credit>;
    using CreditSVec_t = std::vector<CreditSPtr_t>;


}
}
#endif //HEROESPATH_STAGE_CREDIT_INCLUDED
