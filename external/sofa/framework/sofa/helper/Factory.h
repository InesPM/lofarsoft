/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_HELPER_FACTORY_H
#define SOFA_HELPER_FACTORY_H

#include <map>
#include <iostream>
#include <typeinfo>

#include <sofa/helper/system/config.h>

namespace sofa
{

namespace helper
{

/// Decode the type's name to a more readable form if possible
std::string gettypename(const std::type_info& t);

/// Log classes registered in the factory
void logFactoryRegister(std::string baseclass, std::string classname, std::string key, bool multi);

/// Print factory log
void printFactoryLog(std::ostream& out = std::cout);

template <class Object, class Argument>
class BaseCreator
{
public:
	virtual ~BaseCreator() { }
	virtual Object *createInstance(Argument arg) = 0;
	virtual const std::type_info& type() = 0;
};

template <typename TKey, class TObject, typename TArgument>
class Factory
{
public:
	typedef TKey Key;
	typedef TObject Object;
	typedef TArgument Argument;
	typedef BaseCreator<Object, Argument> Creator;
	
protected:
	std::multimap<Key, Creator*> registry;
	
public:
//  typedef InterfaceCreator<Object>* Creator;
	
	bool registerCreator(Key key, Creator* creator, bool multi=false)
	{
		if(!multi && this->registry.find(key) != this->registry.end())
			return false; // key used
		logFactoryRegister(gettypename(typeid(Object)), gettypename(creator->type()), key, multi);
		//std::cout << gettypename(typeid(Object)) << (multi?" template class ":" class ")
		//          << gettypename(creator->type()) << " registered as " << key << std::endl;
		this->registry.insert(std::pair<Key, Creator*>(key, creator));
		return true;
	}
	
	Object* createObject(Key key, Argument arg);
	
	static Factory<Key, Object, Argument>* getInstance();
	
	static Object* CreateObject(Key key, Argument arg)
	{
		return getInstance()->createObject(key, arg);
	}
};

template <class Factory, class RealObject>
class Creator : public Factory::Creator, public Factory::Key
{
public:
	typedef typename Factory::Object Object;
	typedef typename Factory::Argument Argument;
	typedef typename Factory::Key Key;
	Creator(Key key, bool multi=false)
	: Key(key)
	{
		Factory::getInstance()->registerCreator(key, this, multi);
	}
	Object *createInstance(Argument arg)
	{
		RealObject* instance = NULL;
		create(instance, arg);
		return instance;
	}
	const std::type_info& type()
	{
		return typeid(RealObject);
	}
};

/// Generic object creator. Can be specialized for custom objects creation
template<class Object, class Argument>
void create(Object*& obj, Argument arg)
{
	obj = new Object(arg);
}

template <class Factory, class RealObject>
class CreatorFn : public Factory::Creator, public Factory::Key
{
public:
	typedef typename Factory::Object Object;
	typedef typename Factory::Argument Argument;
	typedef typename Factory::Key Key;
	typedef void Fn(RealObject*& obj, Argument arg);
	Fn* constructor;
	
	CreatorFn(Key key, Fn* constructor, bool multi=false)
	: Key(key), constructor(constructor)
	{
		Factory::getInstance()->registerCreator(key, this, multi);
	}
	
	Object *createInstance(Argument arg)
	{
		RealObject* instance = NULL;
		(*constructor)(instance, arg);
		return instance;
	}
};


} // namespace helper

} // namespace sofa

#endif
