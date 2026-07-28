#include "AMSimPathProgressionEvaluator.h"

#include "AMSimPhase5Fixture.h"
#include "AMSimPhase6Fixture.h"

namespace AMSim
{
	namespace
	{
		int32 CountCompletedForSpecialization(
			const FPhase2State& Phase2,
			const EAirportSpecialization Specialization)
		{
			int32 Count = 0;
			for (const FPhase2FlightRecord& Flight : Phase2.Flights)
			{
				if (Flight.State != EPhase2FlightState::Completed)
				{
					continue;
				}
				const FPhase2ContractRecord* Contract =
					Phase2.Contracts.FindByPredicate(
						[&Flight](const FPhase2ContractRecord& Candidate)
						{
							return Candidate.Id == Flight.ContractId;
						});
				if (Contract && Contract->Specialization == Specialization)
				{
					++Count;
				}
			}
			return Count;
		}

		int32 CountDistinctRoles(const FPhase2State& Phase2)
		{
			TSet<FName> Roles;
			for (const FPhase2AircraftRecord& Aircraft : Phase2.Aircraft)
			{
				Roles.Add(Aircraft.RoleId);
			}
			return Roles.Num();
		}

		ECapabilityBand BaseOperationalBand(
			const FPhase5PathEvidenceRecord& Evidence)
		{
			if (MeetsAdvancedRequirements(Evidence))
			{
				return ECapabilityBand::Advanced;
			}
			if (Evidence.bSignatureFacilityOperational &&
				Evidence.CompletedOperations > 0)
			{
				return ECapabilityBand::Regional;
			}
			if (Evidence.bSignatureFacilityOperational)
			{
				return ECapabilityBand::Established;
			}
			return ECapabilityBand::Unavailable;
		}

		void PromoteEarnedBand(
			FPhase5PathEvidenceRecord& Evidence,
			const ECapabilityBand Candidate)
		{
			if (static_cast<uint8>(Candidate) >
				static_cast<uint8>(Evidence.EarnedBand))
			{
				Evidence.EarnedBand = Candidate;
			}
		}

		bool HasProvider(
			const FPhase5State& State,
			const FName ProviderType)
		{
			return State.ProviderTenants.ContainsByPredicate(
				[ProviderType](const FPhase5ProviderTenantRecord& Tenant)
				{
					return Tenant.ProviderType == ProviderType &&
						Tenant.State == EProviderTenantState::Active;
				});
		}
	}

