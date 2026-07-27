// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tools/UAssetEditor.h"
#include "AbilityRouterGraphAssetEditor.generated.h"

class UOverdriveAbilityRouterGraph;

/**
 * 
 */
UCLASS()
class UAbilityRouterGraphAssetEditor : public UAssetEditor
{
	GENERATED_BODY()
	
public:
	void Initialize(UOverdriveAbilityRouterGraph* InRouterGraph);

	UOverdriveAbilityRouterGraph* GetAbilityRouterGraph() const { return AbilityRouterGraph; }

public:

	// UAssetEditor interface
	virtual void GetObjectsToEdit(TArray<UObject*>& InObjectsToEdit) override;
	virtual TSharedPtr<FBaseAssetToolkit> CreateToolkit() override;

private:

	UPROPERTY()
	TObjectPtr<UOverdriveAbilityRouterGraph> AbilityRouterGraph;

};
