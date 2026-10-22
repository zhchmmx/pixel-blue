/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * mrchromebox-plugin.h - minimal ABI-compatible declarations for the
 * libfwupdplugin API surface used by the mrchromebox out-of-tree plugin.
 *
 * Fedora's fwupd-devel does not ship fwupdplugin.h or a fwupdplugin.pc file,
 * so we declare here exactly the types and functions we link against.  The
 * FuPluginClass layout below is copied verbatim from libfwupdplugin/fu-plugin.h
 * (fwupd 2.0.10 - 2.1.8, layouts verified identical), so the vfunc table
 * offsets are ABI-correct.  All other types are used opaquely, and everything
 * else comes from the installed libfwupd headers via <libfwupd/fwupd.h>.
 */

#pragma once

#include <libfwupd/fwupd.h>
#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

/* Opaque libfwupdplugin types; never dereferenced by this plugin. */
typedef struct _FuPlugin FuPlugin;
typedef struct _FuDevice FuDevice;
typedef struct _FuFirmware FuFirmware;
typedef struct _FuProgress FuProgress;
typedef struct _FuContext FuContext;
typedef struct _FuSecurityAttrs FuSecurityAttrs;
typedef struct _FuPluginData FuPluginData;
typedef struct _FuPluginClass FuPluginClass;
typedef FuPluginClass FuPluginVfuncs;

typedef void (*FuPluginInitVfuncsFunc)(FuPluginVfuncs *vfuncs);

/* FuPluginClass - must stay byte-for-byte compatible with libfwupdplugin. */
struct _FuPluginClass {
	FwupdPluginClass parent_class;
	/* signals */
	void (*_device_added)(FuPlugin *self, FuDevice *device);
	void (*_device_removed)(FuPlugin *self, FuDevice *device);
	void (*_status_changed)(FuPlugin *self, FwupdStatus status);
	void (*_percentage_changed)(FuPlugin *self, guint percentage);
	void (*_device_register)(FuPlugin *self, FuDevice *device);
	gboolean (*_check_supported)(FuPlugin *self, const gchar *guid);
	void (*_rules_changed)(FuPlugin *self);
	/* vfuncs */
	void (*constructed)(GObject *obj);
	void (*finalize)(GObject *obj);
	gboolean (*startup)(FuPlugin *self, FuProgress *progress, GError **error);
	gboolean (*ready)(FuPlugin *self, FuProgress *progress, GError **error);
	gboolean (*coldplug)(FuPlugin *self, FuProgress *progress, GError **error);
	gboolean (*device_created)(FuPlugin *self, FuDevice *device, GError **error);
	void (*device_registered)(FuPlugin *self, FuDevice *device);
	void (*device_added)(FuPlugin *self, FuDevice *device);
	gboolean (*verify)(FuPlugin *self,
			  FuDevice *device,
			  FuProgress *progress,
			  guint flags,
			  GError **error);
	gboolean (*get_results)(FuPlugin *self, FuDevice *device, GError **error);
	gboolean (*clear_results)(FuPlugin *self, FuDevice *device, GError **error);
	gboolean (*backend_device_added)(FuPlugin *self,
					 FuDevice *device,
					 FuProgress *progress,
					 GError **error);
	gboolean (*backend_device_changed)(FuPlugin *self,
					   FuDevice *device,
					   GError **error);
	gboolean (*backend_device_removed)(FuPlugin *self,
					   FuDevice *device,
					   GError **error);
	void (*add_security_attrs)(FuPlugin *self, FuSecurityAttrs *attrs);
	gboolean (*write_firmware)(FuPlugin *self,
				  FuDevice *device,
				  FuFirmware *firmware,
				  FuProgress *progress,
				  FwupdInstallFlags flags,
				  GError **error);
	gboolean (*unlock)(FuPlugin *self, FuDevice *device, GError **error);
	gboolean (*activate)(FuPlugin *self,
			    FuDevice *device,
			    FuProgress *progress,
			    GError **error);
	gboolean (*attach)(FuPlugin *self, FuDevice *device, FuProgress *progress, GError **error);
	gboolean (*detach)(FuPlugin *self, FuDevice *device, FuProgress *progress, GError **error);
	gboolean (*prepare)(FuPlugin *self,
			   FuDevice *device,
			   FuProgress *progress,
			   FwupdInstallFlags flags,
			   GError **error);
	gboolean (*cleanup)(FuPlugin *self,
			   FuDevice *device,
			   FuProgress *progress,
			   FwupdInstallFlags flags,
			   GError **error);
	gboolean (*composite_prepare)(FuPlugin *self, GPtrArray *devices, GError **error);
	gboolean (*composite_cleanup)(FuPlugin *self, GPtrArray *devices, GError **error);
	void (*load)(FuContext *ctx);
	void (*to_string)(FuPlugin *self, guint idt, GString *str);
	gboolean (*fix_host_security_attr)(FuPlugin *self,
					  FwupdSecurityAttr *attr,
					  GError **error);
	gboolean (*undo_host_security_attr)(FuPlugin *self,
					   FwupdSecurityAttr *attr,
					   GError **error);
	gboolean (*reboot_cleanup)(FuPlugin *self, FuDevice *device, GError **error);
	gboolean (*modify_config)(FuPlugin *self,
				 const gchar *key,
				 const gchar *value,
				 GError **error);
};

/* The single symbol the fwupd daemon requires from the module. */
void fu_plugin_init_vfuncs(FuPluginVfuncs *vfuncs);

/* libfwupdplugin functions used by this plugin (ABI verified vs 2.1.8). */
FuPluginData *fu_plugin_get_data(FuPlugin *self);
FuPluginData *fu_plugin_alloc_data(FuPlugin *self, gsize data_sz);
FuContext *fu_plugin_get_context(FuPlugin *self);
gchar *fu_plugin_get_config_value(FuPlugin *self, const gchar *key);
void fu_plugin_add_device(FuPlugin *self, FuDevice *device);
FuDevice *fu_device_new(FuContext *ctx);
void fu_device_set_id(FuDevice *self, const gchar *id);
void fu_device_set_physical_id(FuDevice *self, const gchar *physical_id);
void fu_device_set_logical_id(FuDevice *self, const gchar *logical_id);
void fu_device_build_vendor_id(FuDevice *self, const gchar *prefix, const gchar *value);
GBytes *fu_firmware_get_bytes(FuFirmware *self, GError **error);
void fu_progress_set_status(FuProgress *self, FwupdStatus status);
void fu_progress_set_percentage(FuProgress *self, gdouble percentage);
FuProgress *fu_progress_get_child(FuProgress *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(FuDevice, g_object_unref)

G_END_DECLS
