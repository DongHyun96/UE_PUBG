// Fill out your copyright notice in the Description page of Project Settings.


#include "C_SmokeEnteredChecker.h"

#include "Character/C_Player.h"
#include "Item/Weapon/ThrowingWeapon/C_SmokeOverlappedHandler.h"
#include "Utility/C_Util.h"

UC_SmokeEnteredChecker::UC_SmokeEnteredChecker()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_SmokeEnteredChecker::BeginPlay()
{
	Super::BeginPlay();
}


void UC_SmokeEnteredChecker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*if (Cast<AC_Player>(OwnerCharacter))
	{
		FString Str = "Entered Smoke count : " + FString::FromInt(EnteredSmokes.Num());
		UC_Util::Print(Str);
	}*/
}

bool UC_SmokeEnteredChecker::GetRandomLocationInSmokeArea(FVector& OutLocation)
{
	if (!IsCurrentlyInSmokeArea()) return false;

	int RandomIndex = FMath::RandRange(0, EnteredSmokes.Num() - 1);
	OutLocation = EnteredSmokes[RandomIndex]->GetRandomLocationInSmokeArea();
	return true;
}

void UC_SmokeEnteredChecker::AddEnteredSmoke(UC_SmokeOverlappedHandler* InSmoke)
{
	EnteredSmokes.Add(InSmoke);
	InSmoke->Delegate_OnSmokeDurationFinished.AddUObject(this, &UC_SmokeEnteredChecker::RemoveEnteredSmoke);
}

void UC_SmokeEnteredChecker::RemoveEnteredSmoke(UC_SmokeOverlappedHandler* TargetSmoke)
{
	EnteredSmokes.Remove(TargetSmoke);
	TargetSmoke->Delegate_OnSmokeDurationFinished.RemoveAll(this);
}
