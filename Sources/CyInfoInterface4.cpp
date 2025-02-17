#include "CvGameCoreDLL.h"

//
// Python interface for info classes (formerly structs)
// These are simple enough to be exposed directly - no wrappers
//

void CyInfoPythonInterface4()
{
	OutputDebugString("Python Extension Module - CyInfoPythonInterface4\n");
	
	python::class_<CvQuestInfo, python::bases<CvInfoBase> >("CvQuestInfo")
		.def("getQuestMessages", &CvQuestInfo::getQuestMessages, "int ()")
		.def("getNumQuestLinks", &CvQuestInfo::getNumQuestLinks, "int ()")
		.def("getNumQuestSounds", &CvQuestInfo::getNumQuestSounds, "int ()")

		.def("getQuestObjective", &CvQuestInfo::getQuestObjective, "string ()")
		.def("getQuestBodyText", &CvQuestInfo::getQuestBodyText, "string ()")
		.def("getNumQuestMessages", &CvQuestInfo::getNumQuestMessages, "string ()")
		.def("getQuestLinkType", &CvQuestInfo::getQuestLinkType, "string ()")
		.def("getQuestLinkName", &CvQuestInfo::getQuestLinkName, "string ()")
		.def("getQuestSounds", &CvQuestInfo::getQuestSounds, "string ()")

		.def("setNumQuestMessages", &CvQuestInfo::setNumQuestMessages, "void (int)")

		.def("setQuestObjective", &CvQuestInfo::setQuestObjective, "void (string)")
		.def("setQuestBodyText", &CvQuestInfo::setQuestBodyText, "void (string)")
		.def("setQuestMessages", &CvQuestInfo::setQuestMessages, "void (int iIndex, string)")
		;

	python::class_<CvTutorialMessage>("CvTutorialMessage")
		.def("getText", &CvTutorialMessage::getText, "string ()")
		.def("getImage", &CvTutorialMessage::getImage, "string ()")
		.def("getSound", &CvTutorialMessage::getSound, "string ()")

		.def("getNumTutorialScripts", &CvTutorialMessage::getNumTutorialScripts, "int ()")
		.def("getTutorialScriptByIndex", &CvTutorialMessage::getTutorialScriptByIndex, "int (int i)")
		;

	python::class_<CvTutorialInfo, python::bases<CvInfoBase> >("CvTutorialInfo")
		.def("getNextTutorialInfoType", &CvTutorialInfo::getNextTutorialInfoType, "string ()")

		.def("getNumTutorialMessages", &CvTutorialInfo::getNumTutorialMessages, "int ()")
		.def("getTutorialMessage", &CvTutorialInfo::getTutorialMessage,  python::return_value_policy<python::reference_existing_object>(), "CvTutorialMessage* (int iIndex)")
		;

	python::class_<CvAutomateInfo, python::bases<CvInfoBase> >("CvAutomateInfo")
		;

	python::class_<CvCommandInfo, python::bases<CvInfoBase> >("CvCommandInfo")
		;

	python::class_<CvGameOptionInfo, python::bases<CvInfoBase> >("CvGameOptionInfo")
		.def("getDefault", &CvGameOptionInfo::getDefault, "bool ()")
		.def("getVisible", &CvGameOptionInfo::getVisible, "bool ()")
		;

	python::class_<CvMPOptionInfo, python::bases<CvInfoBase> >("CvMPOptionInfo")
		.def("getDefault", &CvMPOptionInfo::getDefault, "bool ()")
		;

	python::class_<CvForceControlInfo, python::bases<CvInfoBase> >("CvForceControlInfo")
		.def("getDefault", &CvForceControlInfo::getDefault, "bool ()")
		;

	python::class_<CvPlayerOptionInfo, python::bases<CvInfoBase> >("CvPlayerOptionInfo")
		.def("getDefault", &CvPlayerOptionInfo::getDefault, "bool ()")
		;

	python::class_<CvGraphicOptionInfo, python::bases<CvInfoBase> >("CvGraphicOptionInfo")
		.def("getDefault", &CvGraphicOptionInfo::getDefault, "bool ()")
		;

	python::class_<CvEventTriggerInfo, python::bases<CvInfoBase> >("CvEventTriggerInfo")
		.def("getPercentGamesActive", &CvEventTriggerInfo::getPercentGamesActive, "int ()")
		.def("getProbability", &CvEventTriggerInfo::getProbability, "int ()")
		.def("getNumUnits", &CvEventTriggerInfo::getNumUnits, "int ()")
		.def("getNumBuildings", &CvEventTriggerInfo::getNumBuildings, "int ()")
		.def("getNumUnitsGlobal", &CvEventTriggerInfo::getNumUnitsGlobal, "int ()")
		.def("getNumBuildingsGlobal", &CvEventTriggerInfo::getNumBuildingsGlobal, "int ()")
		.def("getNumPlotsRequired", &CvEventTriggerInfo::getNumPlotsRequired, "int ()")
		.def("getPlotsType", &CvEventTriggerInfo::getPlotType, "int ()")
		.def("getNumReligions", &CvEventTriggerInfo::getNumReligions, "int ()")
		.def("getNumCorporations", &CvEventTriggerInfo::getNumCorporations, "int ()")
		.def("getOtherPlayerShareBorders", &CvEventTriggerInfo::getOtherPlayerShareBorders, "int ()")
		.def("getOtherPlayerHasTech", &CvEventTriggerInfo::getOtherPlayerHasTech, "int ()")
		.def("getCityFoodWeight", &CvEventTriggerInfo::getCityFoodWeight, "int ()")
		.def("getCivic", &CvEventTriggerInfo::getCivic, "int ()")
		.def("getMinPopulation", &CvEventTriggerInfo::getMinPopulation, "int ()")
		.def("getMaxPopulation", &CvEventTriggerInfo::getMaxPopulation, "int ()")
		.def("getMinMapLandmass", &CvEventTriggerInfo::getMinMapLandmass, "int ()")
		.def("getMinOurLandmass", &CvEventTriggerInfo::getMinOurLandmass, "int ()")
		.def("getMaxOurLandmass", &CvEventTriggerInfo::getMaxOurLandmass, "int ()")
		.def("getMinDifficulty", &CvEventTriggerInfo::getMinDifficulty, "int ()")
		.def("getAngry", &CvEventTriggerInfo::getAngry, "int ()")
		.def("getUnhealthy", &CvEventTriggerInfo::getUnhealthy, "int ()")
		.def("getUnitDamagedWeight", &CvEventTriggerInfo::getUnitDamagedWeight, "int ()")
		.def("getUnitDistanceWeight", &CvEventTriggerInfo::getUnitDistanceWeight, "int ()")
		.def("getUnitExperienceWeight", &CvEventTriggerInfo::getUnitExperienceWeight, "int ()")
		.def("getMinTreasury", &CvEventTriggerInfo::getMinTreasury, "int ()")

		.def("getUnitRequired", &CvEventTriggerInfo::getUnitRequired, "int (int)")
		.def("getNumUnitsRequired", &CvEventTriggerInfo::getNumUnitsRequired, "int ()")
		.def("getBuildingRequired", &CvEventTriggerInfo::getBuildingRequired, "int (int)")
		.def("getNumBuildingsRequired", &CvEventTriggerInfo::getNumBuildingsRequired, "int ()")
		.def("getPrereqOrTechs", &CvEventTriggerInfo::getPrereqOrTechs, "int (int)")
		.def("getNumPrereqOrTechs", &CvEventTriggerInfo::getNumPrereqOrTechs, "int ()")
		.def("getPrereqAndTechs", &CvEventTriggerInfo::getPrereqAndTechs, "int (int)")
		.def("getNumPrereqAndTechs", &CvEventTriggerInfo::getNumPrereqAndTechs, "int ()")
		.def("getObsoleteTech", &CvEventTriggerInfo::getObsoleteTech, "int (int)")
		.def("getNumObsoleteTechs", &CvEventTriggerInfo::getNumObsoleteTechs, "int (int)")
		.def("getEvent", &CvEventTriggerInfo::getEvent, "int (int)")
		.def("getNumEvents", &CvEventTriggerInfo::getNumEvents, "int ()")
		.def("getPrereqEvent", &CvEventTriggerInfo::getPrereqEvent, "int (int)")
		.def("getNumPrereqEvents", &CvEventTriggerInfo::getNumPrereqEvents, "int ()")
		.def("getFeatureRequired", &CvEventTriggerInfo::getFeatureRequired, "int (int)")
		.def("getNumFeaturesRequired", &CvEventTriggerInfo::getNumFeaturesRequired, "int ()")
		.def("getTerrainRequired", &CvEventTriggerInfo::getTerrainRequired, "int (int)")
		.def("getNumTerrainsRequired", &CvEventTriggerInfo::getNumTerrainsRequired, "int ()")
		.def("getImprovementRequired", &CvEventTriggerInfo::getImprovementRequired, "int (int)")
		.def("getNumImprovementsRequired", &CvEventTriggerInfo::getNumImprovementsRequired, "int ()")
		.def("getBonusRequired", &CvEventTriggerInfo::getBonusRequired, "int (int)")
		.def("getNumBonusesRequired", &CvEventTriggerInfo::getNumBonusesRequired, "int ()")
		.def("getRouteRequired", &CvEventTriggerInfo::getRouteRequired, "int (int)")
		.def("getNumRoutesRequired", &CvEventTriggerInfo::getNumRoutesRequired, "int ()")
		.def("getReligionRequired", &CvEventTriggerInfo::getBonusRequired, "int (int)")
		.def("getNumReligionsRequired", &CvEventTriggerInfo::getNumReligionsRequired, "int ()")
		.def("getCorporationRequired", &CvEventTriggerInfo::getCorporationRequired, "int (int)")
		.def("getNumCorporationsRequired", &CvEventTriggerInfo::getNumCorporationsRequired, "int ()")

		.def("isSinglePlayer", &CvEventTriggerInfo::isSinglePlayer, "bool ()")
		.def("isTeam", &CvEventTriggerInfo::isTeam, "bool ()")
		.def("isRecurring", &CvEventTriggerInfo::isRecurring, "bool ()")
		.def("isGlobal", &CvEventTriggerInfo::isGlobal, "bool ()")
		.def("isPickPlayer", &CvEventTriggerInfo::isPickPlayer, "bool ()")
		.def("isOtherPlayerHasReligion", &CvEventTriggerInfo::isOtherPlayerHasReligion, "bool ()")
		.def("isOtherPlayerHasOtherReligion", &CvEventTriggerInfo::isOtherPlayerHasOtherReligion, "bool ()")
		.def("isOtherPlayerAI", &CvEventTriggerInfo::isOtherPlayerAI, "bool ()")
		.def("isPickCity", &CvEventTriggerInfo::isPickCity, "bool ()")
		.def("isPickOtherPlayerCity", &CvEventTriggerInfo::isPickCity, "bool ()")
		.def("isUnitsOnPlot", &CvEventTriggerInfo::isUnitsOnPlot, "bool ()")
		.def("isOwnPlot", &CvEventTriggerInfo::isOwnPlot, "bool ()")
		.def("isPickReligion", &CvEventTriggerInfo::isPickReligion, "bool ()")
		.def("isStateReligion", &CvEventTriggerInfo::isStateReligion, "bool ()")
		.def("isProbabilityUnitMultiply", &CvEventTriggerInfo::isProbabilityUnitMultiply, "bool ()")
		.def("isProbabilityBuildingMultiply", &CvEventTriggerInfo::isProbabilityBuildingMultiply, "bool ()")
		.def("isPrereqEventCity", &CvEventTriggerInfo::isPrereqEventCity, "bool ()")
		;

	python::class_<CvEventInfo, python::bases<CvInfoBase> >("CvEventInfo")
		.def("isQuest", &CvEventInfo::isQuest, "bool ()")
		.def("isGlobal", &CvEventInfo::isGlobal, "bool ()")
		.def("isTeam", &CvEventInfo::isTeam, "bool ()")
		.def("isCityEffect", &CvEventInfo::isCityEffect, "bool ()")
		.def("isOtherPlayerCityEffect", &CvEventInfo::isOtherPlayerCityEffect, "bool ()")
		.def("isGoldToPlayer", &CvEventInfo::isGoldToPlayer, "bool ()")
		.def("isGoldenAge", &CvEventInfo::isGoldenAge, "bool ()")
		.def("isDeclareWar", &CvEventInfo::isDeclareWar, "bool ()")
		.def("isDisbandUnit", &CvEventInfo::isDisbandUnit, "bool ()")
		.def("getGold", &CvEventInfo::getGold, "int ()")
		.def("getRandomGold", &CvEventInfo::getRandomGold, "int ()")
		.def("getCulture", &CvEventInfo::getCulture, "int ()")
		.def("getEspionagePoints", &CvEventInfo::getEspionagePoints, "int ()")
		.def("getTech", &CvEventInfo::getTech, "int ()")
		.def("getTechPercent", &CvEventInfo::getTechPercent, "int ()")
		.def("getTechCostPercent", &CvEventInfo::getTechCostPercent, "int ()")
		.def("getTechMinTurnsLeft", &CvEventInfo::getTechMinTurnsLeft, "int ()")
		.def("getPrereqTech", &CvEventInfo::getPrereqTech, "int ()")
		.def("getUnitClass", &CvEventInfo::getUnitClass, "int ()")
		.def("getNumUnits", &CvEventTriggerInfo::getNumUnits, "int ()")
		.def("getBuildingClass", &CvEventInfo::getBuildingClass, "int ()")
		.def("getBuildingChange", &CvEventInfo::getBuildingChange, "int ()")
		.def("getHappy", &CvEventInfo::getHappy, "int ()")
		.def("getHealth", &CvEventInfo::getHealth, "int ()")
		.def("getHurryAnger", &CvEventInfo::getHurryAnger, "int ()")
		.def("getHappyTurns", &CvEventInfo::getHappyTurns, "int ()")
		.def("getFood", &CvEventInfo::getFood, "int ()")
		.def("getFoodPercent", &CvEventInfo::getFoodPercent, "int ()")
		.def("getFeature", &CvEventInfo::getFeature, "int ()")
		.def("getFeatureChange", &CvEventInfo::getFeatureChange, "int ()")
		.def("getImprovement", &CvEventInfo::getImprovement, "int ()")
		.def("getImprovementChange", &CvEventInfo::getImprovementChange, "int ()")
		.def("getBonus", &CvEventInfo::getBonus, "int ()")
		.def("getBonusChange", &CvEventInfo::getBonusChange, "int ()")
		.def("getRoute", &CvEventInfo::getRoute, "int ()")
		.def("getRouteChange", &CvEventInfo::getRouteChange, "int ()")
		.def("getBonusRevealed", &CvEventInfo::getBonusRevealed, "int ()")
		.def("getBonusGift", &CvEventInfo::getBonusGift, "int ()")
		.def("getUnitExperience", &CvEventInfo::getUnitExperience, "int ()")
		.def("getUnitImmobileTurns", &CvEventInfo::getUnitImmobileTurns, "int ()")
		.def("getConvertOwnCities", &CvEventInfo::getConvertOwnCities, "int ()")
		.def("getConvertOtherCities", &CvEventInfo::getConvertOtherCities, "int ()")
		.def("getMaxNumReligions", &CvEventInfo::getMaxNumReligions, "int ()")
		.def("getOurAttitudeModifier", &CvEventInfo::getOurAttitudeModifier, "int ()")
		.def("getAttitudeModifier", &CvEventInfo::getAttitudeModifier, "int ()")
		.def("getTheirEnemyAttitudeModifier", &CvEventInfo::getTheirEnemyAttitudeModifier, "int ()")
		.def("getPopulationChange", &CvEventInfo::getPopulationChange, "int ()")
		.def("getRevoltTurns", &CvEventInfo::getRevoltTurns, "int ()")
		.def("getMinPillage", &CvEventInfo::getMinPillage, "int ()")
		.def("getMaxPillage", &CvEventInfo::getMaxPillage, "int ()")
		.def("getUnitPromotion", &CvEventInfo::getUnitPromotion, "int ()")
		.def("getFreeUnitSupport", &CvEventInfo::getFreeUnitSupport, "int ()")
		.def("getInflationModifier", &CvEventInfo::getInflationModifier, "int ()")
		.def("getSpaceProductionModifier", &CvEventInfo::getSpaceProductionModifier, "int ()")
		.def("getAIValue", &CvEventInfo::getAIValue, "int ()")

		.def("getAdditionalEventChance", &CvEventInfo::getAdditionalEventChance, "int (int)")
		.def("getAdditionalEventTime", &CvEventInfo::getAdditionalEventTime, "int (int)")
		.def("getClearEventChance", &CvEventInfo::getClearEventChance, "int (int)")
		.def("getTechFlavorValue", &CvEventInfo::getTechFlavorValue, "int (int)")
		.def("getPlotExtraYield", &CvEventInfo::getPlotExtraYield, "int (int)")
		.def("getFreeSpecialistCount", &CvEventInfo::getFreeSpecialistCount, "int (int)")
		.def("getUnitCombatPromotion", &CvEventInfo::getUnitCombatPromotion, "int (int)")
		.def("getUnitClassPromotion", &CvEventInfo::getUnitClassPromotion, "int (int)")

		.def("getBuildingYieldChange", &CvEventInfo::getBuildingYieldChange, "int (int /*BuildingClassTypes*/, int /*YieldTypes*/)")
		.def("getNumBuildingYieldChanges", &CvEventInfo::getNumBuildingYieldChanges, "int ()")
		.def("getBuildingCommerceChange", &CvEventInfo::getBuildingCommerceChange, "int (int /*BuildingClassTypes*/, int /*CommerceTypes*/)")
		.def("getNumBuildingCommerceChanges", &CvEventInfo::getNumBuildingCommerceChanges, "int ()")
		.def("getBuildingHappyChange", &CvEventInfo::getBuildingHappyChange, "int (int /*BuildingClassTypes*/)")
		.def("getNumBuildingHappyChanges", &CvEventInfo::getNumBuildingHappyChanges, "int ()")
		.def("getBuildingHealthChange", &CvEventInfo::getBuildingHealthChange, "int (int /*BuildingClassTypes*/)")
		.def("getNumBuildingHealthChanges", &CvEventInfo::getNumBuildingHealthChanges, "int ()")
		;

	python::class_<CvEspionageMissionInfo, python::bases<CvInfoBase> >("CvEspionageMissionInfo")
		.def("getCost", &CvEspionageMissionInfo::getCost, "int ()")
		.def("isPassive", &CvEspionageMissionInfo::isPassive, "bool ()")
		.def("isTwoPhases", &CvEspionageMissionInfo::isTwoPhases, "bool ()")
		.def("isTargetsCity", &CvEspionageMissionInfo::isTargetsCity, "bool ()")
		.def("isSelectPlot", &CvEspionageMissionInfo::isSelectPlot, "bool ()")
		.def("getTechPrereq", &CvEspionageMissionInfo::getTechPrereq, "int ()")
		.def("getVisibilityLevel", &CvEspionageMissionInfo::getVisibilityLevel, "int ()")
		.def("isInvestigateCity", &CvEspionageMissionInfo::isInvestigateCity, "bool ()")
		.def("isSeeDemographics", &CvEspionageMissionInfo::isSeeDemographics, "bool ()")
		.def("isNoActiveMissions", &CvEspionageMissionInfo::isNoActiveMissions, "bool ()")
		.def("isSeeResearch", &CvEspionageMissionInfo::isSeeResearch, "bool ()")
		.def("isDestroyImprovement", &CvEspionageMissionInfo::isDestroyImprovement, "bool ()")
		.def("getDestroyBuildingCostFactor", &CvEspionageMissionInfo::getDestroyBuildingCostFactor, "int ()")
		.def("getDestroyUnitCostFactor", &CvEspionageMissionInfo::getDestroyUnitCostFactor, "int ()")
		.def("getBuyUnitCostFactor", &CvEspionageMissionInfo::getBuyUnitCostFactor, "int ()")
		.def("getBuyCityCostFactor", &CvEspionageMissionInfo::getBuyCityCostFactor, "int ()")
		.def("getStealTreasuryTypes", &CvEspionageMissionInfo::getStealTreasuryTypes, "int ()")
		.def("getCityInsertCultureAmountFactor", &CvEspionageMissionInfo::getCityInsertCultureAmountFactor, "int ()")
		.def("getCityInsertCultureCostFactor", &CvEspionageMissionInfo::getCityInsertCultureCostFactor, "int ()")
		.def("getCityPoisonWaterCounter", &CvEspionageMissionInfo::getCityPoisonWaterCounter, "int ()")
		.def("getCityUnhappinessCounter", &CvEspionageMissionInfo::getCityUnhappinessCounter, "int ()")
		.def("getCityRevoltCounter", &CvEspionageMissionInfo::getCityRevoltCounter, "int ()")
		.def("getBuyTechCostFactor", &CvEspionageMissionInfo::getBuyTechCostFactor, "int ()")
		.def("getSwitchCivicCostFactor", &CvEspionageMissionInfo::getSwitchCivicCostFactor, "int ()")
		.def("getSwitchReligionCostFactor", &CvEspionageMissionInfo::getSwitchReligionCostFactor, "int ()")
		.def("getPlayerAnarchyCounter", &CvEspionageMissionInfo::getPlayerAnarchyCounter, "int ()")
		.def("getCounterespionageNumTurns", &CvEspionageMissionInfo::getCounterespionageNumTurns, "int ()")
		.def("getCounterespionageMod", &CvEspionageMissionInfo::getCounterespionageMod, "int ()")
		.def("getDifficultyMod", &CvEspionageMissionInfo::getDifficultyMod, "int ()")
		;

	python::class_<CvVoteSourceInfo, python::bases<CvInfoBase> >("CvVoteSourceInfo")
		.def("getVoteInterval", &CvVoteSourceInfo::getVoteInterval, "int ()")
		.def("getCivic", &CvVoteSourceInfo::getCivic, "int ()")
		.def("getFreeSpecialist", &CvVoteSourceInfo::getFreeSpecialist, "int ()")
		.def("getReligionYield", &CvVoteSourceInfo::getReligionYield, "int (int)")
		.def("getReligionCommerce", &CvVoteSourceInfo::getReligionCommerce, "int (int)")
		.def("getSecretaryGeneralText", &CvVoteSourceInfo::pyGetSecretaryGeneralText, "wstring ()")
		;

	python::class_<CvMainMenuInfo, python::bases<CvInfoBase> >("CvMainMenuInfo")
		.def("getScene", &CvMainMenuInfo::getScene, "string ()")
		.def("getSceneNoShader", &CvMainMenuInfo::getSceneNoShader, "string ()")
		.def("getSoundtrack", &CvMainMenuInfo::getSoundtrack, "string ()")
		.def("getLoading", &CvMainMenuInfo::getLoading, "string ()")
		.def("getLoadingSlideshow", &CvMainMenuInfo::getLoadingSlideshow, "string ()")
		;

	python::class_<CvPropertyInfo, python::bases<CvInfoBase> >("CvPropertyInfo")
		.def("getChar", &CvPropertyInfo::getChar, "int ()")
		.def("isSourceDrain", &CvPropertyInfo::isSourceDrain, "bool ()")
		;

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/24/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// Python Modular Loading
	python::class_<CvPythonModulesInfo, python::bases<CvInfoBase> >("CvPythonModulesInfo")
		.def("isMainInterfaceScreen", &CvPythonModulesInfo::isMainInterfaceScreen, "bool ()")
		.def("isCivicScreen", &CvPythonModulesInfo::isCivicScreen, "bool ()")
		.def("isCorporationScreen", &CvPythonModulesInfo::isCorporationScreen, "bool ()")
		.def("isDomesticAdvisor", &CvPythonModulesInfo::isDomesticAdvisor, "bool ()")
		.def("isEspionageAdvisor", &CvPythonModulesInfo::isEspionageAdvisor, "bool ()")
		.def("isForeignAdvisor", &CvPythonModulesInfo::isForeignAdvisor, "bool ()")
		.def("isMilitaryAdvisor", &CvPythonModulesInfo::isMilitaryAdvisor, "bool ()")
		.def("isVictoryScreen", &CvPythonModulesInfo::isVictoryScreen, "bool ()")
		.def("getScreen", &CvPythonModulesInfo::getScreen, "int ()")
		;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
}
