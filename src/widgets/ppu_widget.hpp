#pragma once

#include "widget.hpp"

class PPU;

class PpuWidget : public Widget
{
public:
    PpuWidget(const PPU& ppu) :
        Widget(WidgetType::PpuWindow),
        m_ppu(ppu)
    {}

    virtual ~PpuWidget() {}

    void render() override;

private:
    const PPU& m_ppu;
};
