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
	AABPlayerController();

	// 해당 컴포넌트가 생성되는 시점에 호출
	virtual void PostInitializeComponents() override;

	// 플레이어 컨트롤러가 폰에 빙의를 진행하는 시점에 호출
	virtual void OnPossess(APawn* InPawn) override;

	class UABHUDWidget* GetHUDWidget() const;
	void NPCKill(class AABCharacter* KilledNPC) const;

	void AddGameScore() const;
	void ChangeInputMode(bool bGameMode = true);

	void ShowResultUI();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	// TSubclassOf : UClass 유형의 안정성을 보장해는 템플릿 클래스. 
	// EditDefaultsOnly: 인스턴스가 공통으로 가져야 할 값의 편집이 가능하도록 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UABGameplayResultWidget> ResultWidgetClass;

private:
	void OnGamePause();

private:
	UPROPERTY()
	class UABHUDWidget* HUDWidget;

	UPROPERTY()
	class AABPlayerState* ABPlayerState;

	UPROPERTY()
	class UABGameplayWidget* MenuWidget;

	UPROPERTY()
	class UABGameplayResultWidget* ResultWidget;

	// FInputMode ~ : 사용자의 입력 모드를 설정하는데 사용되는 구조체
	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;

	// 플레이어 컨트롤러 쪽에서 특정 입력을 처리하는 코드를 구현하면 해당 입력은 폰에 더이상 전달되지 않음. 
//protected:
//	virtual void SetupInputComponent() override;
//	void LeftRight(float NewAxisValue);
};
