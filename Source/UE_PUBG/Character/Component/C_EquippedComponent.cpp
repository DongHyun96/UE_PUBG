// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_EquippedComponent.h"
#include "Character/C_BasicCharacter.h"

#include "Item/Weapon/C_Weapon.h"
#include "Item/Weapon/Gun/C_Gun.h"
#include "Item/Weapon/Gun/C_AR.h"
#include "Item/Weapon/MeleeWeapon/C_MeleeWeapon.h"
#include "Item/Weapon/ThrowingWeapon/C_ThrowingWeapon.h"

#include "Utility/C_Util.h"

// Sets default values for this component's properties
UC_EquippedComponent::UC_EquippedComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}


// Called when the game starts
void UC_EquippedComponent::BeginPlay()
{
    Super::BeginPlay();

    //SpawnWeaponsForTesting();
}


// Called every frame
void UC_EquippedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

AC_Weapon* UC_EquippedComponent::SetSlotWeapon(EWeaponSlot InSlot, AC_Weapon* Weapon)
{
    AC_Weapon* PrevSlotWeapon = Weapons[InSlot];

    // ���� ������ ���� ���Ⱑ ������ �� ���� ���� ����
    if (PrevSlotWeapon)
    {
        // ���� ��� �ִ� ������ slot�� ���ο� ����� �ٲ������ �� ��
        if (GetCurWeapon() == PrevSlotWeapon)
        {
            OwnerCharacter->SetHandState(EHandState::UNARMED);
            if (!Weapon) CurWeaponType = EWeaponSlot::NONE;
        }

        // ���� ���� ������ ���� PoseTransitionEnd ��������Ʈ ����
        OwnerCharacter->Delegate_OnPoseTransitionFin.RemoveAll(PrevSlotWeapon);

        //C_Item�� detachment���� ó����, Ȥ�ø��� ���ܵ�.
        //PrevSlotWeapon->SetOwnerCharacter(nullptr);
    }
    
    Weapons[InSlot] = Weapon; // ���� ���� ����� ��ü

    if (!Weapons[InSlot]) return PrevSlotWeapon; // Slot�� ���� ������ ���Ⱑ nullptr -> early return
    

    SetMainGunOrSubGun(InSlot);
        

    Weapons[InSlot]->SetOwnerCharacter(OwnerCharacter); // ���ο� OwnerCharacter ����

    // Attach to Holster �ϱ� ���� Local transform �ʱ�ȭ
    //Weapons[InSlot]->SetActorRelativeTransform(FTransform::Identity);
 
    Weapons[InSlot]->SetRelativeTranformToInitial();
    Weapons[InSlot]->AttachToHolster(OwnerCharacter->GetMesh());

    // ���� ������ ���⿡ ���� PoseTransitionEnd ��������Ʈ callback �ɱ�
    OwnerCharacter->Delegate_OnPoseTransitionFin.AddUObject(Weapons[InSlot], &AC_Weapon::OnOwnerCharacterPoseTransitionFin);

    return PrevSlotWeapon;
}


void UC_EquippedComponent::DetachmentWeapon(EWeaponSlot InSlot)
{
    AC_Weapon* curWeapon = Weapons[InSlot];
    
    //�������� ���ٸ� ����
    if (!curWeapon) return;
    
    curWeapon->SetOwnerCharacter(nullptr);

    curWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

    curWeapon->SetActorLocation(OwnerCharacter->GetActorLocation() + FVector(0.f, 0.f, -75.f));
    curWeapon->SetActorRotation(FRotator(0.f, 0.f, -90.f));
}

