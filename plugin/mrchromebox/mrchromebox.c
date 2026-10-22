/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * mrchromebox - fwupd out-of-tree plugin for MrChromebox UEFI Full ROM
 * firmware updates on the Google Pixelbook (eve).
 *
 * Pixel Blue image (zhchmmx/pixel-blue), Issue #69.
 *
 * The plugin registers a virtual device whose version is the DMI BIOS release
 * date (YYYYMMDD).  Update metadata comes from the local remote
 * pixel-blue-mrchromebox, generated at runtime by
 * /usr/libexec/firmware-update-check.sh.  When the user clicks Update in
 * GNOME Software, the daemon (running as root) invokes write_firmware(),
 * which mirrors the upstream MrChromebox firmware-util.sh logic: probe for
 * BIOS Lock / SPI protected ranges, auto-clear the software write-protect,
 * back up the current flash, then write and verify while holding a
 * systemd-inhibit lock.  On failure a marker file is left under
 * /var/lib/pixel-blue/fwupd/; the user-level pixel-blue-flash-alert units
 * consume it and keep telling the user never to reboot until a retry
 * succeeds.
 */

#include <fwupd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mrchromebox-plugin.h"

#define MRCHROMEBOX_GUID "44163a1d-13d2-42c7-bce3-fb8483125bb2"
#define MRCHROMEBOX_PHYSICAL_ID "mrchromebox-uefi"
#define MRCHROMEBOX_LOGICAL_ID "full-rom"
#define MRCHROMEBOX_STATE_DIR "/var/lib/pixel-blue/fwupd"
#define MRCHROMEBOX_MARKER MRCHROMEBOX_STATE_DIR "/flash-failed"
#define MRCHROMEBOX_BACKUP MRCHROMEBOX_STATE_DIR "/backup.rom"
#define MRCHROMEBOX_BATTERY_THRESHOLD 50

struct MrChromeboxPluginData {
	gchar *programmer;
};

/* --- process helpers ---------------------------------------------------- */

static gboolean
fu_plugin_run_argv(const gchar *const *argv,
		   gint *exit_status,
		   gchar **stdout_buf,
		   gchar **stderr_buf,
		   GError **error)
{
	g_autoptr(GSubprocess) sub = NULL;
	g_autofree gchar *stdout_data = NULL;
	g_autofree gchar *stderr_data = NULL;
	g_autoptr(GError) error_local = NULL;

	sub = g_subprocess_newv(argv,
				G_SUBPROCESS_FLAGS_STDOUT_PIPE |
				    G_SUBPROCESS_FLAGS_STDERR_PIPE,
				&error_local);
	if (sub == NULL) {
		g_propagate_error(error, g_steal_pointer(&error_local));
		return FALSE;
	}
	if (!g_subprocess_communicate_utf8(sub,
					   NULL,
					   NULL,
					   &stdout_data,
					   &stderr_data,
					   &error_local)) {
		g_propagate_error(error, g_steal_pointer(&error_local));
		return FALSE;
	}
	if (exit_status != NULL)
		*exit_status = g_subprocess_get_exit_status(sub);
	if (stdout_buf != NULL)
		*stdout_buf = g_steal_pointer(&stdout_data);
	if (stderr_buf != NULL) {
		*stderr_buf = g_steal_pointer(&stderr_data);
	} else if (stderr_data != NULL && stderr_data[0] != '\0') {
		/* flashrom routes WARN/ERROR (msg_pwarn/msg_perr) to stderr, and
		 * the SMM/PR safety-gate strings live there; only fall back to
		 * debug output when the caller did not ask for stderr. */
		g_debug("mrchromebox: stderr: %s", stderr_data);
	}
	return TRUE;
}

