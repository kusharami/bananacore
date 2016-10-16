#include "BaseTreeView.h"

#include "AbstractObjectTreeModel.h"
#include "BananaCore/AbstractObjectGroup.h"

#include <QFocusEvent>

namespace Banana
{

BaseTreeView::BaseTreeView(Banana::AbstractObjectTreeModel *model, QWidget *parent)
	: QTreeView(parent)
	, treeModel(model)
	, preventReselectCounter(0)
{
	model->setParent(this);

	QObject::connect(this, &QTreeView::expanded,
					 this, &BaseTreeView::onExpanded);
	QObject::connect(this, &QTreeView::collapsed,
					 this, &BaseTreeView::onCollapsed);
	QObject::connect(treeModel, &AbstractObjectTreeModel::afterModelReset,
					 this, &BaseTreeView::onAfterModelReset);
	QObject::connect(treeModel, &AbstractObjectTreeModel::shouldSelect,
					 this, &BaseTreeView::onShouldSelect);
	QObject::connect(treeModel, &AbstractObjectTreeModel::shouldClearSelection,
					 this, &BaseTreeView::clearSelection);

	setModel(treeModel);

	QObject::connect(selectionModel(), &QItemSelectionModel::selectionChanged,
					 this, &BaseTreeView::onSelectionChanged);
}

void BaseTreeView::select(QObject *item, bool expand)
{
	auto index = treeModel->findModelIndex(item);
	if (index.isValid())
	{
		auto sel_model = selectionModel();
		sel_model->clearSelection();
		setCurrentIndex(index);
		sel_model->select(index, QItemSelectionModel::Select);
		if (expand)
			this->expand(index);

		scrollTo(index);
	}
}

void BaseTreeView::expandItem(QObject *item)
{
	auto index = treeModel->findModelIndex(item);
	if (index.isValid())
		expand(index);
}

QObject *BaseTreeView::getCurrentItem() const
{
	auto index = currentIndex();
	if (index.isValid())
	{
		return treeModel->getItemAt(index);
	}

	return nullptr;
}

bool BaseTreeView::hasItems() const
{
	return (nullptr != treeModel && treeModel->rowCount() > 0);
}

void BaseTreeView::cutToClipboard()
{
	treeModel->copyToClipboard(selectionModel()->selectedIndexes(), true);
}

void BaseTreeView::copyToClipboard()
{
	treeModel->copyToClipboard(selectionModel()->selectedIndexes(), false);
}

void BaseTreeView::pasteFromClipboard()
{
	treeModel->pasteFromClipboard(selectionModel()->selectedIndexes());
}

void BaseTreeView::deleteSelectedItems()
{
	treeModel->deleteItems(selectionModel()->selectedIndexes());
}

bool BaseTreeView::canDeleteItem(QObject *item) const
{
	return treeModel->canDeleteItem(item);
}

bool BaseTreeView::canDeleteSelectedItems() const
{
	return treeModel->canDeleteItems(selectionModel()->selectedIndexes());
}

void BaseTreeView::preventReselect(bool prevent)
{
	if (prevent)
		preventReselectCounter++;
	else
		preventReselectCounter--;
}

void BaseTreeView::onAfterModelReset()
{
	preventReselectCounter++;

	ItemSet items;
	items.swap(expandedItems);

	for (auto &item : items)
	{
		auto index = treeModel->findModelIndex(item);
		if (index.isValid())
			expand(index);
	}

	items.clear();
	items.swap(selectedItems);

	QItemSelection selection;

	QModelIndex firstIndex;

	for (auto &item : items)
	{
		auto index = treeModel->findModelIndex(item);
		if (index.isValid())
		{
			if (!firstIndex.isValid())
				firstIndex = index;
			selection.select(index, index);
		}
	}

	if (firstIndex.isValid())
		setCurrentIndex(firstIndex);
	selectionModel()->select(selection, QItemSelectionModel::Select);

	preventReselectCounter--;
}

void BaseTreeView::onShouldSelect(const QItemSelection &selection)
{
	if (!selection.isEmpty())
	{
		auto indexes = selection.indexes();

		setCurrentIndex(indexes.at(0));

		for (auto &index : indexes)
		{
			if (index.parent().isValid())
				expand(index.parent());
		}

		selectionModel()->select(selection, QItemSelectionModel::Select);
	}
}

void BaseTreeView::onExpanded(const QModelIndex &index)
{
	auto item = treeModel->getItemAt(index);
	expandedItems.insert(item);
	QObject::connect(item, &QObject::destroyed,
					 this, &BaseTreeView::onExpandedItemDestroyed);
}

void BaseTreeView::onExpandedItemDestroyed(QObject *item)
{
	expandedItems.erase(item);
}

void BaseTreeView::onSelectedItemDestroyed(QObject *item)
{
	selectedItems.erase(item);
}

void BaseTreeView::onCollapsed(const QModelIndex &index)
{
	auto item = treeModel->getItemAt(index);
	expandedItems.erase(item);
	QObject::disconnect(item, &QObject::destroyed,
						this, &BaseTreeView::onExpandedItemDestroyed);
}

void BaseTreeView::onSelectionChanged(const QItemSelection &selected,
									  const QItemSelection &deselected)
{
	preventReselectCounter++;

	auto indexes = selected.indexes();

	for (auto it = indexes.begin(); it != indexes.end(); ++it)
	{
		auto item = treeModel->getItemAt(*it);
		selectedItems.insert(item);
		QObject::connect(item, &QObject::destroyed,
						 this, &BaseTreeView::onSelectedItemDestroyed);
	}

	indexes = deselected.indexes();

	for (auto it = indexes.begin(); it != indexes.end(); ++it)
	{
		auto item = treeModel->getItemAt(*it);
		selectedItems.erase(item);
		QObject::disconnect(item, &QObject::destroyed,
							this, &BaseTreeView::onSelectedItemDestroyed);
	}

	preventReselectCounter--;
}
}
