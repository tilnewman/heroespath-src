#ifndef SFMLUTIL_BACKGROUNDINFO_INCLUDED
#define SFMLUTIL_BACKGROUNDINFO_INCLUDED
//
// background-info.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gradient.hpp"

#include <memory>


namespace sfml_util
{
namespace gui
{

    //Encapsulates all the info needed to display a background.
    //Note:  Region width and height can be more or less than the image dimmensions
    //Note:  The default background color is white because that is the default
    //       overlay color for a sprite.  Transparent would make the sprite image
    //       disapear.
    //Note:  Designed so that using the default constructor (with no params) is
    //       safe and causes nothing to be drawn.
    struct BackgroundInfo
    {
        //use this constructor when no background is desired
        BackgroundInfo();

        //use this constructor when only a filled-rectangle background is desired
        explicit BackgroundInfo(const sf::Color &    FILL_COLOR,
                                const GradientInfo & GRADIENT_INFO = GradientInfo());

        //use this constructor when the sf::Texture of the background image already exists
        explicit BackgroundInfo(const TextureSPtr_t & TEXTURESPTR,
                                const sf::FloatRect & REGION        = sf::FloatRect(),
                                const sf::Color &     COLOR         = sf::Color::White,
                                const GradientInfo &  GRADIENT_INFO = GradientInfo());

        //use this constructor when only the path to the background image is known
        explicit BackgroundInfo(const std::string &   TEXTURE_PATH,
                                const sf::FloatRect & REGION        = sf::FloatRect(),
                                const sf::Color &     COLOR         = sf::Color::White,
                                const GradientInfo &  GRADIENT_INFO = GradientInfo());

        //returns false if either region dimmension is zero or less
        inline bool IsValid() const { return ((region.width > 0.0f) && (region.height > 0.0f)); }

        sf::FloatRect region;
        TextureSPtr_t textureSPtr;
        sf::Color     color;
        std::string   path;
        GradientInfo  gradient_info;
    };

    bool operator==(const BackgroundInfo & L, const BackgroundInfo & R);

    bool operator!=(const BackgroundInfo & L, const BackgroundInfo & R);

    using BackgroundInfoSPtr_t = std::shared_ptr<BackgroundInfo>;

}
}

#endif //SFMLUTIL_BACKGROUNDINFO_INCLUDED
