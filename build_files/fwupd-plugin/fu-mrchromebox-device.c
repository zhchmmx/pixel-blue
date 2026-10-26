/*
 * fwupd plugin for MrChromebox UEFI Full ROM on the Google Pixelbook (eve).
 *
 * The image never ships a general purpose firmware updater: the only thing
 * this device knows how to do is drive the vendored MrChromebox helpers, which
 * are the same code path the upstream firmware-util.sh TUI uses, with the same
 * flashrom binary it downloads. Nothing is fetched at run time and nothing
 * links libflashrom, so the plugin keeps working when the vendored helpers
 * change.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <string.h>
#include <sys/wait.h>

#include <glib/gstdio.h>

#include <libfwupdplugin/fu-bytes.h>
#include <libfwupdplugin/fu-device.h>
#include <libfwupdplugin/fu-progress.h>

#include "fu-mrchromebox-device.h"

#define FU_MRCHROMEBOX_VENDOR_DIR "/usr/lib/pixel-blue/mrchromebox"
#define FU_MRCHROMEBOX_STATE_DIR  "/var/lib/pixel-blue/firmware"
/* the build MrChromebox publishes for firmware-util.sh, vendored in the image */
#define FU_MRCHROMEBOX_FLASHROM   FU_MRCHROMEBOX_VENDOR_DIR "/flashrom"

/* flashrom can hang on a wedged SPI controller, so every call is bounded */
#define FU_MRCHROMEBOX_TIMEOUT_PROBE  60
#define FU_MRCHROMEBOX_TIMEOUT_READ   300
#define FU_MRCHROMEBOX_TIMEOUT_WP     120
#define FU_MRCHROMEBOX_TIMEOUT_WRITE  900
#define FU_MRCHROMEBOX_TIMEOUT_VERIFY 600

/* the Pixelbook ships a 16 MiB SPI flash; keep a generous sanity bound */
#define FU_MRCHROMEBOX_SIZE_MIN (1 * 1024 * 1024)
#define FU_MRCHROMEBOX_SIZE_MAX (64 * 1024 * 1024)

struct _FuMrchromeboxDevice {
	FuDevice parent_instance;
	gchar *flashrom_path;
	gchar *rom_path;
	gchar *log_path;
	gchar *sw_wp;
	gboolean has_ifd;
	gboolean first_chip;
	gboolean noverify;
};

G_DEFINE_TYPE(FuMrchromeboxDevice, fu_mrchromebox_device, FU_TYPE_DEVICE)

static void
fu_mrchromebox_device_finalize(GObject *object)
{
	FuMrchromeboxDevice *self = FU_MRCHROMEBOX_DEVICE(object);
	g_free(self->flashrom_path);
	g_free(self->rom_path);
	g_free(self->log_path);
	g_free(self->sw_wp);
	G_OBJECT_CLASS(fu_mrchromebox_device_parent_class)->finalize(object);
}

static void
fu_mrchromebox_device_init(FuMrchromeboxDevice *self)
{
	fu_device_add_flag(FU_DEVICE(self), FWUPD_DEVICE_FLAG_INTERNAL);
	fu_device_add_flag(FU_DEVICE(self), FWUPD_DEVICE_FLAG_UPDATABLE);
	fu_device_add_flag(FU_DEVICE(self), FWUPD_DEVICE_FLAG_REQUIRE_AC);
	fu_device_set_version_format(FU_DEVICE(self), FWUPD_VERSION_FORMAT_PLAIN);
	fu_device_add_icon(FU_DEVICE(self), FU_DEVICE_ICON_COMPUTER);
	fu_device_set_logical_id(FU_DEVICE(self), "bios");
}

FuDevice *
fu_mrchromebox_device_new(FuContext *ctx)
{
	FuMrchromeboxDevice *self = g_object_new(FU_TYPE_MRCHROMEBOX_DEVICE, "context", ctx, NULL);
	self->rom_path = g_build_filename(FU_MRCHROMEBOX_STATE_DIR, "update.rom", NULL);
	self->log_path = g_build_filename(FU_MRCHROMEBOX_STATE_DIR, "mrchromebox.log", NULL);
	return FU_DEVICE(self);
}

/* --- small helpers ------------------------------------------------------ */

static const gchar *
fu_mrchromebox_output_tail(const gchar *output)
{
	gsize len = output != NULL ? strlen(output) : 0;
	if (len > 400)
		return output + len - 400;
	return output != NULL ? output : "";
}

