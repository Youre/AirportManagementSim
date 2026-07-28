#include "AMSimTimetableGeometry.h"

namespace AMSim
{
	namespace TimetableGeometryPrivate
	{
		constexpr float DayRailLeft = 0.055f;
		constexpr float DayRailRight = 0.988f;
		constexpr float DayGap = 0.004f;
		constexpr float CompactLeft = 0.115f;
		constexpr float CompactRight = 0.965f;
		constexpr float MinimumCardHeight = 0.078f;
	}

	using namespace TimetableGeometryPrivate;

	float FTimetableGeometry::MinuteToAnchor(const int32 Minute)
	{
		const int32 ClampedMinute = FMath::Clamp(
			Minute,
			FirstVisibleMinute,
			LastVisibleMinute);
		const float Alpha =
			static_cast<float>(ClampedMinute - FirstVisibleMinute) /
			static_cast<float>(LastVisibleMinute - FirstVisibleMinute);
		return FMath::Lerp(HeaderBottom, GridBottom, Alpha);
	}

	float FTimetableGeometry::DayColumnLeft(const int32 DayIndex)
	{
		const int32 ClampedDay = FMath::Clamp(DayIndex, 1, 7);
		const float ColumnWidth = (DayRailRight - DayRailLeft) / 7.0f;
		return DayRailLeft + (ClampedDay - 1) * ColumnWidth + DayGap;
	}

	float FTimetableGeometry::DayColumnRight(const int32 DayIndex)
	{
		const float ColumnWidth = (DayRailRight - DayRailLeft) / 7.0f;
		return DayColumnLeft(DayIndex) + ColumnWidth - 2.0f * DayGap;
	}

	FTimetableCardGeometry FTimetableGeometry::MakeCard(
		const int32 DayIndex,
		const int32 ArrivalMinute,
		const int32 DepartureMinute,
		const bool bCompact)
	{
		FTimetableCardGeometry Geometry;
		Geometry.Left = bCompact
			? CompactLeft
			: DayColumnLeft(DayIndex);
		Geometry.Right = bCompact
			? CompactRight
			: DayColumnRight(DayIndex);
		Geometry.Top = MinuteToAnchor(ArrivalMinute);
		Geometry.Bottom = FMath::Max(
			MinuteToAnchor(DepartureMinute),
			Geometry.Top + MinimumCardHeight);
		Geometry.Bottom = FMath::Min(Geometry.Bottom, GridBottom);
		return Geometry;
	}
}
