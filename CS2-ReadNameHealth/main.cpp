#include"memory.h"

namespace offsets {
    DWORD64 dwLocalPlayer = 0x1810EE0;
    DWORD64 entityList = 0x17C18E0;
    DWORD64 teamNum = 0xF4;

    DWORD64 m_hPlayerPawn = 0x7EC;
    DWORD64 m_iTeamNum = 0x3BF;
    DWORD64 m_iHealth = 0x32C;
    DWORD64 m_iszPlayerName = 0x640;
}

void ReadNameHealth(Memory mem) {
    DWORD64 processID = mem.GetProcessID(L"Counter-Strike 2");
    DWORD64 client = (DWORD64)mem.GetModuleAddress(processID, L"client.dll").modBaseAddr;

    while (true) {
        DWORD64 localPlayer = mem.ReadMemory<DWORD64>(client + offsets::dwLocalPlayer);
        if (!localPlayer)
            continue;

        DWORD localTeam = mem.ReadMemory<DWORD64>(localPlayer + offsets::m_iTeamNum);
        DWORD64 entityList = mem.ReadMemory<uintptr_t>(client + offsets::entityList);

        for (auto i = 1; i < 32; ++i) {
            DWORD64 listEntry = mem.ReadMemory<DWORD64>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
            if (!listEntry) continue;

            DWORD64 CurrentBasePlayerController = mem.ReadMemory<DWORD64>(listEntry + i * 0x78);
            DWORD64 m_hPlayerPawn = mem.ReadMemory<DWORD64>(CurrentBasePlayerController + offsets::m_hPlayerPawn);
            DWORD64 listEntry2 = mem.ReadMemory<DWORD64>(entityList + 0x8 * ((m_hPlayerPawn & 0x7FFF) >> 9) + 16);
            if (!listEntry2) continue;

            DWORD64 entity = mem.ReadMemory<DWORD64>(listEntry2 + 120 * (m_hPlayerPawn & 0x1FF));
            if (localPlayer == entity || entity == 0) continue;

            DWORD teamID = mem.ReadMemory<BYTE>(entity + offsets::m_iTeamNum);
            if (teamID != 2 && teamID != 3) continue;
            if (teamID == localTeam) continue;

            DWORD entityHealth = mem.ReadMemory<DWORD>(entity + offsets::m_iHealth);
            string name;
            for (int i = 0; i < 10; i++) {
                char x = mem.ReadMemory<char>(CurrentBasePlayerController + offsets::m_iszPlayerName + i);
                if (x == '\0') break;
                name += x;
            }

            if (name != "")
                cout << name << ": " << entityHealth << "HP\n";
        }
        this_thread::sleep_for(chrono::milliseconds(1));
        system("cls");
    }
}

int main() {
    Memory mem;

    while (true) {
        ReadNameHealth(mem);
    }
    return 0;
}
