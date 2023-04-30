// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Fountain.generated.h"

UCLASS()
class ARENABATTLE1_API AFountain : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Body;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Water;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* Light;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* Splash;

	UPROPERTY(EditAnywhere, Category = "ID")
	int32 ID;

	// 무브먼트 컴포넌트를 이용한다면 Tick함수가 없어도 움직임 구현 가능
	// 엑터 컴포넌트에 속함. (트랜스폼 정보가 없고, 기능만 제공하는 컴포넌트)
	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Movement;

private:
	// 기본적으로 언리얼 에디터에는 private로 접근이 제한 된 변수 값 변경이 불가능
	// 매크로에 AllowPrivateAccess 라는 META 키워드를 추가하면 편집과 동시에 캡슐화를 지킬 수 있음.
	UPROPERTY(EditAnywhere, Category = "Stat", Meta = (AllowPrivateAccess = true))
	float RotateSpeed;
};
