// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/SaveGame.h"
#include "ABSaveGame.generated.h"

// SaveGame : ���� �����͸� �����ϰ� �ҷ����̴� ����� �����ϴ� �𸮾� ������Ʈ
// ����ڰ� ������ SaveGame Ŭ������ �𸮾��� SaveGame �ý��ۿ� �Ѱ��ָ� ���� �������� ����� �ε��� ������ �� ����. 
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
