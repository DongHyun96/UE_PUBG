// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Component/C_InvenComponent.h"
#include "Character/C_BasicCharacter.h"
#include "Item/C_Item.h"

// Sets default values for this component's properties
UC_InvenComponent::UC_InvenComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UC_InvenComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UC_InvenComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UC_InvenComponent::CheckVolume(uint8 volume)
{
	if (MaxVolume < CurVolume + volume)
	{
		return false;
	}

	return true;
}
/// <summary>
/// Holster�� nullptr�̸� maxVolume+= volume�� ���ش�.
/// Holster�� ture�� ������ ������ ��. 
/// �ٲٷ��� ������ ������ ������ ���������� vulume�� ���ο� ������ volume���� ����ŭ�� character�� maxVulume�� �����ش�. maxVolume += (�������� volume - ���������� volume);�� ���ش�.
/// �ٲٷ��� ������ ������ ������ testVolume = �������� volume - ���������� volume;
/// if (maxVolume - testVolume > curVolume)
/// {
///		//������ �ٲپ���.
/// }
/// else
/// {
///		//������ �ٲ��� ����.
/// }
/// </summary>
void UC_InvenComponent::ChackMyBackPack()
{

}
/// <summary>
/// Bag �����ۿ��� ��ȣ�ۿ�(interaction)���� ��� ó���ұ�?
/// Bag���� Character�� BagLevel�� ��ȭ ��Ű�����Ѵ�.
/// �̶� Bag�� Interactertion���� 
/// </summary>


/// <summary>
/// ��ȣ�ۿ�(F)�Լ� �������� ����.
/// wilditem�� map�� �������ִ� item�� �ǹ�.
/// CheckVolume���� �� �κ��丮�� ������ üũ�ϰ� �������� ���� �� �ִ��� Ȯ��.
/// true�� �ʿ��� �ش� object�� �����ϰ� Player�� �����ϵ��� �ϰ��� �Ѵ�.
/// AddActorComponentReplicatedSubObject ���� �� �Լ��� ������� �ۿ��ϰ� ����� �ִ��� �� �𸣰���.
/// �׷��ٸ� ��ȣ�ۿ��� object���� ������ ����� �÷��̾�� �� �Լ��� �۵���Ű�� ������� ���� �� �� ����.
/// </summary>
/// <param name="wilditem"></param>
void UC_InvenComponent::Interaction(AC_Item wilditem)
{
	//AC_BasicCharacter* player = OwnerCharacter;
	
	wilditem.Interaction(OwnerCharacter);

	if (CheckVolume(wilditem.GetVolume()))
	{
		//��ȣ�ۿ�� �������� ���Ը� ���ص� �����ѵ��� ���� �ʴ� ���.
		CurVolume += wilditem.GetVolume();
		//������ ����ȭ�ϴµ� �ַ� ����Ѵٰ���. ������ Ŭ���̾�Ʈ���� ������Ʈ ���¸� ����ȭ�ϰ�, �����Ѵ�.
		wilditem.AddActorComponentReplicatedSubObject(this,&wilditem);
	}
	else
	{
		//��ȣ�ۿ�� �������� ���Ը� ������ ��, �����ѵ��� �Ѵ� ���.

		//print("������ �����մϴ�"); �� ���� ��Ʈ�� ��������
		return;
	}
}


 