#include "AMSimPhase1Fixture.h"

namespace AMSim
{
	namespace
	{
		constexpr int64 ParcelMinimum = 0;
		constexpr int64 ParcelMaximum = 100000;
		constexpr int64 ConnectionTolerance = 500;

		bool IsInsideParcel(const FPhase1Point Point)
		{
			return Point.X >= ParcelMinimum &&
				Point.Y >= ParcelMinimum &&
				Point.X <= ParcelMaximum &&
				Point.Y <= ParcelMaximum;
		}

		int64 DistanceSquared(const FPhase1Point Left, const FPhase1Point Right)
		{
			const int64 DeltaX = Left.X - Right.X;
			const int64 DeltaY = Left.Y - Right.Y;
			return DeltaX * DeltaX + DeltaY * DeltaY;
		}

		FDefinition MakeDefinition(
			const TCHAR* StableId,
			const TCHAR* Tag,
			const EProvenanceStatus Provenance = EProvenanceStatus::InternalPrototype)
		{
			FDefinition Definition;
			Definition.StableId = StableId;
			Definition.OwningPhase = TEXT("Phase1");
			Definition.Tags.Add(Tag);
			Definition.Provenance = Provenance;
			Definition.SourcePath = TEXT("ProjectAuthored");
			Definition.SourceChecksum = TEXT("internal-phase1");
			return Definition;
		}
	}

	const FPhase1Fixture& GetPhase1Fixture()
	{
		static const FPhase1Fixture Fixture = []
		{
			FPhase1Fixture Value;
			Value.ContentDefinitions = {
				MakeDefinition(TEXT("Map.TemperateStarter"), TEXT("Facility.Type.Map")),
				MakeDefinition(TEXT("Aircraft.LightPiston.Starter"), TEXT("Aircraft.Role.GeneralAviation")),
				MakeDefinition(TEXT("Operator.RiverbendFlyingClub"), TEXT("Capability.Path.GeneralAviation")),
				MakeDefinition(TEXT("Facility.GrassRunway.Starter"), TEXT("Facility.Type.Runway")),
				MakeDefinition(TEXT("Facility.GrassTaxiway.Starter"), TEXT("Network.Type.AircraftMovement")),
				MakeDefinition(TEXT("Facility.GAStand.Starter"), TEXT("Facility.Type.Stand")),
				MakeDefinition(TEXT("Facility.OperationsHut.Starter"), TEXT("Facility.Type.Operations")),
				MakeDefinition(TEXT("Staff.Construction.Starter"), TEXT("Staff.Role.Construction")),
				MakeDefinition(TEXT("Staff.Ramp.Starter"), TEXT("Staff.Role.Ramp")),
				MakeDefinition(TEXT("Staff.Fueling.Starter"), TEXT("Staff.Role.Fueling"))
			};
			return Value;
		}();
		return Fixture;
	}

	FStarterPlanProposal CreateDefaultStarterPlan()
	{
		FStarterPlanProposal Proposal;
		Proposal.RunwayStart = {20000, 40000};
		Proposal.RunwayEnd = {70000, 40000};
		Proposal.RunwayWidthCentimeters = 2000;
		Proposal.TaxiStart = {45000, 40000};
		Proposal.TaxiEnd = {45000, 55000};
		Proposal.StandCenter = {45000, 55500};
		Proposal.AccessStart = {45000, 56000};
		Proposal.AccessEnd = {45000, 100000};
		Proposal.OperationsHutCenter = {50000, 60000};
		return Proposal;
	}

	FPhase1Validation ValidateStarterPlan(const FStarterPlanProposal& Proposal)
	{
		const FPhase1Fixture& Fixture = GetPhase1Fixture();
		FPhase1Validation Result;
		Result.QuotedCost = Fixture.StarterPlanCost;
		if (Proposal.RunwayWidthCentimeters < 1800 ||
			DistanceSquared(Proposal.RunwayStart, Proposal.RunwayEnd) < 40000ll * 40000ll)
		{
			Result.Result = EPhase1CommandResult::RejectedInvalidGeometry;
			Result.ReasonCode = TEXT("Build.Runway.TooSmall");
			Result.Cause = TEXT("The grass runway is too short or narrow for the starter aircraft.");
			Result.Remedy = TEXT("Use a runway at least 400 m long and 18 m wide.");
			return Result;
		}

		const TArray<FPhase1Point> RequiredPoints = {
			Proposal.RunwayStart,
			Proposal.RunwayEnd,
			Proposal.TaxiStart,
			Proposal.TaxiEnd,
			Proposal.StandCenter,
			Proposal.AccessStart,
			Proposal.AccessEnd,
			Proposal.OperationsHutCenter
		};
		for (const FPhase1Point Point : RequiredPoints)
		{
			if (!IsInsideParcel(Point))
			{
				Result.Result = EPhase1CommandResult::RejectedOutsideOwnedLand;
				Result.ReasonCode = TEXT("Build.OutsideOwnedLand");
				Result.Cause = TEXT("Part of the starter airfield is outside the owned parcel.");
				Result.Remedy = TEXT("Move the highlighted part inside the cyan parcel boundary.");
				return Result;
			}
		}

		const int64 ToleranceSquared = ConnectionTolerance * ConnectionTolerance;
		if (DistanceSquared(Proposal.TaxiStart, {Proposal.TaxiStart.X, Proposal.RunwayStart.Y}) > ToleranceSquared ||
			DistanceSquared(Proposal.TaxiEnd, Proposal.StandCenter) > ToleranceSquared ||
			DistanceSquared(Proposal.AccessStart, Proposal.StandCenter) > ToleranceSquared ||
			Proposal.AccessEnd.Y != ParcelMaximum)
		{
			Result.Result = EPhase1CommandResult::RejectedDisconnected;
			Result.ReasonCode = TEXT("Build.Network.Disconnected");
			Result.Cause = TEXT("The runway, taxi connection, stand, or external access is disconnected.");
			Result.Remedy = TEXT("Join each highlighted endpoint before committing the project.");
			return Result;
		}

		Result.bValid = true;
		Result.Result = EPhase1CommandResult::Accepted;
		Result.ReasonCode = TEXT("Build.Valid");
		Result.Cause = TEXT("The starter airfield fits the parcel and all required paths connect.");
		Result.Remedy = TEXT("Commit the project when ready.");
		return Result;
	}

	FDefinitionValidation ValidatePhase1FixtureDefinitions()
	{
		return ValidateDefinitionRegistry(GetPhase1Fixture().ContentDefinitions, true);
	}

	int32 CalculateContingencyPercent(const int64 StartingCredits, const int64 PlanCost)
	{
		if (StartingCredits <= 0 || PlanCost < 0 || PlanCost > StartingCredits)
		{
			return 0;
		}
		return static_cast<int32>(((StartingCredits - PlanCost) * 100) / StartingCredits);
	}
}
