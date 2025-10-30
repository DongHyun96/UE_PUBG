// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_AttachableItemMeshComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UMG.h"
#include "Character/C_BasicCharacter.h"
#include "Character/C_Player.h"
#include "Character/Component/C_EquippedComponent.h"
//#include "Character/Component/C_InvenComponent.h"
#include "Character/Component/C_EquippedComponent.h"
#include "Character/Component/C_CrosshairWidgetComponent.h"

#include "UObject/ConstructorHelpers.h"
#include "Utility/C_Util.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/NamedSlotInterface.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ShapeComponent.h"
#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"

#include "Item/Weapon/WeaponStrategy/C_GunStrategy.h"
#include "Item/Attachment/C_AttachableItem.h"
#include "Item/AttachmentActors/AttachmentActor.h"

#include "Item/Weapon/Gun/C_Gun.h"
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
	for (int32 i = 0; i < (int32)EPartsName::MAX; ++i) // EAttachmentNames에 MAX가 있다면
	{
		EPartsName AttachmentName = (EPartsName)i;
		AttachableItemsMesh.Add(AttachmentName);
	}
	//AttachableItemsMesh.Add(EPartsName::SCOPE);
	AttachableItemsMesh[EPartsName::SCOPE].Add(EAttachmentNames::REDDOT);
	AttachableItemsMesh[EPartsName::SCOPE].Add(EAttachmentNames::SCOPE4);
	AttachableItemsMesh[EPartsName::GRIP].Add(EAttachmentNames::VERTGRIP);
	AttachableItemsMesh[EPartsName::MUZZLE].Add(EAttachmentNames::COMPENSATOR);
	//AttachableItemsMesh[EPartsName::SCOPE].Add(EAttachmentNames::SCOPE4);
	

	//TArray<UObject*> AttachmentBluePrintAssets1;
	//EngineUtils::FindOrLoadAssetsByPath(TEXT("/Game/Project_PUBG/Hyunho/Weapon/Attatchments/AttachmentBluePrints"), AttachmentBluePrintAssets1, EngineUtils::ATL_Regular);
	//TArray<UObject*> AttachmentBluePrintAssets2;
	//EngineUtils::FindOrLoadAssetsByPath(TEXT("/Game/Project_PUBG/Hyunho/Weapon/Attatchments/AttachmentBluePrints"), AttachmentBluePrintAssets2, EngineUtils::ATL_Regular);

	UDataTable* AttachmentDataTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/Project_PUBG/Common/Item/ItemDataTables/DT_AttachmentActors.DT_AttachmentActors") 
	);

	if (!AttachmentDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Attachment DataTable not found!"));
		return;
	}

	static const FString Context(TEXT("Attachment DataTable Context"));
	TArray<FAttachmentTableRow*> Rows;
	AttachmentDataTable->GetAllRows(Context, Rows);

	for (auto Row : Rows)
	{
		if (!Row || !Row->AttachmentClass)
			continue;

		for (int i = 0; i < 2; ++i) // 🔹 각 부착물을 2개씩 스폰
		{
			UClass* ActorClass = Row->AttachmentClass;
			AAttachmentActor* TempMesh = GetWorld()->SpawnActor<AAttachmentActor>(ActorClass);

			if (TempMesh)
			{
				if (!AttachableItemsMesh.Contains(Row->PartType))
				{
					AttachableItemsMesh.Add(Row->PartType);
				}

				if (!AttachableItemsMesh[Row->PartType].Contains(Row->AttachmentName))
				{
					AttachableItemsMesh[Row->PartType].Add(Row->AttachmentName);
				}

				AttachableItemsMesh[Row->PartType][Row->AttachmentName].Add(TempMesh);
			}
		}
	}

	//for (auto& Attachment : AttachmentBluePrintAssets1)
	//{
	//
	//	UBlueprint* BlueprintAsset = Cast<UBlueprint>(Attachment);
	//	if (BlueprintAsset && BlueprintAsset->GeneratedClass)
	//	{
	//		UClass* ActorClass = BlueprintAsset->GeneratedClass;
	//
	//		AAttachmentActor* TempMesh = GetWorld()->SpawnActor<AAttachmentActor>(ActorClass);
	//		if (TempMesh)
	//		{
	//			if (AttachableItemsMesh[TempMesh->GetPartName()].IsEmpty())
	//			{
	//				AttachableItemsMesh[TempMesh->GetPartName()].Add(TempMesh->GetAttachmentName());
	//			}
	//			AttachableItemsMesh[TempMesh->GetPartName()][TempMesh->GetAttachmentName()].Emplace(TempMesh);
	//			
	//			//UC_Util::Print(TempMesh, FColor::Red, 100);
	//		}
	//	}
	//	
	//}
	//
	//
	//for (auto& Attachment : AttachmentBluePrintAssets2)
	//{
	//
	//	UBlueprint* BlueprintAsset = Cast<UBlueprint>(Attachment);
	//	if (BlueprintAsset && BlueprintAsset->GeneratedClass)
	//	{
	//		UClass* ActorClass = BlueprintAsset->GeneratedClass;
	//
	//		AAttachmentActor* TempMesh = GetWorld()->SpawnActor<AAttachmentActor>(ActorClass);
	//		if (TempMesh)
	//		{
	//			//AttachableItemsMesh[TempMesh->GetPartName()].Add(TempMesh->GetAttachmentName()).Emplace(TempMesh);
	//
	//			AttachableItemsMesh[TempMesh->GetPartName()][TempMesh->GetAttachmentName()].Emplace(TempMesh);
	//
	//			//UC_Util::Print(TempMesh, FColor::Red, 100);
	//		}
	//	}
	//	
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
AAttachmentActor* UC_AttachableItemMeshComponent::GetAttachablePartMesh(EPartsName InPartsName, EAttachmentNames InAttachmentName)
{
	TArray<class AAttachmentActor*> AttachmentItem = AttachableItemsMesh[InPartsName][InAttachmentName];

	if (AttachmentItem.IsEmpty())    return nullptr;

	if (!IsValid(AttachmentItem[0])) return nullptr;

	if (!IsValid(AttachmentItem[1])) return nullptr;


	if (!AttachmentItem[0]->GetAttachParentActor())
		return AttachmentItem[0];
	else
		return AttachmentItem[1];
	return nullptr;
}

