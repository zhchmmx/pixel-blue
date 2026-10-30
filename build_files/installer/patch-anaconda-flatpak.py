#!/usr/bin/python3
"""Teach Anaconda 44 to preinstall the ISO's Flatpaks on a bootc payload.

Two changes, both required, both checked against anaconda 44.30:

1. rpm_ostree.py -- the bootc branch of install_with_tasks() never builds an
   InstallFlatpaksTask. The ostree branch does; the bootc branch, written
   later, simply does not carry those four lines. Without them the refs are
   never installed no matter what else is configured, and nothing is logged
   either: the payload only runs a "Find size of flatpaks" task because a
   FLATPAK source was registered.

2. flatpak_installation.py -- InstallFlatpaksTask builds its FlatpakManager
   without a ref list, and on this payload nothing else supplies one: the only
   caller of set_flatpak_refs() in pyanaconda is the DNF payload. With an empty
   list the manager's install() returns at its "no flatpaks are marked for
   install" guard -- silently, no error, no log line.

The ref list is /flatpak/refs, written by build-flatpak-repo.sh from the repo it
just built, so the list and the repo cannot drift apart.

Both patches assert that they match exactly once, so an anaconda update that
moves the code fails the build instead of silently doing nothing.

Usage: patch-anaconda-flatpak.py [ROOT]
ROOT is only for testing; it prefixes the absolute paths.
"""

import glob
import sys

ROOT = sys.argv[1].rstrip("/") if len(sys.argv) > 1 else ""

RPM_OSTREE = (
    "/usr/lib64/python3*/site-packages/pyanaconda/modules/payloads/"
    "payload/rpm_ostree/rpm_ostree.py"
)
FLATPAK_INSTALLATION = (
    "/usr/lib64/python3*/site-packages/pyanaconda/modules/payloads/"
    "payload/rpm_ostree/flatpak_installation.py"
)

# 1. Give the bootc branch the same flatpak task the ostree branch has.
BOOTC_OLD = """        if isinstance(data, BootcConfigurationData):
            return self._install_with_tasks_bootc(data)"""

BOOTC_NEW = """        if isinstance(data, BootcConfigurationData):
            tasks = self._install_with_tasks_bootc(data)
            flatpak_source = self._get_source(SourceType.FLATPAK)
            if flatpak_source:
                tasks.append(InstallFlatpaksTask(sysroot=conf.target.system_root))
            return tasks"""

# 2. Hand the manager the ref list it never gets.
REFS_OLD = "        flatpak_manager.initialize_with_system_path()"

REFS_NEW = (
    '        flatpak_manager.set_flatpak_refs(open("/flatpak/refs").read().split())\n'
    "        flatpak_manager.initialize_with_system_path()"
)


def patch(pattern, old, new):
    paths = glob.glob(ROOT + pattern)
    if len(paths) != 1:
        sys.exit("error: expected exactly one file matching {} , got {}".format(pattern, paths))

    path = paths[0]
    with open(path) as f:
        source = f.read()

    found = source.count(old)
    if found != 1:
        sys.exit(
            "error: {}: expected the anchor to appear once, found {} times. "
            "Anaconda changed; update this patch.".format(path, found)
        )

    with open(path, "w") as f:
        f.write(source.replace(old, new))
    print("patched {}".format(path))


patch(RPM_OSTREE, BOOTC_OLD, BOOTC_NEW)
patch(FLATPAK_INSTALLATION, REFS_OLD, REFS_NEW)
