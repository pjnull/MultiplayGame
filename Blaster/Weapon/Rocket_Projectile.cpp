// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocket_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"

ARocket_Projectile::ARocket_Projectile()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));

	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
}

void ARocket_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ARocket_Projectile::OnHit);
	}


	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached
		(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);

	}

	if (ProjectileLoop && LoopingSoundAtten)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached
		(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			LoopingSoundAtten,
			(USoundConcurrency*)nullptr,
			false
		);
	}
}

void ARocket_Projectile::Destroyed()
{

}

void ARocket_Projectile::DestroyTimerFinished()
{
	Destroy();
}



void ARocket_Projectile::OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	APawn* FirePawn=GetInstigator();
	if (FirePawn&&HasAuthority())
	{
		AController* FireController = FirePawn->GetController();
		
		if (FireController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff
			(
				this,//���� ���ؽ�Ʈ ������Ʈ
				Damage,//���̽�������
				10.f,//�ּҴ����
				GetActorLocation(),
				200.f,//��� ����
				500.f,//�ٱ��� ����
				1.f,//������ ����?
				UDamageType::StaticClass(),//����� Ÿ��Ŭ����
				TArray<AActor*>(),//������ ���� 
				this,//����� ��ü
				FireController//instigator(����)
			);
		}
	}


	GetWorldTimerManager().SetTimer
	(
		DestroyTimer,
		this,
		&ARocket_Projectile::DestroyTimerFinished,
		DestroyTime
	);

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (TrailSystemComponent&& TrailSystemComponent->GetSystemInstance())
	{
		TrailSystemComponent->GetSystemInstance()->Deactivate();
	}

	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}

