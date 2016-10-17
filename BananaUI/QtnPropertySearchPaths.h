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

#include "QtnProperty/Property.h"
#include "QtnProperty/Delegates/PropertyDelegate.h"

#include <QMetaProperty>

namespace Banana
{
	class SearchPaths;

	class QtnPropertySearchPaths : public QtnProperty
	{
		Q_OBJECT

	public:
		explicit QtnPropertySearchPaths(QObject *object,
										const QMetaProperty &metaProperty);

		SearchPaths *getSearchPaths() const;

		static QString getPlaceholderStr();
		static void Register();

	private:
		QObject *object;
		QMetaProperty metaProperty;
	};

	class QtnPropertyDelegateSearchPaths
		: public QtnPropertyDelegateTyped<QtnPropertySearchPaths>
	{
		Q_DISABLE_COPY(QtnPropertyDelegateSearchPaths)

	public:
		QtnPropertyDelegateSearchPaths(QtnPropertySearchPaths &owner);

	protected:
		virtual void drawValueImpl(QStylePainter &painter,
								   const QRect &rect,
								   const QStyle::State &state,
								   bool *needTooltip = nullptr) const override;

		virtual QWidget* createValueEditorImpl(QWidget *parent,
											   const QRect &rect,
											   QtnInplaceInfo *inplaceInfo = nullptr) override;

		virtual bool propertyValueToStr(QString &strValue) const override;
	};
}