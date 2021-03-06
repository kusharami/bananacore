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

#include "AbstractFileSystemObject.h"

#include "Object.h"
#include "AbstractDirectory.h"
#include "Utils.h"

#include <QDir>
#include <QObject>
#include <QDebug>

namespace Banana
{
AbstractFileSystemObject::AbstractFileSystemObject(QObject *thiz)
	: thiz(thiz)
{
	Q_ASSERT(nullptr != thiz);
}

AbstractFileSystemObject::~AbstractFileSystemObject()
{
	// do nothing
}

bool AbstractFileSystemObject::isSymLink() const
{
	QFileInfo info(getFilePath());
	return info.isSymLink();
}

QString AbstractFileSystemObject::getSymLinkTarget() const
{
	QFileInfo info(getFilePath());
	return info.symLinkTarget();
}

AbstractDirectory *AbstractFileSystemObject::getTopDirectory() const
{
	auto obj = dynamic_cast<AbstractFileSystemObject *>(thiz);

	while (nullptr != obj)
	{
		auto parent = obj->getParentDirectory();

		if (nullptr == parent)
			return dynamic_cast<AbstractDirectory *>(obj);

		obj = parent;
	}

	return dynamic_cast<AbstractDirectory *>(thiz);
}

AbstractDirectory *AbstractFileSystemObject::getParentDirectory() const
{
	return dynamic_cast<AbstractDirectory *>(thiz->parent());
}

QString AbstractFileSystemObject::getFilePath(
	const AbstractDirectory *relative_to) const
{
	QString path;
	auto fileName = getFileName();
	if (QDir::isAbsolutePath(fileName))
	{
		if (nullptr != relative_to)
			path = QDir(relative_to->getFilePath()).relativeFilePath(fileName);
		else
			path = fileName;
	} else
	{
		auto parent_dir = getParentDirectory();
		if (nullptr != parent_dir && relative_to != parent_dir)
			path = parent_dir->getFilePath(relative_to);

		if (fileName.isEmpty())
			return QDir::cleanPath(path) + "/";

		path = QDir(path).filePath(fileName);
	}

	return QDir::cleanPath(path);
}

QString AbstractFileSystemObject::getCanonicalFilePath() const
{
	QFileInfo info(getFilePath());
	if (info.exists())
		return info.canonicalFilePath();

	return info.filePath();
}

QString AbstractFileSystemObject::getFileName() const
{
	return thiz->objectName() + getFileExtension();
}

QString AbstractFileSystemObject::getBaseName() const
{
	QString filename(getFileName());

	return QString(
		filename.constData(), filename.length() - getFileExtension().length());
}

bool AbstractFileSystemObject::rename(const QString &new_name)
{
	if (!new_name.isEmpty())
	{
		return setFileName(new_name);
	}

	updateFileNameError(new_name);
	return false;
}

bool AbstractFileSystemObject::setFileName(const QString &value)
{
	bool result = false;
	QString newFileName(Utils::ConvertToFileName(value));
	if (newFileName == value)
	{
		auto file = dynamic_cast<Object *>(thiz);
		Q_ASSERT(nullptr != file);
		bool modified = file->isModified();
		QString oldFileName(getFileName());
		if (oldFileName != newFileName)
		{
			QString oldExtension(getFileExtension());
			QString newExtension;
			if (updateFileExtension(newFileName, &newExtension))
			{
				if (!newExtension.isEmpty())
				{
					newFileName.setUnicode(newFileName.data(),
						newFileName.length() - newExtension.length());
				}

				bool symLink = isSymLink();
				thiz->blockSignals(true);
				thiz->setObjectName(QString());
				thiz->blockSignals(false);
				thiz->setObjectName(newFileName);

				file->setModified(modified ||
					(!symLink &&
						0 !=
							oldExtension.compare(
								newExtension, Qt::CaseInsensitive)));

				auto dir = getParentDirectory();
				if (nullptr != dir)
					dir->descendantChanged(thiz, DescendantState::Renamed);

				result = true;
			} else
				updateFileNameError(value);
		}
	}

	return result;
}

bool AbstractFileSystemObject::updateFileExtension(
	const QString &filename, QString *outExt)
{
	bool found = false;
	QString extension;
	for (auto &ext : getSupportedExtensions())
	{
		if (ext.isEmpty())
			found = true;
		else if (filename.endsWith(ext, Qt::CaseInsensitive))
		{
			extension = ext;
			found = true;
			break;
		}
	}

	if (found)
	{
		if (nullptr != outExt)
			*outExt = extension;

		return (getFileName().isEmpty() ||
			0 ==
				QString::compare(
					getFileExtension(), extension, Qt::CaseInsensitive) ||
			isWritableFormat(extension));
	}

	return false;
}

void AbstractFileSystemObject::updateFileNameError(const QString &failedName)
{
	QFileInfo info(getFilePath());

	if (failedName.isEmpty())
		executeUpdateFilePathError(info.filePath(), failedName);
	else
		executeUpdateFilePathError(
			info.filePath(), info.dir().absoluteFilePath(failedName));
}

QStringList AbstractFileSystemObject::getSupportedExtensions() const
{
	QStringList result;

	result.push_back(getFileExtension());

	return result;
}

bool AbstractFileSystemObject::isWritableFormat(const QString &extension) const
{
	QString ext(extension);

	if (!ext.isEmpty() && !ext.startsWith('.'))
		ext = '.' + ext;

	return (0 == ext.compare(getFileExtension(), Qt::CaseInsensitive));
}
}
