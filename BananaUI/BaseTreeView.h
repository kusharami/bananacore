/*
 * MIT License
 *
 * Copyright (c) 2016 Alexandra Cherdantseva
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <QTreeView>

#include <set>

namespace Banana
{
	class AbstractObjectTreeModel;


class BaseTreeView : public QTreeView
{
	Q_OBJECT

public:
	explicit BaseTreeView(Banana::AbstractObjectTreeModel *model,
						  QWidget *parent = nullptr);

	void select(QObject *item, bool expand = false);
	void expandItem(QObject *item);

	QObject *getCurrentItem() const;

	bool hasItems() const;

	void cutToClipboard();
	void copyToClipboard();
	void pasteFromClipboard();
	void deleteSelectedItems();

	bool canDeleteItem(QObject *item) const;
	bool canDeleteSelectedItems() const;

	void preventReselect(bool prevent);

protected slots:
	virtual void onAfterModelReset();

private slots:
	void onShouldSelect(const QItemSelection &selection);
	void onExpanded(const QModelIndex &index);
	void onExpandedItemDestroyed(QObject *item);
	void onSelectedItemDestroyed(QObject *item);
	void onCollapsed(const QModelIndex &index);
	void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

protected:
	Banana::AbstractObjectTreeModel *treeModel;
	typedef std::set<QObject *> ItemSet;
	ItemSet expandedItems;
	ItemSet selectedItems;

	unsigned preventReselectCounter;
};
}