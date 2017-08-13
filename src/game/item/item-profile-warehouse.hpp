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
#ifndef GAME_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
#define GAME_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
//
// item-profile-warehouse.hpp
//
#include "game/item/item-profile.hpp"

#include <memory>
#include <vector>
#include <utility>


namespace game
{
namespace item
{

    using BaseMaterialVecPair_t = std::pair<armor::base_type::Enum, MaterialVecPair_t>;
    using BaseMaterialVecPairVec_t = std::vector<BaseMaterialVecPair_t>;


    //Responsible for storing an ItemProfile for each possible item in the game
    class ItemProfileWarehouse
    {
        //prevent copy construction
        ItemProfileWarehouse(const ItemProfileWarehouse &) =delete;

        //prevent copy assignment
        ItemProfileWarehouse & operator=(const ItemProfileWarehouse &) =delete;

        //prevent non-singleton construction
        ItemProfileWarehouse();

    public:
        ~ItemProfileWarehouse();

        static ItemProfileWarehouse * Instance();
        static void Acquire();
        static void Release();

        void Setup();

        inline const ItemProfileVec_t & Get() const { return vec_; }

    private:
        void SetupFromThinProfile(const ItemProfile &    THIN_PROFILE,
                                  const named_type::Enum NAMED_TYPE,
                                  const set_type::Enum   SET_TYPE);

        void SetupFromThinProfile(const ItemProfile &,
                                  const named_type::Enum,
                                  const set_type::Enum,
                                  const element_type::Enum,
                                  const material::Enum,
                                  const material::Enum,
                                  const armor::base_type::Enum);

        void SetupFromThinProfile(const ItemProfile &,
                                  const named_type::Enum,
                                  const set_type::Enum,
                                  const ElementTypeVec_t &,
                                  const BaseMaterialVecPairVec_t &);

        const BaseMaterialVecPairVec_t GetMaterialsFromThinProfile(
            const ItemProfile &    THIN_PROFILE,
            const named_type::Enum NAMED_TYPE);

        void SetupProfilesForMiscType(const item::misc_type::Enum);

    private:
        static std::unique_ptr<ItemProfileWarehouse> instanceUPtr_;
        ItemProfileVec_t vec_;
    };

}
}

#endif //GAME_ITEM_ITEMPROFILEWAREHOUSE_HPP_INCLUDED
