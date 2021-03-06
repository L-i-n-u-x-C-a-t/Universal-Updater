/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "storeUtils.hpp"
#include "structs.hpp"

static const std::vector<Structs::ButtonPos> GridBoxes = {
	{ 25, 45, 50, 50 },
	{ 100, 45, 50, 50 },
	{ 175, 45, 50, 50 },
	{ 250, 45, 50, 50 },
	{ 325, 45, 50, 50 },

	{ 25, 105, 50, 50 },
	{ 100, 105, 50, 50 },
	{ 175, 105, 50, 50 },
	{ 250, 105, 50, 50 },
	{ 325, 105, 50, 50 },

	{ 25, 165, 50, 50 },
	{ 100, 165, 50, 50 },
	{ 175, 165, 50, 50 },
	{ 250, 165, 50, 50 },
	{ 325, 165, 50, 50 }
};

/*
	Draw the Top Grid.

	const std::unique_ptr<Store> &store: Const Reference to the Store class.
	const std::vector<std::unique_ptr<StoreEntry>> &entries: Const Reference to the StoreEntries.
*/
void StoreUtils::DrawGrid(const std::unique_ptr<Store> &store, const std::vector<std::unique_ptr<StoreEntry>> &entries) {
	if (store) { // Ensure, store is not a nullptr.

		if (config->usebg() && store->customBG()) {
			C2D_DrawImageAt(store->GetStoreImg(), 0, 26, 0.5f, nullptr);

		} else {
			Gui::Draw_Rect(0, 26, 400, 214, BG_COLOR);
		}

		for (int i = 0, i2 = 0 + (store->GetScreenIndx() * 5); i2 < 15 + (store->GetScreenIndx() * 5) && i2 < (int)entries.size(); i2++, i++) {

			/* Boxes. */
			if (i == store->GetBox()) GFX::DrawBox(GridBoxes[i].x, GridBoxes[i].y, 50, 50, true);

			/* Ensure, entries is larger than the index. */
			if ((int)entries.size() > i2) {
				if (entries[i2]) { // Ensure, the Entry is not nullptr.
					const C2D_Image tempImg = entries[i2]->GetIcon();
					const uint8_t offsetW = (48 - tempImg.subtex->width) / 2; // Center W.
					const uint8_t offsetH = (48 - tempImg.subtex->height) / 2; // Center H.

					C2D_DrawImageAt(tempImg, GridBoxes[i].x + 1 + offsetW, GridBoxes[i].y + 1 + offsetH, 0.5);

					/* Update Available mark. */
					if (entries[i2]->GetUpdateAvl()) GFX::DrawSprite(sprites_update_app_idx, GridBoxes[i].x + 32, GridBoxes[i].y + 32);
				}
			}
		}
	}
}


/*
	Top Grid Logic Handle.
	Here you can..

	- Scroll through the Grid with the D-Pad.

	std::unique_ptr<Store> &store: Reference to the Store class.
	std::vector<std::unique_ptr<StoreEntry>> &entries: Reference to the StoreEntries.
	const int &currentMode: Reference to the current Mode.
	int &lastMode: Reference to the last mode.
	bool &fetch: Reference to fetch.
	int &smallDelay: Reference to the small delay.
*/
void StoreUtils::GridLogic(std::unique_ptr<Store> &store, std::vector<std::unique_ptr<StoreEntry>> &entries, int &currentMode, int &lastMode, bool &fetch, int &smallDelay) {
	if (store) { // Ensure, store is not a nullptr.
		if (hRepeat & KEY_DOWN) {
			if (store->GetBox() > 9) {
				if (store->GetEntry() + 5 < (int)entries.size() - 1) {
					store->SetEntry(store->GetEntry() + 5);

					if (entries.size() > 15) store->SetScreenIndx((store->GetEntry() / 5) - 2);

				} else {
					if (store->GetEntry() < (int)entries.size() - 1) {
						store->SetEntry(entries.size() - 1);
						store->SetBox(10 + (store->GetEntry() % 5));

						if (entries.size() > 15) store->SetScreenIndx((store->GetEntry() / 5) - 2);
					}
				}

			} else {
				if (store->GetEntry() + 5 < (int)entries.size()) {
					store->SetBox(store->GetBox() + 5);
					store->SetEntry(store->GetEntry() + 5);
				}
			}
		}

		if (hRepeat & KEY_RIGHT) {
			if (store->GetEntry() < (int)entries.size() - 1) {
				if (store->GetBox() < 14) {
					store->SetBox(store->GetBox() + 1);
					store->SetEntry(store->GetEntry() + 1);

				} else {
					store->SetBox(10);
					store->SetEntry(store->GetEntry() + 1);

					store->SetScreenIndx((store->GetEntry() / 5) - 2);
				}
			}
		}

		if (hRepeat & KEY_LEFT) {
			if (store->GetEntry() > 0) {
				if (store->GetBox() > 0) {
					store->SetBox(store->GetBox() - 1);
					store->SetEntry(store->GetEntry() - 1);

				} else {
					store->SetBox(4);
					store->SetEntry(store->GetEntry() - 1);

					store->SetScreenIndx((store->GetEntry() / 5));
				}
			}
		}

		if (hRepeat & KEY_UP) {
			if (store->GetBox() < 5) {
				if (store->GetEntry() > 4) {
					store->SetEntry(store->GetEntry() - 5);

					store->SetScreenIndx((store->GetEntry() / 5));
				}

			} else {
				store->SetBox(store->GetBox() - 5);
				store->SetEntry(store->GetEntry() - 5);
			}
		}

		if (hDown & KEY_A) {
			fetch = true;
			smallDelay = 5;
			lastMode = currentMode;
			currentMode = 1;
		}
	}
}