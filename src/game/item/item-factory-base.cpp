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
// item-factory-base.cpp
//
#include "item-factory-base.hpp"

#include "game/item/item.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/random.hpp"

#include <sstream>


namespace game
{
namespace item
{


    FactoryBase::FactoryBase()
    {}


    FactoryBase::~FactoryBase()
    {}


    const std::string FactoryBase::Make_Name(const std::string &  BASE_NAME,
                                             const material::Enum MATERIAL_PRI,
                                             const material::Enum MATERIAL_SEC,
                                             const bool           IS_PIXIE_ITEM)
    {
        std::ostringstream ssName;
        ssName << material::ToReadableString(MATERIAL_PRI) << " ";

        if ((MATERIAL_SEC != MATERIAL_PRI) && (MATERIAL_SEC != material::Nothing))
        {
            if (material::IsJewel(MATERIAL_SEC))
            {
                ssName << "and "
                    << material::ToReadableString(MATERIAL_SEC)
                    << " "
                    << RandomJeweledAdjective()
                    << " ";
            }
            else if (material::IsPrecious(MATERIAL_SEC))
            {
                ssName << "and "
                    << material::ToReadableString(MATERIAL_SEC)
                    << " "
                    << RandomAdornedAdjective()
                    << " ";
            }
            else if (material::IsLiquid(MATERIAL_SEC))
            {
                ssName << material::ToReadableString(MATERIAL_SEC)
                    << " "
                    << RandomCoatedAdjective()
                    << " ";
            }
            else
            {
                ssName << "and "
                    << material::ToReadableString(MATERIAL_SEC)
                    << " ";
            }
        }

        //don't use the word leather twice in a name
        std::string baseNameToUse(BASE_NAME);
        if ((boost::algorithm::icontains(ssName.str(), "leather")) &&
            (boost::algorithm::icontains(BASE_NAME, "leather")))
        {
            boost::algorithm::ierase_all(baseNameToUse, "leather");
        }

        if (IS_PIXIE_ITEM)
        {
            ssName << " Pixie ";
        }

        ssName << baseNameToUse;
        return ssName.str();
    }


    const std::string FactoryBase::Make_Desc(const std::string &  DESC,
                                             const material::Enum MATERIAL_PRI,
                                             const material::Enum MATERIAL_SEC,
                                             const std::string &  EXTRA_NAME,
                                             const bool           IS_PIXIE_ITEM)
    {
        std::ostringstream ssDesc;
        ssDesc << DESC;

        if (IS_PIXIE_ITEM)
        {
            ssDesc << ", Pixie sized,";
        }

        ssDesc << " made of " << material::ToReadableString(MATERIAL_PRI);
        if ((MATERIAL_SEC != material::Nothing) && (MATERIAL_PRI != MATERIAL_SEC))
        {
            if (EXTRA_NAME.empty())
            {
                ssDesc << " and ";

                if (material::IsJewel(MATERIAL_SEC))
                {
                    ssDesc << RandomJeweledAdjective() << " with ";
                }
                else if (material::IsPrecious(MATERIAL_SEC))
                {
                    //at this point we know MATERIAL_SEC is either silver/gold/platinum
                    ssDesc << RandomAdornedAdjective() << " with ";
                }
                else if (material::IsLiquid(MATERIAL_SEC))
                {
                    ssDesc << RandomCoatedPhrase() << " ";
                }

                ssDesc << material::ToString(MATERIAL_SEC);
            }
            else
            {
                ssDesc << " with a ";

                if (material::IsRigid(MATERIAL_SEC))
                {
                    ssDesc << material::ToReadableString(MATERIAL_SEC) << " ";

                    if (material::IsJewel(MATERIAL_SEC))
                    {
                        ssDesc << RandomJeweledAdjective()
                            << " ";
                    }
                    else if (material::IsPrecious(MATERIAL_SEC))
                    {
                        //at this point we know MATERIAL_SEC is either silver/gold/platinum
                        ssDesc << RandomAdornedAdjective()
                            << " ";
                    }
                    
                    ssDesc << EXTRA_NAME;
                }
                else
                {
                    ssDesc << material::ToReadableString(MATERIAL_SEC)
                        << " "
                        << RandomCoatedPhrase()
                        << " "
                        << EXTRA_NAME;
                }
            }
        }

        ssDesc << ".";
        return ssDesc.str();
    }


