// SPDX-License-Identifier: GPL-2.0
#ifndef MOBILEFILTERMODEL_H
#define MOBILEFILTERMODEL_H

#include "divetripmodel.h"
#include <QSortFilterProxyModel>

class MobileFilterModel : public QSortFilterProxyModel {
	Q_OBJECT
public:
	static MobileFilterModel *instance();
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

	void toggle(int row);
	void expand(int row);
	Q_INVOKABLE QVariantMap get(int row) const;
	Q_INVOKABLE int shown(); // number dives that are accepted by the filter
private:
	int mapRowToSource(int row);
	MobileFilterModel();
};

#endif