static gboolean
fu_plugin_flashrom_run(FuPlugin *self,
		       const gchar *const *args,
		       gint *exit_status,
		       gchar **stdout_buf,
		       gchar **stderr_buf,
		       GError **error)
{
	struct MrChromeboxPluginData *data =
	    (struct MrChromeboxPluginData *)fu_plugin_get_data(self);
	g_autoptr(GPtrArray) argv = g_ptr_array_new_with_free_func(g_free);

	g_ptr_array_add(argv, g_strdup("flashrom"));
	g_ptr_array_add(argv, g_strdup("-p"));
	g_ptr_array_add(argv,
			g_strdup(data->programmer != NULL ? data->programmer
							  : "internal"));
	for (guint i = 0; args[i] != NULL; i++)
		g_ptr_array_add(argv, g_strdup(args[i]));
	g_ptr_array_add(argv, NULL);

	return fu_plugin_run_argv((const gchar *const *)argv->pdata,
				  exit_status,
				  stdout_buf,
				  stderr_buf,
				  error);
}

static gboolean
fu_plugin_flashrom_inhibited_run(FuPlugin *self,
				 const gchar *const *args,
				 gint *exit_status,
				 gchar **stdout_buf,
				 gchar **stderr_buf,
				 GError **error)
{
	struct MrChromeboxPluginData *data =
	    (struct MrChromeboxPluginData *)fu_plugin_get_data(self);
	g_autoptr(GPtrArray) argv = g_ptr_array_new_with_free_func(g_free);

	/* Block shutdown/sleep for the duration of the actual write. */
	if (g_file_test("/usr/bin/systemd-inhibit", G_FILE_TEST_IS_EXECUTABLE)) {
		g_ptr_array_add(argv, g_strdup("systemd-inhibit"));
		g_ptr_array_add(argv, g_strdup("--what=shutdown:sleep"));
		g_ptr_array_add(argv, g_strdup("--mode=block"));
		g_ptr_array_add(argv, g_strdup("--who=mrchromebox-fwupd"));
		g_ptr_array_add(argv, g_strdup("--why=Flashing UEFI firmware"));
		g_ptr_array_add(argv, g_strdup("--"));
	}
	g_ptr_array_add(argv, g_strdup("flashrom"));
	g_ptr_array_add(argv, g_strdup("-p"));
	g_ptr_array_add(argv,
			g_strdup(data->programmer != NULL ? data->programmer
							  : "internal"));
	for (guint i = 0; args[i] != NULL; i++)
		g_ptr_array_add(argv, g_strdup(args[i]));
	g_ptr_array_add(argv, NULL);

	return fu_plugin_run_argv((const gchar *const *)argv->pdata,
				  exit_status,
				  stdout_buf,
				  stderr_buf,
				  error);
}

/* --- DMI helpers -------------------------------------------------------- */

static gchar *
fu_plugin_dmi_read(const gchar *key, GError **error)
{
	g_autofree gchar *path = g_strdup_printf("/sys/class/dmi/id/%s", key);
	g_autofree gchar *buf = NULL;
	gsize len = 0;

	if (!g_file_get_contents(path, &buf, &len, error)) {
		g_prefix_error(error, "failed to read %s: ", path);
		return NULL;
	}
	while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
		buf[--len] = '\0';
	return g_steal_pointer(&buf);
}

/* MM/DD/YYYY -> YYYYMMDD */
static guint
fu_plugin_bios_date_to_number(const gchar *bios_date, GError **error)
{
	guint month = 0, day = 0, year = 0;

	if (sscanf(bios_date, "%u/%u/%u", &month, &day, &year) != 3 ||
	    month < 1 || month > 12 || day < 1 || day > 31 || year < 2000) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_INVALID_DATA,
			    "invalid bios_date '%s'",
			    bios_date);
		return 0;
	}
	return year * 10000 + month * 100 + day;
}

/* YYYYMMDD -> "YYYYMMDD" (owned by caller). */
static gchar *
fu_plugin_number_to_bios_date(guint version)
{
	return g_strdup_printf("%08u", version);
}

/* --- write-protect / flash access helpers ------------------------------- */

