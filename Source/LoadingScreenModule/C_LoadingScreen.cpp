// Fill out your copyright notice in the Description page of Project Settings.


#include "C_LoadingScreen.h"
#include "SlateOptMacros.h"
#include "SlateExtras.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SC_LoadingScreen::Construct(const FArguments& InArgs)
{
	// Store texture from arguments
	BackgroundTexture = InArgs._BackgroundTexture;

	// Create brush for rendering background
	BackgroundBrush = MakeShareable(new FSlateBrush);
	BackgroundBrush->SetResourceObject(BackgroundTexture);

	// Begin constructing the widget hierarchy
	// ChildSlot is the root slot of our compound widget
	ChildSlot
	[
		SNew(SOverlay)
		// Add first slot to the overlay for the background image
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			// Create an image widget using our background brush
			SNew(SImage)
				.Image(BackgroundTexture ? BackgroundBrush.Get() : nullptr)
				.ColorAndOpacity(FLinearColor::White)
		]
		// Add Second slot to the overlay for the loading indicator
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		.Padding(10.f)
		[
			// Create an image widget using our background brush
			SNew(SThrobber)
				.Visibility(EVisibility::HitTestInvisible)	// Visible but doesn't block clicks
				.NumPieces(10)								// Number of spinning pieces
		]
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
