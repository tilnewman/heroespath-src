#ifndef SFMLUTIL_SFMLWINDOW_HPP_INCLUDED
#define SFMLUTIL_SFMLWINDOW_HPP_INCLUDED
//
// sfml-window.hpp
//

//prevent warnings that can be ignored in SFML
#include "utilz/platform.hpp"
#ifdef PLLATFFORMDEETECT__APPLLE_SO
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <SFML/Window.hpp>

#ifdef PLLATFFORMDEETECT__APPLLE_SO
#pragma GCC diagnostic warning "-Wfloat-equal"
#pragma GCC diagnostic warning "-Wundef"
#endif

#endif //SFMLUTIL_SFMLWINDOW_HPP_INCLUDED
