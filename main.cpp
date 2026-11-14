#include <mod/amlmod.h>
#include <aml-psdk/game_sa/Events.h>
#include <aml-psdk/game_sa/entity/PlayerPed.h>
#include <aml-psdk/game_sa/other/Pools.h>
#include <aml-psdk/game_sa/other/Clock.h>
#include <aml-psdk/game_sa/engine/Font.h>
#include <aml-psdk/game_sa/engine/Sprite2d.h>
#include <aml-psdk/game_sa/engine/RsGlobal.h>
#include <aml-psdk/gta_base/Rect.h>
#include <aml-psdk/gta_base/RGBA.h>
#include <cmath>
#include <cstdint>
#include <cstdio>

MYMOD(com.rafi.sa_interactive_npc, GTA SA Interactive NPC, 0.2, Rafi)

namespace {
constexpr float kTalkDistanceSq = 6.25f; // 2.5 game units
constexpr float kButtonL = 458.0f, kButtonT = 374.0f;
constexpr float kButtonR = 628.0f, kButtonB = 430.0f;
CPed* g_nearest = nullptr;
bool g_dialogueOpen = false;
int g_dialogueIndex = 0;
unsigned int g_lastTouch = 0;

const char* kMorning[20] = {
 "Pagi! Cuacanya cerah hari ini.", "Kamu berangkat kerja?", "Jalanan mulai ramai.",
 "Semoga harimu lancar.", "Sudah sarapan?", "Pagi yang tenang, ya.",
 "Aku mau mulai aktivitas.", "Udara pagi terasa segar.", "Hati-hati di jalan.",
 "Kota mulai bangun.", "Ada rencana hari ini?", "Semoga urusanmu mudah.",
 "Aku suka suasana pagi.", "Toko sebentar lagi buka.", "Bus mulai beroperasi.",
 "Jangan lupa istirahat.", "Hari baru, semangat baru.", "Kamu dari mana pagi-pagi?",
 "Semoga tidak terlambat.", "Sampai jumpa nanti."
};
const char* kAfternoon[15] = {
 "Siang ini panas sekali.", "Sudah makan siang?", "Lalu lintas padat.",
 "Aku sedang istirahat sebentar.", "Kota makin ramai.", "Ada yang bisa kubantu?",
 "Jangan lupa minum air.", "Aku mau lanjut bekerja.", "Semoga pekerjaanmu lancar.",
 "Banyak orang di luar hari ini.", "Kamu terlihat sibuk.", "Istirahat kalau lelah.",
 "Sebentar lagi sore.", "Jaga barang bawaanmu.", "Sampai bertemu lagi."
};
const char* kNight[15] = {
 "Malam mulai tiba.", "Suasana lebih sepi sekarang.", "Hati-hati saat pulang.",
 "Lampu jalan sudah menyala.", "Aku mau segera pulang.", "Malam ini cukup tenang.",
 "Jangan berkeliaran terlalu larut.", "Kamu masih ada urusan?", "Jalanan agak lengang.",
 "Semoga perjalananmu aman.", "Aku sedang menunggu seseorang.", "Udara malam dingin.",
 "Besok kita lanjut lagi.", "Jaga diri baik-baik.", "Selamat malam."
};
// Role dialogue bank: five lines for each supported role.
const char* kPolice[5] = {"Tetap patuhi aturan lalu lintas.","Ada yang bisa saya bantu?","Jaga ketertiban di sekitar sini.","Berkendara dengan aman.","Kami sedang patroli."};
const char* kTaxi[5] = {"Butuh taksi?","Silakan sebutkan tujuanmu.","Tarif tergantung jarak.","Saya tahu jalan pintas.","Terima kasih sudah menggunakan taksi."};
const char* kMechanic[5] = {"Kendaraanmu perlu diperiksa?","Mesin harus dirawat rutin.","Bengkel sedang buka.","Periksa ban sebelum pergi.","Saya bisa bantu urusan kendaraan."};
const char* kShop[5] = {"Selamat datang di toko.","Silakan lihat barang-barangnya.","Ada yang sedang kamu cari?","Terima kasih sudah mampir.","Semoga belanjaanmu berguna."};
const char* kService[5] = {"Selamat datang.","Silakan tunggu sebentar.","Ada yang bisa saya bantu?","Terima kasih sudah mampir.","Semoga urusanmu lancar."};
const char* kCitizen[5] = {"Kota ini selalu punya cerita.","Aku tinggal tidak jauh dari sini.","Hari ini cukup sibuk.","Kamu pendatang baru?","Semoga harimu menyenangkan."};

enum Role { ROLE_CITIZEN, ROLE_POLICE, ROLE_TAXI, ROLE_MECHANIC, ROLE_SHOP, ROLE_CASINO, ROLE_SERVICE };
Role GetRole(int model) {
    if (model >= 280 && model <= 288) return ROLE_POLICE;
    if (model == 7 || model == 142 || model == 182 || model == 206 || model == 220 || model == 234 || model == 261 || model == 262) return ROLE_TAXI;
    if (model == 50) return ROLE_MECHANIC;
    if (model == 155 || model == 167 || model == 168 || model == 179 || model == 205 || model == 211 || model == 217) return ROLE_SHOP;
    if (model == 11 || model == 171 || model == 172) return ROLE_CASINO;
    if (model == 16 || model == 253 || model == 255) return ROLE_SERVICE;
    return ROLE_CITIZEN;
}
const char* NpcName(int model) {
    static const char* names[] = {"Alex", "Rico", "Dina", "Bima", "Nadia", "Doni", "Mira", "Ari", "Sinta", "Rian", "Tari", "Fajar", "Lina", "Bayu", "Niko", "Rani", "Dika", "Maya", "Raka", "Nina", "Aldi", "Citra", "Reno", "Ayu"};
    return names[(model < 0 ? -model : model) % (sizeof(names) / sizeof(names[0]))];
}
const char* RoleName(Role role) {
    switch (role) {
        case ROLE_POLICE: return "Petugas";
        case ROLE_TAXI: return "Pengemudi";
        case ROLE_MECHANIC: return "Mekanik";
        case ROLE_SHOP: return "Penjaga toko";
        case ROLE_CASINO: return "Staf kasino";
        case ROLE_SERVICE: return "Petugas layanan";
        default: return "Warga";
    }
}
const char* RoleLine(Role role, int n) {
    switch (role) {
        case ROLE_POLICE: return kPolice[n % 5];
        case ROLE_TAXI: return kTaxi[n % 5];
        case ROLE_MECHANIC: return kMechanic[n % 5];
        case ROLE_SHOP: return kShop[n % 5];
        case ROLE_CASINO: return kService[n % 5];
        case ROLE_SERVICE: return kService[n % 5];
        default: return kCitizen[n % 5];
    }
}
const char* GeneralLine(int hour, int n) {
    if (hour >= 5 && hour < 11) return kMorning[n % 20];
    if (hour >= 11 && hour < 18) return kAfternoon[n % 15];
    return kNight[n % 15];
}

void DrawText(float x, float y, float scale, const char* text, CRGBA color) {
    CFont::SetFontStyle(FONT_SUBTITLES);
    CFont::SetScale(scale, scale);
    CFont::SetColor(color);
    CFont::SetEdge(1);
    CFont::SetDropShadowPosition(1);
    CFont::PrintString(x, y, text);
}

void DrawUI() {
    CPlayerPed* player = FindPlayerPed(-1);
    g_nearest = nullptr;
    if (!player || player->m_pVehicle || player->bInVehicle) {
        g_dialogueOpen = false;
        return;
    }
    auto pool = CPools::ms_pPedPool;
    if (!pool) return;
    float best = kTalkDistanceSq;
    for (int i = 0; i < pool->GetSize(); ++i) {
        CPed* ped = pool->GetAt(i);
        if (!ped || ped == player || ped->IsPlayer()) continue;
        float d = ped->GetDistanceSqrTo(player);
        if (d < best) { best = d; g_nearest = ped; }
    }
    if (!g_nearest) { g_dialogueOpen = false; return; }

    const float sx = RsGlobal.maximumWidth / 640.0f;
    const float sy = RsGlobal.maximumHeight / 448.0f;
    if (!g_dialogueOpen) {
        CRect button(kButtonL*sx, kButtonT*sy, kButtonR*sx, kButtonB*sy);
        CSprite2d::DrawRect(button, CRGBA(45,45,45,205));
        DrawText(486*sx, 391*sy, 0.42f*sy, "BERCAKAP", CRGBA(255,255,255,255));
    } else {
        CRect panel(24*sx, 330*sy, 616*sx, 438*sy);
        CSprite2d::DrawRect(panel, CRGBA(35,35,35,205));
        const int model = g_nearest->m_nModelIndex;
        const Role role = GetRole(model);
        char header[96];
        snprintf(header, sizeof(header), "%s - %s", NpcName(model), RoleName(role));
        DrawText(42*sx, 343*sy, 0.38f*sy, header, CRGBA(255,220,130,255));
        const char* line = (g_dialogueIndex % 2 == 0)
            ? GeneralLine(CClock::ms_nGameClockHours, g_dialogueIndex / 2)
            : RoleLine(role, g_dialogueIndex / 2);
        ++g_dialogueIndex;
        DrawText(42*sx, 375*sy, 0.34f*sy, line, CRGBA(255,255,255,255));
        DrawText(42*sx, 414*sy, 0.28f*sy, "SENTUH LAGI UNTUK MENUTUP", CRGBA(205,205,205,255));
    }
}

void OnTouch(int action, int, int x, int y) {
    if (action != 1) return; // Android ACTION_UP
    CPlayerPed* player = FindPlayerPed(-1);
    auto pool = CPools::ms_pPedPool;
    if (!player || player->m_pVehicle || player->bInVehicle || !g_nearest || !pool || !pool->IsObjectValid(g_nearest)) return;
    const float sx = RsGlobal.maximumWidth / 640.0f;
    const float sy = RsGlobal.maximumHeight / 448.0f;
    const float px = x / sx, py = y / sy;
    if (g_dialogueOpen) {
        if (py >= 330.0f && py <= 438.0f) g_dialogueOpen = false;
    } else if (px >= kButtonL && px <= kButtonR && py >= kButtonT && py <= kButtonB) {
        g_dialogueOpen = true;
        g_dialogueIndex = 0;
    }
}
}

ON_MOD_LOAD() {
    Events::drawHudEvent += []() { DrawUI(); };
    Events::touchScreenEvent += [](int action, int track, int x, int y) { OnTouch(action, track, x, y); };
}
