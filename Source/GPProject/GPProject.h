// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#define COLLISION_PROJECTILE    ECC_GameTraceChannel1
#define COLLISION_REMOTEBOMB    ECC_GameTraceChannel2

// We define this macro here simply to trick Visual Studio's Intellisense
// It must actually be defined in the build system
#define IN_UE4