/* Port of MrChromebox intel_pr_gpr_locked(): true if a PR0/GPR0 read-only
 * range overlaps the BIOS FREG.  ME-only GPR0 is ignored; FREG0/FREG2 being
 * read-only is normal.  Without a parseable BIOS FREG we cannot prove the
 * ranges are disjoint, so behave conservatively.
 *
 * The BIOS FREG line is printed with msg_pinfo (stdout):
 *     "FREG1: BIOS region (0x%08x-0x%08x) is read-write."
 * while the PR/GPR ranges are warnings (msg_pwarn -> stderr), each prefixed:
 *     "PR0: Warning: 0x%08x-0x%08x is read-only."
 *     "GPR0: Warning: 0x%08x-0x%08x is locked."
 * Pass the concatenated stdout+stderr so both classes of lines match. */
static gboolean
fu_plugin_pr_gpr_locked(const gchar *probe_all)
{
	g_auto(GStrv) lines = g_strsplit(probe_all, "\n", -1);
	guint64 bios_start = 0;
	guint64 bios_end = 0;
	gboolean have_bios = FALSE;

	for (guint i = 0; lines[i] != NULL; i++) {
		guint64 a = 0;
		guint64 b = 0;

		if (sscanf(lines[i],
			   "FREG1: BIOS region (0x%llx-0x%llx)",
			   (unsigned long long *)&a,
			   (unsigned long long *)&b) == 2) {
			bios_start = a;
			bios_end = b;
			have_bios = TRUE;
		}
	}
	if (!have_bios) {
		/* Without a parseable BIOS FREG we cannot prove the ranges are
		 * disjoint; this is a bricking-risk flow, so fail closed: the
		 * caller refuses to flash and surfaces the preflight error. */
		return TRUE;
	}

	for (guint i = 0; lines[i] != NULL; i++) {
		guint64 r_start = 0;
		guint64 r_end = 0;
		const gchar *line = lines[i];

		if (sscanf(line,
			   "PR0: Warning: 0x%llx-0x%llx",
			   (unsigned long long *)&r_start,
			   (unsigned long long *)&r_end) != 2 &&
		    sscanf(line,
			   "GPR0: Warning: 0x%llx-0x%llx",
			   (unsigned long long *)&r_start,
			   (unsigned long long *)&r_end) != 2)
			continue;
		/* The trailing state is "read-only." or "locked."; either one
		 * blocks a host write into that window. */
		if (r_start <= bios_end && bios_start <= r_end)
			return TRUE;
	}
	return FALSE;
}

static gboolean
fu_plugin_probe_flash_access(FuPlugin *self, GError **error)
{
	g_autofree gchar *probe_out = NULL;
	g_autofree gchar *probe_err = NULL;
	gint exit_status = 0;
	static const gchar *const args[] = {NULL};

	/* A failed probe is not fatal by itself: treat it as "no info" and let
	 * the actual write surface any real problem.  SMM and PR warnings are
	 * emitted via msg_pwarn -> stderr, so capture both streams. */
	if (!fu_plugin_flashrom_run(self,
				    args,
				    &exit_status,
				    &probe_out,
				    &probe_err,
				    NULL))
		return TRUE;

	/* SMM protection is announced with msg_pwarn -> stderr ("BIOS region SMM
	 * protection is enabled").  The PR/GPR overlap gate needs both classes of
	 * line in ONE buffer: the BIOS FREG is msg_pinfo -> stdout, while the
	 * PR0:/GPR0: warnings are msg_pwarn -> stderr.  Merge before scanning. */
	g_autofree gchar *probe_all = g_strdup_printf("%s%s",
						      probe_out != NULL ? probe_out : "",
						      probe_err != NULL ? probe_err : "");
	if (strstr(probe_all, "BIOS region SMM protection is enabled") != NULL) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_PERMISSION_DENIED,
			    "BIOS Lock (SMM protection) is enabled and blocks host flash "
			    "writes; reboot, disable BIOS Lock in the firmware setup menu, "
			    "then retry");
		return FALSE;
	}
	if (fu_plugin_pr_gpr_locked(probe_all)) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_PERMISSION_DENIED,
			    "Intel SPI protected ranges (PR0/GPR0) overlap the BIOS region; "
			    "disable hardware write-protect, reboot, then retry");
		return FALSE;
	}
	return TRUE;
}

