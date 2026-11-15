#include "ui_system.h"

#include <cfloat>  // Vector.h milik PSDK memakai FLT_EPSILON tanpa meng-include-nya
#include <mod/amlmod.h>
#include <aml-psdk/game_sa/engine/Font.h>
#include <aml-psdk/game_sa/engine/RsGlobal.h>
#include <aml-psdk/game_sa/engine/Sprite2d.h>
#include <aml-psdk/gta_base/RGBA.h>
#include <aml-psdk/gta_base/Rect.h>

namespace sa_npc {
namespace ui {
namespace {

// Satuan "unit" = piksel pada layar setinggi 448 (basis HUD GTA SA), dikali Layout::scale.
constexpr float kBaseHeight = 448.0f;

// Tombol: dekat kanan bawah, di atas klaster tombol aksi bawaan game.
constexpr float kBtnWidth = 150.0f, kBtnHeight = 42.0f;
constexpr float kBtnMarginRight = 40.0f, kBtnMarginBottom = 120.0f;

// Panel dialog: bawah layar, hampir selebar layar.
constexpr float kPanelWidthRatio = 0.92f;
constexpr float kPanelHeight = 104.0f, kPanelMarginBottom = 14.0f;
constexpr float kPad = 14.0f;

// Skala font (x, y) dalam unit. Perlu ditala di game jika terlalu besar/kecil.
constexpr float kFontNameX = 0.36f, kFontNameY = 0.72f;
constexpr float kFontBodyX = 0.32f, kFontBodyY = 0.64f;
constexpr float kFontHintX = 0.26f, kFontHintY = 0.52f;

void BeginText(const Layout& l, float scaleX, float scaleY, CRGBA color, float wrapRight) {
    CFont::SetFontStyle(FONT_SUBTITLES);
    CFont::SetProportional(true);
    CFont::SetJustify(false);
    CFont::SetOrientation(ALIGN_LEFT);
    CFont::SetBackground(false, false);
    CFont::SetAlphaFade(255.0f);
    CFont::SetWrapx(wrapRight);
    CFont::SetScale(scaleX * l.scale, scaleY * l.scale);
    CFont::SetColor(color);
    CFont::SetEdge(1);
}

// GTA SA: CRect(left, bottom, right, top) dengan bottom = y lebih besar pada koordinat layar.
void FillBox(const Box& b, CRGBA color) {
    CRect r(b.left, b.bottom, b.right, b.top);
    CSprite2d::DrawRect(r, color);
}

}  // namespace

bool ComputeLayout(Layout& out) {
    const float w = static_cast<float>(RsGlobal.maximumWidth);
    const float h = static_cast<float>(RsGlobal.maximumHeight);
    if (!(w >= 64.0f) || !(h >= 64.0f)) return false;
    const float s = h / kBaseHeight;
    out.width = w;
    out.height = h;
    out.scale = s;

    out.button.right = w - kBtnMarginRight * s;
    out.button.left = out.button.right - kBtnWidth * s;
    out.button.bottom = h - kBtnMarginBottom * s;
    out.button.top = out.button.bottom - kBtnHeight * s;

    const float pw = w * kPanelWidthRatio;
    out.panel.left = (w - pw) * 0.5f;
    out.panel.right = out.panel.left + pw;
    out.panel.bottom = h - kPanelMarginBottom * s;
    out.panel.top = out.panel.bottom - kPanelHeight * s;
    return true;
}

void DrawButton(const Layout& l) {
    FillBox(l.button, CRGBA(70, 70, 70, 175));
    const float s = l.scale;
    BeginText(l, kFontBodyX, kFontBodyY, CRGBA(255, 255, 255, 255), l.button.right);
    CFont::PrintString(l.button.left + 22.0f * s, l.button.top + 11.0f * s, "BERCAKAP");
}

void DrawPanel(const Layout& l, const char* name, const char* role, const char* line) {
    FillBox(l.panel, CRGBA(28, 28, 28, 205));
    const float s = l.scale;
    const float x = l.panel.left + kPad * s;
    const float wrap = l.panel.right - kPad * s;
    const float y0 = l.panel.top;

    BeginText(l, kFontNameX, kFontNameY, CRGBA(255, 236, 160, 255), wrap);  // kuning pastel
    CFont::PrintString(x, y0 + 8.0f * s, name ? name : "");

    BeginText(l, kFontHintX, kFontHintY, CRGBA(200, 200, 200, 255), wrap);
    CFont::PrintString(x, y0 + 27.0f * s, role ? role : "");

    BeginText(l, kFontBodyX, kFontBodyY, CRGBA(255, 255, 255, 255), wrap);
    CFont::PrintString(x, y0 + 46.0f * s, line ? line : "");

    BeginText(l, kFontHintX, kFontHintY, CRGBA(165, 165, 165, 255), wrap);
    CFont::PrintString(x, y0 + 84.0f * s, "Sentuh panel untuk menutup");
}

}  // namespace ui
}  // namespace sa_npc
