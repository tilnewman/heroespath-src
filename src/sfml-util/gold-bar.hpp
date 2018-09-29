// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GOLD_BAR_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GOLD_BAR_HPP_INCLUDED
//
// gold-bar.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/orientation-enum.hpp"
#include "sfml-util/side-enum.hpp"
#include "sfutil/vector-and-rect.hpp"
#include "sfutil/vertex.hpp"

#include <SFML/Graphics/Drawable.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Reponsible for storing and drawing a gui gold bar, which is just a fat line that is
    // stylized to the game's gui.  These bars have shadows so you have to specify the side.  If
    // the default constructor is used or length < 0.0f or region.width/height < 1.0f, then a
    // valid object will be created that draws nothing.  This this class is just a fancy
    // sf::VertexArray wrapper where all the vertexes share a common texture (GuiElements), you
    // can construct a single GoldBar from one or more others, or just from a custom vector of
    // vertexes.  You can also specify a region to make a gold bar border around any region.
    class GoldBar : public sf::Drawable
    {
    public:
        GoldBar();

        GoldBar(
            const float LEFT,
            const float TOP,
            const float LENGTH,
            const Orientation::Enum ORIENTATION,
            const Side::Enum SIDE,
            const bool WILL_CAP_ENDS);

        // ORIENTATION is only used to determine which region dimmension is the length, defaults
        // to longest dimmension
        explicit GoldBar(
            const std::vector<GoldBar> &, const Orientation::Enum ORIENTATION = Orientation::Count);

        // ORIENTATION is only used to determine which region dimmension is the length, defaults
        // to longest dimmension
        explicit GoldBar(
            const sf::VertexArray &, const Orientation::Enum ORIENTATION = Orientation::Count);

        // ORIENTATION is only used to determine which dimmension is the length, defaults to
        // longest dimmension
        explicit GoldBar(
            const sf::FloatRect & REGION,
            const Orientation::Enum ORIENTATION = Orientation::Count,
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

        GoldBar(const GoldBar &) = default;
        GoldBar(GoldBar &&) = default;
        GoldBar & operator=(const GoldBar &) = default;
        GoldBar & operator=(GoldBar &&) = default;

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        void Setup(
            const float LEFT,
            const float TOP,
            const float LENGTH,
            const Orientation::Enum ORIENTATION,
            const Side::Enum SIDE,
            const bool WILL_CAP_ENDS);

        void Setup(
            const sf::FloatRect & REGION,
            const Orientation::Enum ORIENTATION = Orientation::Count,
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

        // if a rect border then the max dimmension is returned
        float Length() const { return length_; }

        // returns sf::Color::Transparent if !WillDraw()
        const sf::Color Color() const;

        void Color(const sf::Color &);

        // does not actually set the position, instead it moves relative to the orig pos
        void SetPos(const float POS_LEFT, const float POS_TOP);

        void MovePos(const float HORIZ, const float VERT);

        // if WILL_GROW_BORDER_TO_CONTAIN_REGION==false returns the orig region
        const sf::FloatRect OuterRegion() const { return outerRegion_; }

        // if WILL_GROW_BORDER_TO_CONTAIN_REGION==true returns the orig region
        const sf::FloatRect InnerRegion() const { return innerRegion_; }

        bool WillDraw() const { return (vertexArray_.getVertexCount() > 0); }

        bool WasBorderGrown() const { return willGrowBorderToContainRegion_; }

        friend bool operator<(const GoldBar & L, const GoldBar & R);
        friend bool operator==(const GoldBar & L, const GoldBar & R);

    private:
        // return is ordered: top,bottom,middle
        const std::tuple<sf::FloatRect, sf::FloatRect, sf::FloatRect>
            GetTextureRectsBar(const Orientation::Enum, const Side::Enum) const;

        void SetupVertexesAndRegionsForSingleBar(
            const float LEFT,
            const float TOP,
            const float LENGTH,
            const Orientation::Enum ORIENTATION,
            const Side::Enum SIDE,
            const bool WILL_CAP_ENDS);

        void SetupVertexesAndRegionsForSquare(
            const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION);

        void SetupLengthToDefault(const Orientation::Enum ORIENTATION);

        static const std::string IMAGE_PATH_KEY_;

        float length_;
        sf::FloatRect innerRegion_;
        sf::FloatRect outerRegion_;
        sf::VertexArray vertexArray_;
        CachedTexture guiElementsCachedTexture_;
        bool willGrowBorderToContainRegion_;
        misc::NotNull<const sf::Texture *> texturePtr_;
    };

    using GoldBarOpt_t = boost::optional<GoldBar>;
    using GoldBarUPtr_t = std::unique_ptr<GoldBar>;

    // intetionally ignoring the texture because it will be the same for all GoldBars
    inline bool operator<(const GoldBar & L, const GoldBar & R)
    {
        return std::tie(L.length_, L.outerRegion_, L.innerRegion_, L.vertexArray_)
            < std::tie(R.length_, R.outerRegion_, R.innerRegion_, R.vertexArray_);
    }

    // intetionally ignoring the texture because it will be the same for all GoldBars
    inline bool operator==(const GoldBar & L, const GoldBar & R)
    {
        return std::tie(L.length_, L.outerRegion_, L.innerRegion_, L.vertexArray_)
            == std::tie(R.length_, R.outerRegion_, R.innerRegion_, R.vertexArray_);
    }

    // intetionally ignoring the texture because it will be the same for all GoldBars
    inline bool operator!=(const GoldBar & L, const GoldBar & R) { return !(L == R); }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GOLD_BAR_HPP_INCLUDED
