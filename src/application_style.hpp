#pragma once

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
    const ColorTheme selected_theme() const { return m_selected_theme; }

private:
    ColorTheme m_selected_theme = ColorTheme::Default;
};
