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

#ifndef XEFIS__MODULES__SYSTEMS__AFCS_ROLL_AUTOTRIM_H__INCLUDED
#define XEFIS__MODULES__SYSTEMS__AFCS_ROLL_AUTOTRIM_H__INCLUDED

// Standard:
#include <cstddef>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/module.h>
#include <xefis/core/property.h>
#include <xefis/core/setting.h>


class AFCS_RollAutotrim_IO: public xf::ModuleIO
{
  public:
	/*
	 * Settings
	 */

	xf::Setting<double>				ias_coefficient				{ this, "ias_coefficient" };
	xf::Setting<double>				engine_torque_coefficient	{ this, "engine_torque_coefficient" };
	xf::Setting<double>				total_coefficient			{ this, "total_coefficient", 1.0 };

	/*
	 * Input
	 */

	xf::PropertyIn<si::Velocity>	measured_ias				{ this, "measured-ias" };
	xf::PropertyIn<si::Torque>		measured_engine_torque		{ this, "measured-eng-torque" };

	/*
	 * Output
	 */

	xf::PropertyOut<si::Angle>		ailerons_correction			{ this, "ailerons-correction" };
};


/**
 * Compute ailerons correction to apply to counter react engine's torque.
 * Depends on airspeed and engine RPM. Factors need to be obtained experimentally.
 *
 * Works only for air speeds well below Mach 1.
 */
class AFCS_RollAutotrim: public xf::Module<AFCS_RollAutotrim_IO>
{
  public:
	// Ctor
	explicit
	AFCS_RollAutotrim (std::unique_ptr<AFCS_RollAutotrim_IO>, std::string_view const& instance = {});

  protected:
	// Module API
	void
	process (xf::Cycle const&) override;
};

#endif
