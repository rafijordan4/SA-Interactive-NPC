#pragma once
// Pelacakan NPC yang aman terhadap lifetime.
//
// Alih-alih memindai pool ped (stride objek pool belum terverifikasi di Android),
// daftar NPC hidup dibangun dari event konstruktor/destruktor CPed milik PSDK.
// Setiap entri punya nomor seri; pointer yang slotnya dipakai ulang NPC lain
// memiliki seri berbeda sehingga referensi lama otomatis tidak valid.
#include <cstddef>
#include <cstdint>

struct CPed;
struct CPlayerPed;

namespace sa_npc {
namespace npc {

struct Target {
    CPed* ped = nullptr;
    std::uint32_t serial = 0;
    int model = -1;
    bool IsSet() const { return ped != nullptr; }
};

// Daftarkan event PSDK. Panggil sekali dari ON_MOD_LOAD.
void Install();

// Kosongkan daftar (dipakai saat pool game dibuat/dihancurkan).
void Reset();

// NPC hidup, tidak di kendaraan, bukan pemain, terdekat dalam radius maxDist (satuan game).
// Hanya boleh dipanggil dari thread game (event render).
bool FindNearest(CPlayerPed* player, float maxDist, Target& out);

// Apakah target masih NPC yang sama, masih hidup, dan masih dalam radius maxDist.
bool IsStillValid(const Target& target, CPlayerPed* player, float maxDist);

std::size_t TrackedCount();

}  // namespace npc
}  // namespace sa_npc
