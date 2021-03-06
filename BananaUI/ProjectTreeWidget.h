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

#include <QWidget>

namespace Ui
{
class ProjectTreeWidget;
}

class QMenu;
class QLineEdit;
class QActionGroup;

namespace Banana
{
class ProjectTreeView;
class ProjectTreeWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ProjectTreeWidget(QWidget *parent = nullptr);
	virtual ~ProjectTreeWidget();

	ProjectTreeView *getTreeView() const;
	QLineEdit *getFilterEdit() const;

	void showFilter(bool show);
	QMenu *getButtonOptionsMenu() const;
	QActionGroup *getOptionsActionGroup() const;

private slots:
	void onFilterTextChanged();

	void on_filterEdit_editingFinished();

	void on_actionOptionsRegExp_toggled(bool checked);

	void on_actionOptionsWildcard_toggled(bool checked);

	void on_actionOptionsContainingString_toggled(bool checked);

	void loadSettings();

protected:
	enum PatternSyntax
	{
		ContainingString = QRegExp::FixedString,
		RegExp = QRegExp::RegExp,
		Wildcard = QRegExp::WildcardUnix,
		User = 1024,
	};
	virtual PatternSyntax stringToPatternSyntax(const QString &str) const;
	virtual QString patternSyntaxToString(PatternSyntax syntax) const;

	virtual void populatePatternSyntax();
	virtual void applyPatternSyntax();

	virtual QString getPatternSyntaxCaption() const;

	void updateFilter(bool force = false);
	void setPatternSyntax(PatternSyntax syntax);
	void setPatternSyntaxInternal(PatternSyntax syntax);

	Ui::ProjectTreeWidget *ui;
	QActionGroup *optionsActionGroup;
	QString currentFilter;
	int pattern_syntax;

private:
	typedef std::map<PatternSyntax, QString> PatternSyntaxMap;
	static const PatternSyntaxMap &getPatternSyntaxMap();
};
}
