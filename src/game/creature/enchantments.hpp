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
#ifndef GAME_CREATURE_ENCHANTMENTS_HPP_INCLUDE
#define GAME_CREATURE_ENCHANTMENTS_HPP_INCLUDE
//
// enchantments.hpp
//  A collection of custom enchantments.
//
#include "game/creature/enchantment.hpp"

#include "misc/boost-serialize-includes.hpp"

#include <string>


namespace game
{
namespace creature
{

    class Enchantment_PixieBell : public Enchantment
    {
    public:
        Enchantment_PixieBell()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(-1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Exploring |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Adds 10% Mana and removes the conditions: Dazed, and Asleep Natural.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 1800; }
        virtual ~Enchantment_PixieBell() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_CrystalChimes : public Enchantment
    {
    public:
        Enchantment_CrystalChimes()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(-1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Exploring |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Adds 15% Mana and removes the conditions: Dazed, Asleep Natural, and Daunted.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 2000; }
        virtual ~Enchantment_CrystalChimes() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_GoldenGong : public Enchantment
    {
    public:
        Enchantment_GoldenGong()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(-1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Exploring |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Adds 20% Mana and removes the conditions: Dazed, Asleep Natural, Daunted, and Panicked.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 2400; }
        virtual ~Enchantment_GoldenGong() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_DragonToothWhistle : public Enchantment
    {
    public:
        Enchantment_DragonToothWhistle()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(-1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Panics non-dragon flying creatures.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 1500; }
        virtual ~Enchantment_DragonToothWhistle() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_DoveBloodVial : public Enchantment
    {
    public:
        Enchantment_DoveBloodVial()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(10,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Exploring |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Removes conditions: Poison, Disease, Dazed, and Daunted.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 1900; }
        virtual ~Enchantment_DoveBloodVial() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_DriedEdible : public Enchantment
    {
    public:
        Enchantment_DriedEdible()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Exploring |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Drains 10% Mana, and removes the Poison condition.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 800; }
        virtual ~Enchantment_DriedEdible() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_ShamanRainmaker : public Enchantment
    {
    public:
        Enchantment_ShamanRainmaker()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(-1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Exploring |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Adds 10% Mana.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 600; }
        virtual ~Enchantment_ShamanRainmaker() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_SpecterChains : public Enchantment
    {
    public:
        Enchantment_SpecterChains()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(-1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Panics enemies.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 1300; }
        virtual ~Enchantment_SpecterChains() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_VultureGizzard : public Enchantment
    {
    public:
        Enchantment_VultureGizzard()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(3,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Exploring |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Curse Poison.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 1000; }
        virtual ~Enchantment_VultureGizzard() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };


    class Enchantment_WarTrumpet : public Enchantment
    {
    public:
        Enchantment_WarTrumpet()
        :
            Enchantment(static_cast<EnchantmentType::Enum>(EnchantmentType::BoundToItem |
                                                           EnchantmentType::WhenUsed),
                        stats::TraitSet(),
                        UseInfo(-1,
                                static_cast<Phase::Enum>(Phase::Combat |
                                                         Phase::Inventory)))
        {}

        inline virtual const std::string EffectStr(const CreaturePtr_t) const
        {
            return "Panics or Daunts enemies.";
        }

        virtual void UseEffect(const CreaturePtr_t);
        inline virtual int TreasureScore() const { return 1100; }
        virtual ~Enchantment_WarTrumpet() {}

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<Enchantment>(*this);
        }
    };

}
}

#endif //GAME_CREATURE_ENCHANTMENTS_HPP_INCLUDE
