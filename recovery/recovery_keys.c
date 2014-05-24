#include <linux/input.h>

#include "recovery_ui.h"
#include "common.h"
#include "extendedcommands.h"

int device_handle_key(int key_code, int visible) {
    if (visible) {
        switch (key_code) {
            case KEY_VOLUMEUP:
                return HIGHLIGHT_UP;

            case KEY_VOLUMEDOWN:
                return HIGHLIGHT_DOWN;

	    case KEY_HOME:
                return SELECT_ITEM;

            case KEY_END:
            case KEY_BACK:
                if (!ui_root_menu) {
                    return GO_BACK;
                }
        }
    }

    return NO_ACTION;
}