/*
 * Run argv under coreutils timeout, with stderr folded into stdout. Returns the
 * exit status, or -1 when the command could not be started at all.
 *
 * g_spawn_sync() has no stderr merge flag, so the pipeline is wrapped in a
 * tiny POSIX shell that dup2()s stderr onto stdout before exec.
 */
static gint
fu_mrchromebox_spawn(GPtrArray *argv, guint timeout_sec, gchar **output, GError **error)
{
	g_autoptr(GPtrArray) full = g_ptr_array_new();
	g_autofree gchar *timeout_str = g_strdup_printf("%u", timeout_sec);
	g_autoptr(GError) error_local = NULL;
	gint status = 0;

	g_ptr_array_add(full, (gpointer)"/bin/sh");
	g_ptr_array_add(full, (gpointer)"-c");
	g_ptr_array_add(full, (gpointer)"exec \"$@\" 2>&1");
	g_ptr_array_add(full, (gpointer)"sh");
	g_ptr_array_add(full, (gpointer)"timeout");
	g_ptr_array_add(full, (gpointer)"-k");
	g_ptr_array_add(full, (gpointer)"30");
	g_ptr_array_add(full, timeout_str);
	for (guint i = 0; i < argv->len; i++)
		g_ptr_array_add(full, g_ptr_array_index(argv, i));
	g_ptr_array_add(full, NULL);

	if (!g_spawn_sync(NULL,
			  (gchar **)full->pdata,
			  NULL,
			  G_SPAWN_SEARCH_PATH,
			  NULL,
			  NULL,
			  output,
			  NULL,
			  &status,
			  &error_local)) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_INTERNAL,
			    "cannot run '%s': %s",
			    (const gchar *)g_ptr_array_index(argv, 0),
			    error_local->message);
		return -1;
	}
	if (!WIFEXITED(status))
		return -1;
	return WEXITSTATUS(status);
}

/* Run a small shell snippet from the vendored helpers. */
static gint
fu_mrchromebox_run_bash(const gchar *script,
			const gchar *arg1,
			const gchar *arg2,
			const gchar *arg3,
			const gchar *arg4,
			const gchar *arg5,
			const gchar *arg6,
			guint timeout_sec,
			gchar **output,
			GError **error)
{
	g_autoptr(GPtrArray) argv = g_ptr_array_new();
	g_ptr_array_add(argv, (gpointer)"/bin/bash");
	g_ptr_array_add(argv, (gpointer)"-c");
	g_ptr_array_add(argv, (gpointer)script);
	g_ptr_array_add(argv, (gpointer)"pixel-blue-fwupd");
	g_ptr_array_add(argv, (gpointer)arg1);
	g_ptr_array_add(argv, (gpointer)arg2);
	g_ptr_array_add(argv, (gpointer)arg3);
	g_ptr_array_add(argv, (gpointer)arg4);
	g_ptr_array_add(argv, (gpointer)arg5);
	g_ptr_array_add(argv, (gpointer)arg6);
	return fu_mrchromebox_spawn(argv, timeout_sec, output, error);
}

static gchar *
fu_mrchromebox_json_get_string(const gchar *json, const gchar *key)
{
	g_autofree gchar *needle = g_strdup_printf("\"%s\"", key);
	const gchar *p = strstr(json, needle);
	const gchar *start;
	const gchar *end;
	if (p == NULL)
		return NULL;
	start = strchr(p + strlen(needle), ':');
	if (start == NULL)
		return NULL;
	start = strchr(start, '"');
	if (start == NULL)
		return NULL;
	start++;
	end = strchr(start, '"');
	if (end == NULL)
		return NULL;
	return g_strndup(start, (gsize)(end - start));
}

static gchar *
fu_mrchromebox_sha256_file(const gchar *filename)
{
	g_autoptr(GChecksum) csum = g_checksum_new(G_CHECKSUM_SHA256);
	g_autoptr(GFile) file = g_file_new_for_path(filename);
	g_autoptr(GInputStream) stream = NULL;
	guint8 buf[64 * 1024];
	gssize n;

	stream = G_INPUT_STREAM(g_file_read(file, NULL, NULL));
	if (stream == NULL)
		return NULL;
	while ((n = g_input_stream_read(stream, buf, sizeof(buf), NULL, NULL)) > 0)
		g_checksum_update(csum, buf, (gsize)n);
	if (n < 0)
		return NULL;
	return g_strdup(g_checksum_get_string(csum));
}

