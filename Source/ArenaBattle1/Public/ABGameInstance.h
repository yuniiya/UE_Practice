// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"		// 게임 진행 중에 비동기 방식으로 에셋을 로딩하는 기능을 제공하는 클래스.
#include "ABGameInstance.generated.h"

// 엑셀 데이터를 엔진으로 불러들이기 위해, 테이블 데이터의 각 열 이름과 유형이 동일한 구조체를 선언
// 첫번째 열인 Name은 자동으로 키 값으로 사용되기 때문에 선언에서 제외
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

// 게임 앱을 관리하는 클래스
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
	// UDataTable : 테이블 데이터를 관리하기 위해 언리얼 엔진이 제공하는 오브젝트
	UPROPERTY()
	class UDataTable* ABCharacterTable;


	
};
