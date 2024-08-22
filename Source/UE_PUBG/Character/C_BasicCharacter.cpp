// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/C_BasicCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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

	//InvenComponent = CreateDefaultSubobject<UC_InvenComponent>("InvenComponent");
	//InvenComponent->SetOwnerCharacter(this);

	//InvenComponent = CreateDefaultSubobject<UC_InvenComponent>("InvenComponent");
	//InvenComponent->SetOwnerCharacter(this);

	// Find the Blueprint component class, �������Ʈ ������Ʈ Ŭ������ ã��.
	//D:\UE_Project\UE_PUBG\Content\Project_PUBG\Common\InventoryUI\Blueprint_UI
	static ConstructorHelpers::FClassFinder<UC_InvenComponent> BlueprintComponentClassFinder(TEXT("Project_PUBG/Common/InventoryUI/Blueprint_UI/BPC_InvenSystem"));
	if (BlueprintComponentClassFinder.Succeeded())
	{
		BPC_InvenSystem = BlueprintComponentClassFinder.Class;
	}

	BPC_InvenSystem = nullptr;

}

// Called when the game starts or when spawned
void AC_BasicCharacter::BeginPlay()
{
	Super::BeginPlay();

	

	// Find the Blueprint component class, �������Ʈ ������Ʈ Ŭ������ ã��.
	//D:\UE_Project\UE_PUBG\Content\Project_PUBG\Common\InventoryUI\Blueprint_UI
	FString BPPath = TEXT("/Game/Project_PUBG/Common/InventoryUI/Blueprint_UI/BPC_InvenSystem.BPC_InvenSystem_C");
	////D:/UE_Project/UE_PUBG/Content/Project_PUBG/Common/InventoryUI/Blueprint_UI/BPC_InvenSystem.uasset
	////FString BPPath = TEXT("Project_PUBG/Common/InventoryUI/Blueprint_UI/BPC_InvenSystem");
	BPC_InvenSystem = LoadClass<UC_InvenComponent>(nullptr, *BPPath);
	// Add the Blueprint component to the character, ĳ���Ϳ� �������Ʈ ������Ʈ Ŭ������ �߰����ش�.
	// �ٵ� �����Ϳ��� �����ִ°��� Ȯ���ϰ� ���� �߰�����.
	if (BPC_InvenSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded blueprint class."));

		/*
		CreateDefaultSubobject�� �ƴ϶� NewObject�� ����� ������ BPC_InvenSystem�� ��Ÿ�ӿ� �������� �ε�� �������Ʈ Ŭ�����̱� ����.
		CreateDefaultSubobject�� ������Ÿ�ӿ� ������ ���� ������Ʈ�� �����ϴµ� �����ϴ�.
		NewObject�� ��Ÿ�ӿ� �������� ������ ��ü�� �����ϴµ� �� �����ϴ�.
		*/
		BPC_InvenSystemInstance = NewObject<UC_InvenComponent>(this, BPC_InvenSystem);
		if (BPC_InvenSystemInstance)
		{
			BPC_InvenSystemInstance->RegisterComponent();
			AddInstanceComponent(BPC_InvenSystemInstance);
			BPC_InvenSystemInstance->SetOwnerCharacter(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create the Blueprint component instance!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BPC_InvenSystem is null!"));
	}

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

	// �ѹ��� ȣ��� �� ���� �� �׳� ���
	if (!CurPriorityAnimMontage.AnimMontage)
	{
		CurPriorityAnimMontage = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}
	
	// ������ AnimMontage�� ����� �̹� ������ ��
	if (!GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurPriorityAnimMontage.AnimMontage))
	{
		CurPriorityAnimMontage = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// ���� ������� PriorityAnimMontage�� ���� ��
	
	// Priority ���ؼ� ���� Priority���� ũ�ų� ���� Priority��� ���� ���� AnimMontage ���
	if (PAnimMontage.Priority >= CurPriorityAnimMontage.Priority)
	{
		CurPriorityAnimMontage = PAnimMontage;
		return Super::PlayAnimMontage(PAnimMontage.AnimMontage, InPlayRate, StartSectionName);
	}

	// Priority�� ���� ������� Montage�� �� ũ�ٸ� ������ ������� �ʰ� �׳� return
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

	// TODO : Armor Ȯ���ؼ� Armor �κ��̶�� Damage ���� ����
	// TODO : Armor ���� �� ���

	// TODO : �������� ���ط� return ��Ű��
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

