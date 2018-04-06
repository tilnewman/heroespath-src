// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// title-image-manager.cpp
//
#include "title-image-manager.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        std::unique_ptr<TitleImageManager> TitleImageManager::instanceUPtr_;

        TitleImageManager::TitleImageManager()
        {
            M_HP_LOG_DBG("Singleton Construction: TitleImageManager");
        }

        TitleImageManager::~TitleImageManager()
        {
            M_HP_LOG_DBG("Singleton Destruction: TitleImageManager");
        }

        misc::NotNull<TitleImageManager *> TitleImageManager::Instance()
        {
            if (!instanceUPtr_)
            {
                M_HP_LOG_ERR("Singleton Instance() before Acquire(): TitleImageManager");
                Acquire();
            }

            return instanceUPtr_.get();
        }

        void TitleImageManager::Acquire()
        {
            if (!instanceUPtr_)
            {
                instanceUPtr_ = std::make_unique<TitleImageManager>();
            }
            else
            {
                M_HP_LOG_ERR("Singleton Acquire() after Construction: TitleImageManager");
            }
        }

        void TitleImageManager::Release()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (instanceUPtr_),
                "sfml_util::gui::TitleImageManager::Release() found instanceUPtr that was null.");

            instanceUPtr_.reset();
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
