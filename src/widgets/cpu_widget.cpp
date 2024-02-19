#include "cpu_widget.hpp"
#include "cpu.hpp"
#include "imgui.h"

void CpuWidget::render()
{
    if (!m_visible)
        return;

    ImGui::Begin("CPU", &m_visible);

    if (ImGui::CollapsingHeader("Registers", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const CPU::Registers &regs = m_cpu.registers();

        ImGui::Text(" A: 0x%02X", regs.A);
        ImGui::Text(" X: 0x%02X", regs.X);
        ImGui::Text(" Y: 0x%02X", regs.Y);
        ImGui::Text(" P: 0x%02X", regs.P);
        ImGui::Text("SP: 0x%02X", regs.SP);
        ImGui::Text("PC: 0x%04X", regs.PC);
    }

    ImGui::End();
}
