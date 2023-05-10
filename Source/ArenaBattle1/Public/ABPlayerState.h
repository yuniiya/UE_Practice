// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

// 플레이어 데이터가 변결될 때 HUD에 신호를 보내 HUD가 관련 UI 위젯을 업데이트 하도록 하는 델리게이트. 
DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);

// 플레이어의 정보를 관리하는 클래스
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AABPlayerState();

	int32 GetGameScore() const;
	int32 GetGameHighScore() const;
	int32 GetCharacterLevel() const;
	int32 GetCharacterIndex() const;
	float GetExpRatio() const;

	bool AddExp(int32 IncomeExp);
	void AddGameScore();

	void InitPlayerData();
	void SavePlayerData();

	FOnPlayerStateChangedDelegate OnPlayerStateChanged;

public:
	// 게임 세이브 기능에는 각 저장 파일에 접근할 수 있는 고유 이름인 슬롯 이름이 필요. 슬롯 이름을 다르게 지정해 세이브 데이터를 여러개 만들 수 있음.
	FString SaveSlotName;

protected:
	UPROPERTY(Transient)
	int32 GameScore;

	UPROPERTY(Transient)
	int32 CharacterLevel;

	UPROPERTY(Transient)
	int32 Exp;

	UPROPERTY()
	int32 GameHighScore;

	UPROPERTY(Transient)
	int32 CharacterIndex;

private:
	void SetCharacterLevel(int32 NewCharacterLevel);
	struct FABCharacterData* CurrentStatData;
};
