/* vim:ts=4
 *
 * Copyleft 2012…2013  Michał Gawron
 * Marduk Unix Labs, http://mulabs.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Visit http://www.gnu.org/licenses/gpl-3.0.html for more information on licensing.
 */

// Standard:
#include <cstddef>
#include <stdexcept>
#include <set>

// Qt:
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/services.h>
#include <xefis/core/property_node.h>

// Local:
#include "property_tree_widget.h"
#include "property_tree_widget_item.h"


namespace Xefis {

PropertyTreeWidget::PropertyTreeWidget (PropertyNode* root_node, QWidget* parent):
	QTreeWidget (parent),
	_root_node (root_node)
{
	header()->setSectionsClickable (true);
	header()->setMinimumSectionSize (12.f * Services::default_font_size (physicalDpiY()));
	sortByColumn (NameColumn, Qt::AscendingOrder);
	setSortingEnabled (true);
	setSelectionMode (QTreeWidget::SingleSelection);
	setRootIsDecorated (true);
	setAllColumnsShowFocus (true);
	setAcceptDrops (false);
	setAutoScroll (true);
	setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setVerticalScrollMode (QAbstractItemView::ScrollPerPixel);
	setContextMenuPolicy (Qt::CustomContextMenu);
	setHeaderLabels ({ "Property", "Value" });

	addTopLevelItem (new PropertyTreeWidgetItem (root_node, this));

	read();
	setup_appereance();

	_refresh_timer = new QTimer (this);
	_refresh_timer->setInterval (1000.0 / 15.0);
	QObject::connect (_refresh_timer, SIGNAL (timeout()), this, SLOT (read()));
}


PropertyNode*
PropertyTreeWidget::selected_property_node() const
{
	QList<QTreeWidgetItem*> list = selectedItems();
	if (list.empty())
		return nullptr;
	PropertyTreeWidgetItem* item = dynamic_cast<PropertyTreeWidgetItem*> (list.front());
	if (!item)
		return nullptr;
	return item->node();
}


void
PropertyTreeWidget::read()
{
	read (invisibleRootItem(), _root_node);
}


void
PropertyTreeWidget::read (QTreeWidgetItem* item, PropertyNode* node)
{
	TypedPropertyValueNode* val_node = dynamic_cast<TypedPropertyValueNode*> (node);

	if (val_node)
	{
		std::string data = val_node->stringify();
		// If it's a string and contains binary data, display as binary string:
		PropertyValueNode<std::string>* string_node = dynamic_cast<PropertyValueNode<std::string>*> (val_node);
		if (string_node && std::any_of (data.begin(), data.end(), [](unsigned char c) -> bool { return c < 0x20 || 0x7f < c; }))
			data = "binary " + to_binary_form (data);
		item->setData (ValueColumn, Qt::DisplayRole, val_node->is_nil() ? "<nil>" : QString::fromStdString (data));
	}
	else
	{
		PropertyDirectoryNode* dir_node = dynamic_cast<PropertyDirectoryNode*> (node);

		if (dir_node)
		{
			PropertyNodeList subnodes_list = dir_node->children();
			std::set<PropertyNode*> subnodes (subnodes_list.begin(), subnodes_list.end());

			for (int ci = 0; ci < item->childCount(); ++ci)
			{
				PropertyTreeWidgetItem* c = convert_item (item->child (ci));
				auto s = subnodes.find (c->node());

				if (s != subnodes.end())
				{
					c->read();
					subnodes.erase (s);
				}
				else
					delete item->takeChild (ci--);
			}

			for (auto s: subnodes)
				item->addChild (new PropertyTreeWidgetItem (s, item));
		}
	}
}


PropertyTreeWidgetItem*
PropertyTreeWidget::convert_item (QTreeWidgetItem* item)
{
	PropertyTreeWidgetItem* ret = dynamic_cast<PropertyTreeWidgetItem*> (item);
	if (ret)
		return ret;
	throw std::logic_error ("generic QTreeWidgetItem in PropertyTreeWidget");
}


void
PropertyTreeWidget::setup_appereance()
{
	header()->resizeSection (NameColumn, 20.f * Services::default_font_size (physicalDpiY()));
}


std::string
PropertyTreeWidget::to_binary_form (std::string const& blob)
{
	if (blob.empty())
		return "";
	std::string s;
	for (unsigned char v: blob)
		s += QString ("%1").arg (v, 2, 16, QChar ('0')).toStdString() + ":";
	s.pop_back();
	return s;
}


void
PropertyTreeWidget::showEvent (QShowEvent*)
{
	_refresh_timer->start();
}


void
PropertyTreeWidget::hideEvent (QHideEvent*)
{
	_refresh_timer->stop();
}

} // namespace Xefis

