#pragma once
// Tata letak dan penggambaran UI. Satu sumber tata letak dipakai untuk menggambar
// DAN hit-test sentuhan sehingga keduanya tidak bisa melenceng.
// Semua koordinat dalam piksel layar. Fungsi Draw* hanya boleh dipanggil dari event render.

namespace sa_npc {
namespace ui {

struct Box {
    float left = 0, top = 0, right = 0, bottom = 0;  // top < bottom (y ke bawah)
    bool Contains(float x, float y) const { return x >= left && x <= right && y >= top && y <= bottom; }
};

struct Layout {
    Box button;
    Box panel;
    float scale = 1.0f;  // skala seragam berbasis tinggi layar (basis 448)
    float width = 0, height = 0;
};

// false jika ukuran layar belum valid (game belum siap render).
bool ComputeLayout(Layout& out);

void DrawButton(const Layout& l);
void DrawPanel(const Layout& l, const char* name, const char* role, const char* line);

}  // namespace ui
}  // namespace sa_npc
