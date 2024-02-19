#pragma once

enum class WidgetType
{
    None,
    CpuWindow,
    PpuWindow
};

class Widget
{
public:
    explicit Widget(WidgetType type):
        m_type(type)
    {}

    virtual ~Widget() {}

    WidgetType type() const { return m_type; }
    bool visible() const { return m_visible; }
    void set_visible(bool visible) { m_visible = visible; }

    virtual void render() = 0;

protected:
    WidgetType m_type = WidgetType::None;
    bool m_visible = false;
};
