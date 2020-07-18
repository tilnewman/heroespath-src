// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-profile-warehouse.cpp
//
#include "item-profile-warehouse.hpp"

#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "creature/traits-set.hpp"
#include "item/item-profile-factory.hpp"
#include "item/item-template-factory.hpp"
#include "item/materials-factory.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/vectors.hpp"

#include <SFML/System/Clock.hpp>

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace heroespath
{
namespace item
{

    std::unique_ptr<ItemProfileWarehouse> ItemProfileWarehouse::instanceUPtr_;

    ItemProfileWarehouse::ItemProfileWarehouse()
        : normal_()
        , religious_()
        , quest_()
        , fallback_()
    {
        M_HP_LOG_DBG("Subsystem Construction: ItemProfileWarehouse");
    }

    ItemProfileWarehouse::~ItemProfileWarehouse()
    {
        M_HP_LOG_DBG("Subsystem Destruction: ItemProfileWarehouse");
    }

    misc::NotNull<ItemProfileWarehouse *> ItemProfileWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR(
                "Subsystem Instance() called but instanceUPtr_ was null: ItemProfileWarehouse");
            Create();
        }

        return misc::NotNull<ItemProfileWarehouse *>(instanceUPtr_.get());
    }

    void ItemProfileWarehouse::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ItemProfileWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: ItemProfileWarehouse");
        }
    }

    void ItemProfileWarehouse::Destroy() { instanceUPtr_.reset(); }

    void ItemProfileWarehouse::Initialize()
    {
        M_HP_LOG_DBG("ItemProfileWarehouse::Initialize() start");
        sf::Clock totalClock;

        normal_.clear();

        // As of 2017-8-17 there were 849299 raw item profiles created before cleanup.
        // As of 2018-3-17 there were 465091 profiles with no duplicates to cleanup.
        // As of 2018-4-28 457935
        // As of 2018-5-2 236585 (reduced the number of element enchantments)
        // As oF 2018-5-8 59370
        // 2018-5-17 44057 (fixed a bug in materials factory that was allowing invalid materials)
        // 2019-5-19 43775 ?
        normal_.reserve(45000);

        // As of 2018-3-19 there were 251 religious profiles.
        // As of 2018-4-28 107
        // As of 2018-5-2 392
        // As of 2018-5-8 313
        // 2019-5-19 317
        religious_.reserve(320);

        quest_.reserve(1024);

        // this was the size measured on 2018-3-19
        fallback_.reserve(1024);

        sf::Clock jobClock;

        auto logJobDuration
            = [&](const std::string & NAME, const std::string & EXTRA_MESSAGE = "") {
                  M_HP_LOG_DBG(
                      "ItemProfileWarehouse::Initialize() Profiles \""
                      << NAME << "\" took " << jobClock.getElapsedTime().asMilliseconds() << "ms.  "
                      << EXTRA_MESSAGE);
                  jobClock.restart();
              };

        MakeStandardEquipment();
        logJobDuration("MakeStandardEquipment");

        MakeMiscNonSummoning();
        logJobDuration("MakeMiscNonSummoning");

        MakeMiscSummoning();
        logJobDuration("MakeMiscSummoning");

        MakeNamed();
        logJobDuration("MakeNamed");

        MakeSet();
        logJobDuration("MakeSet");

        {
            const auto COUNT_BEFORE = normal_.size();

            normal_.erase(
                std::remove_if(
                    std::begin(normal_),
                    std::end(normal_),
                    [](const ItemProfile & PROFILE) { return PROFILE.IsBodyPart(); }),
                std::end(normal_));

            const auto COUNT_AFTER = normal_.size();

            std::string extraMessage;
            extraMessage += "Removed ";
            extraMessage += std::to_string(COUNT_BEFORE - COUNT_AFTER);
            extraMessage += " Normal Equipemnt BodyPart profiles.";

            logJobDuration("MakeRemoveEquipmentBodyPartProfiles_FromNormal", extraMessage);
        }

        {
            const auto COUNT_BEFORE = religious_.size();

            religious_.erase(
                std::remove_if(
                    std::begin(religious_),
                    std::end(religious_),
                    [](const ItemProfile & PROFILE) { return PROFILE.IsBodyPart(); }),
                std::end(religious_));

            const auto COUNT_AFTER = religious_.size();

            std::string extraMessage;
            extraMessage += "Removed ";
            extraMessage += std::to_string(COUNT_BEFORE - COUNT_AFTER);
            extraMessage += " Religious Equipemnt BodyPart profiles.";

            logJobDuration("MakeRemoveEquipmentBodyPartProfiles_FromReligious", extraMessage);
        }

        {
            const auto COUNT_BEFORE = religious_.size();

            religious_.erase(
                std::unique(std::begin(religious_), std::end(religious_)), std::end(religious_));

            const auto COUNT_AFTER = religious_.size();

            std::string extraMessage;
            extraMessage += "Removed ";
            extraMessage += std::to_string(COUNT_BEFORE - COUNT_AFTER);
            extraMessage += " DUPLICATE Religious profiles.";

            logJobDuration("MakeRemoveDuplicates_FromReligious", extraMessage);
        }

        normal_.shrink_to_fit();
        religious_.shrink_to_fit();
        quest_.shrink_to_fit();
        logJobDuration("shrink_to_fit");

        // correct operation of the Treasure Stage requires sorting by PROFILE.Score()
        // see operator< in ItemProfile.hpp

        std::sort(std::begin(normal_), std::end(normal_));
        logJobDuration("sort normal");

        std::sort(std::begin(religious_), std::end(religious_));
        logJobDuration("sort religious");

        std::sort(std::begin(quest_), std::end(quest_));
        logJobDuration("sort quest");

        {
            // this whole block assumes normal_ is already sorted by score

            const auto DOUBLE_MIN_SCORE { normal_[0].Score() * 2_score };

            for (const auto & PROFILE : normal_)
            {
                if (PROFILE.Score() <= DOUBLE_MIN_SCORE)
                {
                    fallback_.emplace_back(PROFILE);
                }
                else
                {
                    break;
                }
            }

            M_HP_LOG_WRN(
                "fallback_ item profiles count=" << fallback_.size() << " with scores from "
                                                 << fallback_.front().Score() << " to "
                                                 << fallback_.back().Score());

            M_HP_ASSERT_OR_LOG_AND_THROW((!fallback_.empty()), "fallback_ was empty");

            fallback_.shrink_to_fit();
            std::sort(std::begin(fallback_), std::end(fallback_));
        }
        logJobDuration("MakeFallback");

        M_HP_LOG_DBG(
            "ItemProfileWarehouse::Initialize() end, duration="
            << totalClock.getElapsedTime().asMilliseconds() << "ms, normal_profile_count="
            << normal_.size() << ", religious_profile_count=" << religious_.size());

        // TODO remove after testing
        {
            auto temp = normal_;

            const auto COUNT_BEFORE = temp.size();

            temp.erase(
                std::unique(
                    std::begin(temp),
                    std::end(temp),
                    [](const ItemProfile & A, const ItemProfile & B) {
                        return (A.Score() == B.Score());
                    }),
                std::end(temp));

            const auto COUNT_AFTER = temp.size();

            M_HP_LOG_WRN(
                "ItemProfileWarehouse::Initialize() treasure score info:  out of a total of "
                << normal_.size() << " items:  min=" << normal_.front().Score()
                << ", max=" << normal_.back().Score() << ", unique=" << temp.size()
                << ", duplicate=" << (COUNT_BEFORE - COUNT_AFTER));
        }
    }

    void ItemProfileWarehouse::MakeStandardEquipment()
    {
        for (const auto & TEMPLATE : ItemTemplateFactory::Instance()->MakeAllWeapons())
        {
            MakeLoopOverElements(TEMPLATE);
        }

        for (const auto & TEMPLATE : ItemTemplateFactory::Instance()->MakeAllArmor())
        {
            MakeLoopOverElements(TEMPLATE);
        }
    }

    void ItemProfileWarehouse::MakeMiscNonSummoning()
    {
        for (const auto & TEMPLATE : ItemTemplateFactory::Instance()->MakeAllMisc())
        {
            const auto MISC = TEMPLATE.MiscTypeBase();

            if (!Misc::IsSummoning(MISC))
            {
                MakeLoopOverElements(TEMPLATE, MISC);
            }
        }
    }

    void ItemProfileWarehouse::MakeMiscSummoning()
    {
        std::vector<creature::SummonInfo> summonInfos;

        for (EnumUnderlying_t raceIndex(0); raceIndex < creature::race::Count; ++raceIndex)
        {
            const auto RACE { static_cast<creature::race::Enum>(raceIndex) };

            for (const auto ROLE : creature::race::Roles(RACE))
            {
                for (const auto ORIGIN : creature::race::OriginTypes(RACE, ROLE))
                {
                    summonInfos.emplace_back(ORIGIN, RACE, ROLE);
                }
            }
        }

        summonInfos.erase(
            std::unique(std::begin(summonInfos), std::end(summonInfos)), std::end(summonInfos));

        for (const auto & SUMMON_INFO : summonInfos)
        {
            const auto MISC = MiscTypeOfSummoningItem(SUMMON_INFO);

            MakeLoopOverElements(
                ItemTemplateFactory::Instance()->MakeMisc(MISC),
                MISC,
                Named::Count,
                Set::Count,
                SUMMON_INFO);
        }
    }

    void ItemProfileWarehouse::MakeNamed()
    {
        for (const auto & NAMED_PAIR : ItemTemplateFactory::Instance()->MakeAllNamed())
        {
            for (const auto & TEMPLATE : NAMED_PAIR.second)
            {
                MakeLoopOverElements(TEMPLATE, TEMPLATE.MiscTypeBase(), NAMED_PAIR.first);
            }
        }
    }

    void ItemProfileWarehouse::MakeSet()
    {
        for (const auto & SET_PAIR : ItemTemplateFactory::Instance()->MakeAllSet())
        {
            for (const auto & TEMPLATE : SET_PAIR.second)
            {
                MakeLoopOverElements(
                    TEMPLATE, TEMPLATE.MiscTypeBase(), Named::Count, SET_PAIR.first);
            }
        }
    }

    ElementList::Enum ItemProfileWarehouse::ElementsAllowed(
        const ItemTemplate & TEMPLATE, const Named::Enum NAMED, const Set::Enum SET) const
    {
        if (SET < Set::Count)
        {
            return ElementList::None;
        }
        else if (NAMED < Named::Count)
        {
            return Named::ElementsAllowed(NAMED);
        }
        else
        {
            return TEMPLATE.ElementsAllowedBase();
        }
    }

    Misc::Enum ItemProfileWarehouse::MiscTypeOfSummoningItem(
        const creature::SummonInfo & SUMMON_INFO) const
    {
        if (SUMMON_INFO.Race() == creature::race::Spider)
        {
            return Misc::SpiderEggs;
        }

        const auto ORIGIN = SUMMON_INFO.Type();

        if (ORIGIN == creature::origin_type::Statue)
        {
            return Misc::SummoningStatue;
        }
        else if (ORIGIN == creature::origin_type::Egg)
        {
            return Misc::Egg;
        }
        else if (ORIGIN == creature::origin_type::Embryo)
        {
            return Misc::Embryo;
        }
        else
        {
            return Misc::Seeds;
        }
    }

    void ItemProfileWarehouse::MakeLoopOverElements(
        const ItemTemplate & TEMPLATE,
        const Misc::Enum MISC,
        const Named::Enum NAMED,
        const Set::Enum SET,
        const creature::SummonInfo & SUMMON_INFO)
    {
        const auto ALLOWED_ELEMENTS = ElementsAllowed(TEMPLATE, NAMED, SET);

        EnumUnderlying_t flag { 0 };

        while (flag <= ElementList::Last)
        {
            if ((0 == flag) || (ALLOWED_ELEMENTS & flag))
            {
                MakeLoopOverMaterials(
                    TEMPLATE,
                    MISC,
                    NAMED,
                    SET,
                    SUMMON_INFO,
                    ElementList::FromListBitFlag(static_cast<ElementList::Enum>(flag)));
            }

            if (0 == flag)
            {
                flag = 1;
            }
            else
            {
                flag <<= 1;
            }
        }
    }

    void ItemProfileWarehouse::MakeLoopOverMaterials(
        const ItemTemplate & TEMPLATE,
        const Misc::Enum MISC,
        const Named::Enum NAMED,
        const Set::Enum SET,
        const creature::SummonInfo & SUMMON_INFO,
        const Element::Enum ELEMENT)
    {
        const auto IS_MAGICAL
            = ((NAMED < Named::Count) || (SET < Set::Count) || (ELEMENT != Element::None)
               || Misc::IsAlwaysMagical(MISC) || SUMMON_INFO.IsValid());

        for (const auto & MATERIAL_PAIR :
             MaterialFactory::Instance()->MakePairs(TEMPLATE, MISC, NAMED, SET, IS_MAGICAL))
        {
            AppendBothPixieVersionsToCollection(
                TEMPLATE, MISC, NAMED, SET, SUMMON_INFO, ELEMENT, MATERIAL_PAIR);
        }
    }

    void ItemProfileWarehouse::AppendBothPixieVersionsToCollection(
        const ItemTemplate & TEMPLATE,
        const Misc::Enum MISC,
        const Named::Enum NAMED,
        const Set::Enum SET,
        const creature::SummonInfo & SUMMON_INFO,
        const Element::Enum ELEMENT,
        const MaterialPair_t & MATERIAL_PAIR)
    {
        AppendToCollection(ItemProfileFactory::Make(
            TEMPLATE,
            false,
            MATERIAL_PAIR.first,
            MATERIAL_PAIR.second,
            NAMED,
            SET,
            ELEMENT,
            MISC,
            SUMMON_INFO));

        if (TEMPLATE.HasPixieVersion())
        {
            AppendToCollection(ItemProfileFactory::Make(
                TEMPLATE,
                true,
                MATERIAL_PAIR.first,
                MATERIAL_PAIR.second,
                NAMED,
                SET,
                ELEMENT,
                MISC,
                SUMMON_INFO));
        }
    }

    void ItemProfileWarehouse::AppendToCollection(const ItemProfile & PROFILE)
    {
        if (PROFILE.IsQuest())
        {
            quest_.emplace_back(PROFILE);
        }
        else if (PROFILE.IsReligious())
        {
            religious_.emplace_back(PROFILE);
        }
        else
        {
            normal_.emplace_back(PROFILE);
        }
    }

} // namespace item
} // namespace heroespath
