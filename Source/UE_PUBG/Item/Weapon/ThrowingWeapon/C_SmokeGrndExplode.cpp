// Fill out your copyright notice in the Description page of Project Settings.



#include "Item/Weapon/ThrowingWeapon/C_SmokeGrndExplode.h"

#include "C_SmokeOverlappedHandler.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Utility/C_Util.h"

bool AC_SmokeGrndExplode::UseStrategy(AC_ThrowingWeapon* ThrowingWeapon)
{
	if (ThrowingWeapon->GetNiagaraExplodeEffect())
	{
		FVector SpawnLocation = ThrowingWeapon->GetActorLocation();
		SpawnLocation.Z += 25.f;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(ThrowingWeapon->GetWorld(), ThrowingWeapon->GetNiagaraExplodeEffect(), SpawnLocation);
	}

	// ThrowingWeapon->SetActorHiddenInGame(true);
	ThrowingWeapon->GetWorld()->GetTimerManager().SetTimer(ThrowingWeapon->GetDestroyTimerHandle(), [ThrowingWeapon]()
	{
		UC_Util::Print("Destroying Smoke Grenade!", FColor::MakeRandomColor(), 10.f);

		if (UC_SmokeOverlappedHandler* SmokeOverlappedHandler = ThrowingWeapon->FindComponentByClass<UC_SmokeOverlappedHandler>())
			SmokeOverlappedHandler->OnDestroyOwnerSmokeGrenade();
		else UC_Util::Print("Error : Smoke Overlapped Handler not found!", FColor::MakeRandomColor(), 10.f);
		
		ThrowingWeapon->Destroy();
	}, 27.f, false);
	
	if (UC_SmokeOverlappedHandler* SmokeOverlappedHandler = ThrowingWeapon->FindComponentByClass<UC_SmokeOverlappedHandler>())
		SmokeOverlappedHandler->OnExplodeStart();
	else UC_Util::Print("Error : Smoke Overlapped Handler not found!", FColor::MakeRandomColor(), 10.f);

	return true;
}


