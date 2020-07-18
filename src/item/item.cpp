// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item.cpp
//
#include "item.hpp"

#include "creature/enchantment-factory.hpp"
#include "creature/enchantment-warehouse.hpp"
#include "gui/item-image-paths.hpp"
#include "item/category-enum.hpp"
#include "misc/enum-util.hpp"
#include "misc/serialize-helpers.hpp"
#include "misc/vectors.hpp"

namespace heroespath
{
namespace item
{

    Item::Item(const std::string & NAME, const std::string & DESC, const ItemProfile & PROFILE)
        : name_(NAME)
        , desc_(DESC)
        , profile_(PROFILE)
        , category_(PROFILE.Category())
        , enchantmentsPVec_()
        , enchantmentsToSerializePVec_()
        , imageFilename_(gui::ItemImagePaths::Filename(PROFILE, true))
        , imageFullPath_(gui::ItemImagePaths::PathFromFilename(imageFilename_))
    {
        enchantmentsPVec_ = creature::EnchantmentFactory::MakeAndStore(*this);
    }

    Item::~Item() { creature::EnchantmentWarehouse::Access().Free(enchantmentsPVec_); }

    const std::string Item::Name() const { return ComposeName(name_); }

    const std::string Item::Desc() const
    {
        std::string str;
        str.reserve(desc_.size() + 16);

        str += "A ";

        if (IsMagical())
        {
            str += "magical ";
        }
        else if (IsBroken())
        {
            str += "broken ";
        }

        str += desc_;
        str += '.';
        return str;
    }

    const std::string Item::ShortName() const
    {
        if (IsUnique())
        {
            return ComposeName(std::string(Misc::Name(profile_.MiscType())));
        }
        else if (IsSet())
        {
            return ComposeName(std::string(Set::Name(SetType())) + " " + ReadableName());
        }
        else if (IsNamed())
        {
            return ComposeName(std::string(Named::Name(NamedType())) + " " + ReadableName());
        }
        else
        {
            return BaseName();
        }
    }

    const std::string Item::BaseName() const { return ComposeName(ReadableName()); }

    const std::string Item::ReadableName() const { return profile_.ReadableName(); }

    const std::string Item::ComposeName(const std::string & ROOT_NAME) const
    {
        std::string str;
        str.reserve(16 + ROOT_NAME.size());

        if (IsBroken())
        {
            str += "broken ";
        }

        str += ROOT_NAME;

        if (IsMagical())
        {
            str += " *";
        }

        return str;
    }

    const std::string Item::ToString() const
    {
        std::string str;
        str.reserve(1024);

        str += "name=\"";
        str += misc::Quoted(Name());
        str += "\"";

        str += ", desc=\"";
        str += misc::Quoted(Desc());
        str += "\"";

        str += ", category=";
        str += EnumUtil<heroespath::item::Category>::ToString(category_, EnumStringHow(Wrap::Yes));

        str += profile_.ToString();

        str += "image_file_name=\"";
        str += imageFilename_;
        str += "\"";

        if (enchantmentsPVec_.empty() == false)
        {
            str += ", enchantments={";

            for (const auto & ENCHANTMENT_PTR : enchantmentsPVec_)
            {
                str += ENCHANTMENT_PTR->ToString();
                str += " | ";
            }

            str += '}';
        }

        return str;
    }

    void Item::BeforeSerialize()
    {
        misc::SerializeHelp::BeforeSerialize(enchantmentsPVec_, enchantmentsToSerializePVec_);
    }

    void Item::AfterSerialize()
    {
        misc::SerializeHelp::AfterSerialize(
            enchantmentsToSerializePVec_, creature::EnchantmentWarehouse::Access());
    }

    void Item::AfterDeserialize()
    {
        misc::SerializeHelp::AfterDeserialize(
            enchantmentsPVec_,
            enchantmentsToSerializePVec_,
            creature::EnchantmentWarehouse::Access());
    }

    // ignore name_/desc_/imageFilename_/imageFullPath_ since random or change betweeen load/save
    bool operator==(const Item & L, const Item & R)
    {
        return (
            (L.profile_ == R.profile_) && (L.category_ == R.category_)
            && misc::Vector::OrderlessCompareLess(L.enchantmentsPVec_, R.enchantmentsPVec_));
    }

    // ignore name_/desc_/imageFilename_/imageFullPath_ since random or change betweeen load/save
    bool operator<(const Item & L, const Item & R)
    {
        if (L.profile_ != R.profile_)
        {
            return (L.profile_ < R.profile_);
        }
        else if (L.category_ != R.category_)
        {
            return (L.category_ < R.category_);
        }
        else
        {
            return misc::Vector::OrderlessCompareLess(L.enchantmentsPVec_, R.enchantmentsPVec_);
        }
    }

} // namespace item
} // namespace heroespath
