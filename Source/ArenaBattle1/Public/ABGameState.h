// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

// 플레이어 데이터 + 게임 데이터를 관리하기 위한 클래스
// 싱글 플레이의 경우 플레이어 스테이트에서 모두 관리해도 무방하지만, 멀티 플레이의 경우 게임 스테이트를 이용해 데이터를 관리해야 함.
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AABGameState();

public:
	int32 GetTotalGameScore() const;
	void AddGameScore();

	// 게임 미션을 달성했는지 여부는 게임의 정보이므로 GameState에 구현
	void SetGameCleared();
	bool IsGameCleared() const;

private:
	UPROPERTY(Transient)
	int32 TotalGameScore;
	
	UPROPERTY(Transient)
	bool bGameCleared;
};
