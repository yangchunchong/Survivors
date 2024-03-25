// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SurvivorLeaderboard.h"
#include "SurvivorGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVORS_API USurvivorGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init();

};