/* UINT8 payload of an EFI global variable, or -1 when it cannot be read. */
static gint
fu_mrchromebox_efi_var_u8(const gchar *name)
{
	g_autofree gchar *path = g_strdup_printf("/sys/firmware/efi/efivars/"
						 "%s-8be4df61-93ca-11d2-aa0d-00e098032b8c",
						 name);
	g_autofree gchar *data = NULL;
	gsize len = 0;
	if (!g_file_get_contents(path, &data, &len, NULL) || len < 5)
		return -1;
	return (gint)(guint8)data[4];
}

/* --- flashrom ----------------------------------------------------------- */

static void
fu_mrchromebox_add_flashrom_base(FuMrchromeboxDevice *self, GPtrArray *argv)
{
	g_ptr_array_add(argv, self->flashrom_path);
	g_ptr_array_add(argv, (gpointer)"-p");
	g_ptr_array_add(argv, (gpointer)"internal:boardmismatch=force");
	/* upstream appends this whenever isChromeOS=false, which is this device */
	if (self->first_chip)
		g_ptr_array_add(argv, (gpointer)"--use-first-chip");
}

/* Upstream expands $flashromcmd unquoted, so this must stay a single string. */
static gchar *
fu_mrchromebox_flashrom_string(FuMrchromeboxDevice *self)
{
	GString *str = g_string_new(self->flashrom_path);
	g_string_append(str, " -p internal:boardmismatch=force");
	if (self->first_chip)
		g_string_append(str, " --use-first-chip");
	return g_string_free(str, FALSE);
}

static gboolean
fu_mrchromebox_device_detect_caps(FuMrchromeboxDevice *self, GError **error)
{
	g_autoptr(GPtrArray) argv = g_ptr_array_new();
	g_autofree gchar *output = NULL;
	gint rc;

	if (self->flashrom_path == NULL)
		self->flashrom_path = g_strdup(FU_MRCHROMEBOX_FLASHROM);
	if (!g_file_test(self->flashrom_path, G_FILE_TEST_IS_EXECUTABLE)) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_NOT_SUPPORTED,
			    "the vendored flashrom %s is missing",
			    self->flashrom_path);
		return FALSE;
	}

	fu_mrchromebox_add_flashrom_base(self, argv);
	g_ptr_array_add(argv, (gpointer)"-h");
	rc = fu_mrchromebox_spawn(argv, FU_MRCHROMEBOX_TIMEOUT_PROBE, &output, error);
	if (rc < 0)
		return FALSE;
	if (rc != 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_NOT_SUPPORTED,
			    "flashrom is not usable: %s",
			    fu_mrchromebox_output_tail(output));
		return FALSE;
	}

	/*
	 * Probe rather than assume: the image pins the binary, but a future
	 * vendor bump must not silently change the flags the helpers rely on.
	 */
	self->has_ifd = strstr(output, "--ifd") != NULL;
	self->first_chip = strstr(output, "use-first-chip") != NULL;
	self->noverify = strstr(output, "noverify-all") != NULL;
	return TRUE;
}

/*
 * The Intel PR0/GPR0 check comes from the vendored helpers: it knows that a
 * read-only GPR0 covering only the ME region is harmless while one overlapping
 * the BIOS FREG is fatal.
 */
static gboolean
fu_mrchromebox_device_protection_range_ok(const gchar *probe_output, GError **error)
{
	static const gchar *script =
	    "set -euo pipefail\n"
	    ". \"$1/functions.sh\"\n"
	    "if intel_pr_gpr_locked \"$2\"; then exit 0; else exit 1; fi\n";
	g_autofree gchar *out = NULL;
	gint rc = fu_mrchromebox_run_bash(script,
					  FU_MRCHROMEBOX_VENDOR_DIR,
					  probe_output,
					  NULL,
					  NULL,
					  NULL,
					  NULL,
					  30,
					  &out,
					  error);
	if (rc < 0)
		return FALSE;
	if (rc == 0) {
		g_set_error_literal(error,
				    FWUPD_ERROR,
				    FWUPD_ERROR_NOT_SUPPORTED,
				    "Intel SPI protected range (PR0/GPR0) covers the BIOS region");
		return FALSE;
	}
	if (rc != 1) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_NOT_SUPPORTED,
			    "cannot evaluate the Intel SPI protected ranges: %s",
			    fu_mrchromebox_output_tail(out));
		return FALSE;
	}
	return TRUE;
}

