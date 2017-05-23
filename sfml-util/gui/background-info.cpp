//
// background-info.cpp
//
#include "background-info.hpp"

#include <string>


namespace sfml_util
{
namespace gui
{

    BackgroundInfo::BackgroundInfo()
    :
        region       (sf::FloatRect()),
        textureSPtr  (),
        color        (sf::Color::Transparent),
        path         (""),
        gradient_info()
    {}


    BackgroundInfo::BackgroundInfo(const sf::Color &    FILL_COLOR,
                                   const GradientInfo & GRADIENT_INFO)
    :
        region       (sf::FloatRect()),
        textureSPtr  (),
        color        (FILL_COLOR),
        path         (""),
        gradient_info(GRADIENT_INFO)
    {}


    BackgroundInfo::BackgroundInfo( const TextureSPtr_t & TEXTURESPTR,
                                    const sf::FloatRect & REGION,
                                    const sf::Color &     COLOR,
                                    const GradientInfo &  GRADIENT_INFO )
    :
        region       (REGION),
        textureSPtr  (TEXTURESPTR),
        color        (COLOR),
        path         (""),
        gradient_info(GRADIENT_INFO)
    {}


    BackgroundInfo::BackgroundInfo( const std::string &   TEXTURE_PATH,
                                    const sf::FloatRect & REGION,
                                    const sf::Color &     COLOR,
                                    const GradientInfo &  GRADIENT_INFO)
    :
        region       (REGION),
        textureSPtr  (),
        color        (COLOR),
        path         (TEXTURE_PATH),
        gradient_info(GRADIENT_INFO)
    {}


    bool operator==(const BackgroundInfo & L, const BackgroundInfo & R)
    {
        return ((L.region == R.region) &&
                (L.textureSPtr.get() == R.textureSPtr.get()) &&
                (L.color == R.color) &&
                (L.path == R.path) &&
                (L.gradient_info == R.gradient_info));
    }


    bool operator!=(const BackgroundInfo & L, const BackgroundInfo & R)
    {
        return ! (L == R);
    }

}
}
