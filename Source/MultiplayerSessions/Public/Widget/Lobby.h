// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Lobby.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API ULobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetupMenu();
	
	UFUNCTION(BlueprintCallable)
	void TearDownLobbyMenu();
	
	virtual bool Initialize() override;
	
private:
	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartSessionButton;
	
	UFUNCTION()
	void StartSessionButtonClicked();
	
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
};
