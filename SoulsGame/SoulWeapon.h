// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "SoulWeapon.generated.h"


class UBoxComponent;
/**
 * 
 */
UCLASS()
class SOULSGAME_API ASoulWeapon : public AItem
{
	GENERATED_BODY()

public:
	ASoulWeapon();
	void EquipItem(USceneComponent* InParent, FName InSocket, AActor* NewOwner, APawn* NewInstigator);

	TArray<AActor*> IgnoreActors;
	
protected:
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* SwordBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 2.f;

public:
	FORCEINLINE UBoxComponent* GetSwordBox() const { return SwordBox; }
};
