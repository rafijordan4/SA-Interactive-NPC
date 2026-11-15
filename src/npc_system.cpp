#include "npc_system.h"

#include <cfloat>  // Vector.h milik PSDK memakai FLT_EPSILON tanpa meng-include-nya
#include <mod/amlmod.h>
#include <mod/logger.h>
#include <aml-psdk/game_sa/Events.h>
#include <aml-psdk/game_sa/entity/PlayerPed.h>

#include <cmath>
#include <mutex>
#include <vector>

static_assert(sizeof(void*) == 8, "Mod ini hanya untuk ARM64 (PedLifetimeReturn = void)");

namespace sa_npc {
namespace npc {
namespace {

struct Entry {
    CPed* ped;
    std::uint32_t serial;
};

std::mutex g_mutex;
std::vector<Entry> g_entries;
std::uint32_t g_nextSerial = 1;
bool g_loggedFirst = false;

// Pemanggil harus memegang g_mutex.
std::size_t IndexOf(const CPed* ped) {
    for (std::size_t i = 0; i < g_entries.size(); ++i) {
        if (g_entries[i].ped == ped) return i;
    }
    return static_cast<std::size_t>(-1);
}

void RemoveLocked(const CPed* ped) {
    const std::size_t i = IndexOf(ped);
    if (i == static_cast<std::size_t>(-1)) return;
    g_entries[i] = g_entries.back();
    g_entries.pop_back();
}

void OnPedCreated(CPed* ped) {
    if (!ped) return;
    std::lock_guard<std::mutex> lock(g_mutex);
    RemoveLocked(ped);  // slot dipakai ulang tanpa dtor terlihat -> buang entri lama
    g_entries.push_back({ped, g_nextSerial++});
    if (g_nextSerial == 0) g_nextSerial = 1;
    if (!g_loggedFirst) {
        g_loggedFirst = true;
        logger->Info("NPC tracker aktif: ped pertama terdaftar via pedCtorEvent");
    }
}

void OnPedDestroyed(CPed* ped) {
    if (!ped) return;
    std::lock_guard<std::mutex> lock(g_mutex);
    RemoveLocked(ped);
}

// Hanya dipanggil untuk pointer yang ada di registry (objek sudah dikonstruksi dan belum didestruksi).
bool IsEligible(CPed* ped, CPlayerPed* player, float maxDistSq, float& distSqOut) {
    if (!ped || static_cast<void*>(ped) == static_cast<void*>(player)) return false;
    if (!(ped->m_fHealth > 0.0f)) return false;  // mati (juga menolak NaN)
    if (ped->bInVehicle) return false;
    const float d = ped->GetDistanceSqrTo(player);
    if (!std::isfinite(d) || d > maxDistSq) return false;
    distSqOut = d;
    return true;
}

}  // namespace

void Install() {
    Events::pedCtorEvent += [](CPed* ped) { OnPedCreated(ped); };
    Events::pedDtorEvent += [](CPed* ped) { OnPedDestroyed(ped); };
    // Pool dibuat/dihancurkan: semua pointer lama pasti tidak berlaku lagi.
    Events::initPoolsEvent += []() { Reset(); };
    Events::shutdownPoolsEvent += []() { Reset(); };
}

void Reset() {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_entries.clear();
}

bool FindNearest(CPlayerPed* player, float maxDist, Target& out) {
    out = Target();
    if (!player) return false;
    const float maxDistSq = maxDist * maxDist;
    // Salin snapshot supaya tidak memegang lock saat menyentuh objek game.
    std::vector<Entry> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        snapshot = g_entries;
    }
    float best = maxDistSq;
    for (const Entry& e : snapshot) {
        float d = 0.0f;
        if (!IsEligible(e.ped, player, best, d)) continue;
        if (d <= best) {
            best = d;
            out.ped = e.ped;
            out.serial = e.serial;
            out.model = e.ped->m_nModelIndex;
        }
    }
    return out.IsSet();
}

bool IsStillValid(const Target& target, CPlayerPed* player, float maxDist) {
    if (!target.IsSet() || !player) return false;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        const std::size_t i = IndexOf(target.ped);
        if (i == static_cast<std::size_t>(-1) || g_entries[i].serial != target.serial) return false;
    }
    float d = 0.0f;
    return IsEligible(target.ped, player, maxDist * maxDist, d);
}

std::size_t TrackedCount() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_entries.size();
}

}  // namespace npc
}  // namespace sa_npc
