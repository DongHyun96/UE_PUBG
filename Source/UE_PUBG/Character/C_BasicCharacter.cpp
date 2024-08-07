// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_BasicCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"

#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Utility/C_Util.h"


// Sets default values
AC_BasicCharacter::AC_BasicCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EquippedComponent = CreateDefaultSubobject<UC_EquippedComponent>("EquippedComponent");
	EquippedComponent->SetOwnerCharacter(this);

	InvenComponent = CreateDefaultSubobject<UC_InvenComponent>("InvenComponent");
	InvenComponent->SetOwnerCharacter(this);


}

// Called when the game starts or when spawned
void AC_BasicCharacter::BeginPlay()
{
	Super::BeginPlay();
	MainCamera = Cast<UCameraComponent>(GetDefaultSubobjectByName("Camera"));
}

// Called every frame
void AC_BasicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AC_BasicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

float AC_BasicCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	// Deprecated in UE_PUBG Project
	// Do Nothing

	FString DebugMessage = "In AC_BasicCharacter::PlayAnimMontage : This function is deprecated. Use AC_PriorityAnimMontage instead.";
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *DebugMessage);

	return 0.0f;
}

float AC_BasicCharacter::PlayAnimMontage(const FPriorityAnimMontage& PAnimMontage, float InPlayRate, FName StartSectionName)
{
	if (!IsValid(PAnimMontage.AnimMontage)) return 0.f;

	// 한번도 호출된 적 없을 땐 그냥 재생
	if (!CurPriorityAnimMontage.AnimMontage)
	{
		CurPriorityAnimMontage = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}
	
	// 직전의 AnimMontage의 재생이 이미 끝났을 때
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurPriorityAnimMontage.AnimMontage))
	{
		CurPriorityAnimMontage = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// 현재 재생중인 PriorityAnimMontage가 있을 때
	
	// Priority 비교해서 현재 Priority보다 크거나 같은 Priority라면 새로 들어온 AnimMontage 재생
	if (PAnimMontage.Priority >= CurPriorityAnimMontage.Priority)
	{
		CurPriorityAnimMontage = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// Priority가 현재 재생중인 Montage가 더 크다면 새로이 재생하지 않고 그냥 return
	return 0.0f;
}

float AC_BasicCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	FString Str = "Character Damaged! Damaged Amount : " + FString::SanitizeFloat(DamageAmount);

	UC_Util::Print(Str, FColor::Cyan, 3.f);

	return 0.0f;
}

float AC_BasicCharacter::TakeDamage(float DamageAmount, EDamagingPartType DamagingPartType, AActor* DamageCauser)
{
	//FString Str = "Character Damaged on certain damaging part! Damaged Amount : " + FString::SanitizeFloat(DamageAmount);
	//UC_Util::Print(Str, FColor::Cyan, 3.f);

	// TODO : Armor 확인해서 Armor 부분이라면 Damage 감소 적용
	// TODO : Armor 또한 피 깎기

	// TODO : 실질적인 피해량 return 시키기
	return DamageAmount;
}

float AC_BasicCharacter::TakeDamage(float DamageAmount, FName DamagingPhyiscsAssetBoneName, AActor* DamageCauser)
{
	if (!DAMAGINGPARTS_MAP.Contains(DamagingPhyiscsAssetBoneName))
	{
		UC_Util::Print("From AC_BasicCharacter::TakeDamage : No Such PhysicsAsset Bone Name exists!");
		return 0.f;
	}

	//UC_Util::Print(DamagingPhyiscsAssetBoneName.ToString() + " Parts damaged! Amount : " + FString::SanitizeFloat(DamageAmount));

	return TakeDamage(DamageAmount, DAMAGINGPARTS_MAP[DamagingPhyiscsAssetBoneName], DamageCauser);
}

