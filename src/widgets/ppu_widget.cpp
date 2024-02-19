#include "ppu_widget.hpp"
#include "ppu.hpp"
#include "imgui.h"

void PpuWidget::render()
{
    if (!m_visible)
        return;

    ImGui::Begin("PPU", &m_visible);

    if (ImGui::CollapsingHeader("Registers", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Control: 0x%02X", m_ppu.control());
        ImGui::Text("   Mask: 0x%02X", m_ppu.mask());
        ImGui::Text(" Status: 0x%02X", m_ppu.status());
    }

    if (ImGui::CollapsingHeader("OAM", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const uint8_t* oam = m_ppu.oam();
        for (uint8_t i = 0; i < 14; i++)
        {
            // X, Y, Id, Attribute
            ImGui::Text("(%3u,%3u) 0x%02X 0x%02X", oam[i * 4 + 3], oam[i * 4], oam[i * 4 + 1], oam[i * 4 + 2]);
        }
    }

    ImGui::End();
}
