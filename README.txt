GTA:SA + AML 2.10 - Interactive NPC (source/build package)

Target: Android ARM64 / GTA SA 2.10 64-bit using AndroidModLoader PSDK.
Expected output filename from GitHub Actions: LibSAInteractiveNPC.so
Install destination after build: GTA SA/Mods/

Included implementation:
- Finds the closest pedestrian within 2.5 game units while player is on foot.
- Draws a Bercakap touch button and a translucent grey dialogue panel.
- Handles Android touch release to open/close the panel.
- Contains 50 general dialogue lines: 20 morning, 15 afternoon, 15 night.
- Contains 30 role dialogue lines: six groups of five (citizen, police, taxi, mechanic, shop, service).
- Uses verified pedestrian model IDs for taxi, mechanic, shop/service, casino staff, and police categories.
- Displays a generated NPC display name and role label. Display names are aliases, not canonical game character names.

BUILD:
1. Upload the contents of this folder to a new GitHub repository on branch main.
2. Open Actions and run "Build GTA SA Interactive NPC" (or push to main).
3. Download the artifact named LibSAInteractiveNPC.
4. Place LibSAInteractiveNPC.so in GTA SA/Mods/ and test the game.

IMPORTANT:
This package contains source code and a GitHub Actions build workflow. It does NOT
contain a compiled .so binary. The plugin has not yet been compiled or tested in-game.
Touch coordinate scaling, HUD rendering, and the final binary must be verified on
GTA SA + AML 2.10 before calling it release-ready.

SDK: https://github.com/AndroidModLoader/aml-psdk
AML docs: https://github.com/RusJJ/AndroidModLoader/blob/main/docs/DOCUMENTATION_ID.MD
Pedestrian model reference: https://wiki.gtaconnected.com/Resources/GTASA/PedSkins