/*
 * The kernel's spi_intel_platform driver owns the SPI bus, which makes the
 * flashrom internal programmer fail or hang. Upstream firmware-util.sh does
 * the same rmmod before it probes anything.
 */
static void
fu_mrchromebox_device_unload_spi_driver(void)
{
	g_autoptr(GPtrArray) argv = g_ptr_array_new();
	g_autoptr(GError) error_local = NULL;
	g_autofree gchar *output = NULL;
	gint rc;

	g_ptr_array_add(argv, (gpointer)"rmmod");
	g_ptr_array_add(argv, (gpointer)"spi_intel_platform");
	rc = fu_mrchromebox_spawn(argv, 30, &output, &error_local);
	if (rc < 0)
		g_debug("cannot run rmmod: %s",
			error_local != NULL ? error_local->message : "unknown error");
	else if (rc != 0)
		g_debug("spi_intel_platform is not loaded: %s",
			fu_mrchromebox_output_tail(output));
}

static gboolean
fu_mrchromebox_device_preflight(FuMrchromeboxDevice *self, GError **error)
{
	g_autoptr(GPtrArray) argv = NULL;
	g_autofree gchar *probe = NULL;
	g_autofree gchar *wp = NULL;
	g_autofree gchar *lower = NULL;
	gint rc;
	gint secure;
	gint setup;

	fu_mrchromebox_device_unload_spi_driver();

	if (!fu_mrchromebox_device_detect_caps(self, error))
		return FALSE;
	if (!self->has_ifd) {
		g_set_error_literal(error,
				    FWUPD_ERROR,
				    FWUPD_ERROR_NOT_SUPPORTED,
				    "flashrom does not support --ifd region writes");
		return FALSE;
	}

	/* UEFI Secure Boot blocks the flashrom internal programmer */
	secure = fu_mrchromebox_efi_var_u8("SecureBoot");
	setup = fu_mrchromebox_efi_var_u8("SetupMode");
	if (secure < 0) {
		g_set_error_literal(error,
				    FWUPD_ERROR,
				    FWUPD_ERROR_NOT_SUPPORTED,
				    "cannot read the Secure Boot state: efivars are unavailable");
		return FALSE;
	}
	if (secure == 1 && setup != 1) {
		g_set_error_literal(error,
				    FWUPD_ERROR,
				    FWUPD_ERROR_NOT_SUPPORTED,
				    "Secure Boot is enabled; turn it off in the firmware setup first");
		return FALSE;
	}

	argv = g_ptr_array_new();
	fu_mrchromebox_add_flashrom_base(self, argv);
	rc = fu_mrchromebox_spawn(argv, FU_MRCHROMEBOX_TIMEOUT_PROBE, &probe, error);
	if (rc < 0)
		return FALSE;
	lower = g_ascii_strdown(probe, -1);
	if (strstr(lower, "bios region smm protection is enabled") != NULL) {
		g_set_error_literal(error,
				    FWUPD_ERROR,
				    FWUPD_ERROR_NOT_SUPPORTED,
				    "BIOS Lock / SMM write protection is enabled; "
				    "reboot into the firmware setup and disable BIOS Lock");
		return FALSE;
	}
	/*
	 * A probe-only run exits non-zero unless exactly one chip was found: no
	 * chip, several matching definitions and "too big for this programmer" all
	 * fail here, which is the gate we want before touching anything.
	 */
	if (rc != 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_NOT_SUPPORTED,
			    "flashrom cannot probe the SPI flash: %s",
			    fu_mrchromebox_output_tail(probe));
		return FALSE;
	}
	if (!fu_mrchromebox_device_protection_range_ok(probe, error))
		return FALSE;

	g_clear_pointer(&argv, g_ptr_array_unref);
	argv = g_ptr_array_new();
	fu_mrchromebox_add_flashrom_base(self, argv);
	g_ptr_array_add(argv, (gpointer)"--wp-status");
	rc = fu_mrchromebox_spawn(argv, FU_MRCHROMEBOX_TIMEOUT_WP, &wp, error);
	if (rc < 0)
		return FALSE;
	if (rc != 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_NOT_SUPPORTED,
			    "cannot read the write protection status: %s",
			    fu_mrchromebox_output_tail(wp));
		return FALSE;
	}
	g_free(lower);
	lower = g_ascii_strdown(wp, -1);
	if (strstr(lower, "protection mode: hardware") != NULL ||
	    strstr(lower, "protection mode: permanent") != NULL ||
	    strstr(lower, "write protect is hardware") != NULL) {
		g_set_error_literal(error,
				    FWUPD_ERROR,
				    FWUPD_ERROR_NOT_SUPPORTED,
				    "hardware write protection is enabled; "
				    "the case must be opened and WP disconnected before flashing");
		return FALSE;
	}

	/* same classification as upstream clear_software_wp() */
	g_free(self->sw_wp);
	self->sw_wp = strstr(lower, "enabled") != NULL ? g_strdup("enabled")
						      : g_strdup("disabled");
	return TRUE;
}

