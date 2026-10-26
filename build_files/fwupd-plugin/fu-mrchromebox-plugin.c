/*
 * fwupd plugin for MrChromebox UEFI Full ROM on the Google Pixelbook (eve).
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <stdio.h>

#include <libfwupdplugin/fu-device.h>
#include <libfwupdplugin/fu-plugin.h>
#include <libfwupdplugin/fu-smbios.h>

#include "fu-mrchromebox-device.h"

/* SMBIOS fields, see DSP0134: type 0 (BIOS), type 1 (System), type 2 (Baseboard) */
#define FU_MRCHROMEBOX_SMBIOS_TYPE_BIOS           0x00
#define FU_MRCHROMEBOX_SMBIOS_TYPE_SYSTEM         0x01
#define FU_MRCHROMEBOX_SMBIOS_TYPE_BASEBOARD      0x02
#define FU_MRCHROMEBOX_SMBIOS_BIOS_VERSION        0x05
#define FU_MRCHROMEBOX_SMBIOS_BIOS_DATE           0x08
#define FU_MRCHROMEBOX_SMBIOS_SYSTEM_PRODUCT      0x05
#define FU_MRCHROMEBOX_SMBIOS_BOARD_PRODUCT       0x05

#define FU_MRCHROMEBOX_BOARD          "eve"
#define FU_MRCHROMEBOX_PRODUCT        "Pixelbook"
#define FU_MRCHROMEBOX_DEVICE_ID      "mrchromebox-eve"
#define FU_MRCHROMEBOX_INSTANCE_ID    "mrchromebox:board:eve"

/*
 * The MrChromebox release date (DMI bios_date, MM/DD/YYYY) is the only value
 * that orders two Full ROM builds; the Coreboot version string is not
 * monotonic. fwupd compares it as a plain 8-digit number.
 */
static gchar *
fu_mrchromebox_date_to_yyyymmdd(const gchar *date)
{
	guint month, day, year;

	if (date == NULL)
		return NULL;
	if (sscanf(date, "%u/%u/%u", &month, &day, &year) != 3)
		return NULL;
	if (month < 1 || month > 12 || day < 1 || day > 31 || year < 2000 || year > 9999)
		return NULL;
	return g_strdup_printf("%04u%02u%02u", year, month, day);
}

static const gchar *
fu_mrchromebox_get_smbios(FuDevice *device, guint8 type, guint8 offset)
{
	return fu_device_get_smbios_string(device,
					   type,
					   FU_SMBIOS_STRUCTURE_LENGTH_ANY,
					   offset,
					   NULL);
}

static gboolean
fu_mrchromebox_plugin_coldplug(FuPlugin *plugin, FuProgress *progress, GError **error)
{
	g_autoptr(FuDevice) device = fu_mrchromebox_device_new(fu_plugin_get_context(plugin));
	const gchar *bios_version;
	const gchar *board_name;
	const gchar *product_name;
	const gchar *bios_date;
	g_autofree gchar *version = NULL;

	/* the vendored MrChromebox helpers only handle the UEFI Full ROM */
	if (!g_file_test("/sys/firmware/efi", G_FILE_TEST_IS_DIR)) {
		g_debug("not booted in UEFI mode, ignoring");
		return TRUE;
	}

	/* stock Google firmware is flashed with a different flow */
	bios_version = fu_mrchromebox_get_smbios(device,
						 FU_MRCHROMEBOX_SMBIOS_TYPE_BIOS,
						 FU_MRCHROMEBOX_SMBIOS_BIOS_VERSION);
	if (bios_version == NULL) {
		g_debug("no SMBIOS BIOS version, ignoring");
		return TRUE;
	}
	if (g_str_has_prefix(bios_version, "Google_")) {
		g_debug("running Google stock firmware, ignoring");
		return TRUE;
	}

	board_name =
	    fu_mrchromebox_get_smbios(device,
				      FU_MRCHROMEBOX_SMBIOS_TYPE_BASEBOARD,
				      FU_MRCHROMEBOX_SMBIOS_BOARD_PRODUCT);
	product_name =
	    fu_mrchromebox_get_smbios(device,
				      FU_MRCHROMEBOX_SMBIOS_TYPE_SYSTEM,
				      FU_MRCHROMEBOX_SMBIOS_SYSTEM_PRODUCT);
	if (g_strcmp0(board_name, FU_MRCHROMEBOX_BOARD) != 0 &&
	    g_ascii_strcasecmp(product_name != NULL ? product_name : "",
			       FU_MRCHROMEBOX_PRODUCT) != 0) {
		g_debug("unsupported board '%s' / product '%s', ignoring",
			board_name != NULL ? board_name : "",
			product_name != NULL ? product_name : "");
		return TRUE;
	}

	bios_date = fu_mrchromebox_get_smbios(device,
					     FU_MRCHROMEBOX_SMBIOS_TYPE_BIOS,
					     FU_MRCHROMEBOX_SMBIOS_BIOS_DATE);
	version = fu_mrchromebox_date_to_yyyymmdd(bios_date);

	fu_device_set_id(device, FU_MRCHROMEBOX_DEVICE_ID);
	fu_device_set_physical_id(device, FU_MRCHROMEBOX_DEVICE_ID);
	fu_device_set_name(device, "UEFI Full ROM (MrChromebox)");
	fu_device_set_vendor(device, "MrChromebox");
	if (version != NULL)
		fu_device_set_version(device, version);
	fu_device_add_instance_id(device, FU_MRCHROMEBOX_INSTANCE_ID);
	fu_plugin_add_device(plugin, device);
	return TRUE;
}

void
fu_plugin_init_vfuncs(FuPluginVfuncs *vfuncs)
{
	vfuncs->coldplug = fu_mrchromebox_plugin_coldplug;
}
