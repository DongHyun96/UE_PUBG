// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/C_Weapon.h"

#include "Item/Weapon/WeaponStrategy/I_WeaponButtonStrategy.h"


// Sets default values
AC_Weapon::AC_Weapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Weapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AC_Weapon::ExecuteBKey()
{

	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseBKeyStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteRKey()
{
	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseRKeyStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_Started()
{
	if (!WeaponButtonStrategy) return false;

	/*UObject* StrategyObj = Cast<UObject>(WeaponButtonStrategy);

	if (!StrategyObj)
	{
		UE_LOG(LogTemp, Warning, TEXT("From AC_Weapon::ExecuteMlb_Started : Can't cast to UObject"));
		return false;
	}

	if (!StrategyObj->GetClass()->ImplementsInterface(UI_WeaponButtonStrategy::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("From AC_Weapon::ExecuteMlb_Started : WeaponButtonStrategy does not implement UseMlb_StartedStrategy"));
		return false;
	}*/
	return WeaponButtonStrategy->UseMlb_StartedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_OnGoing()
{
	if (!WeaponButtonStrategy) return false;

	//WeaponButtonStrategy->_getUObject()->GetClass()->ImplementsInteraf

	/*UObject* StrategyObj = Cast<UObject>(WeaponButtonStrategy);

	if (!StrategyObj)
	{
		UE_LOG(LogTemp, Warning, TEXT("From AC_Weapon::ExecuteMlb_OnGoing : Can't cast to UObject"));
		return false;
	}

	if (!StrategyObj->GetClass()->ImplementsInterface(UI_WeaponButtonStrategy::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("From AC_Weapon::ExecuteMlb_OnGoing : WeaponButtonStrategy does not implement UseMlb_StartedStrategy"));
		return false;
	}*/
	return WeaponButtonStrategy->UseMlb_OnGoingStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMlb_Completed()
{
	if (!WeaponButtonStrategy) return false;

	/*UObject* StrategyObj = Cast<UObject>(WeaponButtonStrategy);

	if (!StrategyObj)
	{
		UE_LOG(LogTemp, Warning, TEXT("From AC_Weapon::ExecuteMlb_Completed : Can't cast to UObject"));
		return false;
	}

	if (!StrategyObj->GetClass()->ImplementsInterface(UI_WeaponButtonStrategy::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("From AC_Weapon::ExecuteMlb_Completed : WeaponButtonStrategy does not implement UseMlb_StartedStrategy"));
		return false;
	}*/
	return WeaponButtonStrategy->UseMlb_CompletedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_Started()
{

	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMrb_StartedStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_OnGoing()
{

	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMrb_OnGoingStrategy(OwnerCharacter, this);
}

bool AC_Weapon::ExecuteMrb_Completed()
{

	if (!WeaponButtonStrategy) return false;
	return WeaponButtonStrategy->UseMrb_CompletedStrategy(OwnerCharacter, this);
}