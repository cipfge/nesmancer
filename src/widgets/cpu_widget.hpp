#pragma once

class CPU;

class CpuWidget
{
public:
    CpuWidget(const CPU& cpu): m_cpu(cpu) {}
    void render();

    bool visible() { return m_visible; }
    void set_visible(bool visible) { m_visible = visible; }

private:
    const CPU& m_cpu;
    bool m_visible = false;
};
