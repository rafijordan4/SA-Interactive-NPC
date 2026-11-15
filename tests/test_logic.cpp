// Tes logika murni (tanpa game): jumlah & keunikan dialog, jam game, klasifikasi, rotasi indeks.
#include <cstdio>
#include <cstring>
#include <set>
#include <string>

#include "dialogue_system.h"
#include "role_classifier.h"

using namespace sa_npc;

static int g_fail = 0;
#define CHECK(cond, msg)                                         \
    do {                                                         \
        if (!(cond)) { std::printf("GAGAL: %s\n", msg); ++g_fail; } \
    } while (0)

int main() {
    // Jumlah sesuai spesifikasi: 20 + 15 + 15 + 6x5 = 80.
    CHECK(MorningCount() == 20, "pagi harus 20");
    CHECK(AfternoonCount() == 15, "siang harus 15");
    CHECK(NightCount() == 15, "malam harus 15");
    const Role roles[] = {Role::Police, Role::Taxi, Role::Mechanic, Role::Shop, Role::Casino, Role::Citizen};
    for (Role r : roles) CHECK(RoleBankCount(r) == 5, "tiap bank profesi harus 5");
    CHECK(RoleBankCount(Role::Service) == RoleBankCount(Role::Citizen), "layanan berbagi bank warga");
    CHECK(TotalLineCount() == 80, "total harus 80");

    // Semua kalimat unik, tidak kosong, tidak ada placeholder, tidak terlalu panjang.
    std::set<std::string> seen;
    for (std::size_t i = 0; i < TotalLineCount(); ++i) {
        const char* s = LineAt(i);
        CHECK(s && *s, "kalimat kosong");
        if (!s) continue;
        CHECK(seen.insert(s).second, "kalimat duplikat");
        CHECK(std::strlen(s) <= 60, "kalimat terlalu panjang");
        CHECK(!std::strchr(s, '%') && !std::strchr(s, '{') && !std::strstr(s, "TODO"), "placeholder terdeteksi");
    }
    CHECK(LineAt(TotalLineCount()) == nullptr, "indeks di luar batas harus nullptr");

    // Batas jam game.
    CHECK(DayPartFromHour(4) == DayPart::Night, "04 malam");
    CHECK(DayPartFromHour(5) == DayPart::Morning, "05 pagi");
    CHECK(DayPartFromHour(10) == DayPart::Morning, "10 pagi");
    CHECK(DayPartFromHour(11) == DayPart::Afternoon, "11 siang");
    CHECK(DayPartFromHour(17) == DayPart::Afternoon, "17 siang");
    CHECK(DayPartFromHour(18) == DayPart::Night, "18 malam");
    CHECK(DayPartFromHour(0) == DayPart::Night, "00 malam");
    CHECK(DayPartFromHour(23) == DayPart::Night, "23 malam");
    CHECK(DayPartFromHour(-1) == DayPart::Night && DayPartFromHour(99) == DayPart::Night, "jam tak valid -> malam");

    // Klasifikasi: ID terverifikasi dan fallback.
    CHECK(ClassifyModel(280) == Role::Police, "280 polisi");
    CHECK(ClassifyModel(285) == Role::Citizen, "285 (SWAT) bukan polisi biasa -> fallback");
    CHECK(ClassifyModel(7) == Role::Citizen, "7 (male01) bukan taksi");
    CHECK(ClassifyModel(142) == Role::Citizen, "142 bukan taksi");
    CHECK(ClassifyModel(182) == Role::Taxi, "182 taksi");
    CHECK(ClassifyModel(50) == Role::Mechanic, "50 mekanik");
    CHECK(ClassifyModel(171) == Role::Casino, "171 kasino");
    CHECK(ClassifyModel(179) == Role::Shop, "179 toko");
    CHECK(ClassifyModel(253) == Role::Service, "253 layanan");
    CHECK(ClassifyModel(-5) == Role::Citizen && ClassifyModel(99999) == Role::Citizen, "ID tak dikenal -> warga");
    CHECK(AliasName(-1) != nullptr && AliasName(100000) != nullptr, "alias aman untuk ID aneh");

    // Rotasi: 30 pemilihan berturut-turut pada pagi hari tidak boleh mengulang kalimat waktu
    // sebelum 15 kalimat waktu (separuh dari 30 panggilan) berbeda semuanya.
    std::set<std::string> timeLines;
    for (int i = 0; i < 30; ++i) {
        const char* s = PickLine(Role::Police, 8);
        CHECK(s != nullptr, "PickLine tidak boleh nullptr");
        if (i % 2 == 0) CHECK(timeLines.insert(s).second, "kalimat waktu berulang sebelum bank habis");
    }

    if (g_fail == 0) std::printf("SEMUA TES LOGIKA LULUS (80 kalimat unik)\n");
    return g_fail == 0 ? 0 : 1;
}
