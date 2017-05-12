//
// phase-enum.cpp
//
#include "phase-enum.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>


namespace game
{

    const std::string Phase::ToString(const Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E & Phase::Combat)          ss << "Combat";
        if (E & Phase::Exploring)       ss << ((ss.str().empty()) ? "" : ", ") << "Exploring";
        if (E & Phase::Conversation)    ss << ((ss.str().empty()) ? "" : ", ") << "Conversation";
        if (E & Phase::Quest)           ss << ((ss.str().empty()) ? "" : ", ") << "Quest";
        if (E & Phase::Inventory)       ss << ((ss.str().empty()) ? "" : ", ") << "Inventory";

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "game::Phase::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
            return "(" + ss.str() + ")";
        else
            return ss.str();
    }


    Phase::Enum Phase::FromString(const std::string & S)
    {
        auto lowerCaseStr{ boost::algorithm::to_lower_copy(S) };

        unsigned int x{ 0 };
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(Phase::ToString(Phase::Combat, false))))         x = x & Phase::Combat;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(Phase::ToString(Phase::Exploring, false))))      x = x & Phase::Exploring;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(Phase::ToString(Phase::Conversation, false))))   x = x & Phase::Conversation;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(Phase::ToString(Phase::Quest, false))))          x = x & Phase::Quest;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(Phase::ToString(Phase::Inventory, false))))      x = x & Phase::Inventory;

        if (0 == x)
        {
            std::ostringstream ssErr;
            ssErr << "game::Phase::FromString(\"" << S << "\") unable to convert that string into a set of Phase::Enum flags.";
            throw std::runtime_error(ssErr.str());
        }

        return static_cast<Phase::Enum>(x);
    }


}
