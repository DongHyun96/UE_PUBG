// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/C_Util.h"

#include "Animation/WidgetAnimation.h"
#include "Blueprint/UserWidget.h"

void UC_Util::Print(const FString& str, const FColor& InColor, const float& TimeToDisplay)
{
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *str);
}

void UC_Util::Print(const int& data, const FColor& InColor, const float& TimeToDisplay)
{
	FString str = FString::FromInt(data);
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *str);
}

void UC_Util::Print(const float& data, const FColor& InColor, const float& TimeToDisplay)
{
	FString str = FString::SanitizeFloat(data);
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *str);
}

void UC_Util::Print(const double& data, const FColor& InColor, const float& TimeToDisplay)
{
	FString str = FString::SanitizeFloat(data);
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *str);
}

void UC_Util::Print(const FVector& data, const FColor& InColor, const float& TimeToDisplay)
{
	FString str = data.ToString();
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *str);
}

void UC_Util::Print(const FVector2D& data, const FColor& InColor, const float& TimeToDisplay)
{
	FString str = data.ToString();
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *str);
}

void UC_Util::Print(const FTransform& data, const FColor& InColor, const float& TimeToDisplay)
{
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *data.ToString());
}

void UC_Util::Print(const FRotator& data, const FColor& InColor, const float& TimeToDisplay)
{
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, *data.ToString());
}

void UC_Util::Print(AActor* Actor, const FColor& InColor, const float& TimeToDisplay)
{
	FString AddressString = FString::Printf(TEXT("%p"), Actor);
	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, InColor, AddressString);
}

void UC_Util::PrintLogMessage(const FString& str)
{
	UE_LOG(LogTemp, Log, TEXT("&s"), *str);
}

UWidgetAnimation* UC_Util::GetWidgetAnimationByName(UUserWidget* TargetUserWidget, const FName& AnimationName)
{
	UClass* WidgetClass = TargetUserWidget->GetClass();
	FProperty* Property = WidgetClass->FindPropertyByName(AnimationName);

	if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
	{
		if (ObjProp->PropertyClass->IsChildOf(UWidgetAnimation::StaticClass()))
		{
			return Cast<UWidgetAnimation>(ObjProp->GetObjectPropertyValue_InContainer(TargetUserWidget));
		}
	}

	return nullptr;
}


