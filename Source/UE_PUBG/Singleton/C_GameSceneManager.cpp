// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/C_GameSceneManager.h"

#include "EngineUtils.h"
#include "Character/C_Player.h"
#include "Character/C_Enemy.h"
#include "MagneticField/C_MagneticFieldManager.h"
#include "Airplane/C_AirplaneManager.h"
#include "Algo/RandomShuffle.h"
#include "Utility/C_Util.h"

#include "Character/Component/C_InvenSystem.h"

#include "HUD/C_HUDWidget.h"
#include "HUD/C_MainMapWidget.h"
#include "HUD/C_MapWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "HUD/C_TimeBoxWidget.h"
#include "Loot/C_LootCrate.h"
#include "Item/ItemManager/C_ItemManager.h"
#include "Sound/C_SoundManager.h"
#include "Singleton/C_GameInstance.h"
#include "TrainingLevel/C_TrainingGroundManager.h"
#include "TrainingLevel/Tutorial/C_TutorialManager.h"

#include "Utility/C_TickRandomColorGenerator.h"

UC_GameSceneManager::UC_GameSceneManager()
{
	static ConstructorHelpers::FClassFinder<AC_LootCrate> BPC_LootCrate(TEXT("/Game/Project_PUBG/Common/Loot/BPC_LootCrate.BPC_LootCrate_C"));
	if (BPC_LootCrate.Succeeded())
	{
		LootCrateClass = BPC_LootCrate.Class;
	}
}

void UC_GameSceneManager::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// TimerHandles pooling
	for (int i = 0; i < TimerHandlePoolCount; ++i) GameSceneTimerHandles.AddDefaulted();
	
	UC_GameInstance* GI = Cast<UC_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	UDataTable* RandomNameDataTable = GI->GetDataTables()[EDataTableType::RandomName];

	if (!RandomNameDataTable) UC_Util::Print("RandomNameDataTable not Loaded properly!", FColor::Red, 20.f);

	TArray<FName> RowNames = RandomNameDataTable->GetRowNames();
	Algo::RandomShuffle(RowNames);

	int EnemyCount{};
	int CharacterNumber{};
	
	// Level에 배치된 Actor들의 BeginPlay 호출되기 이전에 객체 초기화
	for (FActorIterator Actor(&InWorld); Actor; ++Actor)
	{
		if (AC_BasicCharacter* Character = Cast<AC_BasicCharacter>(*Actor))
		{
			AllCharacters.Add(Character);
			AllCharacterActors.Add(Character);
		}

		if (AC_Player* P = Cast<AC_Player>(*Actor))
		{
			Player = P;
			Player->SetCharacterNumber(++CharacterNumber);
			// /*HUDWidgets.Add(EHUDMode::IDLE,    Player->GetHUDWidget()); -> DONE
			// HUDWidgets.Add(EHUDMode::INVEN,   Player->GetInvenSystem()->GetInvenUI()); -> DONE
			// HUDWidgets.Add(EHUDMode::MAINMAP, Player->GetMainMapWidget()); -> DONE
			// MiniMapWidget = Player->GetHUDWidget()->GetMiniMapWidget();*/ ->
			continue;
		}

		if (AC_Enemy* E = Cast<AC_Enemy>(*Actor))
		{
			FNameStruct* Row = RandomNameDataTable->FindRow<FNameStruct>(RowNames[++EnemyCount], TEXT(""));

			if (Row)	E->SetCharacterName(Row->Name);
			else		UC_Util::Print("From GameSceneManager : RandomName Row missing!", FColor::Red, 10.f);

			E->SetCharacterNumber(++CharacterNumber);
			
			Enemies.Add(E);
			continue;
		}

		if (AC_MagneticFieldManager* MGF_Manager = Cast<AC_MagneticFieldManager>(*Actor))
		{
			MagneticFieldManager = MGF_Manager;
			continue;
		}
		
		if (AC_AirplaneManager* AP_Manager = Cast<AC_AirplaneManager>(*Actor))
		{
			AirplaneManager = AP_Manager;
			continue;
		}
		
		if (AC_TickRandomColorGenerator* RandomColorGenerator = Cast<AC_TickRandomColorGenerator>(*Actor))
		{
			TickRandomColorGenerator = RandomColorGenerator;
			continue;
		}
			
		if (AC_ItemManager* Item_Manager = Cast<AC_ItemManager>(*Actor))
		{
			ItemManager = Item_Manager;
			continue;
		}
		
		if (AC_SoundManager* Sound_Manager = Cast<AC_SoundManager>(*Actor)) SoundManager = Sound_Manager;
		
		if (AC_TutorialManager* Tutorial_Manager = Cast<AC_TutorialManager>(*Actor)) TutorialManager = Tutorial_Manager;
		if (AC_TrainingGroundManager* TrainingGround_Manager = Cast<AC_TrainingGroundManager>(*Actor)) TrainingGroundManager = TrainingGround_Manager;
	}

	CurrentRanking = AllCharacters.Num();
	TotalPlayedCharacterCount = AllCharacters.Num();
	CurrentAliveCharacterCount = TotalPlayedCharacterCount;
}

