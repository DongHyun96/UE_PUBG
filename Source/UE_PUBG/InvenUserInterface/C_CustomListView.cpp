// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ListView.h"

#include "InvenUserInterface/C_CustomListView.h"
#include "InvenUserInterface/C_ItemBarWidget.h"

UC_CustomListView::UC_CustomListView(const FObjectInitializer& Initializer)
	:Super(Initializer)
{
}

void UC_CustomListView::Init()
{
	EntryWidgetClass = UC_ItemBarWidget::StaticClass();

}
