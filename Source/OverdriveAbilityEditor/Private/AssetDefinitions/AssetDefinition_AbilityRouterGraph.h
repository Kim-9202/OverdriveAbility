// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "AssetDefinition_AbilityRouterGraph.generated.h"

/**
 * 
 */
UCLASS()
class OVERDRIVEABILITYEDITOR_API UAssetDefinition_AbilityRouterGraph : public UAssetDefinitionDefault
{
	GENERATED_BODY()
	
public:
	UAssetDefinition_AbilityRouterGraph();

	virtual FText GetAssetDisplayName() const override;

	virtual FLinearColor GetAssetColor() const override { return FLinearColor(FColor(201, 29, 85)); }

	virtual TSoftClassPtr<UObject> GetAssetClass() const override;

	virtual EAssetCommandResult PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const override;

	virtual bool CanMerge() const override { return true; }
	virtual EAssetCommandResult Merge(const FAssetAutomaticMergeArgs& MergeArgs) const override;
	virtual EAssetCommandResult Merge(const FAssetManualMergeArgs& MergeArgs) const override;

	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;

	virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
};