    const std::string FactoryBase::Make_Desc_Clasped(const std::string &  DESC,
                                                    const material::Enum MATERIAL_PRI,
                                                    const material::Enum MATERIAL_SEC,
                                                    const bool           IS_PIXIE_ITEM)
    {
        std::ostringstream ssDesc;
        ssDesc << DESC;

        if (IS_PIXIE_ITEM)
        {
            ssDesc << ", Pixie sized,";
        }

        ssDesc << " made of " << material::ToReadableString(MATERIAL_PRI);

        if ((MATERIAL_SEC != material::Nothing) && (MATERIAL_SEC != MATERIAL_PRI))
        {
            if (material::IsJewel(MATERIAL_SEC))
            {
                ssDesc << " with a " << material::ToReadableString(MATERIAL_SEC)
                    << RandomJeweledAdjective()
                    << " "
                    << RandomClaspNoun();
            }
            else if (material::IsMetal(MATERIAL_SEC))
            {
                ssDesc << " with a "
                    << material::ToReadableString(MATERIAL_SEC)
                    << " "
                    << RandomChainNoun();
            }
            else if (material::IsLiquid(MATERIAL_SEC))
            {
                ssDesc << " "
                    << RandomCoatedPhrase() 
                    << material::ToReadableString(MATERIAL_SEC);
            }
            else if (material::IsRigid(MATERIAL_SEC))
            {
                ssDesc << " with a "
                    << material::ToReadableString(MATERIAL_SEC)
                    << " "
                    << RandomClaspNoun();
            }
            else
            {
                ssDesc << " and " << material::ToReadableString(MATERIAL_SEC);
            }
        }

        ssDesc << ".";
        return ssDesc.str();
    }


    const std::string FactoryBase::Make_Desc_BladdedStaff(const std::string &  BASE_NAME,
                                                          const bool           IS_SPEAR,
                                                          const material::Enum MATERIAL_PRI,
                                                          const material::Enum MATERIAL_SEC)
    {
        std::ostringstream ssDesc;
        ssDesc << "A "
            << BASE_NAME
            << " made of "
            << material::ToReadableString(MATERIAL_PRI);

        if ((MATERIAL_SEC != MATERIAL_PRI) && (MATERIAL_SEC != material::Nothing))
        {
            if (material::IsJewel(MATERIAL_SEC))
            {
                ssDesc << " "
                    << RandomJeweledAdjective()
                    << " with ";
            }
            else if (material::IsPrecious(MATERIAL_SEC))
            {
                //at this point we know MATERIAL_SEC is either silver/gold/platinum
                ssDesc << " "
                    << RandomAdornedAdjective()
                    << " with ";
            }
            else if (material::IsRigid(MATERIAL_SEC))
            {
                if (IS_SPEAR)
                {
                    ssDesc << " tipped with ";
                }
                else
                {
                    ssDesc << " bladed with ";
                }
            }
            else if (material::IsLiquid(MATERIAL_SEC))
            {
                ssDesc << " and coated in ";
            }
            else
            {
                ssDesc << " and ";
            }

            ssDesc << material::ToReadableString(MATERIAL_SEC) << ".";
        }

        return ssDesc.str();
    }


    void FactoryBase::AdjustPrice(Coin_t &             price,
                                  const material::Enum MATERIAL_PRI,
                                  const material::Enum MATERIAL_SEC,
                                  const bool           IS_PIXIE_ITEM)
    {
        price += material::PriceAdj(MATERIAL_PRI, MATERIAL_SEC);

        if (IS_PIXIE_ITEM)
        {
            price = static_cast<Coin_t>(static_cast<float>(price) * 1.5f);
        }
    }


    void FactoryBase::AdjustWeight(Weight_t &           weight,
                                   const material::Enum MATERIAL_PRI,
                                   const material::Enum MATERIAL_SEC)
    {
        weight = static_cast<Weight_t>(static_cast<float>(weight) *
            material::WeightMult(MATERIAL_PRI, MATERIAL_SEC));
    }


    void FactoryBase::AdjustArmorRating(stats::Armor_t &    armorRating,
                                        const material::Enum MATERIAL_PRI,
                                        const material::Enum MATERIAL_SEC)
    {
        armorRating += material::ArmorRatingBonus(MATERIAL_PRI, MATERIAL_SEC);
    }


    const std::string FactoryBase::RandomCoatedPhrase()
    {
        if (misc::random::Bool())
        {
            return "dripping with";
        }
        else
        {
            return RandomCoatedAdjective() + " in";
        }
    }


    const std::string FactoryBase::RandomCoatedAdjective()
    {
        switch (misc::random::Int(3))
        {
            case 0:  { return "covered"; }
            case 1:  { return "soaked"; }
            case 2:  { return "coated"; }
            case 3:
            default: { return "drenched"; }
        }
    }


    const std::string FactoryBase::RandomJeweledAdjective()
    {
        auto const RAND{ misc::random::Int(2) };
        if (RAND == 0)
        {
            return "jeweled";
        }
        else if (RAND == 1)
        {
            return "gemmed";
        }
        else
        {
            return "bejeweled";
        }
    }


    const std::string FactoryBase::RandomAdornedAdjective()
    {
        auto const RAND{ misc::random::Int(2) };
        if (RAND == 0)
        {
            return "adorned";
        }
        else if (RAND == 1)
        {
            return "decorated";
        }
        else
        {
            return "trimmed with";
        }
    }


    const std::string FactoryBase::RandomChainNoun()
    {
        switch (misc::random::Int(3))
        {
            case 1:  { return "tie"; }
            case 2:  { return "tether"; }
            case 3:  { return "fastener"; }
            case 0:
            default: { return "chain"; }
        }
    }


    const std::string FactoryBase::RandomClaspNoun()
    {
        switch(misc::random::Int(2))
        {
            case 1:  { return "clasp"; }
            case 2:  { return "tether"; }
            case 0:
            default: { return "fastener"; }
        }
    }

}
}