	void RefreshAdvancedPathProgression(
		FPhase5State& State,
		const int64 CurrentGameMilliseconds,
		const FPhase1State& Phase1State,
		const FPhase2State& Phase2State,
		const FPhase3State& Phase3State,
		const FPhase4State& Phase4State,
		FPhase5ProgressionEmitter Emit)
	{
		TMap<FName, int32> ComponentTotals;
		for (const FPhase5RatingContributionRecord& Contribution :
			State.RatingContributions)
		{
			ComponentTotals.FindOrAdd(Contribution.ComponentId) +=
				Contribution.Magnitude;
		}
		int32 RatingTotal = 0;
		for (const FName Component : {
			TEXT("Rating.SafetyReadiness"),
			TEXT("Rating.OperationalReliability"),
			TEXT("Rating.CustomerExperience"),
			TEXT("Rating.TenantRelationships"),
			TEXT("Rating.AccessCleanlinessAmenities")})
		{
			RatingTotal += FMath::Clamp(
				50 + ComponentTotals.FindRef(Component),
				0,
				100);
		}
		State.OverallRating = RatingTotal / 5;
		const int32 TenantRelationshipRating = FMath::Clamp(
			50 + ComponentTotals.FindRef(TEXT("Rating.TenantRelationships")),
			0,
			100);
		const int32 OperatingDays = FMath::Max(
			State.CurrentOperatingDay,
			FMath::Max(
				Phase2State.CurrentOperatingDay,
				Phase4State.CurrentOperatingDay));

		for (FPhase5PathEvidenceRecord& Evidence : State.Paths)
		{
			const ECapabilityBand PreviousEarned = Evidence.EarnedBand;
			if (Evidence.EarnedBand == ECapabilityBand::Unavailable &&
				Evidence.Band != ECapabilityBand::Unavailable)
			{
				Evidence.EarnedBand = Evidence.Band;
			}
			Evidence.AirportPoints = Phase1State.AirportPoints;
			Evidence.OperatingDays = OperatingDays;
			Evidence.SafetyRating = State.OverallRating;
			Evidence.ReliabilityRating = State.OverallRating;
			Evidence.TenantRelationshipRating = TenantRelationshipRating;
			Evidence.bSharedResourceDayCompleted = false;
			switch (Evidence.Path)
			{
			case ESpecializationPath::GeneralAviation:
				Evidence.CompletedOperations =
					CountCompletedForSpecialization(
						Phase2State,
						EAirportSpecialization::GeneralAviation);
				Evidence.DistinctRolesOrClasses =
					CountDistinctRoles(Phase2State);
				Evidence.bPrimaryTenantActive =
					HasProvider(State, TEXT("FBO"));
				Evidence.bSecondaryProviderActive =
					HasProvider(State, TEXT("Maintenance"));
				Evidence.bSignatureFacilityOperational =
					Phase2State.bInitialized;
				break;
			case ESpecializationPath::FlightSchool:
				Evidence.CompletedOperations =
					CountCompletedForSpecialization(
						Phase2State,
						EAirportSpecialization::FlightSchool);
				Evidence.DistinctRolesOrClasses =
					Phase2State.Aircraft.Num() >= 2
						? 2
						: Phase2State.Aircraft.Num();
				Evidence.bPrimaryTenantActive =
					Phase2State.Tenants.ContainsByPredicate(
						[](const FPhase2TenantRecord& Tenant)
						{
							return Tenant.Specialization ==
									EAirportSpecialization::FlightSchool &&
								Tenant.bActive;
						});
				Evidence.bSecondaryProviderActive =
					HasProvider(State, TEXT("Maintenance"));
				Evidence.bSignatureFacilityOperational =
					Evidence.bPrimaryTenantActive;
				break;
			case ESpecializationPath::Charter:
				Evidence.CompletedOperations =
					CountCompletedForSpecialization(
						Phase2State,
						EAirportSpecialization::Charter);
				Evidence.bPrimaryTenantActive =
					Phase2State.Tenants.ContainsByPredicate(
						[](const FPhase2TenantRecord& Tenant)
						{
							return Tenant.Specialization ==
									EAirportSpecialization::Charter &&
								Tenant.bActive;
						});
				Evidence.bSecondaryProviderActive =
					HasProvider(State, TEXT("Fuel"));
				Evidence.bSignatureFacilityOperational =
					Evidence.bPrimaryTenantActive;
				break;
			case ESpecializationPath::Cargo:
				Evidence.CompletedOperations =
					State.CompletedShipmentCount;
				Evidence.DistinctRolesOrClasses =
					State.CompletedCargoClassCount;
				Evidence.bPrimaryTenantActive =
					HasProvider(State, TEXT("CargoOperator"));
				Evidence.bSecondaryProviderActive =
					HasProvider(State, TEXT("Maintenance"));
				Evidence.bSignatureFacilityOperational =
					State.WarehouseZones.Num() == 4 &&
					State.WarehouseZones.ContainsByPredicate(
						[](const FPhase5WarehouseZoneRecord& Zone)
						{
							return Zone.bOperational &&
								Zone.bRoadConnected &&
								Zone.bAirsideConnected;
						});
				break;
			case ESpecializationPath::Passenger:
				Evidence.CompletedOperations =
					Phase4State.CompletedFlightCount;
				Evidence.CompletedPassengers =
					Phase3State.CompletedPassengerCount;
				Evidence.bPrimaryTenantActive =
					Phase3State.bTerminalOpen;
				Evidence.bSecondaryProviderActive =
					HasProvider(State, TEXT("FoodConcession")) ||
					HasProvider(State, TEXT("RetailConcession"));
				Evidence.bSignatureFacilityOperational =
					Phase3State.bTerminalOpen &&
					Phase3State.bSecurityIntegrityValid;
				break;
			case ESpecializationPath::Mixed:
				break;
			}

			if (Evidence.Path != ESpecializationPath::Mixed)
			{
				Evidence.OperationalBand = BaseOperationalBand(Evidence);
				PromoteEarnedBand(Evidence, Evidence.OperationalBand);
				Evidence.Band = Evidence.OperationalBand;
				if (Evidence.OperationalBand >= ECapabilityBand::Advanced)
				{
					Evidence.OperationalStatus =
						ECapabilityOperationalStatus::Healthy;
				}
			}
			if (PreviousEarned != Evidence.EarnedBand)
			{
				Emit(
					EPhase5EventType::CapabilityBandChanged,
					{},
					static_cast<uint64>(Evidence.Path),
					CurrentGameMilliseconds,
					FString::Printf(
						TEXT("%s earned %s capability."),
						*SpecializationPathDisplayName(Evidence.Path),
						*CapabilityBandDisplayName(Evidence.EarnedBand)));
			}
		}

		int32 RegionalPathCount = 0;
		int32 ActivePathOperationCount = 0;
		for (const FPhase5PathEvidenceRecord& Evidence : State.Paths)
		{
			if (Evidence.Path != ESpecializationPath::Mixed &&
				Evidence.OperationalBand >= ECapabilityBand::Regional)
			{
				++RegionalPathCount;
			}
			if (Evidence.Path != ESpecializationPath::Mixed &&
				Evidence.CompletedOperations > 0)
			{
				++ActivePathOperationCount;
			}
		}
		FPhase5PathEvidenceRecord* Mixed = State.Paths.FindByPredicate(
			[](const FPhase5PathEvidenceRecord& Evidence)
			{
				return Evidence.Path == ESpecializationPath::Mixed;
			});
		if (Mixed)
		{
			Mixed->AirportPoints = Phase1State.AirportPoints;
			Mixed->OperatingDays = OperatingDays;
			Mixed->SafetyRating = State.OverallRating;
			Mixed->ReliabilityRating = State.OverallRating;
			Mixed->TenantRelationshipRating = TenantRelationshipRating;
			Mixed->RegionalPathCount = RegionalPathCount;
			Mixed->bPrimaryTenantActive = RegionalPathCount >= 2;
			Mixed->bSecondaryProviderActive =
				State.ProviderTenants.ContainsByPredicate(
					[](const FPhase5ProviderTenantRecord& Tenant)
					{
						return Tenant.State ==
							EProviderTenantState::Active;
					});
			Mixed->bSignatureFacilityOperational = RegionalPathCount >= 2;
			Mixed->bSharedResourceDayCompleted =
				RegionalPathCount >= 2 &&
				ActivePathOperationCount >= 2 &&
				Mixed->bSecondaryProviderActive;
			Mixed->OperationalBand = MeetsAdvancedRequirements(*Mixed)
				? ECapabilityBand::Advanced
				: RegionalPathCount >= 2
					? ECapabilityBand::Regional
					: ECapabilityBand::Established;
			PromoteEarnedBand(*Mixed, Mixed->OperationalBand);
			Mixed->Band = Mixed->OperationalBand;
		}

		State.AdvancedPathCount = 0;
		State.MajorPathCount = 0;
		for (FPhase5PathEvidenceRecord& Evidence : State.Paths)
		{
			Evidence.CurrentEvidence = FString::Printf(
				TEXT("%d AP - day %d - %d operations - rating %d"),
				Evidence.AirportPoints,
				Evidence.OperatingDays,
				Evidence.CompletedOperations,
				State.OverallRating);
			Evidence.NextRequirement =
				Evidence.EarnedBand >= ECapabilityBand::Advanced
					? TEXT("Reach 100 AP, 14 qualifying days, ratings 80, and Major path evidence.")
					: TEXT("Reach 50 AP, day 7, rating 70, and path evidence.");
			if (Evidence.EarnedBand >= ECapabilityBand::Advanced)
			{
				++State.AdvancedPathCount;
				FPhase5AchievementRecord* Achievement =
					State.Achievements.FindByPredicate(
						[&Evidence](const FPhase5AchievementRecord& Candidate)
						{
							return Candidate.Path == Evidence.Path;
						});
				if (Achievement && !Achievement->bEarned)
				{
					Achievement->bEarned = true;
					Achievement->EarnedAtGameMilliseconds =
						CurrentGameMilliseconds;
					Emit(
						EPhase5EventType::AchievementEarned,
						{},
						Achievement->Id.Value,
						CurrentGameMilliseconds,
						Achievement->DisplayName);
				}
			}
		}

		for (FPhase5ObjectiveRecord& Objective : State.Objectives)
		{
			const FPhase5PathEvidenceRecord* Evidence =
				State.Paths.FindByPredicate(
					[&Objective](const FPhase5PathEvidenceRecord& Candidate)
					{
						return Candidate.Path == Objective.Path;
					});
			Objective.Current =
				Evidence &&
					Evidence->EarnedBand >= ECapabilityBand::Advanced
				? 1
				: 0;
			Objective.bCompleted = Objective.Current >= Objective.Target;
		}
		State.bFixtureCompleted =
			State.CompletedCargoClassCount == 4 &&
			State.CompletedCargoFlowCount == 3 &&
			State.SpecialEvents.ContainsByPredicate(
				[](const FPhase5SpecialEventRecord& Event)
				{
					return Event.bRewardRecognized ||
						Event.State == ESpecialEventState::Cooldown;
				});
	}

