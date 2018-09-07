/* vim:ts=4
 *
 * Copyleft 2012…2016  Michał Gawron
 * Marduk Unix Labs, http://mulabs.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Visit http://www.gnu.org/licenses/gpl-3.0.html for more information on licensing.
 */

#ifndef XEFIS__MODULES__INSTRUMENTS__LINEAR_GAUGE_H__INCLUDED
#define XEFIS__MODULES__INSTRUMENTS__LINEAR_GAUGE_H__INCLUDED

// Standard:
#include <cstddef>
#include <optional>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/instrument.h>
#include <xefis/core/property.h>
#include <xefis/core/property_observer.h>
#include <xefis/core/setting.h>
#include <xefis/support/instrument/instrument_support.h>
#include <xefis/utility/synchronized.h>

// Local:
#include "basic_gauge.h"


template<class Value>
	class LinearGaugeIO: public BasicGaugeIO<Value>
	{
	  public:
		/*
		 * Settings
		 */

		xf::Setting<bool>			mirrored_style	{ this, "mirrored_style", false };
		xf::Setting<bool>			line_hidden		{ this, "line_hidden", false };
		xf::Setting<float>			font_scale		{ this, "font_scale", 1.0f };
		xf::Setting<std::string>	note			{ this, "note", "" };

		/*
		 * Input
		 */

		xf::PropertyIn<Value>		value			{ this, "value" };
	};


class BasicLinearGauge:
	protected BasicGauge,
	protected xf::InstrumentSupport
{
  protected:
	/**
	 * Critical points (ie. minimum/maximum warning/critical values.
	 */
	struct PointInfo
	{
		enum Zone {
			Minimums,
			Normal,
			Maximums,
		};

		Zone	zone;
		float	length;
		QPen	pen;
		float	tick_len;
		bool	critical;
	};

	struct GaugeValues: public BasicGauge::GaugeValues
	{
		bool					mirrored_style;
		bool					line_hidden;
		float					font_scale;
		std::string				note_str;
		bool					inbound;

		// Cached struff, to prevent allocation on heap on every repaint:
		std::vector<PointInfo>	point_infos;
	};

  protected:
	void
	paint (xf::PaintRequest&, GaugeValues& value) const;

  private:
	void
	paint_indicator (GaugeValues& values, xf::InstrumentAids&, xf::InstrumentPainter&, float q, QPointF p0, QPointF p1) const;

	void
	paint_text (GaugeValues& values, xf::InstrumentAids&, xf::InstrumentPainter&, float q, QPointF p0) const;
};


template<class Value>
	class LinearGauge:
		public xf::Instrument<LinearGaugeIO<Value>>,
		private BasicLinearGauge
	{
	  public:
		using Converter = std::function<float128_t (Value const&)>;

	  public:
		// Ctor
		explicit
		LinearGauge (std::unique_ptr<LinearGaugeIO<Value>>, Converter = nullptr, std::string const& instance = {});

		// Module API
		void
		process (xf::Cycle const&) override;

		// Instrument API
		void
		paint (xf::PaintRequest&) const override;

	  private:
		xf::PropertyObserver					_inputs_observer;
		xf::Synchronized<GaugeValues> mutable	_values;
		Converter								_converter;
	};


template<class Value>
	inline
	LinearGauge<Value>::LinearGauge (std::unique_ptr<LinearGaugeIO<Value>> module_io, Converter converter, std::string const& instance):
		xf::Instrument<LinearGaugeIO<Value>> (std::move (module_io), instance),
		_converter (converter)
	{
		_inputs_observer.set_callback ([&]{
			this->mark_dirty();
		});
		_inputs_observer.observe ({
			&this->io.value,
		});
	}


template<class Value>
	inline void
	LinearGauge<Value>::process (xf::Cycle const& cycle)
	{
		_inputs_observer.process (cycle.update_time());
	}


template<class Value>
	inline void
	LinearGauge<Value>::paint (xf::PaintRequest& paint_request) const
	{
		auto& io = this->io;
		xf::Range<Value> const range { *io.value_minimum, *io.value_maximum };

		auto values = _values.lock();
		values->get_from (io, range, (_converter && io.value) ? _converter (*io.value) : io.value.to_floating_point());
		values->mirrored_style = *io.mirrored_style;
		values->line_hidden = *io.line_hidden;
		values->font_scale = *io.font_scale;
		values->note_str = *io.note;

		if (io.value)
			values->inbound = xf::Range { 0.0f, 1.0f }.includes (xf::renormalize (*io.value, range, kNormalizedRange));

		BasicLinearGauge::paint (paint_request, *values);
	}

#endif

