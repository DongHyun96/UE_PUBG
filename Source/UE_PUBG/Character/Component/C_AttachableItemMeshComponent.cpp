// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_AttachableItemMeshComponent.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UMG.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_InvenComponent.h"

#include "Components/PanelWidget.h"
#include "Components/NamedSlotInterface.h"
#include "Utility/C_Util.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CanvasPanelSlot.h"
#include "Character/C_BasicCharacter.h"
#include "Character/Component/C_EquippedComponent.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/C_Player.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Component/C_CrosshairWidgetComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "Item/Attachment/C_AttachableItem.h"

#include "Item/Weapon/Gun/C_Bullet.h"
// Sets default values for this component's properties
UC_AttachableItemMeshComponent::UC_AttachableItemMeshComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UC_AttachableItemMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	AttachableItemsMesh.Add(EPartsName::SCOPE);
	AttachableItemsMesh[EPartsName::SCOPE].Add(EAttachmentNames::REDDOT);
	;
	// ...
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Project_PUBG/Hyunho/Weapon/Attatchments/SM_T4_Sight.SM_T4_Sight"));
	
	TArray<UObject*> MeshAssets1;
	EngineUtils::FindOrLoadAssetsByPath(TEXT("/Game/Project_PUBG/Hyunho/Weapon/Attatchments/"), MeshAssets1, EngineUtils::ATL_Regular);
	TArray<UObject*> MeshAssets2;
	EngineUtils::FindOrLoadAssetsByPath(TEXT("/Game/Project_PUBG/Hyunho/Weapon/Attatchments/"), MeshAssets2, EngineUtils::ATL_Regular);
	//if (!MeshAssets1.IsEmpty())
	//{
	//	UC_Util::Print("It's not Empty", FColor::Blue, 100);
	//}
	for (auto& mesh : MeshAssets1)
	{
		if (mesh->GetName() == "A_RedDot")
		{
			UStaticMesh* TempMesh = Cast<UStaticMesh>(mesh);
			UStaticMesh* TempMeshComponent{};
			UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this);
			MeshComponent->RegisterComponent();
			MeshComponent->SetStaticMesh(TempMesh);
			AttachableItemsMesh[EPartsName::SCOPE][EAttachmentNames::REDDOT].Emplace(MeshComponent);
			//UC_Util::Print(TempMesh, FColor::Red, 100);
		}
	}
	for (auto& mesh : MeshAssets2)
	{
		if (mesh->GetName() == "A_RedDot")
		{
			UStaticMesh* TempMesh = Cast<UStaticMesh>(mesh);
			UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this);
			MeshComponent->RegisterComponent();

			MeshComponent->SetStaticMesh(TempMesh);
			AttachableItemsMesh[EPartsName::SCOPE][EAttachmentNames::REDDOT].Emplace(MeshComponent);
			//UC_Util::Print(TempMesh, FColor::Red, 100);
		}


	}
	UC_Util::Print(AttachableItemsMesh[EPartsName::SCOPE][EAttachmentNames::REDDOT].Num(), FColor::Emerald, 100);


	//if (IsValid(MeshAsset.Object))
	//{
	//	UStaticMeshComponent* TempMesh{};
	//	TempMesh->SetStaticMesh(MeshAsset.Object);
	//	AttachableItemsMesh[EPartsName::SCOPE][EAttachmentNames::REDDOT].Add(TempMesh);
	//}
	//if (IsValid(MeshAsset.Object))
	//{
	//	UStaticMeshComponent* TempMesh{};
	//	TempMesh->SetStaticMesh(MeshAsset.Object);
	//	AttachableItemsMesh[EPartsName::SCOPE][EAttachmentNames::REDDOT].Add(TempMesh);
	//}
}


// Called every frame
void UC_AttachableItemMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
/// <summary>
/// 같은 메쉬의 부착물 2개 중 AttachParent가 없는 메쉬를 찾는 함수
/// </summary>
/// <param name="InPartsName">부착물의 부위</param>
/// <param name="InAttachmentName">부착물의 이름</param>
/// <returns></returns>
UMeshComponent* UC_AttachableItemMeshComponent::GetAttachablePartMesh(EPartsName InPartsName, EAttachmentNames InAttachmentName)
{
	TArray<class UMeshComponent*> AttachmentItem = AttachableItemsMesh[InPartsName][InAttachmentName];

	if (AttachmentItem.IsEmpty())    return nullptr;

	if (!IsValid(AttachmentItem[0])) return nullptr;

	if (!IsValid(AttachmentItem[1])) return nullptr;


	if (!AttachmentItem[0]->GetAttachParent())
		return AttachmentItem[0];
	else
		return AttachmentItem[1];
	return nullptr;
}

