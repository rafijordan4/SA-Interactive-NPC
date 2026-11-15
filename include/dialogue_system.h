#pragma once
// Bank 80 kalimat dialog + pengelolaan indeks. Murni logika (tanpa PSDK).
#include <cstddef>

#include "role_classifier.h"

namespace sa_npc {

enum class DayPart { Morning, Afternoon, Night };

// Jam GAME (bukan jam Android). 05-10 pagi, 11-17 siang, selain itu malam.
// Jam di luar 0..23 dianggap malam.
DayPart DayPartFromHour(int gameHour);

// Memilih satu kalimat. Panggil HANYA saat pemain membuka dialog (bukan per frame).
// Pemanggilan berurutan bergantian antara kalimat waktu dan kalimat profesi;
// tiap tabel punya kursor sendiri sehingga semua kalimat berputar tanpa pengulangan
// sebelum seluruh tabel habis. Pointer yang dikembalikan menunjuk string statis.
const char* PickLine(Role role, int gameHour);

// Untuk pengujian: total kalimat unik yang tersedia dan akses per indeks.
std::size_t TotalLineCount();
const char* LineAt(std::size_t index);
std::size_t MorningCount();
std::size_t AfternoonCount();
std::size_t NightCount();
std::size_t RoleBankCount(Role role);  // Citizen dan Service berbagi bank yang sama

}  // namespace sa_npc
