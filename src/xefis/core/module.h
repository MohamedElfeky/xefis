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

#ifndef XEFIS__CORE__MODULE_H__INCLUDED
#define XEFIS__CORE__MODULE_H__INCLUDED

// Standard:
#include <cstddef>
#include <functional>
#include <ostream>

// Qt:
#include <QtXml/QDomElement>

// Xefis:
#include <xefis/config/all.h>
#include <xefis/core/application.h>
#include <xefis/core/navaid_storage.h>
#include <xefis/core/property.h>
#include <xefis/core/config_reader.h>
#include <xefis/utility/noncopyable.h>
#include <xefis/utility/i2c.h>
#include <xefis/utility/logger.h>


#define XEFIS_REGISTER_MODULE_CLASS(module_name, klass) \
	static Xefis::Module::Registrator module_registrator (module_name, [](Xefis::ModuleManager* module_manager, QDomElement const& config) -> Xefis::Module* { \
		return new klass (module_manager, config); \
	});


namespace Xefis {

class ModuleManager;

class Module: private Noncopyable
{
  public:
	typedef std::function<Module* (ModuleManager*, QDomElement const&)>	FactoryFunction;
	typedef std::map<std::string, FactoryFunction>						FactoriesMap;

	class Registrator
	{
	  public:
		Registrator (std::string const& module_name, FactoryFunction);
	};

	class Pointer
	{
	  public:
		// Ctor
		Pointer() = default;

		// Ctor
		Pointer (std::string const& name, std::string const& instance);

		/**
		 * Comparison operator: first by name, then by instance.
		 */
		bool
		operator< (Pointer const& other) const;

		/**
		 * Return module name (class).
		 */
		std::string const&
		name() const noexcept;

		/**
		 * Return module instance.
		 */
		std::string const&
		instance() const noexcept;

	  private:
		std::string	_name;
		std::string	_instance;
	};

  public:
	/**
	 * Create a module.
	 * \param	config
	 * 			DOM configuration element for the module.
	 */
	Module (ModuleManager*, QDomElement const& config);

	// Dtor
	virtual ~Module();

	/**
	 * Signal that the data in property tree has been updated.
	 * Default implementation does nothing.
	 */
	virtual void
	data_updated();

	/**
	 * Return last update time.
	 */
	Time
	update_time() const;

	/**
	 * Return time difference between last and previous update.
	 * Be sure not to use it if you're skipping some of the updates,
	 * because you're watching just one property or something.
	 */
	Time
	update_dt() const;

	/**
	 * Return module name.
	 */
	std::string const&
	name() const noexcept;

	/**
	 * Return module instance.
	 */
	std::string const&
	instance() const noexcept;

	/**
	 * Return configurator widget.
	 * If module doesn't have one, return nullptr.
	 * Default implementation returns nullptr.
	 */
	virtual QWidget*
	configurator_widget() const;

	/**
	 * Register module factory.
	 */
	static void
	register_factory (std::string const& module_name, FactoryFunction);

	/**
	 * Return factory function by name.
	 */
	static FactoryFunction
	find_factory (std::string const& name);

	/**
	 * Reset data update flag. When module calls signal_data_updated(),
	 * the flag is set to true. It can be read with signalled_data_update().
	 */
	void
	reset_data_update_flag() noexcept;

	/**
	 * Return true if the module has signalled data update since last call
	 * to reset_data_update_flag().
	 */
	bool
	signalled_data_update() const noexcept;

	/**
	 * Dumps module info to the log.
	 */
	void
	dump_debug_log();

  protected:
	/**
	 * Parse the <properties> element and initialize properties
	 * by their names matching the <properties> children.
	 * \param	element can be <properties> or parent of <properties> element.
	 * \throw	Xefis::Exception if something's wrong.
	 */
	void
	parse_properties (QDomElement const& element, ConfigReader::PropertiesParser::PropertiesList);

	/**
	 * Parse the <settings> element and initialize variables.
	 * \param	element can be <settings> or parent of <settings> element.
	 * \throw	Xefis::Exception if something's wrong.
	 */
	void
	parse_settings (QDomElement const& element, ConfigReader::SettingsParser::SettingsList);

	/**
	 * Return true if given setting has been found in configuration.
	 */
	bool
	has_setting (QString const& name);

	/**
	 * Signal that this module has updated property tree.
	 */
	void
	signal_data_updated();

	/**
	 * Access NavaidStorage.
	 */
	NavaidStorage*
	navaid_storage() const;

	/**
	 * Access work performer.
	 */
	WorkPerformer*
	work_performer() const;

	/**
	 * Access accounting information for all modules.
	 */
	Accounting*
	accounting() const;

	/**
	 * Add header with module name to the log stream and
	 * return the stream.
	 */
	Xefis::Logger const&
	log() const;

  private:
	/**
	 * Return list of factories.
	 */
	static FactoriesMap&
	factories();

  private:
	ModuleManager*							_module_manager			= nullptr;
	Unique<ConfigReader::SettingsParser>	_settings_parser;
	Unique<ConfigReader::PropertiesParser>	_properties_parser;
	std::string								_name;
	std::string								_instance;
	Xefis::Logger							_logger;
	bool									_signalled_data_update	= false;
};


inline
Module::Registrator::Registrator (std::string const& module_name, FactoryFunction ff)
{
	Module::register_factory (module_name, ff);
}


inline
Module::Pointer::Pointer (std::string const& name, std::string const& instance):
	_name (name),
	_instance (instance)
{ }


inline bool
Module::Pointer::operator< (Pointer const& other) const
{
	if (_name != other._name)
		return _name < other._name;
	if (_instance != other._instance)
		return _instance < other._instance;
	return false;
}


inline std::string const&
Module::Pointer::name() const noexcept
{
	return _name;
}


inline std::string const&
Module::Pointer::instance() const noexcept
{
	return _instance;
}


inline
Module::~Module()
{ }


inline void
Module::data_updated()
{ }


inline std::string const&
Module::name() const noexcept
{
	return _name;
}


inline std::string const&
Module::instance() const noexcept
{
	return _instance;
}


inline void
Module::reset_data_update_flag() noexcept
{
	_signalled_data_update = false;
}


inline bool
Module::signalled_data_update() const noexcept
{
	return _signalled_data_update;
}

} // namespace Xefis

#endif