bool UC_EquippedComponent::ChangeCurWeapon(EWeaponSlot InChangeTo)
{
    if (IsValid(Weapons[CurWeaponType]))
    {
        // ���� ������ Sheath�� Draw animation montage�� �̹� ��� ���̶�� return
        if (OwnerCharacter->GetMesh()->GetAnimInstance()
            ->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurDrawMontage().AnimMontage))
            return false;

        if (OwnerCharacter->GetMesh()->GetAnimInstance()
            ->Montage_IsPlaying(Weapons[CurWeaponType]->GetCurSheathMontage().AnimMontage))
            return false;
    }

    NextWeaponType = InChangeTo;

    if (CurWeaponType == NextWeaponType) return false; // ���� ����� ���� ���Ⱑ ���� �� ���⸦ ���� �ٽ� ������ ����

    // NextWeaponType�� None�� �ƴϰ�, �ٲٷ��� ���� ���Կ� ���Ⱑ ���� ��
    if (NextWeaponType != EWeaponSlot::NONE && !IsValid(Weapons[NextWeaponType])) return false;

    // ���� ���⸦ ���������� ���� �� (UnArmed ����), �Ǵ� ���� ���Կ� ������ ���Ⱑ ���� �� ���� ���� Draw�� ���
    if (CurWeaponType == EWeaponSlot::NONE || !IsValid(GetCurWeapon()))
    {
        // ���� ������ �ٲ� ���Ⱑ None�̰ų� ������ �ٲ� ���� ���Կ� ���Ⱑ ���� �� 
        if (NextWeaponType == EWeaponSlot::NONE || !IsValid(Weapons[NextWeaponType]))
        {
            CurWeaponType = EWeaponSlot::NONE;
            NextWeaponType = EWeaponSlot::NONE;
            OwnerCharacter->SetHandState(EHandState::UNARMED);
            return false;
        }

        // ���� ���Ⱑ ���� ��
        OwnerCharacter->PlayAnimMontage(Weapons[NextWeaponType]->GetCurDrawMontage());
        return true;
    }

    // ���� ���⸦ �������� ��Ȳ

    // ��ô�� ����ó��
    if (CurWeaponType == EWeaponSlot::THROWABLE_WEAPON)
    {
        AC_ThrowingWeapon* ThrowingWeapon = Cast<AC_ThrowingWeapon>(GetCurWeapon());
        if (IsValid(ThrowingWeapon))
        {
            // �̹� ��ŷ�� ���۵Ǿ���, ���� �տ��� ������ ���� ��ô����� ���� ����
            if (ThrowingWeapon->GetIsCooked() && ThrowingWeapon->GetAttachParentActor())
                return ThrowingWeapon->ReleaseOnGround();
        }
    }
    //���� ��� Aiming ���� �� ī�޶� �ٽ� ������� ��ȯ
    if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
    {
        AC_Gun* TempWeapon = Cast<AC_Gun>(Weapons[CurWeaponType]);
        if (IsValid(TempWeapon))
            TempWeapon->BackToMainCamera();
    }

    OwnerCharacter->PlayAnimMontage(Weapons[CurWeaponType]->GetCurSheathMontage()); // �� ���� ����ִ� ���ۿ� Notify�Լ� �ɾ ���� ����� ��ȯ
    return true;
}

bool UC_EquippedComponent::ToggleArmed()
{
    // ���� ���⵵ �������� �ʾҰ� ������ ��� �־��� ���⵵ ���� �� (�ʱ� ����)
    if (CurWeaponType == EWeaponSlot::NONE && PrevWeaponType == EWeaponSlot::NONE) return false;

    // ���� ��� �ִ� ���Ⱑ ���� ��
    if (CurWeaponType != EWeaponSlot::NONE && IsValid(GetCurWeapon()))
    {
        PrevWeaponType = CurWeaponType;


        return ChangeCurWeapon(EWeaponSlot::NONE);
    }

    // ���� ��� �ִ� ���Ⱑ ���� ��
    return ChangeCurWeapon(PrevWeaponType);
}

void UC_EquippedComponent::OnSheathEnd()
{
    // ���⸦ �ٲٴ� ���߿� SlotWeapon ���� ���� ���� ó��
    if (!GetCurWeapon())
    {
        CurWeaponType = NextWeaponType;

        if (!IsValid(GetCurWeapon())) // �������� �ٲ� ���Ⱑ NONE�� ��(UNARMED�� ��)
        {
            OwnerCharacter->SetHandState(EHandState::UNARMED);
            CurWeaponType   = EWeaponSlot::NONE;
            NextWeaponType  = EWeaponSlot::NONE;
            return;
        }

        OwnerCharacter->PlayAnimMontage(GetCurWeapon()->GetCurDrawMontage());
        return;
    }

    // ���� ���� �������� ���̱�
    GetCurWeapon()->AttachToHolster(OwnerCharacter->GetMesh());

    // �ѱ�� ����ó��
    if (CurWeaponType == EWeaponSlot::MAIN_GUN || CurWeaponType == EWeaponSlot::SUB_GUN)
    {
        AC_Gun* TempWeapon = Cast<AC_Gun>(GetCurWeapon());
        if (IsValid(TempWeapon))
            TempWeapon->SetIsAimPress(false);
    }

    CurWeaponType = NextWeaponType;

    if (!IsValid(GetCurWeapon()))
    {
        OwnerCharacter->SetHandState(EHandState::UNARMED);
        return;
    }

    OwnerCharacter->PlayAnimMontage(GetCurWeapon()->GetCurDrawMontage());
}

