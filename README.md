# thirdpersonshooter
Attempt to create a version of my previous Lyra-based game (Pegasus), but without all the Lyra bloat as I've often no idea
what that template is doing.

Also trying to use C++ over blueprints to make source compare easier.

## Getting Started
1. Download & install [Visual Studio Code](https://code.visualstudio.com/download) and [Unreal Engine](https://www.unrealengine.com/en-US/download)
2. Install Git LFS with `> git lfs install`
3. Clone the repo
4. Launch the Unreal editor by double+clicking on the `.uproject` file in `File Explorer` or by launching the editor directly
5. `Edit > Editor Preferences... > Source Code`
6. Change the `Source Code Editor` to `Visual Studio Code`
7. `Tools > Refresh Visual Studio Code Project`
8. Close out the Unreal editor
9. Open the newly created `.code-workspace` file in the root of the project
10. `Ctrl+Shift+D` to show the `Run and Debug` clover
11. Select `Launch ThirdPersonShooterEditor (Development)`
12. `F5` to launch the Unreal editor with the VS Code debugger attached!

### Tips and Tricks
C++ intellisense is borderline-useless with Unreal projects. Might be size, idk. Anyways, I mainly use `Search (Ctrl+F)` to find variable references.
Make sure that if you're doing this globally that you toggle the `Search Details` (triple dot) and add `./ThirdPersonShooter` to the `files to include`.
This will prevent Code from searching the entire Unreal Engine source, which is probably not what you want.

## Backlog
- figure out how to attach components at runtime based on Game Mode or something...
    - AbilitySystemComponent w/ default ability set for the character
- add a silhouette to the gunrange for testing
- need to add the `ContextEffectsLibrary` & `ContextEffectsComponent` to be able to play footsteps

## Developer Notes
- Lyra uses `FFastArraySerializer` in `LyraEquipmentManagerComponent.h` and the linker breaks when I try to import the header file
    - I'm sure this is important for multiplayer, so will need to address this eventually
- Should the `ModularAbilitySet` replace the `InputTag` with a ref to the IA?
    - How would this affect abstract abilities like `GA_Weapon_Fire_Auto`?
- Need to figure out how to keep track of inventory items that can vs can't stack
- Research asset manager registrations for:
    - `UEquipmentConfiguration`
    - `UInventoryItemDefinition`
    - `UEquipmentItemDefinition`

## Open Questions
- What is the difference between a `GameplayCue`, and a `GameplayEvent` in terms of GameplayTags?
    - assume `GameplayCue` 'Hey, you're supposed to do X now', and `GameplayEvent` is 'Y just happened'

---

- Add a PawnCosmeticComponent to `AModularPawn` & have equipment attach to scene elements in that
    - probably fine for this to just be a part of the `EquipmentManagerComponent`?
    - only case where it's not would be for truly cosmetic only things like character customization
        - look to the `SKM_MannyInvis`
- Figure out how to use AssetManager w/cosmetics etc. (Data registries?)
- Add the NotifyName for post `Activate Anim Layer and Play Paired Montage` to listen for animation notify event
- Lyra has the `AbilitySystemComponent` off the PlayerState, not the Pawn, where is the best location?
- `ADrivableSpaceVehicle` doesn't hear input events (IA or otherwise)
    - Lyra uses `GA_Collectable` which grants an Interaction Ability
- Make sure I'm using ALevelInstance right with `GameFeatureAction_AddLevelInstance` (streaming, etc)
    - [World Partition in Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)
    - [Level Instancing in Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/level-instancing-in-unreal-engine)
    - [Data Layers in Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---data-layers-in-unreal-engine)
- Add a func for `InteractableObjectsFound` instead of just changed
- Move `GA_Interaction_Activate` to a C++ class
    - Same with `GA_Interaction_Collection`
