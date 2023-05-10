// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/PlayerController.h"
#include "ABUIPlayerController.generated.h"

// Ÿ��Ʋ �������� UI�� ���� ����� �ô� �÷��̾� ��Ʈ�ѷ�
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API AABUIPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// UI�� Ŭ���� ���� �����Ϳ��� ������ �� �ֵ��� EditDefaultsOnly
	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIWidgetClass;

	UPROPERTY()
	class UUserWidget* UIWidgetInstance;

};
