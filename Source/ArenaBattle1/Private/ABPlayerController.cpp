// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn* InPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(InPawn);
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 플레이 시, UI를 배제하고 게임에게만 입력 전달 
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}
