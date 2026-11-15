#include "role_classifier.h"

#include <cstddef>

namespace sa_npc {
namespace {

// ID = skin ID GTA SA. Nama model dicocokkan dengan daftar skin MTA
// (wiki.multitheftauto.com/wiki/All_Skins_Page). Hanya ID yang namanya
// jelas sesuai profesi yang dimasukkan; sisanya jatuh ke Citizen.
struct Entry { int id; Role role; };

const Entry kTable[] = {
    // Polisi / sheriff: lapd1, sfpd1, lvpd1, csher, lapdm1, dsher
    {280, Role::Police}, {281, Role::Police}, {282, Role::Police},
    {283, Role::Police}, {284, Role::Police}, {288, Role::Police},
    // Sopir taksi (akhiran "cd" = cab driver): vbmocd, vwmycd, sbmocd, swmocd, wmycd1, bmocd
    {182, Role::Taxi}, {206, Role::Taxi}, {220, Role::Taxi},
    {234, Role::Taxi}, {261, Role::Taxi}, {262, Role::Taxi},
    // Mekanik: wmymech
    {50, Role::Mechanic},
    // Staf toko/gerai: wmypizz, wmybell, bmochil, wmyammo, wfyburg, wfyclot, wmyclot
    {155, Role::Shop}, {167, Role::Shop}, {168, Role::Shop}, {179, Role::Shop},
    {205, Role::Shop}, {211, Role::Shop}, {217, Role::Shop},
    // Staf kasino: vbfycrp, vwmybjd, vwfycrp
    {11, Role::Casino}, {171, Role::Casino}, {172, Role::Casino},
    // Petugas layanan: bmyap (petugas bandara), bmosec (satpam)
    {16, Role::Service}, {253, Role::Service},
};

const char* const kNames[] = {
    "Alex", "Rico", "Dina", "Bima", "Nadia", "Doni", "Mira", "Ari",
    "Sinta", "Rian", "Tari", "Fajar", "Lina", "Bayu", "Niko", "Rani",
    "Dika", "Maya", "Raka", "Nina", "Aldi", "Citra", "Reno", "Ayu",
};

}  // namespace

Role ClassifyModel(int modelId) {
    for (const Entry& e : kTable) {
        if (e.id == modelId) return e.role;
    }
    return Role::Citizen;
}

const char* RoleLabel(Role role) {
    switch (role) {
        case Role::Police:   return "Petugas Kepolisian";
        case Role::Taxi:     return "Sopir Taksi";
        case Role::Mechanic: return "Mekanik";
        case Role::Shop:     return "Penjaga Toko";
        case Role::Casino:   return "Staf Kasino";
        case Role::Service:  return "Petugas Layanan";
        case Role::Citizen:
        default:             return "Warga";
    }
}

const char* AliasName(int modelId) {
    const std::size_t n = sizeof(kNames) / sizeof(kNames[0]);
    const unsigned u = modelId < 0 ? 0u : static_cast<unsigned>(modelId);
    return kNames[u % n];
}

}  // namespace sa_npc
