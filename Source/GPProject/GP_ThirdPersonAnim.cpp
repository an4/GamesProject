// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GP_ThirdPersonAnim.h"


void UGP_ThirdPersonAnim::setDeathStatus(bool status) {
	bIsDead = status;
}

void UGP_ThirdPersonAnim::setShootStatus(bool status) {
	bIsShooting = status;
}

void UGP_ThirdPersonAnim::setHitStatus(bool status) {
	bIsHit = status;
}