static gboolean
fu_plugin_wp_status(FuPlugin *self, gboolean *wp_supported, gboolean *wp_enabled, GError **error)
{
	g_autofree gchar *status_out = NULL;
	g_autofree gchar *status_err = NULL;
	gint exit_status = 0;
	static const gchar *const args[] = {"--wp-status", NULL};

	if (!fu_plugin_flashrom_run(self,
				    args,
				    &exit_status,
				    &status_out,
				    &status_err,
				    error))
		return FALSE;
	if (status_out == NULL) {
		*wp_supported = FALSE;
		*wp_enabled = FALSE;
		return TRUE;
	}
	/* flashrom --wp-status (1.4+) prints "Protection range: start=…
	 * length=…" and "Protection mode: disabled|hardware|power_cycle|
	 * permanent|unknown".  Programmers without WP registers print neither
	 * line.  Match on the actual "Protection mode:" line, not the
	 * non-existent "write-protect:" strings older code assumed. */
	{
		const gchar *pos = strstr(status_out, "Protection mode:");
		if (pos == NULL) {
			/* flashrom explains an unreadable/permanent WP on stderr. */
			if (status_err != NULL && status_err[0] != '\0')
				g_debug("mrchromebox: --wp-status stderr: %s", status_err);
			*wp_supported = FALSE;
			*wp_enabled = FALSE;
			return TRUE;
		}
		pos += strlen("Protection mode:");
		while (g_ascii_isspace(*pos))
			pos++;
		*wp_supported = TRUE;
		/* "disabled" may carry a trailing note (e.g. "(active)"). */
		*wp_enabled = !g_str_has_prefix(pos, "disabled");
	}
	return TRUE;
}

/* Mirrors upstream clear_software_wp(): --wp-disable, then
 * --wp-range 0,0 (start=0, len=0 unprotects the whole flash; this is the
 * documented form - the "0 0" two-argument form does not exist). */
static gboolean
fu_plugin_wp_disable(FuPlugin *self, GError **error)
{
	gint exit_status = 0;
	g_autofree gchar *out = NULL;
	static const gchar *const args_disable[] = {"--wp-disable", NULL};
	static const gchar *const args_range[] = {"--wp-range", "0,0", NULL};

	/* --wp-disable fails cleanly when WP is already off, which is fine. */
	if (!fu_plugin_flashrom_run(self, args_disable, &exit_status, &out, NULL, NULL) ||
	    exit_status != 0)
		g_debug("mrchromebox: --wp-disable did not report success");
	if (fu_plugin_flashrom_run(self, args_range, &exit_status, &out, NULL, NULL) &&
	    exit_status == 0)
		return TRUE;
	g_set_error(error,
		    G_IO_ERROR,
		    G_IO_ERROR_FAILED,
		    "failed to clear the software write-protect");
	return FALSE;
}

static gboolean
fu_plugin_check_power(GError **error)
{
	g_autoptr(GDir) dir = NULL;
	const gchar *name = NULL;
	gboolean ac_online = FALSE;

	dir = g_dir_open("/sys/class/power_supply", 0, error);
	if (dir == NULL) {
		g_prefix_error(error, "failed to open /sys/class/power_supply: ");
		return FALSE;
	}
	while ((name = g_dir_read_name(dir)) != NULL) {
		g_autofree gchar *path = NULL;
		g_autofree gchar *buf = NULL;

		if (!g_str_has_prefix(name, "AC") && !g_str_has_prefix(name, "ADP"))
			continue;
		path = g_strdup_printf("/sys/class/power_supply/%s/online", name);
		if (g_file_get_contents(path, &buf, NULL, NULL) &&
		    g_str_has_prefix(g_strstrip(buf), "1"))
			ac_online = TRUE;
	}
	if (ac_online)
		return TRUE;

	g_dir_rewind(dir);
	while ((name = g_dir_read_name(dir)) != NULL) {
		g_autofree gchar *path = NULL;
		g_autofree gchar *buf = NULL;
		guint capacity = 0;

		if (!g_str_has_prefix(name, "BAT"))
			continue;
		path = g_strdup_printf("/sys/class/power_supply/%s/capacity", name);
		if (g_file_get_contents(path, &buf, NULL, NULL) &&
		    sscanf(buf, "%u", &capacity) == 1 &&
		    capacity >= MRCHROMEBOX_BATTERY_THRESHOLD)
			return TRUE;
	}
	g_set_error(error,
		    G_IO_ERROR,
		    G_IO_ERROR_FAILED,
		    "no AC power and battery below %u%%, plug in the charger and retry",
		    MRCHROMEBOX_BATTERY_THRESHOLD);
	return FALSE;
}

