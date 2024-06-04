// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


/**
 * when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list
 **/

// Trace against Actors/Components which provide interactions (`WorldInteractable`).
#define Custom_TraceChannel_Interaction					ECC_GameTraceChannel1

// Allocated to aim assist by the ShooterCore game feature
// ECC_GameTraceChannel5
