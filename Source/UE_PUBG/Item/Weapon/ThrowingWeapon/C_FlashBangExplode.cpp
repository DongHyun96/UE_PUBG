// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_FlashBangExplode.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "Utility/C_Util.h"

bool AC_FlashBangExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	// TODO : �ݰ濡 �ִ� ĳ���� �þ߿� ���� �ð� ��� �ֱ�
	// û����ִ� ��� ���� 

	if (ThrowingWeapon->GetParticleExplodeEffect())
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThrowingWeapon->GetParticleExplodeEffect(), ThrowingWeapon->GetActorLocation());
	
	UC_Util::Print("FlashBang Explode");
	return true;
}