/* --- failure marker ------------------------------------------------------ */

static guint
fu_plugin_marker_attempts(void)
{
	g_autofree gchar *contents = NULL;
	guint attempts = 0;

	if (!g_file_get_contents(MRCHROMEBOX_MARKER, &contents, NULL, NULL))
		return 0;
	g_auto(GStrv) lines = g_strsplit(contents, "\n", -1);
	for (guint i = 0; lines[i] != NULL; i++) {
		if (g_str_has_prefix(lines[i], "attempts=")) {
			attempts = (guint)g_ascii_strtoull(lines[i] +
							   strlen("attempts="),
							   NULL,
							   10);
			break;
		}
	}
	return attempts;
}

static void
fu_plugin_marker_write(gboolean write_started, guint attempts)
{
	g_autofree gchar *buf = NULL;
	g_autoptr(GError) error_local = NULL;

	if (g_mkdir_with_parents(MRCHROMEBOX_STATE_DIR, 0755) != 0) {
		g_warning("mrchromebox: failed to create %s: %s",
			  MRCHROMEBOX_STATE_DIR,
			  g_strerror(errno));
		return;
	}
	buf = g_strdup_printf("class=%s\nattempts=%u\n",
			      write_started ? "B" : "A",
			      attempts);
	if (!g_file_set_contents(MRCHROMEBOX_MARKER, buf, -1, &error_local))
		g_warning("mrchromebox: failed to write marker: %s",
			  error_local->message);
}

static void
fu_plugin_marker_remove(void)
{
	if (g_file_test(MRCHROMEBOX_MARKER, G_FILE_TEST_EXISTS))
		g_debug("mrchromebox: removing %s", MRCHROMEBOX_MARKER);
	(void)unlink(MRCHROMEBOX_MARKER);
}

/* --- write_firmware ------------------------------------------------------ */

/* Unified failure exit: the GError text is fixed and bilingual for both
 * classes (class A is a conservative no-op, class B is life-saving), details
 * go to the journal and the marker.  Never set NEEDS_REBOOT on failure. */
static gboolean
fu_plugin_fail(FuPlugin *self G_GNUC_UNUSED,
	       gboolean write_started,
	       guint attempts,
	       GError **error)
{
	g_set_error_literal(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_WRITE,
			    "固件更新失败：千万不要重启电脑，请在「更新」页再点一次「更新」重试 "
			    "/ FAILED: do NOT restart - click Update again on the "
			    "Updates page");
	fu_plugin_marker_write(write_started, attempts);
	if (attempts >= 3) {
		g_warning("mrchromebox: %u consecutive failures; keep the machine "
			  "powered on and run 'sudo firmware-util.sh' for "
			  "diagnosis - still do NOT restart",
			  attempts);
	}
	return FALSE;
}

static gboolean
fu_plugin_write_flash(FuPlugin *self, const gchar *fwfile, GError **error)
{
	gint exit_status = 0;
	g_autofree gchar *out = NULL;
	const gchar *args[] = {"-w", fwfile, NULL};

	/* fu_plugin_flashrom_inhibited_run returns TRUE as long as the process
	 * ran at all; a non-zero exit leaves *error unset, so fill it here -
	 * callers dereference it unconditionally. */
	if (!fu_plugin_flashrom_inhibited_run(self,
					       args,
					       &exit_status,
					       &out,
					       NULL,
					       error) ||
	    exit_status != 0) {
		if (error != NULL && *error == NULL)
			g_set_error(error,
				    G_IO_ERROR,
				    G_IO_ERROR_FAILED,
				    "flashrom write exited with status %d",
				    exit_status);
		return FALSE;
	}
	return TRUE;
}

