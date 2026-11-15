#pragma once
// Klasifikasi profesi NPC berdasarkan ID model ped GTA SA.
// Murni logika (tanpa PSDK) supaya bisa dites di host.

namespace sa_npc {

enum class Role {
    Citizen,   // fallback untuk semua model yang tidak dikenal
    Police,
    Taxi,
    Mechanic,
    Shop,
    Casino,
    Service,   // label berbeda, tetapi memakai bank dialog warga/layanan
};

// Mengembalikan Role::Citizen jika model tidak ada di tabel (fallback aman).
Role ClassifyModel(int modelId);

// Label profesi yang ditampilkan di panel dialog.
const char* RoleLabel(Role role);

// Nama alias yang stabil per model (BUKAN nama karakter resmi game).
const char* AliasName(int modelId);

}  // namespace sa_npc
