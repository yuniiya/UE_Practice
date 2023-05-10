// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

// �÷��̾� ������ + ���� �����͸� �����ϱ� ���� Ŭ����
// �̱� �÷����� ��� �÷��̾� ������Ʈ���� ��� �����ص� ����������, ��Ƽ �÷����� ��� ���� ������Ʈ�� �̿��� �����͸� �����ؾ� ��.
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

	// ���� �̼��� �޼��ߴ��� ���δ� ������ �����̹Ƿ� GameState�� ����
	void SetGameCleared();
	bool IsGameCleared() const;

private:
	UPROPERTY(Transient)
	int32 TotalGameScore;
	
	UPROPERTY(Transient)
	bool bGameCleared;
};
