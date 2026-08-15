"""
Import a source file (audio, texture, mesh, ...) into an Unreal project as an asset.

Runs inside the Unreal editor's embedded Python, so invoke it through the editor
console rather than a standalone interpreter:

    py "E:/source/repos/.unreal/unreal-python-utils/import_asset.py" --source "E:/audio/CoinPing_01.wav" --dest /Game/Audio/Sounds/Impacts --name CoinPing_01

The editor's `py` command forwards trailing arguments to the script, so the usual
argparse conventions apply.
"""

import argparse
import os
import sys

import unreal


def parse_args(argv=None):
    parser = argparse.ArgumentParser(
        prog="import_asset",
        description="Import a file on disk into an Unreal project as an asset.",
    )
    parser.add_argument(
        "--source",
        required=True,
        help="Absolute path to the file on disk to import (e.g. E:/audio/CoinPing_01.wav).",
    )
    parser.add_argument(
        "--dest",
        required=True,
        help="Destination content path (e.g. /Game/Audio/Sounds/Impacts).",
    )
    parser.add_argument(
        "--name",
        default=None,
        help="Asset name to create. Defaults to the source file's basename.",
    )
    parser.add_argument(
        "--no-replace",
        action="store_true",
        help="Fail rather than overwrite an asset already present at the destination.",
    )
    parser.add_argument(
        "--no-save",
        action="store_true",
        help="Leave the imported asset dirty in memory instead of writing the .uasset to disk.",
    )
    return parser.parse_args(argv)


def import_asset(source, dest, name=None, replace_existing=True, save=True):
    """Import `source` into content path `dest` as `name`.

    Returns the list of imported object paths (empty if the import produced nothing).
    """
    source = os.path.abspath(source)
    if not os.path.isfile(source):
        raise FileNotFoundError("Source file does not exist: %s" % source)

    if not dest.startswith("/"):
        raise ValueError(
            "Destination must be a content path starting with '/' "
            "(e.g. /Game/Audio/Sounds/Impacts), got: %s" % dest
        )

    if not name:
        name = os.path.splitext(os.path.basename(source))[0]

    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source)
    task.set_editor_property("destination_path", dest.rstrip("/"))
    task.set_editor_property("destination_name", name)
    task.set_editor_property("automated", True)  # suppress the import dialog
    task.set_editor_property("replace_existing", replace_existing)
    task.set_editor_property("save", save)

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    return list(task.get_editor_property("imported_object_paths") or [])


def main(argv=None):
    args = parse_args(argv)

    imported = import_asset(
        source=args.source,
        dest=args.dest,
        name=args.name,
        replace_existing=not args.no_replace,
        save=not args.no_save,
    )

    if imported:
        unreal.log("[import_asset] imported: %s" % ", ".join(imported))
        return 0

    unreal.log_error(
        "[import_asset] import produced no assets. Check that the file type is "
        "supported and that the destination path exists."
    )
    return 1


if __name__ == "__main__":
    sys.exit(main())
