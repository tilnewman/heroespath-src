//
// armor-info.cpp
//
#include "armor-info.hpp"


namespace heroespath
{
namespace item
{
namespace armor
{

    ArmorInfo::ArmorInfo(const armor_type::Enum TYPE)
    :
        type        (TYPE),
        is_aventail (false),
        is_bracer   (false),
        is_shirt    (false),
        is_boots    (false),
        is_pants    (false),
        is_gauntlets(false),
        base        (base_type::Count),
        cover       (cover_type::Count),
        helm        (helm_type::Count),
        shield      (shield_type::Count)
    {}

}
}
}
