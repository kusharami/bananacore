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

#include "QtnProperty/IQtnPropertyStateProvider.h"

#include <QObject>
#include <QVariantMap>

#include <bitset>
#include <vector>

class QMimeData;

namespace Banana
{
struct IUndoStack;

extern const char szOBJECT_NAME_KEY[];
extern const char szCLASS_NAME_KEY[];
extern const char szCHILDREN_KEY[];

typedef std::vector<const char *> PropertyNames;
typedef std::vector<QMetaProperty> QMetaPropertyVec;

class Object
	: public QObject
	, public IQtnPropertyStateProvider
{
	Q_OBJECT

public:
	enum SaveMode
	{
		SavePrototyped,
		SaveStandaloneInheritedChild,
		SaveStandalone,
	};

	struct LockUnlock
	{
		QStringList locked;
		QStringList unlocked;
	};
	using LockUnlockByKey = QHash<QString, LockUnlock>;

	explicit Object();
	virtual ~Object() override;

	virtual QtnPropertyState getPropertyState(
		const QMetaProperty &metaProperty) const override;

	bool isPropertyLocked(const QMetaProperty &metaProperty) const;
	void setPropertyLocked(const QMetaProperty &metaProperty, bool locked);

	template <typename T, typename... ARG_T>
	static T *create(QObject *parent, ARG_T... args);

	template <typename CLASS>
	CLASS *getPrototypeAs() const;

	Q_INVOKABLE inline QObject *getPrototype() const;
	void setPrototype(Object *prototype);

	static bool loadContents(const QVariantMap &source, QObject *destination,
		bool skipObjectName, LockUnlock *lockUnlock = nullptr);
	static void saveContents(const QObject *source, QVariantMap &destination,
		QObject *prototype = nullptr);

	virtual bool loadContents(const QVariantMap &source, bool skipObjectName);
	virtual bool loadContents(const QVariantMap &source, bool skipObjectName,
		LockUnlockByKey &lockUnlockMap,
		const QStringList &path = QStringList());
	virtual void saveContents(
		QVariantMap &destination, SaveMode saveMode = SavePrototyped) const;

	// dot separated
	Object *findDescendant(const QString &path);
	Object *findDescendant(const QStringList &path);

	Q_INVOKABLE QVariantMap backupContents() const;
	Q_INVOKABLE void applyContents(const QVariantMap &source);

	inline bool ownsUndoStack() const;
	inline IUndoStack *getUndoStack() const;
	void setUndoStack(IUndoStack *undoStack, bool own = false);
	void beginMacro(const QString &text);
	void endMacro();
	inline unsigned blockMacroCount() const;
	void blockMacro();
	void unblockMacro();
	bool undoStackIsUpdating() const;
	template <typename T>
	inline void pushUndoCommand(const char *propertyName, const T &oldValue);
	bool canPushUndoCommand() const;

	void addChildCommand(QObject *child);
	void moveChildCommand(
		QObject *child, QObject *oldParent, const QString &oldName = QString());
	void deleteChildCommand(QObject *child);

	Q_INVOKABLE void assign(QObject *source);

	Q_INVOKABLE void removeAllChildren();

	Q_INVOKABLE bool isAncestorOf(const QObject *object) const;
	Q_INVOKABLE inline bool isDescendantOf(const QObject *object) const;

	Q_INVOKABLE inline bool isLoading() const;

	inline void beginLoad();
	inline void endLoad();

	Q_INVOKABLE inline bool isInheritedChild() const;
	Q_INVOKABLE inline bool isPrototypedRoot() const;
	Q_INVOKABLE inline bool isPrototyped() const;
	virtual bool canBeUsedAsPrototype(Object *object) const;

	Q_INVOKABLE inline bool isModified() const;
	void setModified(bool value);
	void modify();
	static void modifyObject(
		QObject *object, bool modified, bool signalize, bool children);

	inline bool isDeleted() const;

	void beginReload();
	void endReload();
	void beginUndoStackUpdate();
	void endUndoStackUpdate();

	QObject *getDescendant(const QStringList &path) const;
	QStringList getNamesChain(const QObject *topAncestor = nullptr) const;

	inline bool isPropertyModified(int propertyId) const;
	bool setPropertyModified(int propertyId, bool modified);

	inline quint64 getPropertyModifiedBits() const;
	void setPropertyModifiedBits(quint64 propertyIdBits);

	QMetaPropertyVec getPropertyListAffectedBy(
		const QMetaProperty &metaProperty) const;

	void deprototype();
	static bool isDescendantOf(const QObject *ancestor, const QObject *object);

signals:
	void propertyStateChanged(const QMetaProperty &metaProperty);
	void modifiedSetChanged();
	void modifiedFlagChanged(bool modified);
	void parentChanged();
	void reloadStarted();
	void reloadFinished();
	void childAdded(QObject *child);
	void childRemoved(QObject *child);
	void beforeDestroy(Object *object);
	void beforeDeleteByUser();

private slots:
	void onPrototypeChildAdded(QObject *protoChild);
	void onPrototypeChildRemoved(QObject *protoChild);
	void onPrototypeDestroyed(Object *object);
	void onPrototypeReloadStarted();
	void onPrototypeReloadFinished();
	void onLinkedObjectNameChanged(const QString &name);
	void onObjectNameChanged(const QString &newName);
	void onUndoStackCleanChanged(bool clean);

private:
	void pushUndoCommandInternal(
		const char *propertyName, const QVariant &oldValue);
	Object *getMainPrototype() const;
	void internalAssign(QObject *source, bool fresh, bool top);
	void internalSetPrototype(Object *prototype, bool child, bool fresh);
	void connectChildPrototypeDestroy();
	void disconnectChildPrototypeDestroy();
	void connectPrototypeChildLifeCycle();
	void disconnectPrototypeChildLifeCycle();
	void connectPrototype();
	void disconnectPrototype();
	void connectChildPrototype();
	void disconnectChildPrototype();
	bool checkPrototypeCycling(const Object *object) const;
	Object *assignChild(QObject *sourceChild, bool is_prototype = true);
	void beforePrototypeReloadStarted();
	void beforeChildPrototypeReloadStarted();
	void beforePrototypeChange();
	void afterPrototypeChange();

	void setPropertyLocksForce(const QStringList &propertyNames, bool locked);
	void setPropertyLockedForce(
		const QMetaProperty &metaProperty, bool locked, bool force);
	void setPropertyStateForce(
		const QMetaProperty &metaProperty, QtnPropertyState state);

protected:
	virtual void setPropertyState(
		const QMetaProperty &metaProperty, QtnPropertyState state) override;
	virtual void emitPropertyStateChanged(const QMetaProperty &metaProperty);

	void removeAllChildrenInternal();
	bool shouldSwapModifiedFieldsFor(QObject *source) const;
	virtual bool canAssignPropertyFrom(QObject *source, int propertyId) const;
	virtual void doConnectPrototype();
	virtual void doDisconnectPrototype();
	virtual bool assignBegin(QObject *source, bool top);
	virtual void assignEnd(QObject *source, bool top);
	virtual void assignChildren(QObject *source);
	virtual void assignProperties(QObject *source);
	void newChildFrom(QObject *source, bool childProto);
	virtual const QMetaObject *getStorableChildType(
		const QString &className) const;
	virtual bool isStorableChildType(const QMetaObject *type) const;
	virtual void childEvent(QChildEvent *event) override;
	virtual void doParentChange();
	virtual void doFlagsChanged();
	virtual void doAddChild(QObject *object);
	virtual void doRemoveChild(QObject *object);

	void connectUndoStack();
	void disconnectUndoStack();
	static void getDescendants(QObject *obj, QObjectList &out);

	QHash<int, QtnPropertyState> propertyStates;
	QString oldName;
	Object *prototype;
	Object *childPrototype;
	unsigned reloadCounter;
	unsigned protoReloadCounter;
	unsigned loadCounter;
	unsigned macroCounter;
	unsigned blockCounter;
	unsigned undoStackUpdate;
	IUndoStack *undoStack;
	bool ownUndoStack : 1;
	bool modified : 1;
	bool deleted : 1;

private:
	typedef std::bitset<64> ModifiedSet;
	ModifiedSet modifiedSet;
};

QObject *Object::getPrototype() const
{
	return prototype;
}

bool Object::ownsUndoStack() const
{
	return ownUndoStack;
}

IUndoStack *Object::getUndoStack() const
{
	return undoStack;
}

unsigned Object::blockMacroCount() const
{
	return blockCounter;
}

bool Object::isDescendantOf(const QObject *object) const
{
	return isDescendantOf(object, this);
}

template <typename T>
void Object::pushUndoCommand(const char *propertyName, const T &oldValue)
{
	if (canPushUndoCommand())
		pushUndoCommandInternal(propertyName, QVariant::fromValue<T>(oldValue));
}

bool Object::isLoading() const
{
	return loadCounter > 0;
}

void Object::beginLoad()
{
	loadCounter++;
}

void Object::endLoad()
{
	Q_ASSERT(loadCounter > 0);
	loadCounter--;
}

bool Object::isInheritedChild() const
{
	return (nullptr != childPrototype);
}

bool Object::isPrototypedRoot() const
{
	return (nullptr != prototype && childPrototype != prototype);
}

bool Object::isPrototyped() const
{
	return (nullptr != prototype);
}

bool Object::isModified() const
{
	return modified;
}

bool Object::isDeleted() const
{
	return deleted;
}

bool Object::isPropertyModified(int propertyId) const
{
	return modifiedSet.test(propertyId);
}

quint64 Object::getPropertyModifiedBits() const
{
	return modifiedSet.to_ullong();
}

template <typename T, typename... ARG_T>
T *Object::create(QObject *parent, ARG_T... args)
{
	auto result = new T(args...);

	result->setParent(parent);

	return result;
}

template <typename CLASS>
CLASS *Object::getPrototypeAs() const
{
	if (prototype != nullptr)
		Q_ASSERT(nullptr != dynamic_cast<CLASS *>(prototype));

	return static_cast<CLASS *>(prototype);
}
}

Q_DECLARE_METATYPE(Banana::Object *)
