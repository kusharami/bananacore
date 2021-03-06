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

class QString;

namespace Banana
{
class ProjectDirectoryModel;

enum class Answer
{
	Unknown,
	No,
	NoToAll,
	Yes,
	YesToAll,
	Abort
};

struct IProjectGroupDelegate
{
	virtual ~IProjectGroupDelegate() {}

	virtual Banana::ProjectDirectoryModel *getProjectTreeModel() const = 0;

	virtual Answer shouldReplaceFile(
		const QString &filepath, Answer *remember_answer) = 0;
	virtual void errorMessage(const QString &message) = 0;
	virtual QString fetchFilePath(const QString &title,
		const QString &currentPath, const QString &filters) = 0;
	virtual QString fetchDirPath(
		const QString &title, const QString &currentPath) = 0;
};
}