/* --- backup / write / verify -------------------------------------------- */

static gboolean
fu_mrchromebox_device_backup(FuMrchromeboxDevice *self, GError **error)
{
	g_autofree gchar *backup = g_build_filename(FU_MRCHROMEBOX_STATE_DIR, "backup.rom", NULL);
	g_autofree gchar *json_path =
	    g_build_filename(FU_MRCHROMEBOX_STATE_DIR, "backup.json", NULL);
	g_autofree gchar *region = g_strconcat(backup, ".region", NULL);
	g_autofree gchar *full = g_strconcat(backup, ".full", NULL);
	const gchar *version = fu_device_get_version(FU_DEVICE(self));
	g_autofree gchar *json_old = NULL;
	g_autofree gchar *old_sha = NULL;
	g_autofree gchar *old_ver = NULL;
	g_autofree gchar *output = NULL;
	g_autofree gchar *json_new = NULL;
	g_autofree gchar *sha256 = NULL;
	g_autofree gchar *taken_at = NULL;
	g_autoptr(GDateTime) now = g_date_time_new_now_local();
	g_autoptr(GPtrArray) argv = NULL;
	GStatBuf st = {0};
	const gchar *keep = NULL;
	const gchar *scope = "bios-region";
	gint rc;

	g_mkdir_with_parents(FU_MRCHROMEBOX_STATE_DIR, 0755);

	/*
	 * A retry must not overwrite the good pre-failure backup with the
	 * possibly half-written current flash. When backup.json describes the
	 * firmware installed right now and its checksum still matches, reuse it.
	 */
	if (g_file_get_contents(json_path, &json_old, NULL, NULL)) {
		old_sha = fu_mrchromebox_json_get_string(json_old, "sha256");
		old_ver = fu_mrchromebox_json_get_string(json_old, "installed_version");
	}
	if (g_file_test(backup, G_FILE_TEST_IS_REGULAR) && old_sha != NULL &&
	    g_strcmp0(old_ver, version) == 0) {
		g_autofree gchar *sha_now = fu_mrchromebox_sha256_file(backup);
		if (g_strcmp0(sha_now, old_sha) == 0) {
			g_debug("reusing the existing backup for %s", version);
			return TRUE;
		}
	}

	/* the BIOS region is mandatory: it decides the write parameters */
	g_unlink(region);
	argv = g_ptr_array_new();
	fu_mrchromebox_add_flashrom_base(self, argv);
	g_ptr_array_add(argv, (gpointer)"--ifd");
	g_ptr_array_add(argv, (gpointer)"-i");
	g_ptr_array_add(argv, (gpointer)"bios");
	g_ptr_array_add(argv, (gpointer)"-r");
	g_ptr_array_add(argv, (gpointer)region);
	rc = fu_mrchromebox_spawn(argv, FU_MRCHROMEBOX_TIMEOUT_READ, &output, error);
	if (rc < 0)
		return FALSE;
	if (rc != 0 || !g_file_test(region, G_FILE_TEST_IS_REGULAR)) {
		g_unlink(region);
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_READ,
			    "cannot back up the BIOS region, refusing to touch the flash: %s",
			    fu_mrchromebox_output_tail(output));
		return FALSE;
	}
	keep = region;

	/* a full-chip dump is what an external CH341A restore wants */
	g_clear_pointer(&argv, g_ptr_array_unref);
	g_clear_pointer(&output, g_free);
	g_unlink(full);
	argv = g_ptr_array_new();
	fu_mrchromebox_add_flashrom_base(self, argv);
	g_ptr_array_add(argv, (gpointer)"-r");
	g_ptr_array_add(argv, (gpointer)full);
	rc = fu_mrchromebox_spawn(argv, FU_MRCHROMEBOX_TIMEOUT_READ, &output, error);
	if (rc < 0)
		return FALSE;
	if (rc == 0 && g_file_test(full, G_FILE_TEST_IS_REGULAR)) {
		scope = "full-chip";
		keep = full;
	} else {
		g_unlink(full);
		g_debug("full-chip backup unavailable, keeping the BIOS region");
	}

	if (g_rename(keep, backup) != 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_INTERNAL,
			    "cannot store the firmware backup at %s",
			    backup);
		return FALSE;
	}
	g_chmod(backup, 0600);
	g_unlink(region);
	g_unlink(full);

	sha256 = fu_mrchromebox_sha256_file(backup);
	if (sha256 == NULL) {
		g_set_error_literal(error, FWUPD_ERROR, FWUPD_ERROR_INTERNAL,
				    "cannot checksum the firmware backup");
		return FALSE;
	}
	taken_at = now != NULL ? g_date_time_format(now, "%Y-%m-%dT%H:%M:%S%z") : g_strdup("");
	if (g_stat(backup, &st) != 0)
		st.st_size = 0;
	json_new = g_strdup_printf("{\n"
				   "  \"scope\": \"%s\",\n"
				   "  \"installed_version\": \"%s\",\n"
				   "  \"installed_date\": \"%s\",\n"
				   "  \"sha256\": \"%s\",\n"
				   "  \"size\": %" G_GINT64_FORMAT ",\n"
				   "  \"taken_at\": \"%s\"\n"
				   "}\n",
				   scope,
				   version != NULL ? version : "",
				   version != NULL ? version : "",
				   sha256,
				   (gint64)st.st_size,
				   taken_at);
	g_file_set_contents(json_path, json_new, -1, NULL);
	g_chmod(json_path, 0644);
	g_debug("firmware backup taken (%s)", scope);
	return TRUE;
}

