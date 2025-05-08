#ifndef DARK_MODE_HPP
#define DARK_MODE_HPP

#include <QWidget>

namespace Utils {

// Returns true if dark mode is on for a widget
inline bool isDarkMode(const QWidget* widget) {
    return widget->palette().color(QPalette::Window).lightness() < 128;
}

}

#endif //DARK_MODE_HPP
