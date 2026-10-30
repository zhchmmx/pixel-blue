#!/usr/bin/python3
"""Teach Anaconda 44 to preinstall the ISO's Flatpaks on a bootc payload.

One change, and it is the whole fix: the bootc branch of
RPMOSTreeModule.install_with_tasks() never builds an InstallFlatpaksTask. The
ostree branch does; the bootc branch, written later, simply does not carry those
four lines. Without them the refs are never installed, and nothing is logged
either -- the payload only runs a "Find size of flatpaks" task because a FLATPAK
source was registered.

Nothing else needs patching. The rpm_ostree FlatpakManager works out what to
install by itself: install_all() walks RemoteRefsList, which calls
`list_remote_refs_sync("Anaconda")` against the local repo at
file:///flatpak/repo, then adds every non-runtime ref to the transaction.
(`set_flatpak_refs` belongs to the *other* FlatpakManager, in
payload/flatpak/, and calling it on this one is an AttributeError -- which is
how the first version of this patch failed.)

The patch asserts that it matches exactly once, so an anaconda update that moves
the code fails the build instead of silently doing nothing.

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

BOOTC_OLD = """        if isinstance(data, BootcConfigurationData):
            return self._install_with_tasks_bootc(data)"""

BOOTC_NEW = """        if isinstance(data, BootcConfigurationData):
            tasks = self._install_with_tasks_bootc(data)
            flatpak_source = self._get_source(SourceType.FLATPAK)
            if flatpak_source:
                tasks.append(InstallFlatpaksTask(sysroot=conf.target.system_root))
            return tasks"""


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
