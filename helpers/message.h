#include <sysutil/sysutil_msgdialog.h>

// Full message dialogs
#define ERROR_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_ERROR | CELL_MSGDIALOG_TYPE_SE_MUTE_OFF | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL);

// WebMAN request filepath
#define WM_REQUEST_PATH "/dev_hdd0/tmp/wm_request"

// WebMAN request string for creating a popup
#define WM_POPUP_REQUEST "/popup.ps3?"
#define WM_POPUP_CONFIG_ICON "&icon="
#define WM_POPUP_CONFIG_SND "&snd="
#define WM_POPUP_CONFIG_MAX_LEN 14 // Max valid length of icon and sound sections of popup request

// Premade notification message dialogs
#define SUCCESS_MESSAGE_WITH_PW "Patchwork "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR)" Loaded for LBPX\nLobby password has been set"
#define SUCCESS_MESSAGE_WITHOUT_PW "Patchwork "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR)" Loaded for LBPX\nLobby password has been randomized"

#define WARNING_CONFIG_PARSE_FAIL "Some entries found in the Patchwork configuration file could not properly be read"
#define WARNING_CONFIG_MISSING_URL "The server URL could not be found in the Patchwork configuration file"

void WMPopup(char *message, int icon, int snd);
