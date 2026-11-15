#include "dialogue_system.h"

namespace sa_npc {
namespace {

const char* const kMorning[] = {
    "Selamat pagi. Udaranya masih sejuk, ya.",
    "Pagi-pagi begini jalanan belum terlalu ramai.",
    "Kamu sudah sarapan? Aku baru selesai minum kopi.",
    "Matahari baru naik, enak buat jalan kaki.",
    "Pagi yang cerah. Semoga harimu lancar.",
    "Toko-toko di sekitar sini baru mulai buka.",
    "Sebentar lagi aku harus berangkat kerja.",
    "Embun pagi bikin kota ini terasa tenang.",
    "Kamu bangun pagi juga? Jarang ada yang sepagi ini.",
    "Hari baru, semoga urusanmu dimudahkan.",
    "Bau roti panggang dari kafe itu menggoda sekali.",
    "Kalau mau sarapan, warung di ujung jalan sudah buka.",
    "Burung-burung mulai ribut. Tandanya sudah pagi.",
    "Jangan lupa minum air sebelum beraktivitas.",
    "Pagi hari paling enak untuk jalan santai.",
    "Lalu lintas belum macet. Manfaatkan waktunya.",
    "Aku suka kota ini waktu masih sepi.",
    "Semangat, hari masih panjang.",
    "Kelihatannya kamu sedang terburu-buru pagi ini.",
    "Selamat beraktivitas, hati-hati di jalan.",
};

const char* const kAfternoon[] = {
    "Siang ini panas sekali, ya.",
    "Sudah makan siang? Aku baru mau cari makan.",
    "Jalanan lagi padat, sabar saja.",
    "Cari tempat teduh dulu kalau kepanasan.",
    "Jam istirahat siang, banyak orang keluar.",
    "Mungkin sore nanti suasananya agak sejuk.",
    "Kota paling ramai di jam segini.",
    "Kalau haus, es jeruk di sana segar sekali.",
    "Hari ini lumayan melelahkan, tapi harus jalan terus.",
    "Kamu kelihatan sibuk. Ada yang bisa kubantu?",
    "Jam segini biasanya antrean toko panjang.",
    "Cahaya matahari siang bikin silau.",
    "Jaga barang bawaanmu di tempat ramai.",
    "Sebentar lagi sore, waktu terasa cepat sekali.",
    "Kalau capek, duduk saja dulu sebentar.",
};

const char* const kNight[] = {
    "Sudah malam, kamu belum pulang?",
    "Jalanan mulai sepi kalau malam begini.",
    "Lampu-lampu kota bagus kalau dilihat malam hari.",
    "Hati-hati, malam hari lebih banyak orang aneh.",
    "Anginnya dingin. Pakai jaket kalau perlu.",
    "Aku masih menunggu seseorang di sini.",
    "Malam begini enaknya ngopi sambil ngobrol.",
    "Jangan berkeliaran terlalu larut.",
    "Kota ini punya sisi lain kalau malam.",
    "Besok pagi aku harus bangun lagi.",
    "Langitnya cerah malam ini, bintangnya kelihatan.",
    "Kamu sendirian malam-malam begini?",
    "Kalau mau pulang, ambil jalan yang terang.",
    "Suasananya tenang, cocok buat istirahat.",
    "Selamat malam, jaga diri baik-baik.",
};

const char* const kPolice[] = {
    "Tolong patuhi aturan lalu lintas.",
    "Ada keributan di sekitar sini? Laporkan ke kami.",
    "Kami sedang berpatroli di area ini.",
    "Jangan membawa barang berbahaya sembarangan.",
    "Kalau butuh bantuan, panggil petugas terdekat.",
};

const char* const kTaxi[] = {
    "Butuh tumpangan? Sebutkan saja tujuanmu.",
    "Argo mulai jalan begitu kamu naik.",
    "Saya hafal semua jalan pintas di kota ini.",
    "Taksi kosong, tinggal naik.",
    "Penumpang hari ini lumayan ramai.",
};

const char* const kMechanic[] = {
    "Mesin kendaraanmu masih sehat?",
    "Periksa oli dan ban sebelum berangkat jauh.",
    "Bengkel ini bisa urus hampir semua kerusakan.",
    "Bunyi aneh dari mesin jangan didiamkan.",
    "Kendaraan yang dirawat bisa awet bertahun-tahun.",
};

const char* const kShop[] = {
    "Selamat datang, silakan lihat-lihat dulu.",
    "Cari sesuatu yang spesifik? Biar saya bantu.",
    "Hari ini ada barang baru di rak depan.",
    "Terima kasih sudah berbelanja di sini.",
    "Kalau barangnya tidak ketemu, tanya saja.",
};

const char* const kCasino[] = {
    "Selamat datang. Bermainlah dengan bijak.",
    "Meja kartu di dalam masih buka.",
    "Keberuntungan bisa datang kapan saja.",
    "Batasi taruhanmu supaya tetap menyenangkan.",
    "Silakan nikmati suasana kasino ini.",
};

// Warga umum dan petugas layanan berbagi satu bank (5 kalimat).
const char* const kGeneral[] = {
    "Kota ini selalu punya cerita baru.",
    "Aku tinggal tidak jauh dari sini.",
    "Kamu pendatang baru di sini?",
    "Hari ini kerjaanku cukup banyak.",
    "Kalau tersesat, tanya saja warga sekitar.",
};

template <typename T, std::size_t N>
constexpr std::size_t Count(const T (&)[N]) { return N; }

// Kursor per tabel: 0 pagi, 1 siang, 2 malam, 3.. bank profesi.
enum Table { T_MORNING, T_AFTERNOON, T_NIGHT, T_POLICE, T_TAXI, T_MECHANIC,
             T_SHOP, T_CASINO, T_GENERAL, T_COUNT };
std::size_t g_cursor[T_COUNT] = {};
unsigned g_openCounter = 0;

const char* Next(Table t, const char* const* table, std::size_t n) {
    const char* line = table[g_cursor[t] % n];
    g_cursor[t] = (g_cursor[t] + 1) % n;
    return line;
}

const char* RoleLine(Role role) {
    switch (role) {
        case Role::Police:   return Next(T_POLICE, kPolice, Count(kPolice));
        case Role::Taxi:     return Next(T_TAXI, kTaxi, Count(kTaxi));
        case Role::Mechanic: return Next(T_MECHANIC, kMechanic, Count(kMechanic));
        case Role::Shop:     return Next(T_SHOP, kShop, Count(kShop));
        case Role::Casino:   return Next(T_CASINO, kCasino, Count(kCasino));
        case Role::Service:
        case Role::Citizen:
        default:             return Next(T_GENERAL, kGeneral, Count(kGeneral));
    }
}

const char* TimeLine(int gameHour) {
    switch (DayPartFromHour(gameHour)) {
        case DayPart::Morning:   return Next(T_MORNING, kMorning, Count(kMorning));
        case DayPart::Afternoon: return Next(T_AFTERNOON, kAfternoon, Count(kAfternoon));
        case DayPart::Night:
        default:                 return Next(T_NIGHT, kNight, Count(kNight));
    }
}

}  // namespace

DayPart DayPartFromHour(int h) {
    if (h >= 5 && h <= 10) return DayPart::Morning;
    if (h >= 11 && h <= 17) return DayPart::Afternoon;
    return DayPart::Night;
}

const char* PickLine(Role role, int gameHour) {
    const bool useTime = (g_openCounter++ % 2u) == 0u;
    return useTime ? TimeLine(gameHour) : RoleLine(role);
}

std::size_t MorningCount() { return Count(kMorning); }
std::size_t AfternoonCount() { return Count(kAfternoon); }
std::size_t NightCount() { return Count(kNight); }

std::size_t RoleBankCount(Role role) {
    switch (role) {
        case Role::Police:   return Count(kPolice);
        case Role::Taxi:     return Count(kTaxi);
        case Role::Mechanic: return Count(kMechanic);
        case Role::Shop:     return Count(kShop);
        case Role::Casino:   return Count(kCasino);
        case Role::Service:
        case Role::Citizen:
        default:             return Count(kGeneral);
    }
}

std::size_t TotalLineCount() {
    return Count(kMorning) + Count(kAfternoon) + Count(kNight) + Count(kPolice) +
           Count(kTaxi) + Count(kMechanic) + Count(kShop) + Count(kCasino) + Count(kGeneral);
}

const char* LineAt(std::size_t i) {
    const char* const* tables[] = {kMorning, kAfternoon, kNight, kPolice, kTaxi,
                                   kMechanic, kShop, kCasino, kGeneral};
    const std::size_t sizes[] = {Count(kMorning), Count(kAfternoon), Count(kNight),
                                 Count(kPolice), Count(kTaxi), Count(kMechanic),
                                 Count(kShop), Count(kCasino), Count(kGeneral)};
    for (std::size_t t = 0; t < 9; ++t) {
        if (i < sizes[t]) return tables[t][i];
        i -= sizes[t];
    }
    return nullptr;
}

}  // namespace sa_npc
