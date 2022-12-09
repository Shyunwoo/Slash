// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Slash/Interfaces/HitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
protected:
	virtual void BeginPlay() override;

	void PlayHitReactMontage(const FName& SectionName);

public:	
	

private:
	UPROPERTY(EditDefaultsOnly, Category=Montages)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category=VisualEffects)
	UParticleSystem* HitParticles;

};
