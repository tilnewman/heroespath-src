// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enum-image-loader.cpp
//
#include "enum-image-loader.hpp"

#include "game/game-controller.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace EnumImageLoaderHelpers
    {

        void TestLog(const std::string & MESSAGE)
        {
            game::GameController::Instance()->TestingStrAppend(MESSAGE);
        }

    } // namespace EnumImageLoaderHelpers
} // namespace sfml_util
} // namespace heroespath
