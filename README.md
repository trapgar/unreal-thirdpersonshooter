## Backlog
- figure out how to attach components at runtime based on Game Mode or something...
    - AbilitySystemComponent w/ default ability set for the character
    - InventoryManagerComponent w/ default items & equipment
        - add ability sets on item equip
- create lookup table for GameplayTag to sockets so Rifles all go off the left shoulder etc.
- make a URangedWeaponStatSheet for the ballistics, handling, etc & have the gun actor expose it on spawn

## Conversion Notes
- Lyra uses `FFastArraySerializer` in `LyraEquipmentManagerComponent.h` and the linker breaks when I import

```md
>>> How do I use Gameplay Abilities in Unreal Engine 5?
Gameplay Abilities are a powerful feature in Unreal Engine 5 that allows you to create complex behaviors and interactions for your characters.
Here's a step-by-step guide on how to use Gameplay Abilities:

1. **Create a new Ability**:
        * In the Content Browser, right-click and select "Blueprint Class" > "GameplayAbility".
        * Name your ability (e.g., "JumpAttack").
2. **Set up the Ability Blueprint**:
        * Double-click the Ability blueprint to open it in the Blueprint Editor.
        * Create a new event graph by clicking the "+" icon at the top-right corner of the editor.
3. **Define the Ability's Behavior**:
        * In the event graph, create an event node (e.g., "OnAbilityActivated") that will trigger when the ability is activated.
        * Add nodes to define the ability's behavior, such as:
                + Playing a sound or animation
                + Applying forces or impulses to the character
                + Changing the character's state (e.g., making them invincible)
4. **Set up Ability Parameters**:
        * In the Ability blueprint, you can add parameters that will affect its behavior.
        * For example, you might create a parameter called "JumpHeight" that determines how high the character jumps when using the JumpAttack ability.
5. **Create an Ability Blueprint Class**:
        * Right-click in the Content Browser and select "Blueprint Class" > "GameplayAbility".
        * Name your blueprint class (e.g., "JumpAttackBP").
6. **Create an Ability Asset**:
        * In the Content Browser, right-click and select "GameplayAbilityAsset".
        * Name your ability asset (e.g., "JumpAttackAA").
7. **Add the Ability to a Character Class**:
        * Open the character blueprint (e.g., "MyCharacterBP") in the Blueprint Editor.
        * Add an instance of the GameplayAbility class you created (e.g., "JumpAttackBP") as a component to the character.
8. **Configure Ability Activation**:
        * In the character blueprint, create an event node that will trigger when the ability is activated (e.g., "OnJumpAttackActivated").
        * Add nodes to define how the ability should be activated, such as:
                + Pressing a specific key or button
                + Meeting certain conditions (e.g., being close enough to an enemy)
9. **Test and Refine Your Ability**:
        * Compile and save your blueprint changes.
        * Test your game to ensure the ability works as expected.
        * Refine the ability's behavior by adjusting parameters, adding more nodes, or creating additional blueprints.

By following these steps, you'll be able to create complex gameplay abilities for your characters in Unreal Engine 5. Happy coding!
```
