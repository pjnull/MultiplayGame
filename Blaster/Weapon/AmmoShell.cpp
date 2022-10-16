// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoShell.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
AAmmoShell::AAmmoShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AmmoShell = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shell Mesh"));

	SetRootComponent(AmmoShell);
	
	AmmoShell->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
	AmmoShell->SetSimulatePhysics(true);
	AmmoShell->SetEnableGravity(true);
	AmmoShell->SetNotifyRigidBodyCollision(true);


	ShellEjectionImpulse = 10.f;

}

// Called when the game starts or when spawned
void AAmmoShell::BeginPlay()
{
	Super::BeginPlay();
	
	FVector Rand=(FMath::VRand());
	AmmoShell->OnComponentHit.AddDynamic(this,&AAmmoShell::OnHit);

	AmmoShell->AddImpulse(Rand*GetActorForwardVector()* ShellEjectionImpulse);
	
	SetLifeSpan(0.5f);


}

// Called every frame
void AAmmoShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AAmmoShell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	
	
}