void UC_EquippedComponent::OnDrawStart()
{
    // ���⸦ �ٲٴ� ���߿� SlotWeapon ���� ���� ���� ó�� -> �ٲ�� ���Ⱑ ������� ��
    if (!Weapons[NextWeaponType])
    {
        //OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, GetCurWeapon()->GetCurDrawMontage().AnimMontage);
        NextWeaponType  = EWeaponSlot::NONE;
        CurWeaponType   = EWeaponSlot::NONE;
        OwnerCharacter->SetHandState(EHandState::UNARMED);
        return;
    }

    Weapons[NextWeaponType]->AttachToHand(OwnerCharacter->GetMesh());
    //GetCurWeapon()->AttachToHand(OwnerCharacter->GetMesh());
}   

void UC_EquippedComponent::OnDrawEnd()
{
    if (NextWeaponType == EWeaponSlot::NONE) return;

    // ���⸦ �ٲٴ� ���߿� SlotWeapon ���� ���� ���� ó�� -> �ٲ�� ���Ⱑ ������� ��
    if (!Weapons[NextWeaponType])
    {
        //OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, GetCurWeapon()->GetCurDrawMontage().AnimMontage);
        NextWeaponType  = EWeaponSlot::NONE;
        CurWeaponType   = EWeaponSlot::NONE;
        OwnerCharacter->SetHandState(EHandState::UNARMED);
        return;
    }

    UC_Util::Print("OnDrawEnd", FColor::Cyan, 5.f);

    Weapons[NextWeaponType]->AttachToHand(OwnerCharacter->GetMesh());
    CurWeaponType = NextWeaponType;
    NextWeaponType = EWeaponSlot::NONE;
}

void UC_EquippedComponent::SpawnWeaponsForTesting()
{
    // Test�� weapon spawn��
    FActorSpawnParameters Param{};
    Param.Owner = OwnerCharacter;
    AC_MeleeWeapon* MeleeTemp = GetWorld()->SpawnActor<AC_MeleeWeapon>(WeaponClasses[EWeaponSlot::MELEE_WEAPON], Param);
    MeleeTemp->SetOwnerCharacter(OwnerCharacter);
    MeleeTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::MELEE_WEAPON, MeleeTemp);

    FActorSpawnParameters Param2{};
    Param2.Owner = OwnerCharacter;
    AC_Gun* ARTemp = GetWorld()->SpawnActor<AC_Gun>(WeaponClasses[EWeaponSlot::MAIN_GUN], Param2);
    ARTemp->SetOwnerCharacter(OwnerCharacter);
    ARTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::MAIN_GUN, ARTemp);

    FActorSpawnParameters Param3{};
    Param3.Owner = OwnerCharacter;
    AC_ThrowingWeapon* ThrowTemp = GetWorld()->SpawnActor<AC_ThrowingWeapon>(WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], Param3);
    ThrowTemp->SetOwnerCharacter(OwnerCharacter);
    ThrowTemp->AttachToHolster(OwnerCharacter->GetMesh());

    SetSlotWeapon(EWeaponSlot::THROWABLE_WEAPON, ThrowTemp);

    AC_ThrowingWeapon::InitTestPool(OwnerCharacter, WeaponClasses[EWeaponSlot::THROWABLE_WEAPON], this);
}

void UC_EquippedComponent::SetMainGunOrSubGun(EWeaponSlot InSlot)
{
    if (InSlot == EWeaponSlot::MAIN_GUN || InSlot == EWeaponSlot::SUB_GUN)
    {
        AC_Gun* CurrentGun = Cast<AC_Gun>(Weapons[InSlot]);
        if (IsValid(CurrentGun))
        {
            if (InSlot == EWeaponSlot::MAIN_GUN)
                CurrentGun->SetMainOrSubSlot(EGunState::MAIN_GUN);
            else
                CurrentGun->SetMainOrSubSlot(EGunState::SUB_GUN);

        }
    }
}


