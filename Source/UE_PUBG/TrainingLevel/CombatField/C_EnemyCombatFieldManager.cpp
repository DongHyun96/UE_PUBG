// Fill out your copyright notice in the Description page of Project Settings.


#include "C_EnemyCombatFieldManager.h"

#include "C_CombatFieldManager.h"
#include "Character/C_Player.h"
#include "Components/BoxComponent.h"
#include "Utility/C_Util.h"


UC_EnemyCombatFieldManager::UC_EnemyCombatFieldManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_EnemyCombatFieldManager::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < 4; ++i)
	{	
		FString BoxNameString = "CombatSpectatorBox" + FString::FromInt(i);
		
		UBoxComponent* BoxComponent = Cast<UBoxComponent>(OwnerCombatFieldManager->GetDefaultSubobjectByName(FName(*BoxNameString)));

		if (!BoxComponent)
		{
			UC_Util::Print("From UC_EnemyCombatFieldManager : Spectator box not found!", FColor::Red, 10.f);
			continue;
		}
		
		SpectatorBoxes.Add(BoxComponent);

		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UC_EnemyCombatFieldManager::OnSpectatorBoxBeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &UC_EnemyCombatFieldManager::OnSpectatorBoxEndOverlap);
	}
}


void UC_EnemyCombatFieldManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_EnemyCombatFieldManager::OnSpectatorBoxBeginOverlap
(
	UPrimitiveComponent*	OverlappedComponent,
	AActor*					OtherActor,
	UPrimitiveComponent*	OtherComp,
	int32					OtherBodyIndex,
	bool					bFromSweep,
	const FHitResult&		SweepResult
)
{
	if (!Cast<AC_Player>(OtherActor)) return;

	UC_Util::Print("Player Character BeginOverlapped with SpectatorBoxComponent", FColor::MakeRandomColor(), 10.f);
}

void UC_EnemyCombatFieldManager::OnSpectatorBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<AC_Player>(OtherActor)) return;

	UC_Util::Print("Player Character EndOverlap with SpectatorBoxComponent", FColor::MakeRandomColor(), 10.f);
}

