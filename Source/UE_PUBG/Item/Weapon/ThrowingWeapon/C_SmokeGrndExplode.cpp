// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Weapon/ThrowingWeapon/C_SmokeGrndExplode.h"

#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Utility/C_Util.h"

bool AC_SmokeGrndExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	if (ThrowingWeapon->GetNiagaraExplodeEffect())
	{
		FVector SpawnLocation = ThrowingWeapon->GetActorLocation();
		SpawnLocation.Z += 25.f;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ThrowingWeapon->GetNiagaraExplodeEffect(), SpawnLocation);
	}

	UC_Util::Print("Smoke Grenade Explode!");
	return true;
}
