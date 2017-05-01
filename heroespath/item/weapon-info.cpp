//
// weapon-info.cpp
//
#include "weapon-info.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <exception>
#include <sstream>


namespace heroespath
{
namespace item
{
namespace weapon
{

    WeaponInfo::WeaponInfo(const weapon_type::Enum TYPE, const std::string & NAME)
    :
        name            (NAME),
        type            (TYPE),
        is_bite         (false),
        is_claws        (false),
        is_fists        (false),
        is_staff        (false),
        is_knife        (false),
        is_dagger       (false),
        is_tendrils     (false),
        is_quarterstaff (false),
        is_breath       (false),
        knife_size      (sfml_util::Size::Count),
        sword           (sword_type::Count),
        axe             (axe_type::Count),
        club            (club_type::Count),
        whip            (whip_type::Count),
        projectile      (projectile_type::Count),
        bladedstaff     (bladedstaff_type::Count)
    {
        if (name.empty() == false)
            ProcessName();
    }


    void WeaponInfo::ProcessName()
    {
        if      (name == "claymore")      { sword = sword_type::Claymore;               type = weapon_type::Sword;  }
        else if (name == "longsword")     { sword = sword_type::Longsword;              type = weapon_type::Sword;  }
        else if (name == "flamberg")      { sword = sword_type::Flamberg;               type = weapon_type::Sword;  }
        else if (name == "knightlysword") { sword = sword_type::Knightlysword;          type = weapon_type::Sword;  }
        else if (name == "broadsword")    { sword = sword_type::Broadsword;             type = weapon_type::Sword;  }
        else if (name == "falcata")       { sword = sword_type::Falcata;                type = weapon_type::Sword;  }
        else if (name == "saber")         { sword = sword_type::Saber;                  type = weapon_type::Sword;  }
        else if (name == "cutlass")       { sword = sword_type::Cutlass;                type = weapon_type::Sword;  }
        else if (name == "rapier")        { sword = sword_type::Rapier;                 type = weapon_type::Sword;  }
        else if (name == "gladius")       { sword = sword_type::Gladius;                type = weapon_type::Sword;  }
        else if (name == "shortsword")    { sword = sword_type::Shortsword;             type = weapon_type::Sword;  }
        else if (name == "handaxe")       { axe = axe_type::Handaxe;                    type = weapon_type::Axe;  }
        else if (name == "sickle")        { axe = axe_type::Sickle;                     type = weapon_type::Axe;  }
        else if (name == "battleaxe")     { axe = axe_type::Battleaxe;                  type = weapon_type::Axe;  }
        else if (name == "waraxe")        { axe = axe_type::Waraxe;                     type = weapon_type::Axe;  }
        else if (name == "spiked")        { club = club_type::Spiked;                   type = weapon_type::Club;  }
        else if (name == "maul")          { club = club_type::Maul;                     type = weapon_type::Club;  }
        else if (name == "mace")          { club = club_type::Mace;                     type = weapon_type::Club;  }
        else if (name == "warhammer")     { club = club_type::Warhammer;                type = weapon_type::Club;  }
        else if (name == "bullwhip")      { whip = whip_type::Bullwhip;                 type = weapon_type::Whip;  }
        else if (name == "flail")         { whip = whip_type::Flail;                    type = weapon_type::Whip;  }
        else if (name == "maceandchain")  { whip = whip_type::MaceAndChain;             type = weapon_type::Whip;  }
        else if (name == "blowpipe")      { projectile = projectile_type::Blowpipe;     type = weapon_type::Projectile;  }
        else if (name == "sling")         { projectile = projectile_type::Sling;        type = weapon_type::Projectile;  }
        else if (name == "longbow")       { projectile = projectile_type::Longbow;      type = weapon_type::Projectile;  }
        else if (name == "compositebow")  { projectile = projectile_type::CompositeBow; type = weapon_type::Projectile;  }
        else if (name == "crossbow")      { projectile = projectile_type::Crossbow;     type = weapon_type::Projectile;  }
        else if (name == "spear")         { bladedstaff = bladedstaff_type::Spear;      type = weapon_type::BladedStaff;  }
        else if (name == "shortspear")    { bladedstaff = bladedstaff_type::ShortSpear; type = weapon_type::BladedStaff;  }
        else if (name == "scythe")        { bladedstaff = bladedstaff_type::Scythe;     type = weapon_type::BladedStaff;  }
        else if (name == "pike")          { bladedstaff = bladedstaff_type::Pike;       type = weapon_type::BladedStaff;  }
        else if (name == "partisan")      { bladedstaff = bladedstaff_type::Partisan;   type = weapon_type::BladedStaff;  }
        else if (name == "halberd")       { bladedstaff = bladedstaff_type::Halberd;    type = weapon_type::BladedStaff;  }
        else if (name == "staff")         { is_staff = true;                            type = weapon_type::Staff; }
        else if (name == "quarterstaff")  { is_quarterstaff = true;                     type = weapon_type::Staff; }
        else if (name == "bite")          { is_bite = true;                             type = weapon_type::Bite; }
        else if (name == "breath")        { is_breath = true;                           type = weapon_type::Breath; }
        else if (name == "claws")         { is_claws = true;                            type = weapon_type::Claws; }
        else if (name == "fists")         { is_fists = true;                            type = weapon_type::Fists; }
        else if (name == "tendrils")      { is_tendrils = true;                         type = weapon_type::Tendrils; }
        else if (name == "knife")         { is_knife = true;  knife_size = sfml_util::Size::Medium; type = weapon_type::Knife; }
        else if (name == "(knife)")       { is_knife = true;  type = weapon_type::Knife; }
        else if (name == "dagger")        { is_dagger = true; knife_size = sfml_util::Size::Medium; type = weapon_type::Knife; }
        else if (name == "(dagger)")      { is_dagger = true; type = weapon_type::Knife; }
        else if (name == "(sword)")       { type = weapon_type::Sword; }
        else if (name == "(axe)")         { type = weapon_type::Axe; }
        else if (name == "(club)")        { type = weapon_type::Club; }
        else if (name == "(whip)")        { type = weapon_type::Whip; }
        else if (name == "(projectile)")  { type = weapon_type::Projectile; }
        else if (name == "(bladedstaff)") { type = weapon_type::BladedStaff; }
        else
        {
            M_ASSERT_OR_LOGANDTHROW_SS((name.empty()), "heroespath::item::weapon::WeaponInfo::ProcessName(name=\"" << name << "\") was not a recognized weapon.");
        }
    }

}
}
}
