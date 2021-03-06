/*******************************************************************************
Banana Qt Libraries

Copyright (c) 2016-2017 Alexandra Cherdantseva

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#pragma once

#include "QtnProperty/PropertyWidgetEx.h"
#include "QtnProperty/PropertyBase.h"
#include "QtnProperty/Enum.h"

#include "BananaScript/ScriptManager.h"

#include <QKeySequence>

#include <vector>

class ScriptEntriesWidget : public QtnPropertyWidgetEx
{
	Q_OBJECT

	enum
	{
		ENTRY_PROPERTY_ID = 1,
		TYPES_PROPERTY_ID,
		FILE_PATH_PROPERTY_ID,
		CAPTION_PROPERTY_ID,
		KEYSEQ_PROPERTY_ID
	};

	QtnPropertyDelegateInfo mFileDelegateInfo;
	QtnPropertyDelegateInfo mCaptionDelegateInfo;

	struct EnumItem
	{
		QtnEnumValueInfo valueInfo;
		const QMetaObject *metaObject;
	};

	using EnumItems = std::vector<EnumItem>;
	EnumItems mEnumItems;
	QtnEnumInfo mObjectTypeEnumInfo;

public:
	using Entry = Banana::ScriptManager::Entry;
	using Entries = Banana::ScriptManager::Entries;

	explicit ScriptEntriesWidget(QWidget *parent = nullptr);

	void setRootDirectory(const QString &rootDirectory);
	Entries entries() const;
	void setEntries(const Entries &entries);
	void addEntry(const Entry &entry);
	void clear();

	QtnPropertySet *getActiveEntryProperty() const;

	virtual bool canDeleteProperty(QtnPropertyBase *property) override;
	virtual bool canCutToClipboard() override;
	virtual void deleteProperty(QtnPropertyBase *property) override;

protected:
	virtual bool dataHasSupportedFormats(const QMimeData *data) override;
	virtual QMimeData *getPropertyDataForAction(
		QtnPropertyBase *property, Qt::DropAction dropAction) override;
	virtual bool applyPropertyData(const QMimeData *data,
		QtnPropertyBase *destination, QtnApplyPosition position) override;

private:
	static QString entryDisplayCaption(const QString &caption);
	static EnumItems objectTypeEnumItems();
	static bool enumItemLessThan(const EnumItem &a, const EnumItem &b);
	qint32 metaObjectsToFlags(
		const Banana::ScriptCommand::MetaObjects &metaObjects) const;
	Banana::ScriptCommand::MetaObjects metaObjectsFromFlags(qint32 flags) const;
	QtnPropertySet *newPropertySetForEntry(const Entry &entry) const;

	Entry entryFrom(QtnPropertySet *set) const;
};
