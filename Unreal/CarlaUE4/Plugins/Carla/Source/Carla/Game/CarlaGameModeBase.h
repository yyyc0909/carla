// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include <compiler/disable-ue4-macros.h>
#include <boost/optional.hpp>
#include <compiler/enable-ue4-macros.h>

#include "carla/rpc/MapLayer.h"

#include "Carla/Actor/CarlaActorFactory.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Game/CarlaGameInstance.h"
#include "Carla/Game/TaggerDelegate.h"
#include "Carla/OpenDrive/OpenDrive.h"
#include "Carla/Recorder/CarlaRecorder.h"
#include "Carla/Sensor/SceneCaptureSensor.h"
#include "Carla/Settings/CarlaSettingsDelegate.h"
#include "Carla/Traffic/TrafficLightManager.h"
#include "Carla/Util/EnvironmentObject.h"
#include "Carla/Weather/Weather.h"

#include "CarlaGameModeBase.generated.h"

/// Base class for the CARLA Game Mode.
UCLASS(HideCategories=(ActorTick))
class CARLA_API ACarlaGameModeBase : public AGameModeBase
{
  GENERATED_BODY()

public:

  ACarlaGameModeBase(const FObjectInitializer& ObjectInitializer);

  const UCarlaEpisode &GetCarlaEpisode() const
  {
    check(Episode != nullptr);
    return *Episode;
  }

  const boost::optional<carla::road::Map>& GetMap() const {
    return Map;
  }

  UFUNCTION(Exec, Category = "CARLA Game Mode")
  void DebugShowSignals(bool enable);

  UFUNCTION(BlueprintCallable, Category = "CARLA Game Mode")
  ATrafficLightManager* GetTrafficLightManager();

  UFUNCTION(Category = "Carla Game Mode", BlueprintCallable, CallInEditor, Exec)
  TArray<FBoundingBox> GetAllBBsOfLevel(uint8 TagQueried = 0);

  UFUNCTION(Category = "Carla Game Mode", BlueprintCallable, CallInEditor, Exec)
  TArray<FEnvironmentObject> GetEnvironmentObjects() const
  {
    return EnvironmentObjects;
  }

  void EnableEnvironmentObjects(const TSet<uint64>& EnvObjectIds, bool Enable);

  UFUNCTION(Category = "Carla Game Mode", BlueprintCallable)
  int32 GetCurrentMapLayer() const
  {
    return CurrentMapLayer;
  }

  UFUNCTION(Category = "Carla Game Mode", BlueprintCallable)
  void SetMapLayer(int32 MapLayer)
  {
    CurrentMapLayer = MapLayer;
  }

  UFUNCTION(Category = "Carla Game Mode", BlueprintCallable, CallInEditor, Exec)
  void LoadMapLayer(int32 MapLayers);

  UFUNCTION(Category = "Carla Game Mode", BlueprintCallable, CallInEditor, Exec)
  void UnLoadMapLayer(int32 MapLayers);

protected:

  void InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage) override;

  void RestartPlayer(AController *NewPlayer) override;

  void BeginPlay() override;

  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  void Tick(float DeltaSeconds) override;

private:

  void SpawnActorFactories();

  void ParseOpenDrive(const FString &MapName);

  void RegisterEnvironmentObject();

  void ConvertMapLayerMaskToMapNames(int32 MapLayer, TArray<FName>& OutLevelNames);

  UPROPERTY()
  UCarlaGameInstance *GameInstance = nullptr;

  UPROPERTY()
  UTaggerDelegate *TaggerDelegate = nullptr;

  UPROPERTY()
  UCarlaSettingsDelegate *CarlaSettingsDelegate = nullptr;

  UPROPERTY()
  UCarlaEpisode *Episode = nullptr;

  UPROPERTY()
  ACarlaRecorder *Recorder = nullptr;

  TArray<FEnvironmentObject> EnvironmentObjects;

  /// The class of Weather to spawn.
  UPROPERTY(Category = "CARLA Game Mode", EditAnywhere)
  TSubclassOf<AWeather> WeatherClass;

  /// List of actor spawners that will be used to define and spawn the actors
  /// available in game.
  UPROPERTY(Category = "CARLA Game Mode", EditAnywhere)
  TSet<TSubclassOf<ACarlaActorFactory>> ActorFactories;

  UPROPERTY()
  TArray<ACarlaActorFactory *> ActorFactoryInstances;

  UPROPERTY()
  ATrafficLightManager* TrafficLightManager = nullptr;

  boost::optional<carla::road::Map> Map;

  UPROPERTY(Category = "CARLA Game Mode", EditAnywhere)
  int32 CurrentMapLayer = static_cast<int32>(carla::rpc::MapLayer::All);

};
