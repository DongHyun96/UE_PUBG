// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "C_PlayerCombatFieldManager.generated.h"


enum class EAttachmentNames : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_PUBG_API UC_PlayerCombatFieldManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UC_PlayerCombatFieldManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	class AC_Enemy* GetCombatFieldEnemy() const { return CombatFieldEnemy; }

	void SetOwnerCombatFieldManager(class AC_CombatFieldManager* InCombatFieldManager) { OwnerCombatFieldManager = InCombatFieldManager; }
	void SetPlayerCombatFieldWidget(class UC_PlayerCombatFieldWidget* InPlayerCombatFieldWidget) { PlayerCombatFieldWidget = InPlayerCombatFieldWidget; }

private: // Combat Start 처리 관련 (OpeningGate Interaction)
	
	UFUNCTION()
	void OnGateOpeningBoxBeginOverlap
	(
		UPrimitiveComponent*	OverlappedComp,
		AActor*					OtherActor,
		UPrimitiveComponent*	OtherComp,
		int32					OtherBodyIndex,
		bool					bFromSweep,
		const FHitResult&		SweepResult
	);

	UFUNCTION()
	void OnGateOpeningBoxEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor*				 OtherActor,
		UPrimitiveComponent* OtherComp,
		int32				 OtherBodyIndex
	);

	/// <summary>
	/// Start Gate OpeningBox Overlap되었을 때, F Key Start 처리
	/// </summary>
	/// <returns> : 제대로 Start 처리가 되었다면 return true </returns>
	bool OnStartGateFKeyInteraction();

private:

	AC_CombatFieldManager* OwnerCombatFieldManager{};

protected:

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	AC_Enemy* CombatFieldEnemy{};

private: // Spawn(Respawn) Transform 관련
	
	TArray<FTransform> SpawnTransforms{};
	
private:
	
	UC_PlayerCombatFieldWidget* PlayerCombatFieldWidget{};

protected:

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	class AC_TutorialGate* CombatFieldStartGate{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<EAttachmentNames, TSubclassOf<class AC_AttachableItem>> GunAttachableClasses{};

};