/// <summary>
/// 해당 총기에 부착물을 부착하는 함수
/// 부착하는 동시에 IronSightMesh를 HiddenInGame시켜줌
/// </summary>
/// <param name="InParent">장착할 총기</param>
/// <param name="InPartsName">부착물의 부위</param>
/// <param name="InAttachmentName">부착물의 이름</param>
/// <returns></returns>
bool UC_AttachableItemMeshComponent::AttachToGun(USceneComponent* InParent, EPartsName InPartsName, EAttachmentNames InAttachmentName)
{
	
	AC_Gun* ParentGun = Cast<AC_Gun>(InParent->GetOuter());

	UMeshComponent* AttachmentMesh = GetAttachablePartMesh(InPartsName, InAttachmentName);
	if (!IsValid(AttachmentMesh)) return false;
	if (!IsValid(ParentGun))      return false;

	AttachmentMesh->SetHiddenInGame(false);
	if (ParentGun->GetIsPartAttached(InPartsName))
		DetachFromGun(InParent, InPartsName, ParentGun->GetAttachedItemName(InPartsName));

	ParentGun->SetIronSightMeshHiddenInGame(true);
	ParentGun->SetIsPartAttached(InPartsName, true);
	ParentGun->SetAttachedItemNameInPart(InPartsName, InAttachmentName);
	switch (InAttachmentName)
	{
	case EAttachmentNames::REDDOT:
	case EAttachmentNames::SCOPE4:
	case EAttachmentNames::SCOPE8:
		ParentGun->SetSightCameraSpringArmLocation(ParentGun->GetAttachmentPartsHolsterCameraLocations()[InAttachmentName]);
		ParentGun->SetScopeCameraMode(InAttachmentName);
		break;
	default:
		break;
	}
	UC_Util::Print(AttachmentMesh->GetName(), FColor::Black, 100);
	return AttachmentMesh->AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		ParentGun->GetAttachmentPartsHolsterNames()[InAttachmentName]
	);

}
/// <summary>
/// 해당 총기에 부착된 Attachment를 찾아서 탈착
/// 탈착하는 동시에 IronSightMesh를 보이게 돌려줌
/// </summary>
/// <param name="InParent">해당 아이템이 부착된 총기</param>
/// <param name="InPartsName">부착물의 부위</param>
/// <param name="InAttachmentName">부착물의 이름</param>
void UC_AttachableItemMeshComponent::DetachFromGun(USceneComponent* InParent, EPartsName InPartsName, EAttachmentNames InAttachmentName)
{
	TArray<class UMeshComponent*> AttachmentItem = AttachableItemsMesh[InPartsName][InAttachmentName];
	AC_Gun* ParentGun = Cast<AC_Gun>(InParent->GetOuter());
	if (!IsValid(ParentGun))         return;
	if (AttachmentItem.IsEmpty())    return;
	if (!IsValid(AttachmentItem[0])) return;
	if (!IsValid(AttachmentItem[1])) return;
	ParentGun->SetIsPartAttached(InPartsName, false);
	ParentGun->SetSightCameraSpringArmLocation(ParentGun->GetAttachmentPartsHolsterCameraLocations()[EAttachmentNames::MAX]);
	ParentGun->SetScopeCameraMode(EAttachmentNames::MAX);

	if (AttachmentItem[0]->GetAttachParent() == InParent)
	{
		AttachmentItem[0]->DetachFromParent();
		AttachmentItem[0]->SetHiddenInGame(true);
		ParentGun->SetIronSightMeshHiddenInGame(false);
	}
	else if (AttachmentItem[1]->GetAttachParent() == InParent)
	{
		AttachmentItem[1]->DetachFromParent();
		AttachmentItem[1]->SetHiddenInGame(true);
		ParentGun->SetIronSightMeshHiddenInGame(false);

	}
	return;

}