void UC_GameSceneManager::Initialize(FSubsystemCollectionBase& Collection)
{
	// UC_Util::Print("Initialize GameSceneManager", FColor::Red, 20.f);
	Super::Initialize(Collection);
	// 월드 파괴 전 호출되는 델리게이트 등록
	//FWorldDelegates::OnPreWorldFinishDestroy.AddUObject(this, &UC_GameSceneManager::OnWorldEndPlay);
}

void UC_GameSceneManager::Deinitialize()
{
	Super::Deinitialize();

	// UC_Util::Print("Deinitialize GameSceneManager", FColor::Red, 10.f);

	for (UObject* NONGCObject : GCProtectedObjects)
		NONGCObject->RemoveFromRoot();

	for (FTimerHandle& TimerHandle : GameSceneTimerHandles)
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	GameSceneTimerHandles.Empty();
	GCProtectedObjects.Empty();

	HUDWidgets.Empty();
	MiniMapWidget = nullptr;
}

AC_LootCrate* UC_GameSceneManager::SpawnLootCrateAt(FVector SpawnLocation, AC_BasicCharacter* DeadCharacter)
{
	if (!LootCrateClass || !DeadCharacter)
		return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AC_LootCrate* LootCrate = GetWorld()->SpawnActor<AC_LootCrate>(LootCrateClass, SpawnLocation, FRotator::ZeroRotator, Params);

	if (LootCrate)
	{
		LootCrate->SetCharacterLootCrate(DeadCharacter);
		LootCrate->SetActorHiddenInGame(false);
	}

	return LootCrate;
}

FTimerHandle& UC_GameSceneManager::GetTimerHandle()
{
	for (FTimerHandle& TimerHandle : GameSceneTimerHandles)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
			return TimerHandle;
	}

	// 만약 모든 TimerHandle이 사용중이라면 새로운 TimerHandle 추가해서 처리시킴
	GameSceneTimerHandles.AddDefaulted();
	return GameSceneTimerHandles.Last();
}

void UC_GameSceneManager::SetCurrentHUDMode(EHUDMode InHUDMode)
{
	UC_Util::Print("Set Current HUD Mode", FColor::Red, 10.f);
	
	CurrentHUDMode = InHUDMode;

	if (!IsValid(HUDWidgets[EHUDMode::IDLE]))		HUDWidgets[EHUDMode::IDLE]    = Player->GetHUDWidget();
	if (!IsValid(HUDWidgets[EHUDMode::MAINMAP]))	HUDWidgets[EHUDMode::MAINMAP] = Player->GetMainMapWidget();
	if (!IsValid(HUDWidgets[EHUDMode::INVEN]))		HUDWidgets[EHUDMode::INVEN]   = Player->GetInvenSystem()->GetInvenUI();
	if (!IsValid(MiniMapWidget))					MiniMapWidget                 = Player->GetHUDWidget()->GetMiniMapWidget();
	//if (!IsValid(HUDWidgets[EHUDMode::MAINMENU]))	HUDWidgets[EHUDMode::MAINMENU] = Player->GetController()->

	switch (InHUDMode)
	{
	case EHUDMode::IDLE:
		MiniMapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Collapsed);
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Collapsed);
		Player->GetInvenSystem()->GetInvenUI()->CloseDivideItemWidget();
		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Player->GetHUDWidget()->GetTimeBoxWidget()->ToggleKilledCountTextVisibility(false);
		return;
	case EHUDMode::INVEN:
		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::Collapsed);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Collapsed);
		MiniMapWidget->SetVisibility(ESlateVisibility::Collapsed);
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Visible);
		Player->GetHUDWidget()->GetTimeBoxWidget()->ToggleKilledCountTextVisibility(true);
		return;
	case EHUDMode::MAINMAP:
		HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Collapsed);
		Player->GetInvenSystem()->GetInvenUI()->CloseDivideItemWidget();

		HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::Collapsed);
		MiniMapWidget->SetVisibility(ESlateVisibility::Collapsed);
		HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Visible);
		Player->GetHUDWidget()->GetTimeBoxWidget()->ToggleKilledCountTextVisibility(true);

		//case EHUDMode::MAINMENU:

		return;
	case EHUDMode::MAX: default: return;
	}

	//switch (InHUDMode)
	//{
	//case EHUDMode::IDLE:
	//	MiniMapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//	HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Hidden);
	//	HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Hidden);
	//	Player->GetInvenSystem()->GetInvenUI()->CloseDivideItemWidget();
	//	HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//	return;
	//case EHUDMode::INVEN:
	//	HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::Hidden);
	//	HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Hidden);
	//	MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
	//	HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Visible);
	//	return;
	//case EHUDMode::MAINMAP:
	//	HUDWidgets[EHUDMode::INVEN]->SetVisibility(ESlateVisibility::Hidden);
	//	Player->GetInvenSystem()->GetInvenUI()->CloseDivideItemWidget();

	//	HUDWidgets[EHUDMode::IDLE]->SetVisibility(ESlateVisibility::Hidden);
	//	MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
	//	HUDWidgets[EHUDMode::MAINMAP]->SetVisibility(ESlateVisibility::Visible);

	////case EHUDMode::MAINMENU:

	//	return;
	//case EHUDMode::MAX: default: return;
	//}
}

