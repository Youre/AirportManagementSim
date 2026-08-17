#pragma once

#include "CoreMinimal.h"

namespace AMSim::MatureAirportLayout
{
	struct FLinearConnection
	{
		FVector2D Center;
		double Length = 0.0;
	};

	struct FFacility
	{
		FVector2D Center;
		FVector2D Forward;
		double Length = 0.0;
		double Width = 0.0;
	};

	struct FAxisAlignedFootprint
	{
		double MinimumX = 0.0;
		double MinimumY = 0.0;
		double MaximumX = 0.0;
		double MaximumY = 0.0;

		bool Intersects(const FAxisAlignedFootprint& Other) const
		{
			return MinimumX < Other.MaximumX && MaximumX > Other.MinimumX &&
				MinimumY < Other.MaximumY && MaximumY > Other.MinimumY;
		}
	};

	inline constexpr double TerminalCenterWorldX = -28000.0;
	inline constexpr double TerminalCenterWorldY = 0.0;
	inline constexpr double RunwayCenterWorldX = 38000.0;
	inline constexpr double RunwayHalfLength = 60000.0;
	inline constexpr double RunwayWidth = 11000.0;
	inline constexpr double TaxiwayCenterWorldX = 27000.0;
	inline constexpr double TaxiwayHalfLength = 54000.0;
	inline constexpr double TaxiwayWidth = 6200.0;
	inline constexpr double ApronCenterWorldX = 9000.0;
	inline constexpr double ApronLength = 78000.0;
	inline constexpr double ApronWidth = 30000.0;
	inline constexpr double AccessRoadCenterWorldX = -50000.0;
	inline constexpr double AccessRoadHalfLength = 56000.0;
	inline constexpr double AccessRoadWidth = 4600.0;
	inline constexpr double GateCenterWorldX = -9800.0;
	inline constexpr double GateLength = 13000.0;
	inline constexpr double GateWidth = 9000.0;
	inline constexpr double GateCenterWorldY[] = {-15000.0, 0.0, 15000.0};
	inline constexpr int32 FacilityCount = 8;

	inline TConstArrayView<FLinearConnection> GetAirsideConnections()
	{
		static const FLinearConnection Connections[] = {
			{{32500.0, -42000.0}, 11000.0},
			{{32500.0, 0.0}, 11000.0},
			{{32500.0, 42000.0}, 11000.0},
			{{25500.0, -26000.0}, 7000.0},
			{{25500.0, 0.0}, 7000.0},
			{{25500.0, 26000.0}, 7000.0}};
		return MakeArrayView(Connections);
	}

	inline TConstArrayView<FLinearConnection> GetLandsideConnections()
	{
		static const FLinearConnection Connections[] = {
			{{-46200.0, -12000.0}, 7600.0},
			{{-46200.0, 12000.0}, 7600.0},
			{{-45250.0, -39000.0}, 9500.0},
			{{-43750.0, 34000.0}, 12500.0},
			{{-45000.0, 0.0}, 10000.0},
			{{-41625.0, 52000.0}, 16750.0},
			{{-45000.0, -50000.0}, 10000.0}};
		return MakeArrayView(Connections);
	}

	inline TConstArrayView<FFacility> GetFacilities()
	{
		static const FFacility Facilities[] = {
			{{9000.0, -49000.0}, {0.0, 1.0}, 17000.0, 12000.0},
			{{9000.0, 48000.0}, {0.0, 1.0}, 11000.0, 8000.0},
			{{19000.0, 49000.0}, {0.0, 1.0}, 9000.0, 7000.0},
			{{-31500.0, -39000.0}, {0.0, 1.0}, 24000.0, 15000.0},
			{{-31500.0, 34000.0}, {0.0, 1.0}, 15000.0, 10000.0},
			{{-30000.0, 52000.0}, {0.0, 1.0}, 13000.0, 6000.0},
			{{-45000.0, 0.0}, {0.0, 1.0}, 40000.0, 3000.0},
			{{-50000.0, -50000.0}, {0.0, 1.0}, 6000.0, 3000.0}};
		return MakeArrayView(Facilities);
	}

	inline FAxisAlignedFootprint MakeFootprint(const FFacility& Facility)
	{
		return {
			Facility.Center.X - Facility.Width * 0.5,
			Facility.Center.Y - Facility.Length * 0.5,
			Facility.Center.X + Facility.Width * 0.5,
			Facility.Center.Y + Facility.Length * 0.5};
	}
}
