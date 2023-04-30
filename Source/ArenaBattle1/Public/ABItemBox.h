// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle1.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

UCLASS()
class ARENABATTLE1_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABItemBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 콜리전 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;

	// 스테틱 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* Effect;

	// TSubclassOf : UClass와 같이 클래스의 정보를 저장하는 변수를 선언할 때 사용하지만, 특정 클래스와 상속받은 클래스들로 목록을 한정하는 키워드
	// UClass : 현재 프로젝트에 사용하는 모든 언리얼 오브젝트의 선언을 가져옴. 
	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon> WeaponItemClass;

private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// OnSystemFinished 델리게이트는 다이나믹 형식이기 때문에, 바인딩할 멤버 함수에 UFUNCTION() 매크로 선언 
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);
};
