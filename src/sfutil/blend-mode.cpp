// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// blend-mode.cpp
//
#include "blend-mode.hpp"

#include "misc/strings.hpp"

#include <ostream>
#include <tuple>

namespace sf
{

bool operator<(const sf::BlendMode & L, const sf::BlendMode & R)
{
    return std::tie(
               L.colorSrcFactor,
               L.colorDstFactor,
               L.colorEquation,
               L.alphaSrcFactor,
               L.alphaDstFactor,
               L.alphaEquation)
        < std::tie(
               R.colorSrcFactor,
               R.colorDstFactor,
               R.colorEquation,
               R.alphaSrcFactor,
               R.alphaDstFactor,
               R.alphaEquation);
}

std::ostream & operator<<(std::ostream & os, const sf::BlendMode & BM)
{
    os << "(";

    if (BM == sf::BlendAlpha)
    {
        os << "Alpha";
    }
    else if (BM == sf::BlendAdd)
    {
        os << "Add";
    }
    else if (BM == sf::BlendMultiply)
    {
        os << "Multiply";
    }
    else if (BM == sf::BlendNone)
    {
        os << "None";
    }
    else
    {
        auto factorToString = [](const sf::BlendMode::Factor FACTOR) -> std::string {
            switch (FACTOR)
            {
                case sf::BlendMode::Factor::Zero:
                {
                    return "Zero";
                }
                case sf::BlendMode::Factor::One:
                {
                    return "One";
                }
                case sf::BlendMode::Factor::SrcColor:
                {
                    return "SrcColor";
                }
                case sf::BlendMode::Factor::OneMinusSrcColor:
                {
                    return "OneMinusSrcColor";
                }
                case sf::BlendMode::Factor::DstColor:
                {
                    return "DstColor";
                }
                case sf::BlendMode::Factor::OneMinusDstColor:
                {
                    return "OneMinusDstColor";
                }
                case sf::BlendMode::Factor::SrcAlpha:
                {
                    return "SrcAlpha";
                }
                case sf::BlendMode::Factor::OneMinusSrcAlpha:
                {
                    return "OneMinusSrcAlpha";
                }
                case sf::BlendMode::Factor::DstAlpha:
                {
                    return "DstAlpha";
                }
                default:
                case sf::BlendMode::Factor::OneMinusDstAlpha:
                {
                    return "OneMinusDstAlpha";
                }
            }
        };

        auto equationToString = [](const sf::BlendMode::Equation EQUATION) -> std::string {
            switch (EQUATION)
            {
                case sf::BlendMode::Equation::Add:
                {
                    return "Add";
                }
                case sf::BlendMode::Equation::Subtract:
                {
                    return "Subtract";
                }
                default:
                case sf::BlendMode::Equation::ReverseSubtract:
                {
                    return "ReverseSubtract";
                }
            }
        };

        os << factorToString(BM.colorSrcFactor) << "," << factorToString(BM.colorDstFactor) << ","
           << equationToString(BM.colorEquation) << "," << factorToString(BM.alphaSrcFactor) << ","
           << factorToString(BM.alphaDstFactor) << "," << equationToString(BM.alphaEquation);
    }

    os << ")";
    return os;
}

} // namespace sf

namespace heroespath
{
namespace sfutil
{

    const std::string ToString(const sf::BlendMode & BM, const misc::ToStringPrefix::Enum OPTIONS)
    {
        std::ostringstream ss;
        ss << misc::MakeToStringPrefix<sf::BlendMode>(OPTIONS, "BlendMode") << BM;
        return ss.str();
    }

} // namespace sfutil
} // namespace heroespath
