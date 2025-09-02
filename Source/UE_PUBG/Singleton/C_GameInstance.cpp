// Fill out your copyright notice in the Description page of Project Settings.


#include "C_GameInstance.h"
#include "LoadingScreenModule.h"
#include "Utility/C_Util.h"

void UC_GameInstance::Init()
{
	Super::Init();
	
	// This ensures our function is called right before a map begins loading
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UC_GameInstance::BeginLoadingScreen);

	// This ensures our function is called after a map has finished loading
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UC_GameInstance::EndLoadingScreen);

	InitItemDataCache();

}

void UC_GameInstance::BeginLoadingScreen(const FString& InMapName)
{
	UE_LOG(LogTemp, Warning, TEXT("UC_GameInstance::BeginLoadingScreen: %s"), *InMapName);

	// Try to get the loading screen module
	FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule");
	if (LoadingScreenModule)
	{
		// Module found - Start the loading screen
		LoadingScreenModule->StartLoadingScreen(InMapName);
	}
	else
	{
		// Module not found
		UE_LOG(LogTemp, Warning, TEXT("UC_GameInstance::BeginLoadingScreen: LoadingScreenModule not found"));
	}
	
	
}

void UC_GameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("UC_GameInstance::EndLoadingScreen: %s"), *InLoadedWorld->GetName());
}

void UC_GameInstance::SetPlayerNickName(const FString& InNickName)
{
	PlayerNickName = InNickName;
	bPlayerNickNameSet = true;
}

const FItemData* UC_GameInstance::GetItemData(FName ItemRowName) const
{
	if (const FItemData* FoundData = CachedItemData.Find(ItemRowName))
	{
		return FoundData;
	}

	//UE_LOG(LogTemp, Warning, TEXT("ItemRowName %s not found in CachedItemData"), *ItemRowName.ToString());
	UC_Util::Print(FString::Printf(TEXT("ItemRowName %s not found in CachedItemData"), *ItemRowName.ToString()), FColor::Red, 20.f);
	return nullptr;
}

void UC_GameInstance::InitItemDataCache()
{
	// 먼저 CachedItemData 초기화
	CachedItemData.Empty();

	// DataTables에서 Item DataTable 가져오기
	UDataTable** ItemTablePtr = DataTables.Find(EDataTableType::Item);
	if (!ItemTablePtr || !*ItemTablePtr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Item DataTable is not set!"));
		UC_Util::Print("Item DataTable is not set!", FColor::Red, 20.f);
		return;
	}

	UDataTable* ItemTable = *ItemTablePtr;

	// RowName 가져오기
	TArray<FName> RowNames = ItemTable->GetRowNames();

	for (FName RowName : RowNames)
	{
		FItemData* RowData = ItemTable->FindRow<FItemData>(RowName, TEXT("InitItemDataCache"));
		if (RowData)
		{
			// 캐싱
			CachedItemData.Add(RowName, *RowData);
		}
	}
}
