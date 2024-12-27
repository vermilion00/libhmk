/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "keycodes.h"

uint8_t keycode_to_modifier(uint16_t keycode) { return 1 << (keycode & 0xF); }

uint16_t keycode_to_system(uint16_t keycode) {
    // Generic Desktop Page (0x01)
    // https://usb.org/document-library/hid-usage-tables-15
    switch (keycode) {
    case KC_SYSTEM_POWER:
        return 0x0081; // System Power Down (4.5)
    case KC_SYSTEM_SLEEP:
        return 0x0082; // System Sleep (4.5)
    case KC_SYSTEM_WAKE:
        return 0x0083; // System Wake Up (4.5)
    default:
        return KC_NO;
    }
}

uint16_t keycode_to_consumer(uint16_t keycode) {
    // Consumer Page (0x0C)
    // https://usb.org/document-library/hid-usage-tables-15
    switch (keycode) {
    case KC_AUDIO_MUTE:
        return 0x00E2; // Mute (15.9)
    case KC_AUDIO_VOL_UP:
        return 0x00E9; // Volume Increment (15.9)
    case KC_AUDIO_VOL_DOWN:
        return 0x00EA; // Volume Decrement (15.9)
    case KC_MEDIA_NEXT_TRACK:
        return 0x00B5; // Scan Next Track (15.7)
    case KC_MEDIA_PREV_TRACK:
        return 0x00B6; // Scan Previous Track (15.7)
    case KC_MEDIA_STOP:
        return 0x00B7; // Stop (15.7)
    case KC_MEDIA_PLAY_PAUSE:
        return 0x00CD; // Play/Pause (15.7)
    case KC_MEDIA_SELECT:
        return 0x0183; // AL Consumer Control Configuration (15.15)
    case KC_MEDIA_EJECT:
        return 0x00CC; // Stop/Eject (15.7)
    case KC_MAIL:
        return 0x018A; // AL Email Reader (15.15)
    case KC_CALCULATOR:
        return 0x0192; // AL Calculator (15.15)
    case KC_MY_COMPUTER:
        return 0x0194; // AL Local Machine Browser (15.15)
    case KC_WWW_SEARCH:
        return 0x0221; // AC Search (15.16)
    case KC_WWW_HOME:
        return 0x0223; // AC Home (15.16)
    case KC_WWW_BACK:
        return 0x0224; // AC Back (15.16)
    case KC_WWW_FORWARD:
        return 0x0225; // AC Forward (15.16)
    case KC_WWW_STOP:
        return 0x0226; // AC Stop (15.16)
    case KC_WWW_REFRESH:
        return 0x0227; // AC Refresh (15.16)
    case KC_WWW_FAVORITES:
        return 0x022A; // AC Bookmarks (15.16)
    case KC_MEDIA_FAST_FORWARD:
        return 0x00B3; // Fast Forward (15.7)
    case KC_MEDIA_REWIND:
        return 0x00B4; // Rewind (15.7)
    case KC_BRIGHTNESS_UP:
        return 0x006F; // Display Brightness Increment (15.5)
    case KC_BRIGHTNESS_DOWN:
        return 0x0070; // Display Brightness Decrement (15.5)
    case KC_CONTROL_PANEL:
        return 0x019F; // AL Control Panel (15.15)
    case KC_ASSISTANT:
        return 0x01CB; // AL Context-aware Desktop Assistan (15.15)
    case KC_MISSION_CONTROL:
        return 0x029F; // AC Desktop Show All Windows (15.16)
    case KC_LAUNCHPAD:
        return 0x02A0; // AC Soft Key Left (15.16)
    default:
        return KC_NO;
    }
}
