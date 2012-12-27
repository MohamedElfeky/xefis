/* vim:ts=4
 *
 * Copyleft 2008…2012  Michał Gawron
 * Marduk Unix Labs, http://mulabs.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Visit http://www.gnu.org/licenses/gpl-3.0.html for more information on licensing.
 */

#ifndef XEFIS__WIDGETS__POWER_WIDGET_H__INCLUDED
#define XEFIS__WIDGETS__POWER_WIDGET_H__INCLUDED

// Standard:
#include <cstddef>

// Qt:
#include <QtGui/QWidget>
#include <QtGui/QPaintEvent>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/instrument_widget.h>
#include <xefis/utility/text_painter.h>
#include <xefis/utility/range.h>


class PowerWidget: public Xefis::InstrumentWidget
{
  public:
	// Ctor
	PowerWidget (QWidget* parent);

	/**
	 * Return current range used for PowerWidget.
	 * By default, the range is [0..1].
	 */
	Range<float>
	range() const;

	/**
	 * Set new range to be used with PowerWidget.
	 */
	void
	set_range (Range<float>);

	/**
	 * Return current indicated power value.
	 */
	float
	value() const;

	/**
	 * Set indicated power value.
	 */
	void
	set_value (float value);

	/**
	 * Set validity of the input value.
	 */
	void
	set_value_visible (bool visible);

	/**
	 * Warning value.
	 */
	float
	warning_value() const;

	/**
	 * Set warning value. This starts the section that is drawn in yellow.
	 */
	void
	set_warning_value (float);

	/**
	 * Set visibility of the warning bug.
	 */
	void
	set_warning_visible (bool visible);

	/**
	 * Critical value. This starts the section that is drawn in red.
	 */
	float
	critical_value() const;

	/**
	 * Set critical value.
	 */
	void
	set_critical_value (float);

	/**
	 * Set visibility of the critical bug.
	 */
	void
	set_critical_visible (bool visible);

	/**
	 * Normal/desired value.
	 */
	float
	normal_value() const;

	/**
	 * Set normal/desired value. This sets the green bug position on the scale.
	 */
	void
	set_normal_value (float);

	/**
	 * Set visibility of the normal/desired value bug.
	 */
	void
	set_normal_visible (bool visible);

  protected:
	void
	paintEvent (QPaintEvent*) override;

	void
	paint_text (QPainter&, TextPainter&, float q, float r);

	void
	paint_indicator (QPainter&, TextPainter&, float q, float r);

  private:
	// Parameters:
	Range<float>	_range				= { 0.f, 1.f };
	float			_value				= 0.f;
	bool			_value_visible		= false;
	float			_warning_value		= 1.f;
	bool			_warning_visible	= false;
	float			_critical_value		= 1.f;
	bool			_critical_visible	= false;
	float			_normal_value		= 1.f;
	bool			_normal_visible		= false;
};


inline Range<float>
PowerWidget::range() const
{
	return _range;
}


inline void
PowerWidget::set_range (Range<float> range)
{
	_range = range;
	update();
}


inline float
PowerWidget::value() const
{
	return _value;
}


inline void
PowerWidget::set_value (float value)
{
	_value = value;
	update();
}


inline void
PowerWidget::set_value_visible (bool visible)
{
	_value_visible = visible;
	update();
}


inline float
PowerWidget::warning_value() const
{
	return _warning_value;
}


inline void
PowerWidget::set_warning_value (float warning_value)
{
	_warning_value = warning_value;
	update();
}


inline void
PowerWidget::set_warning_visible (bool visible)
{
	_warning_visible = visible;
	update();
}


inline float
PowerWidget::critical_value() const
{
	return _critical_value;
}


inline void
PowerWidget::set_critical_value (float critical_value)
{
	_critical_value = critical_value;
	update();
}


inline void
PowerWidget::set_critical_visible (bool visible)
{
	_critical_visible = visible;
	update();
}


inline float
PowerWidget::normal_value() const
{
	return _normal_value;
}


inline void
PowerWidget::set_normal_value (float normal_value)
{
	_normal_value = normal_value;
	update();
}


inline void
PowerWidget::set_normal_visible (bool visible)
{
	_normal_visible = visible;
	update();
}

#endif
