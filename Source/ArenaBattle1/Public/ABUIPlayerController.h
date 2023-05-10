// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/PlayerController.h"
#include "ABUIPlayerController.generated.h"

// 타이틀 레벨에서 UI를 띄우는 기능을 맡는 플레이어 컨트롤러
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API AABUIPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// UI의 클래스 값을 에디터에서 설정할 수 있도록 EditDefaultsOnly
	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIWidgetClass;

	UPROPERTY()
	class UUserWidget* UIWidgetInstance;

};
