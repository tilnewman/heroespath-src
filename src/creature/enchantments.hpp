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
#ifndef HEROESPATH_CREATURE_ENCHANTMENTS_HPP_INCLUDE
#define HEROESPATH_CREATURE_ENCHANTMENTS_HPP_INCLUDE
//
// enchantments.hpp
//  A collection of custom enchantments.
//
#include "creature/enchantment.hpp"

#include "misc/boost-serialize-includes.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    class Enchantment_PixieBell : public Enchantment
    {
    public:
        Enchantment_PixieBell()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      -1,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Adds 10% Mana and removes the conditions: Dazed, and Asleep Natural.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 1800_score; }
        virtual ~Enchantment_PixieBell() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_CrystalChimes : public Enchantment
    {
    public:
        Enchantment_CrystalChimes()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      -1,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Adds 15% Mana and removes the conditions: Dazed, Asleep Natural, and Daunted.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 2000_score; }
        virtual ~Enchantment_CrystalChimes() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_GoldenGong : public Enchantment
    {
    public:
        Enchantment_GoldenGong()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      -1,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Adds 20% Mana and removes the conditions: Dazed, Asleep Natural, Daunted, and "
                   "Panicked.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 2400_score; }
        virtual ~Enchantment_GoldenGong() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_DragonToothWhistle : public Enchantment
    {
    public:
        Enchantment_DragonToothWhistle()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      -1,
                      static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Panics non-dragon flying creatures.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 1500_score; }
        virtual ~Enchantment_DragonToothWhistle() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_DoveBloodVial : public Enchantment
    {
    public:
        Enchantment_DoveBloodVial()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      10,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Removes conditions: Poison, Disease, Dazed, and Daunted.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 1900_score; }
        virtual ~Enchantment_DoveBloodVial() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_DriedEdible : public Enchantment
    {
    public:
        Enchantment_DriedEdible()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      1,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Drains 10% Mana, and removes the Poison condition.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 800_score; }
        virtual ~Enchantment_DriedEdible() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_ShamanRainmaker : public Enchantment
    {
    public:
        Enchantment_ShamanRainmaker()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      -1,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const { return "Adds 10% Mana."; }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 600_score; }
        virtual ~Enchantment_ShamanRainmaker() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_SpecterChains : public Enchantment
    {
    public:
        Enchantment_SpecterChains()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      -1,
                      static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const { return "Panics enemies."; }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 1300_score; }
        virtual ~Enchantment_SpecterChains() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_VultureGizzard : public Enchantment
    {
    public:
        Enchantment_VultureGizzard()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      3,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Exploring | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const { return "Curse Poison."; }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 1000_score; }
        virtual ~Enchantment_VultureGizzard() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_WarTrumpet : public Enchantment
    {
    public:
        Enchantment_WarTrumpet()
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenUsed),
                  stats::TraitSet(),
                  UseInfo(
                      -1,
                      static_cast<game::Phase::Enum>(game::Phase::Combat | game::Phase::Inventory)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Panics or Daunts enemies.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        virtual Score_t TreasureScore() const { return 1100_score; }
        virtual ~Enchantment_WarTrumpet() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_MiscBlessed : public Enchantment
    {
    public:
        explicit Enchantment_MiscBlessed(const int USE_COUNT)
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenHeld),
                  stats::TraitSet({ std::make_pair(stats::Traits::Encounter, 3),
                                    std::make_pair(stats::Traits::BlessEffect, 3) }),
                  UseInfo(
                      USE_COUNT,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Inventory | game::Phase::Exploring)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Used when casting or playing a Bless.";
        }

        virtual void UseEffect(const CreaturePtr_t) {}
        virtual Score_t TreasureScore() const { return 250_score; }
        virtual ~Enchantment_MiscBlessed() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

    class Enchantment_MiscCursed : public Enchantment
    {
    public:
        explicit Enchantment_MiscCursed(const int USE_COUNT)
            : Enchantment(
                  static_cast<EnchantmentType::Enum>(
                      EnchantmentType::BoundToItem | EnchantmentType::WhenHeld),
                  stats::TraitSet({ std::make_pair(stats::Traits::Luck, -3),
                                    std::make_pair(stats::Traits::CurseEffect, 3) }),
                  UseInfo(
                      USE_COUNT,
                      static_cast<game::Phase::Enum>(
                          game::Phase::Combat | game::Phase::Inventory | game::Phase::Exploring)))
        {}

        virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Used when casting or playing a Curse.";
        }

        virtual void UseEffect(const CreaturePtr_t) {}
        virtual Score_t TreasureScore() const { return 250_score; }
        virtual ~Enchantment_MiscCursed() = default;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENTS_HPP_INCLUDE
