/*******************************************************************************
Banana Qt Libraries

Copyright (c) 2016 Alexandra Cherdantseva

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

#include "BananaCore/IUndoCommand.h"

#include <QObject>
#include <QUndoCommand>

namespace Banana
{
class AbstractObjectUndoCommand
	: public QObject
	, public QUndoCommand
	, public IUndoCommand
{
public:
	AbstractObjectUndoCommand(QObject *object);

protected:
	virtual void undo() override;
	virtual void redo() override;

	virtual QUndoCommand *qundoCommand() override;

	virtual void doUndo() = 0;
	virtual void doRedo() = 0;

	QObject *getObject() const;
	void fetchObject();

private slots:
	void onObjectDestroyed();

protected:
	void connectObject();
	void disconnectObject();

	QObject *object;

	struct PathEntry
	{
		QObject *object;
		QString objectName;
	};

	std::vector<PathEntry> objectPath;

	int fetchIndex;
	bool skipRedoOnPush;
};
}
