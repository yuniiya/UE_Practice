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
	// �ݸ��� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Box)
	UBoxComponent* Trigger;

	// ����ƽ �޽� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Box)
	UStaticMeshComponent* Box;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* Effect;

	// TSubclassOf : UClass�� ���� Ŭ������ ������ �����ϴ� ������ ������ �� ���������, Ư�� Ŭ������ ��ӹ��� Ŭ������� ����� �����ϴ� Ű����
	// UClass : ���� ������Ʈ�� ����ϴ� ��� �𸮾� ������Ʈ�� ������ ������. 
	UPROPERTY(EditInstanceOnly, Category = Box)
	TSubclassOf<class AABWeapon> WeaponItemClass;

private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherCom, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// OnSystemFinished ��������Ʈ�� ���̳��� �����̱� ������, ���ε��� ��� �Լ��� UFUNCTION() ��ũ�� ���� 
	UFUNCTION()
	void OnEffectFinished(class UParticleSystemComponent* PSystem);
};
