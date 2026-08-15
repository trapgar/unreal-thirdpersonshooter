# Content/Python

Python utilities for driving the editor for this project.

`Content/Python` is not an arbitrary location — the Python plugin adds the
`Python` subfolder of **every mounted content root** to `sys.path` at startup
(project, engine, and each enabled plugin). So anything here is importable from
the editor console with no configuration.

A file named `init_unreal.py` in this folder would additionally be **executed
automatically** at editor startup. That is the usual hook for registering menu
entries, toolbar buttons, or asset actions. There is none here yet.

These scripts run **inside the editor's embedded interpreter** (UE 5.8 ships
CPython 3.11.8). They cannot be run from a normal shell — see below.

## Usage

From the editor console (`~`, or the Output Log command box):

```
py "E:/source/repos/.unreal/ThirdPersonShooter5_8/Content/Python/import_asset.py" --source "E:/path/to/CoinPing_01.wav" --dest /Game/Audio/Sounds/Impacts --name CoinPing_01
```

The `py` command re-quotes the script path and prepends it to the argument
string, so `sys.argv` follows the normal convention (`argv[0]` is the script)
and `argparse` works as expected. Quoted paths containing spaces are handled.

### `import_asset.py`

Imports a file from disk into the project as an asset. Works for any type the
editor has an importer for — audio, textures, meshes.

| Argument | Required | Description |
| --- | --- | --- |
| `--source` | yes | Absolute path to the file on disk. |
| `--dest` | yes | Content path, e.g. `/Game/Audio/Sounds/Impacts`. |
| `--name` | no | Asset name. Defaults to the source file's basename. |
| `--no-replace` | no | Fail instead of overwriting an existing asset. |
| `--no-save` | no | Leave the asset dirty in memory rather than writing the `.uasset`. |

Importable as a module too, since this folder is on `sys.path`:

```python
from import_asset import import_asset
paths = import_asset("E:/raw/CoinPing_01.wav", "/Game/Audio/Sounds/Impacts")
```

## Why these files are not Git LFS

`.gitattributes` sends all of `Content/**` to LFS, which is right for `.uasset`
but wrong for source code — it would cost readable diffs and three-way merges.
A later rule exempts this folder:

```
Content/Python/** !filter !diff !merge text
```

Keep new Python and Markdown under `Content/Python/` so it stays covered.

## The `unreal` module is not a pip dependency

It is injected by the editor at runtime and is **not** on PyPI. Do not
`pip install unreal`: that name is taken by an unrelated synthetic-data library
("A Pythonic implementation of R package conjurer", v0.1.1) whose
`unreal/__init__.py` would shadow the editor's real module.

The module is built in-process from Unreal's reflection data, so its API depends
on the engine version *and* on which plugins are enabled. That is also why no
pinned package could ever be correct.

For autocomplete and type checking, use Epic's generated stub:

1. **Edit > Project Settings > Plugins > Python**
2. Enable **Developer Mode (Unsupported)**
3. Restart the editor — it writes `Intermediate/PythonStub/unreal.py`

Point your IDE's extra-paths setting at that folder. Do not create a virtualenv
for this project: the editor builds `sys.path` from the user dir, content roots,
`AdditionalPaths`, and `UE_PYTHONPATH` — never from an activated venv.

## Third-party libraries

Two supported routes:

**Vendored** — drop pure-Python packages into any of these, which are added as
site-packages when present:

```
Content/Python/Lib/<Platform>/<Arch>/site-packages    e.g. Lib/Win64/x64/site-packages
Content/Python/Lib/<Platform>/site-packages           e.g. Lib/Win64/site-packages
Content/Python/Lib/site-packages
```

**Declared** — add a `PythonRequirements` array to a **plugin's** `.uplugin`.
The engine merges the requirements of all enabled plugins and pip-installs them
into a managed virtualenv at `Intermediate/PipInstall/`. Note this is read from
plugin descriptors only; there is no project-level equivalent, so a game project
needs a small local plugin to use it.

Relevant settings under **Project Settings > Plugins > Python**:
`bPipStrictHashCheck` (default **on**, so requirements need pinned hashes),
`OverrideIndexURL`, and `ExtraIndexUrls` in the `.uplugin` for private indexes.

## Running headless

```
UnrealEditor-Cmd.exe <project>.uproject -run=pythonscript -script="Content/Python/import_asset.py --source ... --dest ..."
```
