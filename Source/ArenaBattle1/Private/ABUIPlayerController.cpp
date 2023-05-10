// Fill out your copyright notice in the Description page of Project Settings.


#include "ABUIPlayerController.h"
#include "Blueprint/UserWidget.h"

void AABUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ABCHECK(nullptr != UIWidgetClass);

	// ������ �����ϸ� �����Ϳ��� ������ Ŭ�����κ��� UI �ν��Ͻ��� �����ϰ�, �̸� ����Ʈ�� ��� �� �Է��� UI���� ���޵ǵ��� ��. 
	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
	ABCHECK(nullptr != UIWidgetInstance);

	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	bShowMouseCursor = true;
}