AAttachmentActor* UC_AttachableItemMeshComponent::GetCurrentAttachment(USceneComponent* InParent, EPartsName InPartsName, EAttachmentNames InAttachmentName)
{
	TArray<class AAttachmentActor*> AttachmentItem = AttachableItemsMesh[InPartsName][InAttachmentName];
	AC_Gun* ParentGun = Cast<AC_Gun>(InParent->GetOuter());
	if (!IsValid(ParentGun))         return nullptr;
	if (AttachmentItem.IsEmpty())    return nullptr;
	if (!IsValid(AttachmentItem[0])) return nullptr;
	if (!IsValid(AttachmentItem[1])) return nullptr;
	UC_Util::Print("Detached Attachment!", FColor::Black, 20);

	if (Cast<AC_Gun>(AttachmentItem[0]->GetAttachParentActor()) == ParentGun)
		return AttachmentItem[0];
	else if (Cast<AC_Gun>(AttachmentItem[1]->GetAttachParentActor()) == ParentGun)
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
	AAttachmentActor* AttachmentMesh = GetAttachablePartMesh(InPartsName, InAttachmentName);
	if (!IsValid(AttachmentMesh)) return false;
	if (!IsValid(ParentGun))      return false;
	if (!ParentGun->GetAttachableParts().Contains(InPartsName)) return false;
	UC_Util::Print("Attached Attachment!", FColor::Black, 20);

	AttachmentMesh->SetActorHiddenInGame(false);
	if (ParentGun->GetIsPartAttached(InPartsName))
		DetachFromGun(InParent, InPartsName, ParentGun->GetAttachedItemName(InPartsName));

	ParentGun->SetIsPartAttached(InPartsName, true);
	ParentGun->SetAttachedItemNameInPart(InPartsName, InAttachmentName);
	ParentGun->SetAttachedItems(InPartsName, AttachmentMesh);
	AttachmentMesh->AttachToComponent
	(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		ParentGun->GetAttachmentPartsHolsterNames()[InAttachmentName]
	);
	UC_Util::Print(AttachmentMesh->GetName(), FColor::Black, 100);
	return AttachmentMesh->UseAttachStrategy();

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
	TArray<AAttachmentActor*> AttachmentItem = AttachableItemsMesh[InPartsName][InAttachmentName];
	AC_Gun* ParentGun = Cast<AC_Gun>(InParent->GetOuter());
	if (!IsValid(ParentGun))         return;
	if (AttachmentItem.IsEmpty())    return;
	if (!IsValid(AttachmentItem[0])) return;
	if (!IsValid(AttachmentItem[1])) return;
	UC_Util::Print("Detached Attachment!", FColor::Black, 20);
	ParentGun->SetIsPartAttached(InPartsName, false);

	ParentGun->SetAttachedItems(InPartsName, nullptr);

	if (Cast<AC_Gun>(AttachmentItem[0]->GetAttachParentActor()) == ParentGun)
		AttachmentItem[0]->UseDetachStrategy();
	else if (Cast<AC_Gun>(AttachmentItem[1]->GetAttachParentActor()) == ParentGun)
		AttachmentItem[1]->UseDetachStrategy();
}

void UC_AttachableItemMeshComponent::UseAttachmentStrategy(USceneComponent* InParent)
{
}




