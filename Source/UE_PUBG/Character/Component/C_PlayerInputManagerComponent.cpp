// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/C_PlayerInputManagerComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedActionKeyMapping.h"
#include "UserSettings/EnhancedInputUserSettings.h"
UC_PlayerInputManagerComponent::UC_PlayerInputManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	// BP로 만든 기본 IMC를 로드
	// 에디터에서 할당해 주는게 안정적일 지도?
	//static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultIMCObj(TEXT("/Game/Project_PUBG/Common/Character/Input/IMC_Player.IMC_Player"));
	//if (DefaultIMCObj.Succeeded())
	//{
	//	IMC_Player = DefaultIMCObj.Object;
	//}

	
}

void UC_PlayerInputManagerComponent::BeginPlay()
{
	Super::BeginPlay();

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (PC)
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
        if (Subsystem)
        {
            // 복제본 생성
            IMC_User = DuplicateObject<UInputMappingContext>(IMC_Player, this);
            Subsystem->AddMappingContext(IMC_User, 0);
            //Subsystem->GetUserSettings()->GetCurrentKeyProfile()->GetPlayerMappingRows().FindKey()
            //Subsystem->AddPlayerMappedKey()
        }

        //LoadKeyMappings();
    }
}

//void UC_PlayerInputManagerComponent::InitializeInputContext()
//{
//}
//
//void UC_PlayerInputManagerComponent::SetKeyForAction(FName ActionName, const FKey& MainKey, const FKey& SubKey)
//{
//    if (!IMC_User) return;
//
// 
//    const FEnhancedActionKeyMapping* ActionKeyMapping = FindInputActionByName(ActionName);
//
//    if (!ActionKeyMapping) return;
//
//
//    // 기존 매핑 제거
//    IMC_User->UnmapKey(ActionKeyMapping->Action, ActionKeyMapping->Key);
//
//    // 새 매핑 추가
//    IMC_User->MapKey(ActionKeyMapping->Action, MainKey);
//
//    // 내부 데이터 업데이트
//    FInputKeyData& Data = SavedKeyMappings.FindOrAdd(ActionName);
//    //if (bIsMain) Data.MainKey = NewKey;
//    //else Data.SubKey = NewKey;
//	Data.MainKey = MainKey;
//}
//
//void UC_PlayerInputManagerComponent::SaveKeyMappings()
//{
//    //UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
//    //SaveGameInstance->SavedKeyData = InputKeyDataMap;
//    //
//    //UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerKeyBindings"), 0);
//}
//
//void UC_PlayerInputManagerComponent::LoadKeyMappings()
//{
//}
//
//void UC_PlayerInputManagerComponent::ResetToDefault()
//{
//    if (!IMC_User || !IMC_Player) return;
//
//    IMC_User->UnmapAll(); // 기존 매핑 제거
//
//    TArray<FEnhancedActionKeyMapping> Mappings = IMC_User->GetMappings();
//
//	if (Mappings.Num() == 0) return;
//
//    for (const FEnhancedActionKeyMapping& Mapping : IMC_Player->GetMappings())
//    {
//        IMC_User->MapKey(Mapping.Action, Mapping.Key);
//    }
//
//    SaveKeyMappings();
//}
//
//void UC_PlayerInputManagerComponent::SyncWithDefaultIMC()
//{
//}
//
//const FEnhancedActionKeyMapping* UC_PlayerInputManagerComponent::FindInputActionByName(FName ActionName)
//{
//    const TArray<FEnhancedActionKeyMapping>& Mappings = IMC_User->GetMappings();
//
//	if (Mappings.Num() == 0) return nullptr;
//
//    for (const FEnhancedActionKeyMapping& Mapping : Mappings)
//    {
//        if (Mapping.Action && Mapping.Action->GetFName() == ActionName)
//        {
//            return &Mapping;
//        }
//    }
//
//    return nullptr;
//}

//const FEnhancedActionKeyMapping* UC_PlayerInputManagerComponent::FindInputActionByName(FName ActionName)
//{
//    TArray<FEnhancedActionKeyMapping> Mappings = IMC_User->GetMappings();
//
//    if (Mappings.Num() == 0) return;
//
//    for (const FEnhancedActionKeyMapping& Mapping : IMC_User->GetMappings())
//    {
//        if (Mapping.Action && Mapping.Action->GetFName() == ActionName)
//        {
//            return Mapping;
//        }
//    }
//    return nullptr;
//}

//const FEnhancedActionKeyMapping* UC_PlayerInputManagerComponent::FindInputActionByName(FName ActionName)
//{
//	TArray<FEnhancedActionKeyMapping> Mappings = IMC_User->GetMappings();
//
//    if (Mappings.Num() == 0) return;
//
//    for (const FEnhancedActionKeyMapping& Mapping : IMC_User->GetMappings())
//    {
//        if (Mapping.Action && Mapping.Action->GetFName() == ActionName)
//        {
//            return &Mapping;
//        }
//    }
//    return nullptr;
//}

