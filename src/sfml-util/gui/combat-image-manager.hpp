///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
//
// combat-image-manager.hpp
//
#include "sfml-util/gui/combat-image-enum.hpp"
#include "sfml-util/gui/image-manager-base.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Responsible for loading and dispensing combat images.
        class CombatImageManager : public ImageManagerBase<CombatImageType>
        {
        public:
            CombatImageManager & operator=(const CombatImageManager &) = delete;
            CombatImageManager & operator=(CombatImageManager &&) = delete;
            CombatImageManager(const CombatImageManager &) = delete;
            CombatImageManager(CombatImageManager &&) = delete;
            CombatImageManager() = delete;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COMBATIMAGEMANAGER_HPP_INCLUDED
