/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "flowlayout.h"

FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
		: QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
	setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
		: m_hSpace(hSpacing), m_vSpace(vSpacing)
{
	setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
	QLayoutItem *item;
	while ((item = takeAt(0)))
		delete item;
}

void FlowLayout::addItem(QLayoutItem *item)
{
	itemList.append(item);
}

int FlowLayout::horizontalSpacing() const
{
	if (m_hSpace >= 0)
	{
		return m_hSpace;
	}
	else
	{
		return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
	}
}

int FlowLayout::verticalSpacing() const
{
	if (m_vSpace >= 0)
	{
		return m_vSpace;
	}
	else
	{
		return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
	}
}

int FlowLayout::count() const
{
	return itemList.size();
}

QLayoutItem *FlowLayout::itemAt(int index) const
{
	return itemList.value(index);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
	if (index >= 0 && index < itemList.size())
		return itemList.takeAt(index);
	else
		return 0;
}

Qt::Orientations FlowLayout::expandingDirections() const
{
	return 0;
}

bool FlowLayout::hasHeightForWidth() const
{
	return true;
}

/// 根据宽度计算出高度
int FlowLayout::heightForWidth(int width) const
{
	qDebug() << __func__ << "width=" << width;
	int height = doLayout(QRect(0, 0, width, 0), false);
	return height;
}

void FlowLayout::setGeometry(const QRect &rect)
{
	qDebug() << __func__;
	QLayout::setGeometry(rect);
	doLayout(rect, true);
}

QSize FlowLayout::sizeHint() const
{
	return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
	QSize size;
	for (QLayoutItem *item: itemList)
		size = size.expandedTo(item->minimumSize());

	size += QSize(2 * margin(), 2 * margin());
	return size;
}

int FlowLayout::doLayout(const QRect &rect, bool setGeometry) const
{
	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);    // 获取容器的内边距（子控件与布局边界之间间距）

	// 计算有效的布局矩形，考虑了内边距
	QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
	qDebug() << effectiveRect;
	int x = effectiveRect.x();  // 子控件的水平位置
	int y = effectiveRect.y();  // 子控件的垂直位置
	int lineHeight = 0;         // 当前行的最大高度

	QWidget *wid = parentWidget();
	int spaceX = horizontalSpacing();    // 子控件间的水平间距
	if (spaceX == -1)
		spaceX = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
	int spaceY = verticalSpacing();        // 子控件间的垂直间距
	if (spaceY == -1)
		spaceY = wid->style()->layoutSpacing(QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);

	QList<QPoint> item_point;
	int line_item_width{0};        // 当前行的所有子控件的总宽度

	// 遍历布局中的每个子控件
	for (QLayoutItem *item: itemList)
	{
		// 计算子控件的右边界位置
		int nextX = x + item->sizeHint().width();

		// 如果子控件将超出容器右侧，换行到下一行
		if (nextX > effectiveRect.right() && lineHeight > 0)
		{
			x = effectiveRect.x();  // 返回到行的起始位置
			y = y + lineHeight + spaceY;  // 下移一行
			nextX = x + item->sizeHint().width() + spaceX;  // 更新右边界
			lineHeight = 0;  // 重置行高
			line_item_width = 0;    // 重置当前行的所有子控件的总宽度
		}
		else
			nextX += spaceX;

		// 设置子控件的位置
		if (setGeometry)
		{
			line_item_width += item->sizeHint().width() + spaceX;
			item_point.push_back(QPoint(x, y));
		}

		x = nextX;  // 更新下一个子控件的水平位置
		lineHeight = qMax(lineHeight, item->sizeHint().height());  // 更新当前行的最大高度
	}

	if (setGeometry)
		for (int i = 0; i < itemList.size(); i++)
		{
			int offset = (effectiveRect.width() - (line_item_width - spaceX)) / 2;
			qDebug() << "offset=" << offset;
			item_point[i].rx() += offset;
			itemList[i]->setGeometry(QRect(item_point[i], itemList[i]->sizeHint()));
		}

	// 返回整个流式布局的高度
	return y + lineHeight - rect.y() + bottom;
}


int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
	QObject *parent = this->parent();
	if (!parent)
	{
		return -1;
	}
	else if (parent->isWidgetType())
	{
		QWidget *pw = static_cast<QWidget *>(parent);
		return pw->style()->pixelMetric(pm, 0, pw);
	}
	else
	{
		return static_cast<QLayout *>(parent)->spacing();
	}
}

