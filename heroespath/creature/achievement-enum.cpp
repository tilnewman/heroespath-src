//
// achievement.cpp
//
#include "achievement-enum.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace creature
{

    const std::string AchievementType::ToString(const AchievementType::Enum E)
    {
        switch (E)
        {
            case None:              { return "(None)";              }
            case EnemiesFaced:      { return "EnemiesFaced";        }
            case MeleeHits:         { return "MeleeHits";           }
            case BattlesSurvived:   { return "BattlesSurvived";     }
            case ProjectileHits:    { return "ProjectileHits";      }
            case BeastMindLinks:    { return "BeastMindLinks";      }
            case DodgedStanding:    { return "DodgedStanding";      }
            case DodgedFlying:      { return "DodgedFlying";        }
            case LocksPicked:       { return "LocksPicked";         }
            case BackstabsHits:     { return "BackstabHits";        }
            case SongsPlayed:       { return "SongsPlayed";         }
            case SpiritsLifted:     { return "SpiritsLifted";       }
            case BeastRoars:        { return "BeastRoars";          }
            case MoonHowls:         { return "MoonHowls";           }
            case PackActions:       { return "PackActions";         }
            case FlyingAttackHits:  { return "FlyingAttackHits";    }
            case TurnsInFlight:     { return "TurnsInFlight";       }
            case SpellsCast:        { return "SpellsCast";          }
            case HealthGiven:       { return "HealthGiven";         }
            case HealthTraded:      { return "HealthTraded";        }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "heroespath::creature::AchievementType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string AchievementType::Name(const AchievementType::Enum E)
    {
        switch (E)
        {
            case None:              { return "(None)";                  }
            case EnemiesFaced:      { return "Enemies Faced";           }
            case MeleeHits:         { return "Melee Hits";              }
            case BattlesSurvived:   { return "Battles Survived";        }
            case ProjectileHits:    { return "Projectile Hits";         }
            case BeastMindLinks:    { return "Beast Mind-Links";        }
            case DodgedStanding:    { return "Dodges While Standing";   }
            case DodgedFlying:      { return "Dodges While Flying";     }
            case LocksPicked:       { return "Locks Picked";            }
            case BackstabsHits:     { return "Backstab Hits";           }
            case SongsPlayed:       { return "Songs Played";            }
            case SpiritsLifted:     { return "Spirits Lifted";          }
            case BeastRoars:        { return "Beast Roars";             }
            case MoonHowls:         { return "Moon Howls";              }
            case PackActions:       { return "Pack Actions";            }
            case FlyingAttackHits:  { return "Flying Attack Hits";      }
            case TurnsInFlight:     { return "Turns In Flight";         }
            case SpellsCast:        { return "Spells Cast";             }
            case HealthGiven:       { return "Health Given";            }
            case HealthTraded:      { return "Health Traded";           }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "heroespath::creature::AchievementType::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
