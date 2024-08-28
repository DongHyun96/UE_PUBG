// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/ThrowingWeapon/C_ScreenShotWidget.h"
#include "Components/Image.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Utility/C_Util.h"

void UC_ScreenShotWidget::DisplayCapturedImage(UTextureRenderTarget2D* RenderTarget)
{
	//if (!RenderTarget) return;

 //   //UTexture2D* Texture = RenderTarget->CreateTransient(RenderTarget->SizeX, RenderTarget->SizeY);

 //   UTexture2D* Texture{};

 //   FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
 //   TArray<FColor> Bitmap;
 //   RenderTargetResource->ReadPixels(Bitmap);

 //   // Create a new texture from the bitmap
 //   FTexture2DMipMap* MipMap = &Texture->PlatformData->Mips[0];
 //   void* Data = MipMap->BulkData.GetAllocation();
 //   FMemory::Memcpy(Data, Bitmap.GetData(), Bitmap.Num() * sizeof(FColor));
 //   Texture->UpdateResource();

 //   SetImageTexture(Texture);
}

void UC_ScreenShotWidget::SetImageTexture(UTexture2D* NewTexture)
{
	if (!DisplayImage)
	{
		UC_Util::Print("From UC_ScreenShotWidget::SetImageTexture : Display Image is nullptr!", FColor::Red, 5.f);
		return;
	}
	DisplayImage->SetBrushFromTexture(NewTexture);
}