static gboolean
fu_plugin_verify_flash(FuPlugin *self, const gchar *fwfile, GError **error)
{
	gint exit_status = 0;
	g_autofree gchar *out = NULL;
	const gchar *args[] = {"-v", fwfile, NULL};

	/* Same contract as write: a non-zero exit must still yield an error
	 * object so callers can dereference it safely. */
	if (!fu_plugin_flashrom_run(self,
				     args,
				     &exit_status,
				     &out,
				     NULL,
				     error) ||
	    exit_status != 0) {
		if (error != NULL && *error == NULL)
			g_set_error(error,
				    G_IO_ERROR,
				    G_IO_ERROR_FAILED,
				    "flashrom verify exited with status %d",
				    exit_status);
		return FALSE;
	}
	return TRUE;
}

static gboolean
fu_plugin_backup_flash(FuPlugin *self, GError **error)
{
	g_autoptr(GError) error_local = NULL;
	gint exit_status = 0;
	g_autofree gchar *out = NULL;
	static const gchar *const args[] = {"-r", MRCHROMEBOX_BACKUP, NULL};

	if (g_mkdir_with_parents(MRCHROMEBOX_STATE_DIR, 0755) != 0) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_FAILED,
			    "failed to create %s: %s",
			    MRCHROMEBOX_STATE_DIR,
			    g_strerror(errno));
		return FALSE;
	}
	if (!fu_plugin_flashrom_run(self,
				     args,
				     &exit_status,
				     &out,
				     NULL,
				     &error_local) ||
	    exit_status != 0) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_FAILED,
			    "failed to back up the current flash to %s",
			    MRCHROMEBOX_BACKUP);
		return FALSE;
	}
	return TRUE;
}