static gboolean
fu_mrchromebox_device_clear_software_wp(FuMrchromeboxDevice *self, GError **error)
{
	static const gchar *script =
	    "set -euo pipefail\n"
	    ". \"$1/functions.sh\"\n"
	    "flashromcmd=\"$2\"\n"
	    "swWp=\"$3\"\n"
	    "export MRCBX_LOG=\"$4\"\n"
	    "clear_software_wp flash\n";
	g_autofree gchar *flashrom_cmd = fu_mrchromebox_flashrom_string(self);
	g_autofree gchar *output = NULL;
	gint rc = fu_mrchromebox_run_bash(script,
					  FU_MRCHROMEBOX_VENDOR_DIR,
					  flashrom_cmd,
					  self->sw_wp != NULL ? self->sw_wp : "disabled",
					  self->log_path,
					  NULL,
					  NULL,
					  FU_MRCHROMEBOX_TIMEOUT_WP,
					  &output,
					  error);
	if (rc < 0)
		return FALSE;
	if (rc != 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_WRITE,
			    "cannot clear the software write protection, the flash was not modified");
		return FALSE;
	}
	return TRUE;
}

static gboolean
fu_mrchromebox_device_flash(FuMrchromeboxDevice *self, GError **error)
{
	static const gchar *script =
	    "set -euo pipefail\n"
	    ". \"$1/functions.sh\"\n"
	    "flashromcmd=\"$2\"\n"
	    "noverify=\"$3\"\n"
	    "flashrom_params=\"$4\"\n"
	    "export MRCBX_LOG=\"$5\"\n"
	    "firmwareType='Full ROM / UEFI'\n"
	    "isChromeOS=false\n"
	    "isChromiumOS=false\n"
	    "isIntel=true\n"
	    "flashrom_write_image \"$6\" pixel-blue\n";
	g_autofree gchar *flashrom_cmd = fu_mrchromebox_flashrom_string(self);
	g_autofree gchar *output = NULL;
	gint rc = fu_mrchromebox_run_bash(script,
					  FU_MRCHROMEBOX_VENDOR_DIR,
					  flashrom_cmd,
					  self->noverify ? "-N" : "",
					  "--ifd -i bios",
					  self->log_path,
					  self->rom_path,
					  FU_MRCHROMEBOX_TIMEOUT_WRITE,
					  &output,
					  error);
	if (rc < 0)
		return FALSE;
	if (rc != 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_WRITE,
			    "the SPI flash write failed or was interrupted. "
			    "Do NOT reboot: retry the update first. %s",
			    fu_mrchromebox_output_tail(output));
		return FALSE;
	}
	return TRUE;
}

