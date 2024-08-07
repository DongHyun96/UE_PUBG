// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_FlashBangExplode.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "Utility/C_Util.h"

bool AC_FlashBangExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : 반경에 있는 캐릭터 시야에 따른 시각 장애 주기
	// 청각장애는 모두 동일 

	if (ThrowingWeapon->GetParticleExplodeEffect())
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThrowingWeapon->GetParticleExplodeEffect(), ThrowingWeapon->GetActorLocation());
	
	UC_Util::Print("FlashBang Explode");
	return true;
}
