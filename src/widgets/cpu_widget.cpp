#include "cpu_widget.hpp"
#include "cpu.hpp"
#include "imgui.h"

void CpuWidget::render()
{
    if (!m_visible)
        return;

    CPU::Registers regs = m_cpu.registers();

    ImGui::Begin("CPU", &m_visible);
    ImGui::Text("  A: 0x%02X", regs.A);
    ImGui::Text("  X: 0x%02X", regs.X);
    ImGui::Text("  Y: 0x%02X", regs.Y);
    ImGui::Text(" SP: 0x%02X", regs.SP);
    ImGui::Text(" PC: 0x%04X", regs.PC);
    ImGui::End();
}
