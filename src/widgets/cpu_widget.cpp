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

        auto status_color = [](bool status) -> const ImVec4&
        {
            static ImVec4 green{ .0f, 1.f, .0f, 1.f };
            static ImVec4 red{ 1.f, .0f, .0f, 1.f };

            if (status)
                return green;
            return red;
        };

        ImGui::Text(" A: 0x%02X", regs.A);
        ImGui::Text(" X: 0x%02X", regs.X);
        ImGui::Text(" Y: 0x%02X", regs.Y);

        ImGui::Text(" P: 0x%02X", regs.P);
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_N)), "N");
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_V)), "V");
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_U)), "U");
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_B)), "B");
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_D)), "D");
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_I)), "I");
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_Z)), "Z");
        ImGui::SameLine();
        ImGui::TextColored(status_color(m_cpu.check_status_flag(CPU::STATUS_C)), "C");

        ImGui::Text("SP: 0x%02X", regs.SP);
        ImGui::Text("PC: 0x%04X", regs.PC);
    }

    ImGui::End();
}
