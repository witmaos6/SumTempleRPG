// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


void APaladinPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);

		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if(WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);

		if(EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}

		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	if(WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);

		if(PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if(WSkillGage)
	{
		SkillGage = CreateWidget<UUserWidget>(this, WSkillGage);

		if(SkillGage)
		{
			SkillGage->AddToViewport();
			SkillGage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void APaladinPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(EnemyHealthBar)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 25.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void APaladinPlayerController::DisplayEnemyHealthBar()
{
	if(EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void APaladinPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APaladinPlayerController::DisplayPauseMenu_Implementation()
{
	if(PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);

		bShowMouseCursor = true;

		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

void APaladinPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameModeOnly();

		bShowMouseCursor = false;

		bPauseMenuVisible = false;

		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}
}

void APaladinPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}

void APaladinPlayerController::DisplaySkillGage()
{
	if(SkillGage)
	{
		SkillGage->SetVisibility(ESlateVisibility::Visible);
	}
}

void APaladinPlayerController::RemoveSkillGage()
{
	if(SkillGage)
	{
		SkillGage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APaladinPlayerController::TogglePauseMenu()
{
	if(bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}
