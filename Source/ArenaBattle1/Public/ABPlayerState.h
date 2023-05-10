// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

// �÷��̾� �����Ͱ� ����� �� HUD�� ��ȣ�� ���� HUD�� ���� UI ������ ������Ʈ �ϵ��� �ϴ� ��������Ʈ. 
DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);

// �÷��̾��� ������ �����ϴ� Ŭ����
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
	// ���� ���̺� ��ɿ��� �� ���� ���Ͽ� ������ �� �ִ� ���� �̸��� ���� �̸��� �ʿ�. ���� �̸��� �ٸ��� ������ ���̺� �����͸� ������ ���� �� ����.
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
