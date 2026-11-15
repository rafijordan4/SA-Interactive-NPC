# GTA SA Interactive NPC (v0.3)

Mod native untuk GTA San Andreas Android ARM64 (AML 2.10 + PSDK). Hasil build: `LibSAInteractiveNPC.so`.

Dekati NPC yang berjalan kaki (jarak ~2,5 unit) lalu sentuh tombol **BERCAKAP** untuk membuka panel dialog. Sentuh panel untuk menutup.

## Struktur

| File | Tanggung jawab |
|---|---|
| `main.cpp` | Wiring event, antrean sentuhan, state dialog |
| `src/npc_system.cpp` | Registry NPC hidup (event ctor/dtor CPed) + nomor seri |
| `src/role_classifier.cpp` | ID model -> profesi, nama alias (tanpa PSDK) |
| `src/dialogue_system.cpp` | 80 kalimat + rotasi indeks (tanpa PSDK) |
| `src/ui_system.cpp` | Tata letak, gambar tombol/panel, hit-test |
| `tests/test_logic.cpp` | Tes logika murni (dijalankan di workflow) |

## Desain stabilitas

- **Tidak ada scan pool.** Stride objek `CPedPool` di Android belum terverifikasi, jadi daftar NPC dibangun dari `pedCtorEvent`/`pedDtorEvent`. Entri dihapus saat NPC dihancurkan; pool dibuat/dihancurkan mengosongkan daftar.
- **Referensi NPC = pointer + nomor seri.** Slot pool yang dipakai ulang NPC lain mendapat seri baru, jadi referensi lama otomatis ditolak.
- **NPC mati/di kendaraan/pemain ditolak**, dicek ulang tiap frame; dialog menutup sendiri jika NPC hilang, mati, atau terlalu jauh (>4 unit).
- **Thread:** `touchScreenEvent` hanya mengantre peristiwa (mutex). Semua akses objek game dan state dialog terjadi di `drawHudEvent`.
- **Dialog dipilih sekali saat dibuka**, tidak berganti per frame. Jam memakai `CClock` game.
- Tata letak UI dihitung dari `RsGlobal` (skala berbasis tinggi layar), satu sumber untuk gambar dan hit-test.

## Build

Push ke `main` atau jalankan workflow **Build GTA SA Interactive NPC**. Workflow: tes logika -> ambil PSDK/AML (SHA terpin) -> NDK r25c -> ndk-build arm64-v8a -> validasi ELF (ELF64/AArch64/DYN, NEEDED, simbol `__GetModInfo`/`OnModLoad`) -> artifact `LibSAInteractiveNPC`.

Build sukses **bukan** bukti mod berfungsi di game.

## Pasang

Salin `LibSAInteractiveNPC.so` ke:
`/storage/emulated/0/Android/data/com.rockstargames.gtasa/mods/`
(di Termux: `~/storage/shared/Android_unprotected/data/com.rockstargames.gtasa/mods/`)

## Verifikasi di game

Buka log (`logcat -s "SA Interactive NPC"` atau logger AML). Yang harus muncul:
- `v0.3 dimuat` saat game start.
- `NPC tracker aktif ...` begitu NPC pertama muncul. Jika tidak muncul, hook ctor/dtor tidak jalan; mod aman tapi tidak akan menampilkan tombol.
- Baris `touch action=... x=... y=... layar=WxH` untuk 12 sentuhan pertama. Cocokkan: tombol hanya merespons `action=1`, dan x/y harus dalam rentang piksel layar `W x H`.

## Batasan yang belum terverifikasi

- Belum diuji di game; ukuran font, posisi tombol, dan hit-test perlu dicek visual.
- Sentuhan pada tombol juga diteruskan ke game (event hanya mengamati).
- Kategori ID model toko/layanan hanya cocok berdasarkan nama model skin.
- SHA AML yang dipin belum dipastikan sama dengan rilis "AML 2.10".

Referensi: [aml-psdk](https://github.com/AndroidModLoader/aml-psdk), [AndroidModLoader](https://github.com/RusJJ/AndroidModLoader), [daftar skin MTA](https://wiki.multitheftauto.com/wiki/All_Skins_Page).
