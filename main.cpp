// GTA SA Interactive NPC (AML 2.10, Android ARM64).
// Alur: touchScreenEvent (thread lain) -> antrean aman -> dikonsumsi di drawHudEvent (thread game).
// Semua akses objek game dan semua state dialog hanya terjadi di drawHudEvent.
#include <cfloat>  // Vector.h milik PSDK memakai FLT_EPSILON tanpa meng-include-nya
#include <mod/amlmod.h>
#include <mod/logger.h>
#include <aml-psdk/game_sa/Events.h>
#include <aml-psdk/game_sa/entity/PlayerPed.h>
#include <aml-psdk/game_sa/other/Clock.h>

#include <cstdio>
#include <mutex>

#include "dialogue_system.h"
#include "npc_system.h"
#include "role_classifier.h"
#include "ui_system.h"

MYMOD(com.rafi.sa_interactive_npc, GTA SA Interactive NPC, 0.3, Rafi)

namespace {

using namespace sa_npc;

constexpr float kTalkDistance = 2.5f;      // jarak muncul tombol (satuan game)
constexpr float kKeepOpenDistance = 4.0f;  // dialog tetap terbuka sampai sejauh ini
constexpr int kActionUp = 1;               // Android ACTION_UP (diverifikasi lewat log sentuhan)

// ---- Antrean sentuhan (diisi dari thread sentuh, dikosongkan di thread game) ----
struct TouchEvent { int action, track, x, y; };
constexpr int kQueueSize = 16;
std::mutex g_touchMutex;
TouchEvent g_touchQueue[kQueueSize];
int g_touchCount = 0;

void PushTouch(int action, int track, int x, int y) {
    std::lock_guard<std::mutex> lock(g_touchMutex);
    if (g_touchCount < kQueueSize) g_touchQueue[g_touchCount++] = {action, track, x, y};
}

int DrainTouches(TouchEvent* out) {
    std::lock_guard<std::mutex> lock(g_touchMutex);
    const int n = g_touchCount;
    for (int i = 0; i < n; ++i) out[i] = g_touchQueue[i];
    g_touchCount = 0;
    return n;
}

// ---- State dialog (hanya disentuh di thread game) ----
struct Dialogue {
    bool open = false;
    npc::Target target;
    char name[48] = {};
    char role[48] = {};
    const char* line = "";  // string statis dari dialogue_system, dipilih sekali saat dibuka
};
Dialogue g_dialogue;
int g_loggedTouches = 0;

void CloseDialogue() {
    g_dialogue.open = false;
    g_dialogue.target = npc::Target();
}

void OpenDialogue(const npc::Target& target) {
    const Role role = ClassifyModel(target.model);
    int hour = CClock::ms_nGameClockHours;
    g_dialogue.target = target;
    std::snprintf(g_dialogue.name, sizeof(g_dialogue.name), "%s", AliasName(target.model));
    std::snprintf(g_dialogue.role, sizeof(g_dialogue.role), "%s", RoleLabel(role));
    g_dialogue.line = PickLine(role, hour);
    g_dialogue.open = true;
}

void OnDrawHud() {
    // 1) Selalu kosongkan antrean sentuhan, apa pun kondisinya.
    TouchEvent touches[16];
    const int touchCount = DrainTouches(touches);

    // 2) Game siap render?
    ui::Layout layout;
    if (!ui::ComputeLayout(layout)) { CloseDialogue(); return; }

    CPlayerPed* player = FindPlayerPed(-1);
    if (!player || player->bInVehicle || player->m_pVehicle) { CloseDialogue(); return; }

    // 3) Validasi dialog yang sedang terbuka / cari kandidat baru.
    npc::Target candidate;
    if (g_dialogue.open) {
        if (!npc::IsStillValid(g_dialogue.target, player, kKeepOpenDistance)) CloseDialogue();
    }
    if (!g_dialogue.open) npc::FindNearest(player, kTalkDistance, candidate);

    // 4) Proses sentuhan (hanya saat jari diangkat).
    for (int i = 0; i < touchCount; ++i) {
        const TouchEvent& t = touches[i];
        if (g_loggedTouches < 12) {
            ++g_loggedTouches;
            logger->Info("touch action=%d track=%d x=%d y=%d layar=%.0fx%.0f", t.action, t.track,
                         t.x, t.y, layout.width, layout.height);
        }
        if (t.action != kActionUp) continue;
        const float fx = static_cast<float>(t.x), fy = static_cast<float>(t.y);
        if (g_dialogue.open) {
            if (layout.panel.Contains(fx, fy)) CloseDialogue();
        } else if (candidate.IsSet() && layout.button.Contains(fx, fy)) {
            OpenDialogue(candidate);
        }
    }

    // 5) Gambar sesuai state akhir.
    if (g_dialogue.open) {
        ui::DrawPanel(layout, g_dialogue.name, g_dialogue.role, g_dialogue.line);
    } else if (candidate.IsSet()) {
        ui::DrawButton(layout);
    }
}

}  // namespace

ON_MOD_LOAD() {
    logger->SetTag("SA Interactive NPC");
    npc::Install();
    Events::initPoolsEvent += []() { CloseDialogue(); };
    Events::shutdownPoolsEvent += []() { CloseDialogue(); };
    Events::drawHudEvent += []() { OnDrawHud(); };
    Events::touchScreenEvent += [](int action, int track, int x, int y) { PushTouch(action, track, x, y); };
    logger->Info("v0.3 dimuat");
}
