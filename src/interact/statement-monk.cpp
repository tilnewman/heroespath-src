// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// statement-monk.hpp
//
#include "statement-monk.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string MonkTalk::Compose(const Mood MOOD)
        {
            if (MOOD == Mood::Kind)
            {
                return ComposeKind();
            }
            else
            {
                return ComposeMean();
            }
        }

        const std::string MonkTalk::ComposeMean()
        {
            using namespace compose;

            const std::vector<std::string> DISMISS_REASONS {
                ".",
                ".  Excuse me."
                ".  I'm meditating.",
                ".  This is my time for quiet reflection.",
                ".  This space is sacred.",
                ".  I'm having a vision.",
                ".  I must reach nirvana.",
                ".  Enlightenment is near.",
                ".  I have nothing for you.",
                ".  Our auras quarrel.",
                ".  None of you could ever understand.",
                ".  I have chores to do.",
                ".  Students are waiting.",
                ".  I must find my master.",
                ".  The master needs me."
            };

            switch (misc::random::Int(2))
            {
                case 0:
                {
                    return Random({ "No.  ", "No no.  ", "" }) + "Not "
                        + Random({ "now", "just now", "today" }) + Random(DISMISS_REASONS);
                }
                case 1:
                {
                    return Random({ "Away",
                                    "Away with you",
                                    "Away with you",
                                    "Away, all of you",
                                    "Away with all of you" })
                        + Random({ "", " now" })
                        + Random({ "", ".  I have no time", ".  There is no time" })
                        + Random(DISMISS_REASONS);
                }
                default:
                {
                    return Random({ "Please ", "Kindly " })
                        + Random({ "give me space",
                                   "leave me be",
                                   "try someone else",
                                   "step away",
                                   "I must away" })
                        + Random(DISMISS_REASONS);
                }
            }
        }

        const std::string MonkTalk::ComposeKind()
        {
            using namespace compose;

            switch (misc::random::Int(18))
            {
                case 0:
                {
                    return CapFirstLetter(Random(MonkThings())) + " is " + Random({ "a", "your" })
                        + " choice.  "
                        + Random({ "What have you chosen?", "What will you choose?" });
                }
                case 1:
                {
                    return "May " + Random(MonkThings()) + Random({ " be with ", " find " })
                        + Random({ "you.", "you all." });
                }
                case 2:
                {
                    return "Peace " + Random({ "be with ", "to " })
                        + Random({ "you.", "you all." });
                }
                case 3:
                {
                    return "This is a peaceful place.";
                }
                case 4:
                {
                    return "This is a place " + Random({ "for ", "of " }) + Random(MonkThings())
                        + ".";
                }
                case 5:
                {
                    return "You " + Random({ "have found ", "speak with " }) + "a humble "
                        + Random({ "brother.", "monk." });
                }
                case 6:
                {
                    return Random({ "All are ", "You are all " }) + "welcome here.";
                }
                case 7:
                {
                    return "You have come to the right place.";
                }
                case 8:
                {
                    return "Your lives have led you " + Random({ "here.  ", "to me.  " })
                        + Random({ "Think ", "Meditate " }) + "on " + Random({ "it.", "that." });
                }
                case 9:
                {
                    return "I see only brothers and sisters before me.";
                }
                case 10:
                {
                    return "I " + Random({ "follow the ", "seek the " })
                        + Random({ "righteous ", "one true " }) + Random({ "path.", "way." });
                }
                case 11:
                {
                    return Random({ "The ", "My ", "Our " }) + "master is away.";
                }
                case 12:
                {
                    return "Find " + Random({ "peace ", "your true calling ", "your path " })
                        + Random({ "here in ", "here with us among " })
                        + Random({ "the monks.", "the order.", "our order." });
                }
                case 13:
                {
                    return Random({ "Have you found ", "Are you looking for ", "Are you seeking " })
                        + Random(MonkThings()) + "?";
                }
                case 14:
                {
                    return "Are you considering " + Random({ "a life ", "life " })
                        + Random({ "among ", "with " })
                        + Random({ "us?", "the monks?", "the brothers?" });
                }
                case 15:
                {
                    return "If I speak of " + Random(MonkThings()) + ", will you "
                        + Random({ "really listen?", "truly hear?" });
                }
                case 16:
                {
                    return Random({ "May our paths ", "Our paths " }) + Random({ "cross", "join" })
                        + Random({ " peacefully.", "." });
                }
                case 17:
                {
                    return Random({ "May " + Random(MonkThings()),
                                    CapFirstLetter(Random(MonkThings())) })
                        + Random({ " follow you.", " be with you." });
                }
                default:
                {
                    return Random({ "Do you dream of ", "Have you found " }) + Random(MonkThings())
                        + "?";
                }
            }
        }
    } // namespace talk
} // namespace interact
} // namespace heroespath
