/*****************************************************************************/
/*!
@author			Stefan Marek
@description	Implements:
template <> class TLIFOPool;
*/
/*****************************************************************************/

#ifdef _MSC_VER 
#pragma once
#endif

#ifndef __TEMPLATE_LIFO_POOL_H__
#define __TEMPLATE_LIFO_POOL_H__

#include "Definitions.h"
#include "ErrorHandling.h"

#include "Allocator.h"
#include "ObjectPool.h"
#include "ContainerTools.h"
#include "AllocationTools.h"
#include "PoolGrowing.h"

//=============================================================================
#ifdef _MSC_VER 
#pragma PRAGMA_WARNING_PUSH_LEVEL4
// Compiler Warning (level 4) C4201: 
// nonstandard extension used : nameless struct/union
#pragma PRAGMA_WARNING_DISABLE4201
#endif
//=============================================================================
#ifdef  _MSC_VER
#pragma PRAGMA_PACK_PUSH_CRT_PACKING
#endif 
//=============================================================================

namespace CELL {

namespace LIFO_POOL {
		
	//=============================================================================
	//================================== TLIFOPool ================================
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	class TLIFOPool
	{
	public:
		using this_type = TLIFOPool< TValueType, TAllocType >;

		typedef typename OPOOL::TPool< TValueType, TAllocType >::Type pool_type;

		typedef typename pool_type::size_type size_type;
		typedef typename pool_type::value_type value_type;

		typedef typename pool_type::pointer pointer;
		typedef typename pool_type::const_pointer const_pointer;

		typedef typename pool_type::iterator iterator;
		typedef typename pool_type::const_iterator const_iterator;

	private:
		pool_type mPool;

	public:
		CONSTEXPR TLIFOPool() : mPool()
		{
		}
		CONSTEXPR TLIFOPool(const size_type & size_count) : mPool(size_count)
		{
		}
		CONSTEXPR TLIFOPool(const this_type & queue) : mPool(queue.mPool)
		{
		}
		CONSTEXPR TLIFOPool(const this_type && queue)
		{
			mPool.move(CELL::TOOLS::MEMORY::TForward<value_type>(queue.mPool));
		}
		CONSTEXPRDTOR ~TLIFOPool()
		{
		}
		CONSTEXPR this_type & operator = (const this_type & queue)
		{
			if (this != &queue)
			{
				mPool = queue.mPool;
			}
			return (*this);
		}
		CONSTEXPR size_type size() const
		{
			return mPool.size();
		}
		CONSTEXPR value_type & at(size_type index)
		{
			return mPool.at(index);
		}
		CONSTEXPR const value_type & at(size_type index) const
		{
			return mPool.at(index);
		}
		CONSTEXPR value_type & operator [] (const size_type & i)
		{
			return mPool.at(i);
		}
		CONSTEXPR const value_type & operator [] (const size_type & i) const
		{
			return mPool.at(i);
		}
		CONSTEXPR const value_type & top() const
		{
			return mPool.back();
		}
		CONSTEXPR const value_type & bottom() const
		{
			return mPool.front();
		}
		CONSTEXPR value_type & top()
		{
			return mPool.back();
		}
		CONSTEXPR value_type & bottom()
		{
			return mPool.front();
		}
		CONSTEXPR const_iterator begin() const
		{
			return mPool.begin();
		}
		CONSTEXPR iterator begin()
		{
			return mPool.begin();
		}
		CONSTEXPR const_iterator end() const
		{
			return mPool.end();
		}
		CONSTEXPR iterator end()
		{
			return mPool.end();
		}
		CONSTEXPR void reserve(size_type index)
		{
			return mPool.reserve(index);
		}
		CONSTEXPR void resize(size_type index)
		{
			return mPool.resize(index);
		}
		CONSTEXPR void clear()
		{
			return mPool.clear();
		}
		CONSTEXPR bool empty() const
		{
			return mPool.empty();
		}
		CONSTEXPR void push(const value_type & val)
		{
			mPool.push_back(val);
		}
		CONSTEXPR void push(value_type && val)
		{
			mPool.push_back(CELL::TOOLS::MEMORY::TForward<value_type>(val));
		}
		template <typename... TArgumentTypes>
		CONSTEXPR void push_emplace(TArgumentTypes &&... args)
		{
			mPool.emplace_back(CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
		}
		CONSTEXPR void pop()
		{
			mPool.pop_back();
		}
		CONSTEXPR void swap(this_type & other)
		{
			if (this != &other)
			{
				mPool.swap(other.mPool);
			}
		}
		CONSTEXPR void move(this_type && other)
		{
			if (this != &other)
			{
				mPool.move(CELL::TOOLS::MEMORY::TForward<value_type>(other.mPool));
			}
		}
		CONSTEXPR void assign(const this_type & other)
		{
			if (this != &other)
			{
				mPool.assign(other.mPool);
			}
		}
//=============================================================================
	}; // TLIFOPool
//=============================================================================
	template
	<
		typename TValueType
	>
	struct TLIFOP
	{
		using alloc_type =  DEFAULT_OBJECT_ALLOCATOR< TValueType >;
		using Type = TLIFOPool< TValueType, alloc_type >;
	};
//=============================================================================
} // namespace LIFO_POOL
//=============================================================================
} // namespace CELL
//=============================================================================
#ifdef  _MSC_VER
#pragma PRAGMA_PACK_POP
#endif 
//=============================================================================
#ifdef _MSC_VER 
// Compiler Warning (level 4) C4201: 
// nonstandard extension used : nameless struct/union
#pragma PRAGMA_WARNING_DEFAULT4201
#pragma PRAGMA_WARNING_POP
#endif
//=============================================================================

#endif // __TEMPLATE_LIFO_POOL_H__
