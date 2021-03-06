// SPDX-License-Identifier: GPL-2.0
#include "qt-models/filtermodels.h"
#include "core/display.h"
#include "core/qthelper.h"
#include "core/trip.h"
#include "core/subsurface-string.h"
#include "core/subsurface-qt/divelistnotifier.h"
#include "qt-models/divetripmodel.h"

MultiFilterSortModel *MultiFilterSortModel::instance()
{
	static MultiFilterSortModel self;
	return &self;
}

MultiFilterSortModel::MultiFilterSortModel(QObject *parent) : QSortFilterProxyModel(parent)
{
	resetModel(DiveTripModelBase::TREE);
}

void MultiFilterSortModel::resetModel(DiveTripModelBase::Layout layout)
{
	if (layout == DiveTripModelBase::TREE)
		model.reset(new DiveTripModelTree);
	else
		model.reset(new DiveTripModelList);

	setSourceModel(model.get());
	connect(model.get(), &DiveTripModelBase::selectionChanged, this, &MultiFilterSortModel::selectionChangedSlot);
	connect(model.get(), &DiveTripModelBase::currentDiveChanged, this, &MultiFilterSortModel::currentDiveChangedSlot);
	model->initSelection();
}

void MultiFilterSortModel::clear()
{
	model->clear();
}

// Translate selection into local indexes and re-emit signal
void MultiFilterSortModel::selectionChangedSlot(const QVector<QModelIndex> &indexes)
{
	QVector<QModelIndex> indexesLocal;
	indexesLocal.reserve(indexes.size());
	for (const QModelIndex &index: indexes) {
		QModelIndex local = mapFromSource(index);
		if (local.isValid())
			indexesLocal.push_back(local);
	}
	emit selectionChanged(indexesLocal);
}

// Translate current dive into local indexes and re-emit signal
void MultiFilterSortModel::currentDiveChangedSlot(QModelIndex index)
{
	QModelIndex local = mapFromSource(index);
	if (local.isValid())
		emit currentDiveChanged(mapFromSource(index));
}

bool MultiFilterSortModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	return true;
}

bool MultiFilterSortModel::lessThan(const QModelIndex &i1, const QModelIndex &i2) const
{
	// Hand sorting down to the source model.
	return model->lessThan(i1, i2);
}
