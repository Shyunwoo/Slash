// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Slash/Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;
	virtual void Attack();
	virtual void Die();

	void PlayHitReactMontage(const FName& SectionName);
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack();

protected:
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category=Montages)
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category=Montages)
	class UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category=Montages)
	class UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere)
	class UAttributeComponent* Attributes;

	UPROPERTY(EditAnywhere, Category=VisualEffects)
	UParticleSystem* HitParticles;
};
