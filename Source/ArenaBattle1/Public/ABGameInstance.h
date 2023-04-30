// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"		// ���� ���� �߿� �񵿱� ������� ������ �ε��ϴ� ����� �����ϴ� Ŭ����.
#include "ABGameInstance.generated.h"

// ���� �����͸� �������� �ҷ����̱� ����, ���̺� �������� �� �� �̸��� ������ ������ ����ü�� ����
// ù��° ���� Name�� �ڵ����� Ű ������ ���Ǳ� ������ ���𿡼� ����
USTRUCT(BlueprintType)
struct FABCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterData()
		: Level(1)
		, MaxHP(100.f)
		, Attack(10.f)
		, DropExp(10)
		, NextExp(30)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	int32 DropExp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	int32 NextExp;



};

// ���� ���� �����ϴ� Ŭ����
/**
 * 
 */
UCLASS()
class ARENABATTLE1_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
		
public:
	UABGameInstance();

	virtual void Init() override;
	FABCharacterData* GetABCharacterData(int32 Level);

	FStreamableManager StreamableManager;

private:
	// UDataTable : ���̺� �����͸� �����ϱ� ���� �𸮾� ������ �����ϴ� ������Ʈ
	UPROPERTY()
	class UDataTable* ABCharacterTable;


	
};
