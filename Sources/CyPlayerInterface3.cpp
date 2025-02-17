#include "CvGameCoreDLL.h"


void CyPlayerPythonInterface3(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface1\n");

	// set the docstring of the current module scope 
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		.def("getAdvancedStartPoints", &CyPlayer::getAdvancedStartPoints, "int ()")
		.def("setAdvancedStartPoints", &CyPlayer::setAdvancedStartPoints, "void (int iNewValue)")
		.def("changeAdvancedStartPoints", &CyPlayer::changeAdvancedStartPoints, "void (int iChange)")
		.def("getAdvancedStartUnitCost", &CyPlayer::getAdvancedStartUnitCost, "int (int (UnitTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartCityCost", &CyPlayer::getAdvancedStartCityCost, "int (int (bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartPopCost", &CyPlayer::getAdvancedStartPopCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartCultureCost", &CyPlayer::getAdvancedStartCultureCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartBuildingCost", &CyPlayer::getAdvancedStartBuildingCost, "int (int (BuildingTypes) eUnit, bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartImprovementCost", &CyPlayer::getAdvancedStartImprovementCost, "int (int (ImprovementTypes) eImprovement, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartRouteCost", &CyPlayer::getAdvancedStartRouteCost, "int (int (RouteTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartTechCost", &CyPlayer::getAdvancedStartTechCost, "int (int (TechTypes) eUnit, bool bAdd)")
		.def("getAdvancedStartVisibilityCost", &CyPlayer::getAdvancedStartVisibilityCost, "int (bool bAdd, CyPlot* pPlot)")

		.def("getEspionageSpending", &CyPlayer::getEspionageSpending, "int (PlayerTypes eIndex)")
		.def("canDoEspionageMission", &CyPlayer::canDoEspionageMission, "bool (EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CyPlot* pPlot, int iExtraData)")
		.def("getEspionageMissionCost", &CyPlayer::getEspionageMissionCost, "int (EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CyPlot* pPlot, int iExtraData)")
		.def("doEspionageMission", &CyPlayer::doEspionageMission, "void (EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CyPlot* pPlot, int iExtraData, CyUnit* pUnit)")
		.def("getEspionageSpendingWeightAgainstTeam", &CyPlayer::getEspionageSpendingWeightAgainstTeam, "int (TeamTypes eIndex)")
		.def("setEspionageSpendingWeightAgainstTeam", &CyPlayer::setEspionageSpendingWeightAgainstTeam, "void (TeamTypes eIndex, int iValue)")
		.def("changeEspionageSpendingWeightAgainstTeam", &CyPlayer::changeEspionageSpendingWeightAgainstTeam, "void (TeamTypes eIndex, int iChange)")

		.def("getGoldenAgeTurns", &CyPlayer::getGoldenAgeTurns, "int ()")
		.def("getGoldenAgeLength", &CyPlayer::getGoldenAgeLength, "int ()")
		.def("isGoldenAge", &CyPlayer::isGoldenAge, "bool ()")
		.def("changeGoldenAgeTurns", &CyPlayer::changeGoldenAgeTurns, "void (int iChange)")
		.def("getNumUnitGoldenAges", &CyPlayer::getNumUnitGoldenAges, "int ()")
		.def("changeNumUnitGoldenAges", &CyPlayer::changeNumUnitGoldenAges, "void (int iChange)")
		.def("getAnarchyTurns", &CyPlayer::getAnarchyTurns, "int ()")
		.def("isAnarchy", &CyPlayer::isAnarchy, "bool ()")
		.def("changeAnarchyTurns", &CyPlayer::changeAnarchyTurns, "void ()")
		.def("getStrikeTurns", &CyPlayer::getStrikeTurns, "int ()")
		.def("getMaxAnarchyTurns", &CyPlayer::getMaxAnarchyTurns, "int ()")
		.def("getAnarchyModifier", &CyPlayer::getAnarchyModifier, "int ()")
		.def("getGoldenAgeModifier", &CyPlayer::getGoldenAgeModifier, "int ()")
		.def("getHurryModifier", &CyPlayer::getHurryModifier, "int ()")
		.def("createGreatPeople", &CyPlayer::createGreatPeople, "void (int /*UnitTypes*/ eGreatPersonUnit, bool bIncrementThreshold, int iX, int iY)")
		.def("getGreatPeopleCreated", &CyPlayer::getGreatPeopleCreated, "int ()")
		.def("getGreatGeneralsCreated", &CyPlayer::getGreatGeneralsCreated, "int ()")
		.def("getGreatPeopleThresholdModifier", &CyPlayer::getGreatPeopleThresholdModifier, "int ()")
		.def("getGreatGeneralsThresholdModifier", &CyPlayer::getGreatGeneralsThresholdModifier, "int ()")
		.def("getGreatPeopleRateModifier", &CyPlayer::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CyPlayer::getGreatGeneralRateModifier, "int ()")
		.def("getDomesticGreatGeneralRateModifier", &CyPlayer::getDomesticGreatGeneralRateModifier, "int ()")
		.def("getStateReligionGreatPeopleRateModifier", &CyPlayer::getStateReligionGreatPeopleRateModifier, "int ()")

		.def("getMaxGlobalBuildingProductionModifier", &CyPlayer::getMaxGlobalBuildingProductionModifier, "int ()")
		.def("getMaxTeamBuildingProductionModifier", &CyPlayer::getMaxTeamBuildingProductionModifier, "int ()")
		.def("getMaxPlayerBuildingProductionModifier", &CyPlayer::getMaxPlayerBuildingProductionModifier, "int ()")
		.def("getFreeExperience", &CyPlayer::getFreeExperience, "int ()")
		.def("getFeatureProductionModifier", &CyPlayer::getFeatureProductionModifier, "int ()")
		.def("getWorkerSpeedModifier", &CyPlayer::getWorkerSpeedModifier, "int ()")
		.def("getImprovementUpgradeRateModifier", &CyPlayer::getImprovementUpgradeRateModifier, "int ()")
		.def("getMilitaryProductionModifier", &CyPlayer::getMilitaryProductionModifier, "int ()")
		.def("getSpaceProductionModifier", &CyPlayer::getSpaceProductionModifier, "int ()")
		.def("getCityDefenseModifier", &CyPlayer::getCityDefenseModifier, "int ()")
		/************************************************************************************************/
		/* REVDCM                                 04/29/10                                phungus420    */
		/*                                                                                              */
		/* Player Functions                                                                             */
		/************************************************************************************************/
		.def("getUnitUpgradePriceModifier", &CyPlayer::getUnitUpgradePriceModifier, "int ()")
		.def("getRevIdxNational", &CyPlayer::getRevIdxNational, "int ()")
		.def("isInquisitionConditions", &CyPlayer::isInquisitionConditions, "bool ()")
		.def("canFoundReligion", &CyPlayer::canFoundReligion, "bool ()")
		.def("isBuildingClassRequiredToTrain", &CyPlayer::isBuildingClassRequiredToTrain, "bool ((BuildingClassTypes)iBuildingClass, (UnitTypes)iUnit)")
		/************************************************************************************************/
		/* REVDCM                                  END                                                  */
		/************************************************************************************************/
		.def("getNumNukeUnits", &CyPlayer::getNumNukeUnits, "int ()")
		.def("getNumOutsideUnits", &CyPlayer::getNumOutsideUnits, "int ()")
		.def("getBaseFreeUnits", &CyPlayer::getBaseFreeUnits, "int ()")
		.def("getBaseFreeMilitaryUnits", &CyPlayer::getBaseFreeMilitaryUnits, "int ()")

		.def("getFreeUnitsPopulationPercent", &CyPlayer::getFreeUnitsPopulationPercent, "int ()")
		.def("getFreeMilitaryUnitsPopulationPercent", &CyPlayer::getFreeMilitaryUnitsPopulationPercent, "int ()")
		.def("getGoldPerUnit", &CyPlayer::getGoldPerUnit, "int ()")
		.def("getGoldPerMilitaryUnit", &CyPlayer::getGoldPerMilitaryUnit, "int ()")
		.def("getExtraUnitCost", &CyPlayer::getExtraUnitCost, "int ()")
		.def("getNumMilitaryUnits", &CyPlayer::getNumMilitaryUnits, "int ()")
		.def("getHappyPerMilitaryUnit", &CyPlayer::getHappyPerMilitaryUnit, "int ()")
		.def("isMilitaryFoodProduction", &CyPlayer::isMilitaryFoodProduction, "bool ()")
		.def("getHighestUnitLevel", &CyPlayer::getHighestUnitLevel, "int ()")

		.def("getConscriptCount", &CyPlayer::getConscriptCount, "int ()")
		.def("setConscriptCount", &CyPlayer::setConscriptCount, "void (int iNewValue)")
		.def("changeConscriptCount", &CyPlayer::changeConscriptCount, "void (int iChange)")

		.def("getMaxConscript", &CyPlayer::getMaxConscript, "int ()")
		.def("getOverflowResearch", &CyPlayer::getOverflowResearch, "int ()")
		.def("isNoUnhealthyPopulation", &CyPlayer::isNoUnhealthyPopulation, "bool ()")
		.def("getExpInBorderModifier", &CyPlayer::getExpInBorderModifier, "bool ()")
		.def("isBuildingOnlyHealthy", &CyPlayer::isBuildingOnlyHealthy, "bool ()")

		.def("getDistanceMaintenanceModifier", &CyPlayer::getDistanceMaintenanceModifier, "int ()")
		.def("getNumCitiesMaintenanceModifier", &CyPlayer::getNumCitiesMaintenanceModifier, "int ()")
		.def("getCorporationMaintenanceModifier", &CyPlayer::getCorporationMaintenanceModifier, "int ()")
		.def("getTotalMaintenance", &CyPlayer::getTotalMaintenance, "int ()")
		.def("getUpkeepModifier", &CyPlayer::getUpkeepModifier, "int ()")
		.def("getLevelExperienceModifier", &CyPlayer::getLevelExperienceModifier, "int ()")

		.def("getExtraHealth", &CyPlayer::getExtraHealth, "int ()")
		// BUG - start
		.def("changeExtraHealth", &CyPlayer::changeExtraHealth, "void (int iChange)")
		// BUG - end
		.def("getBuildingGoodHealth", &CyPlayer::getBuildingGoodHealth, "int ()")
		.def("getBuildingBadHealth", &CyPlayer::getBuildingBadHealth, "int ()")

		.def("getExtraHappiness", &CyPlayer::getExtraHappiness, "int ()")
		.def("changeExtraHappiness", &CyPlayer::changeExtraHappiness, "void (int iChange)")

		.def("getBuildingHappiness", &CyPlayer::getBuildingHappiness, "int ()")
		.def("getLargestCityHappiness", &CyPlayer::getLargestCityHappiness, "int ()")
		.def("getWarWearinessPercentAnger", &CyPlayer::getWarWearinessPercentAnger, "int ()")
		.def("getWarWearinessModifier", &CyPlayer::getWarWearinessModifier, "int ()")
		.def("getFreeSpecialist", &CyPlayer::getFreeSpecialist, "int ()")
		.def("isNoForeignTrade", &CyPlayer::isNoForeignTrade, "bool ()")
		.def("isNoCorporations", &CyPlayer::isNoCorporations, "bool ()")
		.def("isNoForeignCorporations", &CyPlayer::isNoForeignCorporations, "bool ()")
		.def("getCoastalTradeRoutes", &CyPlayer::getCoastalTradeRoutes, "int ()")
		.def("changeCoastalTradeRoutes", &CyPlayer::changeCoastalTradeRoutes, "void (int iChange)")
		.def("getTradeRoutes", &CyPlayer::getTradeRoutes, "int ()")
		.def("getConversionTimer", &CyPlayer::getConversionTimer, "int ()")
		.def("getRevolutionTimer", &CyPlayer::getRevolutionTimer, "int ()")
		/************************************************************************************************/
		/* REVOLUTION_MOD                         01/01/08                                jdog5000      */
		/*                                                                                              */
		/*                                                                                              */
		/************************************************************************************************/
		.def("setRevolutionTimer", &CyPlayer::setRevolutionTimer, "void (int newTime)")
		.def("changeRevolutionTimer", &CyPlayer::changeRevolutionTimer, "void (int addTime)")
		/************************************************************************************************/
		/* REVOLUTION_MOD                          END                                                  */
		/************************************************************************************************/

		.def("isStateReligion", &CyPlayer::isStateReligion, "bool ()")
		.def("isNoNonStateReligionSpread", &CyPlayer::isNoNonStateReligionSpread, "bool ()")
		.def("getStateReligionHappiness", &CyPlayer::getStateReligionHappiness, "int ()")
		.def("getNonStateReligionHappiness", &CyPlayer::getNonStateReligionHappiness, "int ()")
		.def("getStateReligionUnitProductionModifier", &CyPlayer::getStateReligionUnitProductionModifier, "int ()")
		.def("changeStateReligionUnitProductionModifier", &CyPlayer::changeStateReligionUnitProductionModifier, "void (int iChange)")
		.def("getStateReligionBuildingProductionModifier", &CyPlayer::getStateReligionBuildingProductionModifier, "int ()")
		.def("changeStateReligionBuildingProductionModifier", &CyPlayer::changeStateReligionBuildingProductionModifier, "void (int iChange)")
		.def("getStateReligionFreeExperience", &CyPlayer::getStateReligionFreeExperience, "int ()")
		.def("getCapitalCity", &CyPlayer::getCapitalCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iID)")
		.def("getCitiesLost", &CyPlayer::getCitiesLost, "int ()")

		.def("getWinsVsBarbs", &CyPlayer::getWinsVsBarbs, "int ()")

		.def("getAssets", &CyPlayer::getAssets, "int ()")
		.def("changeAssets", &CyPlayer::changeAssets, "void (int iChange)")
		.def("getPower", &CyPlayer::getPower, "int ()")
		.def("getUnitPower", &CyPlayer::getUnitPower, "int ()")
		.def("getPopScore", &CyPlayer::getPopScore, "int ()")
		.def("getLandScore", &CyPlayer::getLandScore, "int ()")
#if defined QC_MASTERY_VICTORY
		//Sevo Begin--VCM
		.def("getSevoWondersScore", &CyPlayer::getSevoWondersScore, "int ()")
		//Sevo End VCM
#endif
		.def("getWondersScore", &CyPlayer::getWondersScore, "int ()")
		.def("getTechScore", &CyPlayer::getTechScore, "int ()")
		.def("getTotalTimePlayed", &CyPlayer::getTotalTimePlayed, "int ()")
		.def("isMinorCiv", &CyPlayer::isMinorCiv, "bool ()")
		.def("isAlive", &CyPlayer::isAlive, "bool ()")
		.def("isEverAlive", &CyPlayer::isEverAlive, "bool ()")
		.def("isExtendedGame", &CyPlayer::isExtendedGame, "bool ()")
		.def("isFoundedFirstCity", &CyPlayer::isFoundedFirstCity, "bool ()")
		// < M.A.D. Nukes Start >
		.def("isMADNukesEnabled", &CyPlayer::isMADNukesEnabled, "bool ()")
		.def("setMADNukesEnabled", &CyPlayer::setMADNukesEnabled, "void (bool)")
		// < M.A.D. Nukes End   >

		.def("isStrike", &CyPlayer::isStrike, "bool ()")

		.def("getID", &CyPlayer::getID, "int ()")
		.def("getHandicapType", &CyPlayer::getHandicapType, "int ()")
		.def("getCivilizationType", &CyPlayer::getCivilizationType, "int ()")
		.def("getLeaderType", &CyPlayer::getLeaderType, "int ()")
		.def("getPersonalityType", &CyPlayer::getPersonalityType, "int ()")
		.def("setPersonalityType", &CyPlayer::setPersonalityType, "void (int /*LeaderHeadTypes*/ eNewValue)")
		.def("getCurrentEra", &CyPlayer::getCurrentEra, "int ()")
		.def("setCurrentEra", &CyPlayer::setCurrentEra, "void (int /*EraTypes*/ iNewValue)")
		.def("getStateReligion", &CyPlayer::getStateReligion, "int ()")
		.def("setLastStateReligion", &CyPlayer::setLastStateReligion, "void (int iReligionID) - Sets the player's state religion to iReligionID")
		.def("getTeam", &CyPlayer::getTeam, "int ()")
		.def("isTurnActive", &CyPlayer::isTurnActive, "bool ()")

		.def("getPlayerColor", &CyPlayer::getPlayerColor, "int (PlayerColorTypes) () - returns the color ID of the player")
		.def("getPlayerTextColorR", &CyPlayer::getPlayerTextColorR, "int ()")
		.def("getPlayerTextColorG", &CyPlayer::getPlayerTextColorG, "int ()")
		.def("getPlayerTextColorB", &CyPlayer::getPlayerTextColorB, "int ()")
		.def("getPlayerTextColorA", &CyPlayer::getPlayerTextColorA, "int ()")

		.def("getSeaPlotYield", &CyPlayer::getSeaPlotYield, "int (YieldTypes eIndex)")
		.def("getYieldRateModifier", &CyPlayer::getYieldRateModifier, "int (YieldTypes eIndex)")
		.def("getCapitalYieldRateModifier", &CyPlayer::getCapitalYieldRateModifier, "int (YieldTypes eIndex)")
		.def("getExtraYieldThreshold", &CyPlayer::getExtraYieldThreshold, "int (YieldTypes eIndex)")
		.def("getTradeYieldModifier", &CyPlayer::getTradeYieldModifier, "int (YieldTypes eIndex)")
		.def("getFreeCityCommerce", &CyPlayer::getFreeCityCommerce, "int (CommerceTypes eIndex)")
		.def("getCommercePercent", &CyPlayer::getCommercePercent, "int (CommerceTypes eIndex)")
		.def("setCommercePercent", &CyPlayer::setCommercePercent, "int (CommerceTypes eIndex, int iNewValue)")
		.def("changeCommercePercent", &CyPlayer::changeCommercePercent, "int (CommerceTypes eIndex, int iChange)")
		.def("getCommerceRate", &CyPlayer::getCommerceRate, "int (CommerceTypes eIndex)")
		.def("getCommerceRateModifier", &CyPlayer::getCommerceRateModifier, "int (CommerceTypes eIndex)")
		.def("getCapitalCommerceRateModifier", &CyPlayer::getCapitalCommerceRateModifier, "int (CommerceTypes eIndex)")
		.def("getStateReligionBuildingCommerce", &CyPlayer::getStateReligionBuildingCommerce, "int (CommerceTypes eIndex)")
		.def("getSpecialistExtraCommerce", &CyPlayer::getSpecialistExtraCommerce, "int (CommerceTypes eIndex)")
		.def("isCommerceFlexible", &CyPlayer::isCommerceFlexible, "bool (CommerceTypes eIndex)")
		.def("getGoldPerTurnByPlayer", &CyPlayer::getGoldPerTurnByPlayer, "int (PlayerTypes eIndex)")

		.def("isFeatAccomplished", &CyPlayer::isFeatAccomplished, "bool ()")
		.def("setFeatAccomplished", &CyPlayer::setFeatAccomplished, "void ()")
		.def("isOption", &CyPlayer::isOption, "bool ()")
		.def("setOption", &CyPlayer::setOption, "void ()")
		.def("isLoyalMember", &CyPlayer::isLoyalMember, "bool ()")
		.def("setLoyalMember", &CyPlayer::setLoyalMember, "void ()")
		.def("getVotes", &CyPlayer::getVotes, "int (int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource)")
		.def("isFullMember", &CyPlayer::isFullMember, "bool ()")
		.def("isVotingMember", &CyPlayer::isVotingMember, "bool ()")
		.def("isPlayable", &CyPlayer::isPlayable, "bool ()")
		.def("hasFreedomFighter", &CyPlayer::hasFreedomFighter, "bool ()")
		.def("getExtraFreedomFighters", &CyPlayer::getExtraFreedomFighters, "int ()")
		.def("setPlayable", &CyPlayer::setPlayable, "void ()")
		.def("getBonusExport", &CyPlayer::getBonusExport, "int (CommerceTypes eIndex)")
		.def("getBonusImport", &CyPlayer::getBonusImport, "int (CommerceTypes eIndex)")

		.def("getImprovementCount", &CyPlayer::getImprovementCount, "int (int /*ImprovementTypes*/ iIndex)")

		.def("isBuildingFree", &CyPlayer::isBuildingFree, "bool (int /*BuildingTypes*/ eIndex)")
		.def("getExtraBuildingHappiness", &CyPlayer::getExtraBuildingHappiness, "int (int /*BuildingTypes*/ eIndex)")
		.def("getExtraBuildingHealth", &CyPlayer::getExtraBuildingHealth, "int (int /*BuildingTypes*/ eIndex)")
		.def("getFeatureHappiness", &CyPlayer::getFeatureHappiness, "int (int /*FeatureTypes*/ eIndex)")
		.def("getUnitClassCount", &CyPlayer::getUnitClassCount, "int (int (UnitClassTypes) eIndex)")
		.def("isUnitClassMaxedOut", &CyPlayer::isUnitClassMaxedOut, "bool (int (UnitClassTypes) eIndex, int iExtra)")
		.def("getUnitClassMaking", &CyPlayer::getUnitClassMaking, "int (int (UnitClassTypes) eIndex)")
		.def("getUnitClassCountPlusMaking", &CyPlayer::getUnitClassCountPlusMaking, "int (int (UnitClassTypes) eIndex)")

		.def("getBuildingClassCount", &CyPlayer::getBuildingClassCount, "int (int /*BuildingClassTypes*/ eIndex)")
		.def("isBuildingClassMaxedOut", &CyPlayer::isBuildingClassMaxedOut, "bool (int /*BuildingClassTypes*/ iIndex, int iExtra)")
		.def("getBuildingClassMaking", &CyPlayer::getBuildingClassMaking, "int (int /*BuildingClassTypes*/ iIndex)")
		.def("getBuildingClassCountPlusMaking", &CyPlayer::getBuildingClassCountPlusMaking, "int (int /*BuildingClassTypes*/ iIndex)")
		.def("getHurryCount", &CyPlayer::getHurryCount, "int (int (HurryTypes) eIndex)")
		.def("canHurry", &CyPlayer::canHurry, "int (int (HurryTypes) eIndex)")
		.def("getSpecialBuildingNotRequiredCount", &CyPlayer::getSpecialBuildingNotRequiredCount, "int (int (SpecialBuildingTypes) eIndex)")
		.def("isSpecialBuildingNotRequired", &CyPlayer::isSpecialBuildingNotRequired, "int (int (SpecialBuildingTypes) eIndex)")

		.def("isHasCivicOption", &CyPlayer::isHasCivicOption, "bool (int (CivicOptionTypes) eIndex)")
		.def("isNoCivicUpkeep", &CyPlayer::isNoCivicUpkeep, "bool (int /*CivicOptionTypes*/ iIndex)")
		.def("getHasReligionCount", &CyPlayer::getHasReligionCount)
		.def("countTotalHasReligion", &CyPlayer::countTotalHasReligion, "int () - ")
		.def("getHasCorporationCount", &CyPlayer::getHasCorporationCount)
		.def("countTotalHasCorporation", &CyPlayer::countTotalHasCorporation, "int () - ")
		.def("findHighestHasReligionCount", &CyPlayer::findHighestHasReligionCount, "int () - ")
		.def("getUpkeepCount", &CyPlayer::getUpkeepCount, "int (int (UpkeepTypes) eIndex)")

		.def("isSpecialistValid", &CyPlayer::isSpecialistValid, "bool (int /*SpecialistTypes*/ iIndex)")
		.def("isResearchingTech", &CyPlayer::isResearchingTech, "bool (int /*TechTypes*/ iIndex)")
		.def("getCivics", &CyPlayer::getCivics, "int /*CivicTypes*/ (int /*CivicOptionTypes*/ iIndex)")
		.def("getSingleCivicUpkeep", &CyPlayer::getSingleCivicUpkeep, "int (int /*CivicTypes*/ eCivic, bool bIgnoreAnarchy)")
		.def("getCivicUpkeep", &CyPlayer::getCivicUpkeep, "int (int* /*CivicTypes*/ paiCivics, bool bIgnoreAnarchy)")
		.def("setCivics", &CyPlayer::setCivics, "void (int iCivicOptionType, int iCivicType) - Used to forcibly set civics with no anarchy")

		.def("getCombatExperience", &CyPlayer::getCombatExperience, "int () - Combat experience used to produce Warlords")
		.def("changeCombatExperience", &CyPlayer::changeCombatExperience, "void (int) - Combat experience used to produce Warlords")
		.def("setCombatExperience", &CyPlayer::setCombatExperience, "void (int) - Combat experience used to produce Warlords")

		.def("getExtraSpecialistYield", &CyPlayer::getExtraSpecialistYield, "int (int /*SpecialistTypes*/ eIndex1, int /*YieldTypes*/ eIndex2)")

		.def("findPathLength", &CyPlayer::findPathLength, "int (int (TechTypes) eTech, bool bCost)")

		.def("getQueuePosition", &CyPlayer::getQueuePosition, "int")
		.def("clearResearchQueue", &CyPlayer::clearResearchQueue, "void ()")
		.def("pushResearch", &CyPlayer::pushResearch, "void (int /*TechTypes*/ iIndex, bool bClear)")
		.def("popResearch", &CyPlayer::popResearch, "void (int /*TechTypes*/ eTech)")
		.def("getLengthResearchQueue", &CyPlayer::getLengthResearchQueue, "int ()")
		.def("addCityName", &CyPlayer::addCityName, "void (std::wstring szName)")
		.def("getNumCityNames", &CyPlayer::getNumCityNames, "int ()")
		.def("getCityName", &CyPlayer::getCityName, "std::wstring (int iIndex)")
		.def("firstCity", &CyPlayer::firstCity, "tuple(CyCity, int iterOut) (bool bReverse) - gets the first city")
		.def("nextCity", &CyPlayer::nextCity, "tuple(CyCity, int iterOut) (int iterIn, bool bReverse) - gets the next city")
		.def("getNumCities", &CyPlayer::getNumCities, "int ()")
		.def("getCity", &CyPlayer::getCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iID)")
		.def("firstUnit", &CyPlayer::firstUnit, "tuple(CyUnit, int iterOut) (bool bReverse) - gets the first unit")
		.def("nextUnit", &CyPlayer::nextUnit, "tuple(CyUnit, int iterOut) (int iterIn, bool bReverse) - gets the next unit")
		.def("getNumUnits", &CyPlayer::getNumUnits, "int ()")
		.def("getUnit", &CyPlayer::getUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iID)")
		.def("firstSelectionGroup", &CyPlayer::firstSelectionGroup, "tuple(CySelectionGroup, int iterOut) (bool bReverse) - gets the first selectionGroup")
		.def("nextSelectionGroup", &CyPlayer::nextSelectionGroup, "tuple(CySelectionGroup, int iterOut) (int iterIn, bool bReverse) - gets the next selectionGroup")
		.def("getNumSelectionGroups", &CyPlayer::getNumSelectionGroups, "int ()")
		.def("getSelectionGroup", &CyPlayer::getSelectionGroup, python::return_value_policy<python::manage_new_object>(), "CvSelectionGroup* (int iID)")

		.def("trigger", &CyPlayer::trigger, "void (/*EventTriggerTypes*/int eEventTrigger)")
		.def("getEventOccured", &CyPlayer::getEventOccured, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int /*EventTypes*/ eEvent)")
		.def("resetEventOccured", &CyPlayer::resetEventOccured, "void (int /*EventTypes*/ eEvent)")
		.def("getEventTriggered", &CyPlayer::getEventTriggered, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int iID)")
		.def("initTriggeredData", &CyPlayer::initTriggeredData, python::return_value_policy<python::reference_existing_object>(), "EventTriggeredData* (int eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer, int iOtherPlayerCityId, ReligionTypes eReligion, CorporationTypes eCorporation, int iUnitId, BuildingTypes eBuilding)")
		.def("getEventTriggerWeight", &CyPlayer::getEventTriggerWeight, "int getEventTriggerWeight(int eEventTrigger)")
		.def("doGoldenAgebyPercentage", &CyPlayer::doGoldenAgebyPercentage, "void doGoldenAgebyPercentage(int iPercent)")
		;
}