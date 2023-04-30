// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE1_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// 해당 컴포넌트가 생성되는 시점에 호출
	virtual void PostInitializeComponents() override;

	// 플레이어 컨트롤러가 폰에 빙의를 진행하는 시점에 호출
	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

	// 플레이어 컨트롤러 쪽에서 특정 입력을 처리하는 코드를 구현하면 해당 입력은 폰에 더이상 전달되지 않음. 
//protected:
//	virtual void SetupInputComponent() override;
//	void LeftRight(float NewAxisValue);
};
