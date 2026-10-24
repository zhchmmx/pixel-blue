/*
 * Tablet-mode keyboard backlight handling for Pixel Blue (issue #72).
 *
 * ChromeOS powerd forces the keyboard backlight to 0% while the device is
 * folded into tablet mode and gives the user level back on the way out
 * (platform2 keyboard_backlight_controller.cc). This extension reproduces
 * that behaviour entirely at the UI level, with no root, no driver unbind
 * and no service restarts:
 *
 *  - Tablet detection uses the compositor's own touch-mode state, the
 *    ClutterSeat "touch-mode" property. On the Pixelbook it is driven purely
 *    by the cros_ec SW_TABLET_MODE switch through libinput, and gnome-shell
 *    itself reacts to the same signal (js/ui/keyboard.js, unlockDialog.js).
 *  - The light is switched off and back on by writing gsd-power's
 *    org.gnome.SettingsDaemon.Power.Keyboard "Brightness" property -- the
 *    very same rootless session-bus channel the quick-settings slider uses.
 *  - While folded, anything that raises the light again (e.g. the
 *    Alt+brightness keyd bindings, which go through gsd-power directly and
 *    bypass the UI) is pushed back to 0 -- powerd's forced-0 override.
 *  - The quick-settings "Keyboard" item is hidden by wrapping the
 *    KeyboardBrightnessToggle._sync() that recomputes its visibility from
 *    "Brightness >= 0"; the toggle is the only kbd-backlight UI surface
 *    (gnome-control-center has no keyboard-brightness row).
 *
 * Every lookup is defensive: if a hook is missing (no keyboard LED, a shell
 * that restructured quick-settings), the extension no-ops and leaves the UI
 * stock rather than breaking the shell.
 */

import Gio from 'gi://Gio';

import {Extension} from 'resource:///org/gnome/shell/extensions/extension.js';
import * as Main from 'resource:///org/gnome/shell/ui/main.js';
import {loadInterfaceXML} from 'resource:///org/gnome/shell/misc/dbusUtils.js';

const GSD_POWER_BUS_NAME = 'org.gnome.SettingsDaemon.Power';
const GSD_POWER_OBJECT_PATH = '/org/gnome/SettingsDaemon/Power';
const GSD_KBD_INTERFACE = 'org.gnome.SettingsDaemon.Power.Keyboard';

export default class TabletKbdBacklightExtension extends Extension {
    enable() {
        this._tablet = false;
        this._established = false;
        this._snapshot = -1;
        this._seat = null;
        this._seatChangedId = 0;
        this._toggle = null;
        this._proxy = null;
        this._proxyChangedId = 0;

        const kbdXml = loadInterfaceXML(GSD_KBD_INTERFACE);
        if (!kbdXml) {
            console.warn('tablet-kbd-backlight: gsd Keyboard interface XML not found, disabling');
            return;
        }

        const KbdProxy = Gio.DBusProxy.makeProxyWrapper(kbdXml);
        this._proxy = new KbdProxy(Gio.DBus.session, GSD_POWER_BUS_NAME, GSD_POWER_OBJECT_PATH,
            (proxy, error) => {
                if (error) {
                    console.warn(`tablet-kbd-backlight: no keyboard backlight proxy: ${error.message}`);
                    return;
                }
                // The extension may have been disabled or re-enabled before
                // gsd-power answered; a stale generation's callback must not
                // touch the new state.
                if (proxy !== this._proxy)
                    return;
                this._established = true;
                // The tablet flag may already have been set before gsd-power
                // answered (shell restarted while folded): apply now.
                this._applyTabletMode();
            });
        this._proxyChangedId = this._proxy.connect('g-properties-changed', (proxy, changed) => {
            const props = changed.deep_unpack?.() ?? changed;
            if (this._tablet && Number.isInteger(props?.Brightness) && props.Brightness > 0)
                proxy.Brightness = 0;
        });

        this._seat = global.stage.context.get_backend().get_default_seat();
        this._seatChangedId = this._seat.connect('notify::touch-mode',
            () => this._onTouchModeChanged());

        this._toggle = this._hookQuickSettingsToggle();

        // Adopt the state the device is already in (e.g. enabled while
        // folded, or after a shell restart mid-tablet).
        this._onTouchModeChanged();
    }

    disable() {
        if (this._seat && this._seatChangedId)
            this._seat.disconnect(this._seatChangedId);
        if (this._proxy && this._proxyChangedId)
            this._proxy.disconnect(this._proxyChangedId);

        // Hand the user's level back instead of leaving the light forced to
        // 0 without the machinery that would restore it on unfold.
        if (this._proxy && this._tablet && this._snapshot > 0 && this._established) {
            try {
                this._proxy.Brightness = this._snapshot;
            } catch (e) {
                console.warn(`tablet-kbd-backlight: could not restore brightness: ${e.message}`);
            }
        }

        // Undo the hide hook and let the item recompute its own visibility
        // (stock behaviour: visible iff the keyboard backlight exists).
        if (this._toggle) {
            delete this._toggle._sync;
            this._toggle._sync();
        }

        this._seatChangedId = 0;
        this._proxyChangedId = 0;
        this._seat = null;
        this._proxy = null;
        this._toggle = null;
        this._established = false;
        this._tablet = false;
        this._snapshot = -1;
    }

    _onTouchModeChanged() {
        const tablet = this._seat.touch_mode;
        if (tablet === this._tablet)
            return;

        this._tablet = tablet;
        this._applyTabletMode();
        if (this._toggle)
            this._toggle._sync();
    }

    _applyTabletMode() {
        if (!this._established)
            return;

        const brightness = this._proxy.Brightness;
        if (this._tablet) {
            if (this._snapshot < 0) {
                // Brightness < 0 (or absent) means no keyboard backlight to
                // snapshot; then there is nothing to force off either.
                if (!Number.isInteger(brightness) || brightness < 0)
                    return;
                this._snapshot = brightness;
            }
            if (this._snapshot > 0)
                this._proxy.Brightness = 0;
        } else {
            // Always retire the snapshot, including the legitimate
            // snapshot === 0 (folded with the light already off): leaving it
            // around would make the next fold skip both the re-snapshot and
            // the forced 0.
            if (this._snapshot > 0)
                this._proxy.Brightness = this._snapshot;
            this._snapshot = -1;
        }
    }

    // Find the quick-settings "Keyboard" toggle and make its visibility
    // follow the tablet flag as well. _sync() rederives `visible` from
    // Brightness on every property change -- including our own 0 write,
    // which satisfies `>= 0` -- so a plain visible=false would be undone
    // by the next recompute; wrap the method itself instead.
    _hookQuickSettingsToggle() {
        const items = Main.panel?.statusArea?.quickSettings?._backlight?.quickSettingsItems;
        const toggle = items?.find(i => i.constructor.name === 'KeyboardBrightnessToggle');
        if (!toggle || typeof toggle._sync !== 'function') {
            console.warn('tablet-kbd-backlight: keyboard quick-settings item not found, leaving it visible');
            return null;
        }

        const originalSync = toggle._sync.bind(toggle);
        toggle._sync = () => {
            originalSync();
            toggle.visible = toggle.visible && !this._tablet;
        };
        return toggle;
    }
}
