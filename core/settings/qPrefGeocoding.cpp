// SPDX-License-Identifier: GPL-2.0
#include "qPref.h"
#include "qPrefPrivate.h"

static const QString group = QStringLiteral("geocoding");

qPrefGeocoding::qPrefGeocoding(QObject *parent) : QObject(parent)
{
}
qPrefGeocoding *qPrefGeocoding::instance()
{
	static qPrefGeocoding *self = new qPrefGeocoding;
	return self;
}

void qPrefGeocoding::loadSync(bool doSync)
{
	disk_first_taxonomy_category(doSync);
	disk_second_taxonomy_category(doSync);
	disk_third_taxonomy_category(doSync);
}


void qPrefGeocoding::set_first_taxonomy_category(taxonomy_category value)
{
	if (value != prefs.geocoding.category[0]) {
		prefs.geocoding.category[0] = value;
		disk_first_taxonomy_category(true);
		emit first_taxonomy_category_changed(value);
	}
}
void qPrefGeocoding::disk_first_taxonomy_category(bool doSync)
{
	if (doSync)
		qPrefPrivate::instance()->setting.setValue(group + "/cat0", prefs.geocoding.category[0]);
	else
		prefs.geocoding.category[0] = (enum taxonomy_category)qPrefPrivate::instance()->setting.value(group + "/cat0", default_prefs.geocoding.category[0]).toInt();
}


void qPrefGeocoding::set_second_taxonomy_category(taxonomy_category value)
{
	if (value != prefs.geocoding.category[1]) {
		prefs.geocoding.category[1] = value;
		disk_second_taxonomy_category(true);
		emit second_taxonomy_category_changed(value);
	}
}
void qPrefGeocoding::disk_second_taxonomy_category(bool doSync)
{
	if (doSync)
		qPrefPrivate::instance()->setting.setValue(group + "/cat1", prefs.geocoding.category[1]);
	else
		prefs.geocoding.category[1] = (enum taxonomy_category)qPrefPrivate::instance()->setting.value(group + "/cat1", default_prefs.geocoding.category[1]).toInt();
}


void qPrefGeocoding::set_third_taxonomy_category(taxonomy_category value)
{
	if (value != prefs.geocoding.category[2]) {
		prefs.geocoding.category[2] = value;
		disk_third_taxonomy_category(true);
		emit third_taxonomy_category_changed(value);
	}
}
void qPrefGeocoding::disk_third_taxonomy_category(bool doSync)
{
	if (doSync)
		qPrefPrivate::instance()->setting.setValue(group + "/cat2", prefs.geocoding.category[2]);
	else
		prefs.geocoding.category[2] = (enum taxonomy_category)qPrefPrivate::instance()->setting.value(group + "/cat2", default_prefs.geocoding.category[2]).toInt();
}