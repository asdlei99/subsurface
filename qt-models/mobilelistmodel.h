// SPDX-License-Identifier: GPL-2.0
// This header files declares two linear models used by the mobile UI.
//
// MobileListModel presents a list of trips and optionally the dives of
// one expanded trip. It is used for quick navigation through trips.
//
// MobileSwipeModel gives a linearized view of all dives, sorted by
// trip. Even if there is temporal overlap of trips, all dives of
// a trip are listed in a contiguous block. This model is used for
// swiping through dives.
#ifndef MOBILELISTMODEL_H
#define MOBILELISTMODEL_H

#include "divetripmodel.h"

// This is the base class of the mobile-list model. All it does
// is exporting the various dive fields as roles.
class MobileListModelBase : public QAbstractItemModel {
	Q_OBJECT
public:
	enum Roles {
		IsTopLevelRole = DiveTripModelBase::LAST_ROLE + 1,
		DiveDateRole,
		TripIdRole,
		TripNrDivesRole,
		TripShortDateRole,
		TripTitleRole,
		DateTimeRole,
		IdRole,
		NumberRole,
		LocationRole,
		DepthRole,
		DurationRole,
		DepthDurationRole,
		RatingRole,
		VizRole,
		SuitRole,
		AirTempRole,
		WaterTempRole,
		SacRole,
		SumWeightRole,
		DiveMasterRole,
		BuddyRole,
		NotesRole,
		GpsDecimalRole,
		GpsRole,
		NoDiveRole,
		DiveSiteRole,
		CylinderRole,
		GetCylinderRole,
		CylinderListRole,
		SingleWeightRole,
		StartPressureRole,
		EndPressureRole,
		FirstGasRole,
		CollapsedRole,
		SelectedRole,
		DiveInTripRole,
		TripAbove,
		TripBelow,
	};
	QHash<int, QByteArray> roleNames() const override;
protected:
	DiveTripModelBase *source;
	MobileListModelBase(DiveTripModelBase *source);
private:
	int columnCount(const QModelIndex &parent) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &index) const override;
};

class MobileListModel : public MobileListModelBase {
	Q_OBJECT
public:
	MobileListModel(DiveTripModelBase *source);
	void expand(int row);
	void unexpand();
	void toggle(int row);
private:
	struct IndexRange {
		bool visible;
		int first, last;
	};
	std::vector<IndexRange> rangeStack;
	QModelIndex sourceIndex(int row, int col, int parentRow = -1) const;
	int numSubItems() const;
	int mapRowFromSourceTopLevel(int row) const;
	int mapRowFromSourceTopLevelForInsert(int row) const;
	int mapRowFromSourceTrip(const QModelIndex &parent, int parentRow, int row) const;
	int mapRowFromSource(const QModelIndex &parent, int row) const;
	int invertRow(const QModelIndex &parent, int row) const;
	IndexRange mapRangeFromSource(const QModelIndex &parent, int first, int last) const;
	IndexRange mapRangeFromSourceForInsert(const QModelIndex &parent, int first, int last) const;
	QModelIndex mapFromSource(const QModelIndex &idx) const;
	QModelIndex mapToSource(const QModelIndex &idx) const;
	static void updateRowAfterRemove(const IndexRange &range, int &row);
	static void updateRowAfterMove(const IndexRange &range, const IndexRange &dest, int &row);
	QVariant data(const QModelIndex &index, int role) const override;
	int rowCount(const QModelIndex &parent) const override;

	int expandedRow;
private slots:
	void prepareRemove(const QModelIndex &parent, int first, int last);
	void doneRemove(const QModelIndex &parent, int first, int last);
	void prepareInsert(const QModelIndex &parent, int first, int last);
	void doneInsert(const QModelIndex &parent, int first, int last);
	void prepareMove(const QModelIndex &parent, int first, int last, const QModelIndex &dest, int destRow);
	void doneMove(const QModelIndex &parent, int first, int last, const QModelIndex &dest, int destRow);
	void changed(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
};

class MobileSwipeModel : public MobileListModelBase {
	Q_OBJECT
public:
	MobileSwipeModel(DiveTripModelBase *source);
	static MobileSwipeModel *instance();
	void resetModel(DiveTripModelBase::Layout layout);	// Switch between tree and list view
private:
	std::vector<int> firstElement; // First element of top level item.
	int rows;
	QVariant data(const QModelIndex &index, int role) const override;
	int rowCount(const QModelIndex &parent) const override;

	// Since accesses to data come in bursts, we cache map-to-source lookup.
	// Note that this is not thread safe. We suppose that the model is only ever accessed from the UI thread.
	mutable int cachedRow = -1;
	mutable QModelIndex cacheSourceParent;
	mutable int cacheSourceRow = -1;

	// Translate indexes from/to source
	int topLevelRowCountInSource(int sourceRow) const;
	QModelIndex mapToSource(const QModelIndex &index) const;
	int mapTopLevelFromSource(int row) const;
	int mapTopLevelFromSourceForInsert(int row) const;
	int elementCountInTopLevel(int row) const;
	int mapRowFromSource(const QModelIndex &parent, int row) const;
	int mapRowFromSource(const QModelIndex &parent) const;
	int mapRowFromSourceForInsert(const QModelIndex &parent, int row) const;
	void invalidateSourceRowCache() const;
	void updateSourceRowCache(int row) const;

	// Update elements
	void initData();
	int removeTopLevel(int begin, int end);
	void addTopLevel(int row, std::vector<int> items);
	void updateTopLevel(int row, int delta);
signals:
	void currentDiveChanged(QModelIndex index);
private slots:
	void doneReset();
	void prepareRemove(const QModelIndex &parent, int first, int last);
	void doneRemove(const QModelIndex &parent, int first, int last);
	void prepareInsert(const QModelIndex &parent, int first, int last);
	void doneInsert(const QModelIndex &parent, int first, int last);
	void prepareMove(const QModelIndex &parent, int first, int last, const QModelIndex &dest, int destRow);
	void doneMove(const QModelIndex &parent, int first, int last, const QModelIndex &dest, int destRow);
	void changed(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
};

// This convenience class provides access to the two mobile models.
// Moreover, it provides an interface to the source trip-model.
class MobileModels {
public:
	static MobileModels *instance();
	MobileListModel *listModel();
	MobileSwipeModel *swipeModel();
	void clear(); // Clear all dive data
	void reset(); // Reset model after having reloaded the core data
private:
	MobileModels();
	DiveTripModelTree source;
	MobileListModel lm;
	MobileSwipeModel sm;
};

// Helper functions - these are actually defined in DiveObjectHelper.cpp. Why declare them here?
QString formatSac(const dive *d);
QString formatNotes(const dive *d);
QString format_gps_decimal(const dive *d);
QStringList formatGetCylinder(const dive *d);
QStringList getStartPressure(const dive *d);
QStringList getEndPressure(const dive *d);
QStringList getFirstGas(const dive *d);
QStringList getFullCylinderList();

#endif
