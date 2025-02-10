#pragma once

#include <string>

enum class ColorTheme
{
    Default,
    ImGuiClassic,
    ImGuiLight,
    ImGuiDark
};

class ApplicationStyle
{
public:
    void set_color_theme(ColorTheme theme);
    void set_color_theme(const std::string &name);
    const ColorTheme selected_theme() const { return m_selected_theme; }

private:
    ColorTheme m_selected_theme = ColorTheme::Default;
    ColorTheme get_theme_from_string(const std::string name);
};
