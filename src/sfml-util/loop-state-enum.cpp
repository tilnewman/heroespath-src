// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// loop-state-enum.cpp
//
#include "loop-state-enum.hpp"

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace sfml_util
{

    const std::string LoopState::ToString(const LoopState::Enum E)
    {
        switch (E)
        {
            case None:
            {
                return "None";
            }
            case Intro:
            {
                return "Intro";
            }
            case Menu:
            {
                return "Menu";
            }
            case Load:
            {
                return "Load";
            }
            case Character:
            {
                return "Character";
            }
            case Party:
            {
                return "Party";
            }
            case Adventure:
            {
                return "Adventure";
            }
            case Combat:
            {
                return "Combat";
            }
            case Camp:
            {
                return "Camp";
            }
            case Inn:
            {
                return "Inn";
            }
            case Query:
            {
                return "Query";
            }
            case Popup:
            {
                return "Popup";
            }
            case Credits:
            {
                return "Credits";
            }
            case Settings:
            {
                return "Settings";
            }
            case Exit:
            {
                return "Exit";
            }
            case Inventory:
            {
                return "Inventory";
            }
            case Treasure:
            {
                return "Treasure";
            }
            case Test:
            {
                return "Test";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

} // namespace sfml_util
} // namespace heroespath