static gboolean
fu_mrchromebox_device_verify(FuMrchromeboxDevice *self, GError **error)
{
	g_autoptr(GPtrArray) argv = g_ptr_array_new();
	g_autofree gchar *output = NULL;
	gint rc;

	fu_mrchromebox_add_flashrom_base(self, argv);
	g_ptr_array_add(argv, (gpointer)"--ifd");
	g_ptr_array_add(argv, (gpointer)"-i");
	g_ptr_array_add(argv, (gpointer)"bios");
	g_ptr_array_add(argv, (gpointer)"-v");
	g_ptr_array_add(argv, (gpointer)self->rom_path);
	rc = fu_mrchromebox_spawn(argv, FU_MRCHROMEBOX_TIMEOUT_VERIFY, &output, error);
	if (rc < 0)
		return FALSE;
	if (rc != 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_WRITE,
			    "verifying the written image failed. "
			    "Do NOT reboot: retry the update first. %s",
			    fu_mrchromebox_output_tail(output));
		return FALSE;
	}
	return TRUE;
}

/* --- FuDevice vfuncs ---------------------------------------------------- */

static gboolean
fu_mrchromebox_device_prepare(FuDevice *device,
			      FuProgress *progress,
			      FwupdInstallFlags flags,
			      GError **error)
{
	FuMrchromeboxDevice *self = FU_MRCHROMEBOX_DEVICE(device);

	fu_progress_set_status(progress, FWUPD_STATUS_DEVICE_BUSY);
	if (!fu_mrchromebox_device_preflight(self, error))
		return FALSE;

	fu_progress_set_status(progress, FWUPD_STATUS_DEVICE_READ);
	if (!fu_mrchromebox_device_backup(self, error))
		return FALSE;
	return TRUE;
}

static gboolean
fu_mrchromebox_device_write_firmware(FuDevice *device,
				     FuFirmware *firmware,
				     FuProgress *progress,
				     FwupdInstallFlags flags,
				     GError **error)
{
	FuMrchromeboxDevice *self = FU_MRCHROMEBOX_DEVICE(device);
	g_autoptr(GBytes) blob = fu_firmware_get_bytes(firmware, error);
	gsize sz = 0;

	if (blob == NULL)
		return FALSE;
	g_bytes_get_data(blob, &sz);
	if (sz < FU_MRCHROMEBOX_SIZE_MIN || sz > FU_MRCHROMEBOX_SIZE_MAX) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_INVALID_FILE,
			    "unexpected firmware size 0x%x",
			    (guint)sz);
		return FALSE;
	}

	fu_progress_set_status(progress, FWUPD_STATUS_DEVICE_WRITE);
	if (!fu_bytes_set_contents(self->rom_path, blob, error))
		return FALSE;
	if (g_chmod(self->rom_path, 0600) != 0)
		g_debug("cannot tighten the permissions of %s", self->rom_path);

	/* nothing below here is recoverable by a reboot alone */
	if (!fu_mrchromebox_device_clear_software_wp(self, error))
		return FALSE;
	if (!fu_mrchromebox_device_flash(self, error))
		return FALSE;

	fu_progress_set_status(progress, FWUPD_STATUS_DEVICE_VERIFY);
	if (!fu_mrchromebox_device_verify(self, error))
		return FALSE;

	/* fwupd reads DMI again after the reboot and clears this then */
	fu_device_add_flag(device, FWUPD_DEVICE_FLAG_NEEDS_REBOOT);
	return TRUE;
}

static gboolean
fu_mrchromebox_device_cleanup(FuDevice *device,
			      FuProgress *progress,
			      FwupdInstallFlags flags,
			      GError **error)
{
	FuMrchromeboxDevice *self = FU_MRCHROMEBOX_DEVICE(device);
	g_unlink(self->rom_path);
	g_clear_pointer(&self->sw_wp, g_free);
	return TRUE;
}

static void
fu_mrchromebox_device_class_init(FuMrchromeboxDeviceClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	FuDeviceClass *device_class = FU_DEVICE_CLASS(klass);

	object_class->finalize = fu_mrchromebox_device_finalize;
	device_class->prepare = fu_mrchromebox_device_prepare;
	device_class->write_firmware = fu_mrchromebox_device_write_firmware;
	device_class->cleanup = fu_mrchromebox_device_cleanup;
}
