// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/C_BasicCharacter.h"

#include "C_ParkourComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_PUBG_API UC_ParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UC_ParkourComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerCharacter(class AC_BasicCharacter* InOwnerCharacter) { OwnerCharacter = InOwnerCharacter; }

	/// <summary>
	/// For Testing
	/// </summary>
	void SwapMesh(bool ToRootedMesh);

	void Vault();

	UFUNCTION(BlueprintCallable)
	void OnParkourAnimMontageEnd();

private:

	void VaultMotionWarp();

private:

	class AC_BasicCharacter* OwnerCharacter{};

protected:

	// Root Bone 처리된 Skeletal Mesh
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class USkeletalMesh* RootedSkeletalMesh{};

	// Root Bone 처리된 Skeletal Mesh의 Anim class
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> RootedAnimInstanceClass{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FPriorityAnimMontage VaultingMontage{};

private:

	class USkeletalMesh* MainSkeletalMesh{};

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<class UAnimInstance> MainAnimInstanceClass{};

private:

	FVector VaultStartPos{};
	FVector VaultMiddlePos{};
	FVector VaultLandPos{};
	bool CanWarp{};

private:

	// Tick 함수에서 SkeletalMesh와 AnimInstanceClass를 바꾸는지 -> Deferred Update를 사용할 예정
	bool bPendingMeshUpdateToMainMesh{};

};