static gboolean
fu_plugin_write_firmware(FuPlugin *self,
			 FuDevice *device,
			 FuFirmware *firmware,
			 FuProgress *progress G_GNUC_UNUSED,
			 FwupdInstallFlags flags G_GNUC_UNUSED,
			 GError **error)
{
	g_autoptr(GBytes) fw = NULL;
	g_autofree gchar *fwfile = NULL;
	g_autoptr(GError) error_local = NULL;
	guint attempts = 0;

	/* The daemon has already verified the blob against the metadata
	 * checksum; we only need the raw bytes. */
	fw = fu_firmware_get_bytes(firmware, &error_local);
	if (fw == NULL) {
		g_propagate_error(error, g_steal_pointer(&error_local));
		return FALSE;
	}
	if (g_bytes_get_size(fw) == 0) {
		g_set_error(error,
			    FWUPD_ERROR,
			    FWUPD_ERROR_INVALID_FILE,
			    "firmware blob is empty");
		return FALSE;
	}

	attempts = fu_plugin_marker_attempts() + 1;

	/* 1. pre-flight: SMM / SPI protected ranges. */
	if (!fu_plugin_probe_flash_access(self, &error_local)) {
		g_warning("mrchromebox: %s", error_local->message);
		return fu_plugin_fail(self, FALSE, attempts, error);
	}

	/* 2. auto-clear the software write-protect (upstream sequence); only a
	 * still-enabled WP after the attempt means hardware-level protection. */
	{
		gboolean wp_supported = FALSE;
		gboolean wp_enabled = FALSE;

		if (!fu_plugin_wp_status(self, &wp_supported, &wp_enabled, &error_local)) {
			g_warning("mrchromebox: --wp-status failed: %s",
				  error_local->message);
		} else if (wp_supported && wp_enabled) {
			if (!fu_plugin_wp_disable(self, &error_local)) {
				g_warning("mrchromebox: %s", error_local->message);
				return fu_plugin_fail(self, FALSE, attempts, error);
			}
			if (!fu_plugin_wp_status(self,
						 &wp_supported,
						 &wp_enabled,
						 &error_local) ||
			    wp_enabled) {
				g_warning("mrchromebox: write-protect is still enabled "
					  "after the disable attempt - hardware "
					  "write-protect is on; guide the user to the "
					  "official firmware-util.sh");
				return fu_plugin_fail(self, FALSE, attempts, error);
			}
		}
	}

	/* 3. power check (conservative guard, upstream has none). */
	if (!fu_plugin_check_power(&error_local)) {
		g_warning("mrchromebox: %s", error_local->message);
		return fu_plugin_fail(self, FALSE, attempts, error);
	}

	/* 4. back up the current flash before touching anything. */
	fu_progress_set_status(progress, FWUPD_STATUS_DEVICE_READ);
	fu_progress_set_percentage(progress, 10);
	if (!fu_plugin_backup_flash(self, &error_local)) {
		g_warning("mrchromebox: %s", error_local->message);
		return fu_plugin_fail(self, FALSE, attempts, error);
	}
	fu_progress_set_percentage(progress, 30);

	/* 5. stage the blob (flashrom takes a file path). */
	fwfile = g_strdup_printf("%s/firmware-update.rom", MRCHROMEBOX_STATE_DIR);
	if (!g_file_set_contents(fwfile,
				 g_bytes_get_data(fw, NULL),
				 g_bytes_get_size(fw),
				 &error_local)) {
		g_warning("mrchromebox: failed to stage the firmware blob: %s",
			  error_local->message);
		return fu_plugin_fail(self, FALSE, attempts, error);
	}

	/* 6. write + verify.  From here on any failure means the flash content
	 * is not trustworthy (class B). */
	fu_progress_set_status(progress, FWUPD_STATUS_DEVICE_WRITE);
	fu_progress_set_percentage(progress, 40);
	if (!fu_plugin_write_flash(self, fwfile, &error_local)) {
		g_warning("mrchromebox: flash write failed: %s",
			  error_local->message);
		return fu_plugin_fail(self, TRUE, attempts, error);
	}
	fu_progress_set_percentage(progress, 80);
	if (!fu_plugin_verify_flash(self, fwfile, &error_local)) {
		g_warning("mrchromebox: flash verify failed: %s",
			  error_local->message);
		return fu_plugin_fail(self, TRUE, attempts, error);
	}
	fu_progress_set_percentage(progress, 100);

	/* 7. success: clear the marker, require a reboot to boot the new
	 * firmware.  Do NOT re-enable the software write-protect (upstream Full
	 * ROM path never does; there is no WP_RO window to anchor it on). */
	fu_plugin_marker_remove();
	fwupd_device_add_flag(FWUPD_DEVICE(device), FWUPD_DEVICE_FLAG_NEEDS_REBOOT);
	g_debug("mrchromebox: flash update completed; reboot required");
	return TRUE;
}

/* --- plugin vfuncs -------------------------------------------------------- */

static gboolean
fu_plugin_startup(FuPlugin *self, FuProgress *progress G_GNUC_UNUSED, GError **error)
{
	struct MrChromeboxPluginData *data =
	    (struct MrChromeboxPluginData *)
		fu_plugin_alloc_data(self, sizeof(struct MrChromeboxPluginData));
	g_autofree gchar *bios_version = NULL;
	g_autofree gchar *board_name = NULL;
	g_autofree gchar *product_name = NULL;
	g_autoptr(GError) error_local = NULL;
	gboolean is_eve = FALSE;

	/* Gate 1: only UEFI-booted machines. */
	if (!g_file_test("/sys/firmware/efi", G_FILE_TEST_EXISTS)) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_NOT_SUPPORTED,
			    "not booted in UEFI mode");
		return FALSE;
	}

	/* Gate 2: only machines already running custom (non-Google) firmware;
	 * a bios_version starting with "Google_" is stock ChromeOS. */
	bios_version = fu_plugin_dmi_read("bios_version", &error_local);
	if (bios_version == NULL) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_NOT_SUPPORTED,
			    "cannot read DMI bios_version: %s",
			    error_local->message);
		return FALSE;
	}
	if (g_str_has_prefix(bios_version, "Google_")) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_NOT_SUPPORTED,
			    "stock ChromeOS firmware still installed "
			    "(bios_version=%s)",
			    bios_version);
		return FALSE;
	}

	/* Gate 3: only Pixelbook (eve). */
	board_name = fu_plugin_dmi_read("board_name", &error_local);
	product_name = fu_plugin_dmi_read("product_name", &error_local);
	if (board_name != NULL && g_str_has_prefix(board_name, "eve"))
		is_eve = TRUE;
	if (product_name != NULL && g_str_has_prefix(product_name, "Pixelbook"))
		is_eve = TRUE;
	if (!is_eve) {
		g_set_error(error,
			    G_IO_ERROR,
			    G_IO_ERROR_NOT_SUPPORTED,
			    "unsupported device (board=%s product=%s)",
			    board_name != NULL ? board_name : "?",
			    product_name != NULL ? product_name : "?");
		return FALSE;
	}

	/* Programmer override, mainly for VM testing (dummy); on real hardware
	 * the default "internal" is correct. */
	data->programmer = fu_plugin_get_config_value(self, "Programmer");
	return TRUE;
}