	void ApplyMajorCapabilitySignals(
		FPhase5State& State,
		const FPhase6CapabilitySignals& Signals,
		const int64 CurrentGameMilliseconds,
		FPhase5ProgressionEmitter Emit)
	{
		State.MajorPathCount = 0;
		for (const FPhase6MajorPathSignal& Signal : Signals.Paths)
		{
			FPhase5PathEvidenceRecord* Evidence =
				State.Paths.FindByPredicate(
					[&Signal](const FPhase5PathEvidenceRecord& Candidate)
					{
						return Candidate.Path == Signal.Path;
					});
			if (!Evidence)
			{
				continue;
			}
			const ECapabilityBand PreviousEarned = Evidence->EarnedBand;
			Evidence->QualifyingOperatingDays =
				Signal.QualifyingOperatingDays;
			Evidence->SafetyRating = FMath::Max(
				Evidence->SafetyRating,
				Signal.SafetyRating);
			Evidence->ReliabilityRating = FMath::Max(
				Evidence->ReliabilityRating,
				Signal.ReliabilityRating);
			Evidence->TenantRelationshipRating =
				Signal.TenantRelationshipRating;
			Evidence->CompletedOperations = FMath::Max(
				Evidence->CompletedOperations,
				Signal.CompletedOperations);
			Evidence->DistinctRolesOrClasses = FMath::Max(
				Evidence->DistinctRolesOrClasses,
				Signal.DistinctRolesOrClasses);
			Evidence->CompletedPassengers = FMath::Max(
				Evidence->CompletedPassengers,
				Signal.CompletedPassengers);
			Evidence->RegionalPathCount = Signal.RegionalPathCount;
			Evidence->AdvancedPathCount = Signal.AdvancedPathCount;
			Evidence->SharedResourceDays = Signal.SharedResourceDays;
			Evidence->bPrimaryTenantActive =
				Evidence->bPrimaryTenantActive ||
				Signal.bPrimaryTenantActive;
			Evidence->bSecondaryProviderActive =
				Evidence->bSecondaryProviderActive ||
				Signal.bSecondaryProviderActive;
			Evidence->bSignatureFacilityOperational =
				Evidence->bSignatureFacilityOperational ||
				Signal.bSignatureFacilityOperational;
			for (const FName FacilityId : Signal.FacilityIds)
			{
				Evidence->FacilityIds.AddUnique(FacilityId);
			}
			Evidence->MajorEvidenceIds = Signal.EvidenceIds;
			Evidence->OperationalStatus = Signal.OperationalStatus;
			Evidence->bMajorRequirementsMet =
				Signal.bMajorRequirementsMet &&
				MeetsMajorRequirements(*Evidence);
			if (Evidence->bMajorRequirementsMet)
			{
				Evidence->OperationalBand = ECapabilityBand::Major;
				Evidence->Band = ECapabilityBand::Major;
				PromoteEarnedBand(*Evidence, ECapabilityBand::Major);
				Evidence->OperationalStatus =
					ECapabilityOperationalStatus::Healthy;
				Evidence->NextRequirement =
					TEXT("Major earned; continue safe operation.");
			}
			else if (Evidence->EarnedBand == ECapabilityBand::Major)
			{
				Evidence->OperationalBand =
					Evidence->OperationalStatus ==
							ECapabilityOperationalStatus::Suspended
						? ECapabilityBand::Advanced
						: ECapabilityBand::Major;
				Evidence->Band = Evidence->OperationalBand;
				Evidence->NextRequirement =
					TEXT("Major earned; restore current operating capability.");
			}
			Evidence->CurrentEvidence = FString::Printf(
				TEXT("%d AP - %d qualifying days - %d operations - %s"),
				Evidence->AirportPoints,
				Evidence->QualifyingOperatingDays,
				Evidence->CompletedOperations,
				*CapabilityBandDisplayName(Evidence->EarnedBand));
			if (Evidence->EarnedBand == ECapabilityBand::Major)
			{
				++State.MajorPathCount;
			}
			if (PreviousEarned != Evidence->EarnedBand)
			{
				Emit(
					EPhase5EventType::CapabilityBandChanged,
					{},
					static_cast<uint64>(Evidence->Path),
					CurrentGameMilliseconds,
					FString::Printf(
						TEXT("%s earned Major capability."),
						*SpecializationPathDisplayName(Evidence->Path)));
			}
		}
	}
}
