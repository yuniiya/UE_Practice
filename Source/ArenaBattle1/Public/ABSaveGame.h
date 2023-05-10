// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/SaveGame.h"
#include "ABSaveGame.generated.h"

// SaveGame : 게임 데이터를 저장하고 불러들이는 기능을 제공하는 언리얼 오브젝트
// 사용자가 설계한 SaveGame 클래스를 언리얼의 SaveGame 시스템에 넘겨주면 게임 데이터의 저장과 로딩을 구현할 수 있음. 
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UABSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UABSaveGame();

	UPROPERTY()
	int32 Level;

	UPROPERTY()
	int32 Exp;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	int32 HighScore;

	UPROPERTY()
	int32 CharacterIndex;
};
