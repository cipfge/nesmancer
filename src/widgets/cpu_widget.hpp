#pragma once

#include "widget.hpp"

class CPU;

class CpuWidget : public Widget
{
public:
    CpuWidget(const CPU& cpu):
        Widget(WidgetType::CpuWindow),
        m_cpu(cpu)
    {}

    virtual ~CpuWidget() {}

    void render() override;

private:
    const CPU& m_cpu;
};
