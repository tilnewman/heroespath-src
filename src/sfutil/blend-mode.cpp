// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

#include <sstream>
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
    if (BM == sf::BlendAlpha)
    {
        os << "(Alpha)";
    }
    else if (BM == sf::BlendAdd)
    {
        os << "(Add)";
    }
    else if (BM == sf::BlendMultiply)
    {
        os << "(Multiply)";
    }
    else if (BM == sf::BlendNone)
    {
        os << "(None)";
    }
    else
    {
        auto blendModeEquationToString = [](const sf::BlendMode::Equation EQUATION) {
            switch (EQUATION)
            {
                case sf::BlendMode::Equation::Add: { return "Add";
                }
                case sf::BlendMode::Equation::Subtract: { return "Subtract";
                }
                default:
                case sf::BlendMode::Equation::ReverseSubtract: { return "ReverseSubtract";
                }
            }
        };

        auto blendModeFactorToString = [](const sf::BlendMode::Factor FACTOR) {
            switch (FACTOR)
            {
                case sf::BlendMode::Factor::Zero: { return "Zero";
                }
                case sf::BlendMode::Factor::One: { return "One";
                }
                case sf::BlendMode::Factor::SrcColor: { return "SrcColor";
                }
                case sf::BlendMode::Factor::OneMinusSrcColor: { return "OneMinusSrcColor";
                }
                case sf::BlendMode::Factor::DstColor: { return "DstColor";
                }
                case sf::BlendMode::Factor::OneMinusDstColor: { return "OneMinusDstColor";
                }
                case sf::BlendMode::Factor::SrcAlpha: { return "SrcAlpha";
                }
                case sf::BlendMode::Factor::OneMinusSrcAlpha: { return "OneMinusSrcAlpha";
                }
                case sf::BlendMode::Factor::DstAlpha: { return "DstAlpha";
                }
                default:
                case sf::BlendMode::Factor::OneMinusDstAlpha: { return "OneMinusDstAlpha";
                }
            }
        };

        os << "(" << blendModeFactorToString(BM.colorSrcFactor) << ","
           << blendModeFactorToString(BM.colorDstFactor) << ","
           << blendModeEquationToString(BM.colorEquation) << ","
           << blendModeFactorToString(BM.alphaSrcFactor) << ","
           << blendModeFactorToString(BM.alphaDstFactor) << ","
           << blendModeEquationToString(BM.alphaEquation) << ")";
    }

    return os;
}

} // namespace sf
