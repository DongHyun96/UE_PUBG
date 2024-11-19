// Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_BasicUserWidget.h"
#include "Utility/C_Util.h"

void UC_BasicUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (AC_BasicCharacter* PlayerCharacter = Cast<AC_BasicCharacter>(PlayerController->GetPawn()))
        {
          //  OwnerCharacter = PlayerCharacter;
        }
    }

    // OwnerCharacter가 정상적으로 설정되었는지 확인
    if (!OwnerCharacter)
    {
        UC_Util::Print("No OwnerCharacter");
    }
}