static gboolean
fu_plugin_coldplug(FuPlugin *self, FuProgress *progress G_GNUC_UNUSED, GError **error)
{
	g_autofree gchar *bios_date = NULL;
	g_autoptr(FuDevice) device = NULL;
	g_autoptr(GError) error_local = NULL;
	guint version = 0;

	bios_date = fu_plugin_dmi_read("bios_date", &error_local);
	if (bios_date == NULL) {
		g_propagate_error(error, g_steal_pointer(&error_local));
		return FALSE;
	}
	version = fu_plugin_bios_date_to_number(bios_date, &error_local);
	if (version == 0) {
		g_propagate_error(error, g_steal_pointer(&error_local));
		return FALSE;
	}

	device = fu_device_new(fu_plugin_get_context(self));
	fu_device_set_id(device,
			 MRCHROMEBOX_PHYSICAL_ID ":" MRCHROMEBOX_LOGICAL_ID);
	fu_device_set_physical_id(device, MRCHROMEBOX_PHYSICAL_ID);
	fu_device_set_logical_id(device, MRCHROMEBOX_LOGICAL_ID);
	fwupd_device_set_name(FWUPD_DEVICE(device), "MrChromebox UEFI Full ROM");
	fwupd_device_set_summary(FWUPD_DEVICE(device),
				 "Google Pixelbook (eve) custom UEFI firmware");
	fwupd_device_set_vendor(FWUPD_DEVICE(device), "MrChromebox");
	fu_device_build_vendor_id(device, "DMI", "GOOGLE");
	fwupd_device_add_icon(FWUPD_DEVICE(device), "computer");
	fwupd_device_add_guid(FWUPD_DEVICE(device), MRCHROMEBOX_GUID);
	fwupd_device_set_version_format(FWUPD_DEVICE(device), FWUPD_VERSION_FORMAT_NUMBER);
	/* The version string must be set, not just version_raw: fwupd refuses
	 * devices without a version ("no version set") and GNOME Software hides
	 * them.  It must be the NORMALIZED YYYYMMDD (fu_version_compare_safe
	 * splits on '.' and g_ascii_strtoll stops at '/', so passing the raw
	 * DMI "MM/DD/YYYY" would compare by month and rank every release newer).
	 * Same space as the metadata (firmware-update-check.sh publishes
	 * release version = release_current_date), so 20261015 > 20260715
	 * compares as expected. */
	g_autofree gchar *version_str = fu_plugin_number_to_bios_date(version);
	fwupd_device_set_version(FWUPD_DEVICE(device), version_str);
	fwupd_device_set_version_raw(FWUPD_DEVICE(device), version);
	fwupd_device_add_flag(FWUPD_DEVICE(device), FWUPD_DEVICE_FLAG_UPDATABLE);
	fwupd_device_set_install_duration(FWUPD_DEVICE(device), 60);

	fu_plugin_add_device(self, device);
	return TRUE;
}

void
fu_plugin_init_vfuncs(FuPluginVfuncs *vfuncs)
{
	vfuncs->startup = fu_plugin_startup;
	vfuncs->coldplug = fu_plugin_coldplug;
	vfuncs->write_firmware = fu_plugin_write_firmware;
}