FColor UC_GameSceneManager::GetTickRandomColor() const
{
	if (!IsValid(TickRandomColorGenerator)) return FColor::Red;
	return TickRandomColorGenerator->GetTickRandomColor();
}

FVector UC_GameSceneManager::ConvertTileCoordinateToTileMiddleLocation(const TPair<uint8, uint8>& TileCoordinate) const
{
	uint8 TileX = TileCoordinate.Key;
	uint8 TileY = TileCoordinate.Value;

	// (0, 0) Cell의 World Location X, Y
	float TopRightWorldX =  CELL_WORLDSIZE * 4.5f;
	float TopRightWorldY = -CELL_WORLDSIZE * 4.5f;

	float XLocation = TopRightWorldX - CELL_WORLDSIZE * TileX;
	float YLocation = TopRightWorldY + CELL_WORLDSIZE * TileY;
	
	return FVector(XLocation, YLocation, 0.f);
}

TPair<uint8, uint8> UC_GameSceneManager::GetContainingTileCoordinate(const FVector2D& WorldPositionXY) const
{
	const float X = WorldPositionXY.X;
	const float Y = WorldPositionXY.Y;
	
	FVector2D XYBorderLimits = {-CELL_WORLDSIZE * 5.f, CELL_WORLDSIZE * 5.f};

	if (X < XYBorderLimits.X || X > XYBorderLimits.Y || Y < XYBorderLimits.X || Y > XYBorderLimits.Y)
	{
		UC_Util::Print("From UC_GameSceneManager::GetContainingTileCoordinate : Parameter WorldPositionXY out of border!", FColor::Red, 10.f);
		return {};
	}

	return { (CELL_WORLDSIZE * 5.f - X) / 100, (CELL_WORLDSIZE * 5.f + Y) / 100 };
}

//void UC_GameSceneManager::AddSpawnedItemToContainer(class AC_Item* InItem)
//{
//	ItemContainer.Add(InItem);
//}

void UC_GameSceneManager::AddSpawnedItemToContainer(AC_Item* InItem)
{
	if (!ItemManager) return;
	if (!InItem) return;
	ItemManager->AddSpawnedItemToContainer(InItem);
}

void UC_GameSceneManager::DeleteSpawnedItemToContainer(AC_Item* InItem)
{
	if (!ItemManager) return;
	if (!InItem) return;
	ItemManager->DeleteSpawnedItemToContainer(InItem);
}

void UC_GameSceneManager::ToggleItemsHiddenInGame(bool InHiddenInGame)
{
	for (AC_Item* SpawnedItem : ItemContainer)
	{
		SpawnedItem->SetActorHiddenInGame(InHiddenInGame);
	}


	for (APawn* SpawnedVehicle : VehicleContainer)
	{
		SpawnedVehicle->SetActorHiddenInGame(InHiddenInGame);
		UC_Util::Print("UnHiddenCars: ", FColor::Blue, 10.f);
	}
	
	if (!InHiddenInGame) SetVehiclesCollision();
}

void UC_GameSceneManager::AddVehiclesToContainer(APawn* InVehicle)
{
	VehicleContainer.Add(InVehicle);
	UC_Util::Print("Container: ", FColor::Red, 10.f);

	
}

void UC_GameSceneManager::SetVehiclesCollision()
{
	OnSetVehiclesCollision.Broadcast();
}
