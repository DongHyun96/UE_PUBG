 // Fill out your copyright notice in the Description page of Project Settings.


#include "InvenUserInterface/C_DragDropOperation.h"
#include "Item/C_Item.h"
#include "Item/C_ItemBox.h"
void UC_DragDropOperation::SetDraggedItemBox(AC_Item* InItem)
{
	DraggedItem = InItem;

	UC_ItemBox* CreateItemBox = NewObject<UC_ItemBox>();
	CreateItemBox->Init(InItem, InItem->GetItemCurStack());

	DraggedItemBox = CreateItemBox;
}

void UC_DragDropOperation::SetDraggedItemBox(UC_ItemBox* InItemBox)
{
	DraggedItem = InItemBox->GetItemRef();

	DraggedItemBox = InItemBox;
}
