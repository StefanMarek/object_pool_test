/*****************************************************************************/
/*!
	@author			Stefan Marek
	@description	Implements:
	template<,,> class TObjectPoolPOD
					struct TPoolPOD
					struct TDynamicPoolPOD
					struct TStaticPoolPOD
					struct TArrayPoolPOD
					struct TVectorPoolPOD
					struct TBinaryTreePoolPOD
*/
/*****************************************************************************/

#ifdef _MSC_VER 
#pragma once
#endif

#ifndef __POD_OBJECT_POOL_H__
#define __POD_OBJECT_POOL_H__

#include "Definitions.h"
#include "ErrorHandling.h"

#include "ContainerTools.h"
#include "Allocator.h"
#include "ObjectPool.h"
#include "PODObjectPoolBuffer.h"
#include "PoolGrowing.h"

#include <cassert>
#include <cstring> //memcpy

#ifdef USE_INITIALIZER_LIST
#include <initializer_list>
#endif

#ifndef POOL_USE_EXTENDED_METHODS
#define POOL_USE_EXTENDED_METHODS
#endif // POOL_USE_EXTENDED_METHODS

//=============================================================================
#ifdef  _MSC_VER
	#pragma PRAGMA_PACK_PUSH_CRT_PACKING
	// warning C4996: 
	// Function call with parameters that may be unsafe - 
	// this call relies on the caller to check that the passed values are 
	// correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. 
	#pragma PRAGMA_WARNING_PUSH_LEVEL3
	#pragma PRAGMA_WARNING_DISABLE4996
	#pragma PRAGMA_WARNING_DISABLE4201
#endif  
//=============================================================================

namespace CELL {

namespace OPOOLPOD {

//=============================================================================
//============================= TObjectPoolPOD ================================
//=============================================================================
/*! @brief ObjectPoolPOD see ObjectPool with small string optimization strategies.
*/
//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >,
		typename TGrowStratType = DEFAULT_GROWING_STRATEGY< typename TAllocType::size_type >
	>
	class TObjectPoolPOD
	{
	public:
		using this_type = TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			TGrowStratType
		>;
		using swap_type = OPOOL::TObjectPool
		<
			TValueType,
			TAllocType,
			TGrowStratType
		>;
		friend class OPOOL::TObjectPool		
		<
			TValueType,
			TAllocType,
			TGrowStratType
		>;

		typedef TAllocType allocator_type;
		typedef typename allocator_type::size_type size_type;
		typedef typename allocator_type::difference_type difference_type;
		typedef typename allocator_type::value_type value_type;

		typedef TGrowStratType growing_type;

		typedef value_type & reference;
		typedef value_type const & const_reference;
		typedef value_type * pointer;
		typedef value_type const * const_pointer;

		// The iterator typedefs are provided to be compliant with the STL. 
		typedef pointer iterator;
		typedef const_pointer const_iterator;
		typedef pointer reverse_iterator;
		typedef const_pointer const_reverse_iterator;

		typedef CELL::ALLOC::BUFFERPOD::TPODObjectPoolBuffer< TValueType, TGrowStratType::BUFFER::TELEMENTS > buffer_t;

	protected:
		//================================ VALUES =====================================
		allocator_type mAllocator;	// the allmighty allocator
		growing_type mGrow;			// the reason for this class

		buffer_t mBuffer;		// fast stack based allocater for small object pools
		pointer	mpBeginOfPool;	// pointer begin
		pointer	mpFirstElement; // pointer mpData;
		pointer	mpLastElement;	// size_type mSize;
		pointer	mpEndOfPool;	// size_type mCapacity; 
		//================================ ACCESSORS===================================
	public:
		// default constructor
		CONSTEXPR TObjectPoolPOD() THROW_NEVER : mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			//initializeFill( 0, value_type() );
			if (buffer_t::SIZE::TSIZE > 0)
			{
				initialize(buffer_t::SIZE::TSIZE, false);
			}
			else
			{
				initializeDefault(0);
			}
		}
		// default constructor
		CONSTEXPR TObjectPoolPOD(const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			//initializeFill( 0, value_type() );
			if (buffer_t::SIZE::TSIZE > 0)
			{
				initialize(buffer_t::SIZE::TSIZE, false);
			}
			else
			{
				initializeDefault(0);
			}
		}
		// initializer for default constructor
		CONSTEXPR TObjectPoolPOD(const size_type & size_count) THROW_NEVER :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			//initializeFill( size_count, value_type() );
			initializeDefault(size_count);
		}
		CONSTEXPR TObjectPoolPOD(const size_type & size_count,
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			//initializeFill( size_count, value_type() );
			initializeDefault(size_count);
		}
		// fill initializer for copy constructor
		CONSTEXPR TObjectPoolPOD(const size_type & size_count, const_reference val ) THROW_NEVER : // = value_type()) :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeFill(size_count, val);
		}
		CONSTEXPR TObjectPoolPOD(const size_type & size_count,
			const_reference val,// = value_type(),
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeFill(size_count, val);
		}
		// fill initializer for copy constructor
		CONSTEXPR TObjectPoolPOD(const size_type & size_count,
			const size_type & capacity_count,
			const_reference val = value_type()) THROW_NEVER :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeFill(size_count, capacity_count, val);
		}
		CONSTEXPR TObjectPoolPOD(const size_type & size_count,
			const size_type & capacity_count,
			const_reference val,
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeFill(size_count, capacity_count, val);
		}
		// copy initializer for copy constructor
		template <typename InputIterator>
		CONSTEXPR TObjectPoolPOD(InputIterator _Begin, InputIterator _End) THROW_NEVER :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeCopy<InputIterator>(_Begin, _End);
		}
		template <typename InputIterator>
		CONSTEXPR TObjectPoolPOD(InputIterator _Begin, InputIterator _End,
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeCopy<InputIterator>(_Begin, _End);
		}
		// copy initializer for copy constructor ( should be done by constructor for iterators )
		//template < typename UValueType >
		//TObjectPoolPOD( const TObjectPoolPOD<UValueType, TAllocType, TGrowStratType> & pool ) : 
		//	mGrow( growing_type() ),
		//	mAllocator( allocator_type() ), 
		//	mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr)
		//{
		//	initializeCopy<UValueType *>( pool.begin(), pool.end() );
		//}
		// copy initializer for copy constructor
		CONSTEXPR TObjectPoolPOD(const this_type & pool) THROW_NEVER :
			mGrow(pool.get_growing()), mAllocator(pool.get_allocator()),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeCopy(pool.begin(), pool.end());
		}
		CONSTEXPR TObjectPoolPOD(this_type && pool) THROW_NEVER :
			mGrow(pool.get_growing()), mAllocator(pool.get_allocator()),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			move(CELL::TOOLS::MEMORY::TForward<this_type>(pool));
		}
#ifdef USE_INITIALIZER_LIST
		CONSTEXPR TObjectPoolPOD(std::initializer_list<TValueType> pool) THROW_NEVER :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeCopy(pool.begin(), pool.end());
		}
		CONSTEXPR TObjectPoolPOD(std::initializer_list<TValueType> pool,
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeCopy(pool.begin(), pool.end());
		}
#endif
		// copy and merge initializer for copy constructor
		template <typename InputIterator1, typename InputIterator2>
		//template <typename InputIterator>
		CONSTEXPR TObjectPoolPOD(
			InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2) THROW_NEVER :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeMergeCopy<InputIterator1, InputIterator2>(
				_Begin1, _End1, _Begin2, _End2);
			//initializeMergeCopy<pointer, InputIterator2>( 
			//	_Begin1, _End1, _Begin2, _End2 );
		}
		template <typename InputIterator1, typename InputIterator2>
		//template <typename InputIterator>
		CONSTEXPR TObjectPoolPOD(
			InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2,
			// pointer _Begin1, pointer _End1, 
			//InputIterator _Begin2, InputIterator _End2, 
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeMergeCopy<InputIterator1, InputIterator2>(
				_Begin1, _End1, _Begin2, _End2);
			//initializeMergeCopy<pointer, InputIterator2>( 
			//	_Begin1, _End1, _Begin2, _End2 );
		}
		// copy and merge initializer for copy constructor
		template <typename InputIterator1, typename InputIterator2, typename InputIterator3>
		//template <typename InputIterator>
		CONSTEXPR TObjectPoolPOD(
			InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2,
			InputIterator3 _Begin3, InputIterator3 _End3) THROW_NEVER :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeMergeCopy3<InputIterator1, InputIterator2, InputIterator3>(
				_Begin1, _End1, _Begin2, _End2, _Begin3, _End3);
		}
		template <typename InputIterator1, typename InputIterator2, typename InputIterator3>
		//template <typename InputIterator>
		CONSTEXPR TObjectPoolPOD(
			InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2,
			InputIterator3 _Begin3, InputIterator3 _End3,
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeMergeCopy3<InputIterator1, InputIterator2, InputIterator3>(
				_Begin1, _End1, _Begin2, _End2, _Begin3, _End3);
		}
		// copy and fill initializer for copy constructor
		CONSTEXPR TObjectPoolPOD(const size_type & size_count,
			const size_type & capacity_count,
			pointer _Begin, pointer _End,
			const_reference val = value_type()) THROW_NEVER :
			mGrow(), mAllocator(),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeCopyFill(size_count, capacity_count, _Begin, _End, val);
		}
		CONSTEXPR TObjectPoolPOD(const size_type & size_count,
			const size_type & capacity_count,
			pointer _Begin, pointer _End,
			const_reference val,
			const allocator_type & alloc,
			const growing_type & grow = growing_type()) THROW_NEVER :
			mGrow(grow), mAllocator(alloc),
			mpFirstElement(nullptr), mpLastElement(nullptr), mpEndOfPool(nullptr), mpBeginOfPool(nullptr)
		{
			initializeCopyFill(size_count, capacity_count, _Begin, _End, val);
		}
		CONSTEXPRDTOR ~TObjectPoolPOD() THROW_NEVER
		{
			release(true);
		}

		CONSTEXPR const buffer_t & get_buffer() const
		{
			return mBuffer;
		}
		CONSTEXPR const_pointer c_data() const
		{
			return mpFirstElement;
		}
		CONSTEXPR pointer data() const
		{
			return mpFirstElement;
		}
		CONSTEXPR pointer get() const
		{
			return mpFirstElement;
		}
		CONSTEXPR void * get_void() const
		{
			return mpFirstElement;
		}
		CONSTEXPR const growing_type & get_growing() const
		{
			return mGrow;
		}
		CONSTEXPR growing_type & get_growing()
		{
			return mGrow;
		}
		// validation
		CONSTEXPR bool is_full() const
		{
			return (mpFirstElement ? (mpLastElement >= mpEndOfPool) : true);
		}
		CONSTEXPR bool is_from(pointer pObject) const
		{
			return ((mpFirstElement <= pObject) && (pObject <= mpLastElement));
		}
		CONSTEXPR bool is_range_from(pointer pFirst, pointer pLast) const
		{
			return
			(
				(mpFirstElement <= pFirst) &&
					(pFirst <= pLast) &&
					(pLast <= mpLastElement)
			);
		}
		// size getter
		CONSTEXPR size_type type_size() const
		{
			//static const size_type TYPE_SIZE = sizeof(value_type);
			return sizeof(value_type);
		}
		CONSTEXPR size_type max_byte_size() const
		{
			return (max_size() * type_size());
		}
		CONSTEXPR size_type max_size_type() const
		{
			return (size_type(-1));
		}
		CONSTEXPR size_type byte_size() const
		{
			return (size() * type_size());
		}
		CONSTEXPR size_type capacity_byte_size() const
		{
			return (capacity() * type_size());
		}
		CONSTEXPR size_type pool_capacity_byte_size() const
		{
			return (pool_capacity() * type_size());
		}
		CONSTEXPR pointer begin_of_pool()
		{
			return mpBeginOfPool;
		}
		CONSTEXPR const_pointer begin_of_pool() const
		{
			return mpBeginOfPool;
		}
		CONSTEXPR pointer end_of_pool()
		{
			return mpEndOfPool;
		}
		CONSTEXPR const_pointer end_of_pool() const
		{
			return mpEndOfPool;
		}
		CONSTEXPR void swap_allocator(allocator_type & other)
		{
			allocator_type temp(mAllocator);
			mAllocator = other;
			other = temp;
		}
		CONSTEXPR void swap_growing(growing_type & other)
		{
			growing_type temp(mGrow);
			mGrow = other;
			other = temp;
		}
		// merge/assign will destroy all existing elements and initialize 
		// the pool with the given two pool ranges.
		template <typename InputIterator1, typename InputIterator2>
		CONSTEXPR this_type & assign(InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2)
		{
			bool isfrombuffer = is_buffered();
			swap_type new_pool(_Begin1, _End1, _Begin2, _End2);
			swap(new_pool);
			if (isfrombuffer)
			{
				new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
				//new_pool.mpBeginOfPool = nullptr;
				//new_pool.mpFirstElement = nullptr;
				//new_pool.mpLastElement = nullptr;
				//new_pool.mpEndOfPool = nullptr;
			}
			return (*this);
		}
		template <typename InputIterator1, typename InputIterator2, typename InputIterator3>
		CONSTEXPR this_type & assign(
			InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2,
			InputIterator3 _Begin3, InputIterator3 _End3)
		{
			bool isfrombuffer = is_buffered();
			swap_type new_pool(_Begin1, _End1, _Begin2, _End2, _Begin3, _End3);
			swap(new_pool);
			if (isfrombuffer)
			{
				new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
				//new_pool.mpBeginOfPool = nullptr;
				//new_pool.mpFirstElement = nullptr;
				//new_pool.mpLastElement = nullptr;
				//new_pool.mpEndOfPool = nullptr;
			}
			return (*this);
		}
		//=========================== STL compliant interface begin ===================
		CONSTEXPR this_type & operator = (const this_type & pool)
		{
			return assign(pool);
		}
		CONSTEXPR this_type & operator = (this_type && pool) THROW_NEVER
		{
			move(CELL::TOOLS::MEMORY::TForward<this_type>(pool));
			return (*this);
		}
#ifdef USE_INITIALIZER_LIST
		CONSTEXPR this_type & operator = (std::initializer_list<TValueType> pool)
		{
			return assign(pool.begin(), pool.end());
		}
#endif
		CONSTEXPR this_type & operator += (const this_type & pool)
		{
			insert(pool.begin(), pool.end());
			return (*this);
		}
		//template < typename UValueType >
		//this_type & operator = ( const TObjectPoolPOD<UValueType, TAllocType, TGrowStratType> & pool )
		//{
		//	assign( pool.begin(), pool.end() );
		//	return (*this);
		//}
		CONSTEXPR const allocator_type & get_allocator() const
		{
			return mAllocator;
		}
		CONSTEXPR allocator_type & get_allocator()
		{
			return mAllocator;
		}
		CONSTEXPR size_type max_size() const
		{
			return std::allocator_traits<allocator_type>::max_size(mAllocator);
			//return mAllocator.max_size();
			//return ( max_size_type() / sizeof( value_type ) ); 
		}
		CONSTEXPR size_type size() const
		{
			return (mpFirstElement ? (mpLastElement - mpFirstElement) : 0);
		}
		CONSTEXPR size_type capacity() const
		{
			return (mpFirstElement ? (mpEndOfPool - mpFirstElement) : 0);
		}
		CONSTEXPR size_type pool_capacity() const
		{
			return (mpBeginOfPool ? (mpEndOfPool - mpBeginOfPool) : 0);
		}
		CONSTEXPR reference operator [] (const size_type & i)
		{
			return at(i);
		}
		CONSTEXPR const_reference operator [] (const size_type & i) const
		{
			return at(i);
		}
		CONSTEXPR reference at(size_type index)
		{
			GLOBAL_ASSERT_NOT_INITIALIZED(mpFirstElement);
			GLOBAL_ASSERT_INDEX_OUT_OF_RANGE(index, size());
			return *(mpFirstElement + index);
		}
		CONSTEXPR const_reference at(size_type index) const
		{
			GLOBAL_ASSERT_NOT_INITIALIZED(mpFirstElement);
			GLOBAL_ASSERT_INDEX_OUT_OF_RANGE(index, size());
			return *(mpFirstElement + index);
		}
		CONSTEXPR reference front()
		{
			return (*mpFirstElement);
		}
		CONSTEXPR const_reference front() const
		{
			return (*mpFirstElement);
		}
		CONSTEXPR reference back()
		{
			return (*(mpLastElement - 1));
		}
		CONSTEXPR const_reference back() const
		{
			return (*(mpLastElement - 1));
		}
		CONSTEXPR pointer pointer_at(size_type pos)
		{
			return (mpFirstElement + pos);
		}
		CONSTEXPR const_pointer pointer_at(size_type pos) const
		{
			return (mpFirstElement + pos);
		}
		CONSTEXPR pointer begin()
		{
			return mpFirstElement;
		}
		CONSTEXPR const_pointer begin() const
		{
			return mpFirstElement;
		}
		CONSTEXPR pointer end()
		{
			return mpLastElement;
		}
		CONSTEXPR const_pointer end() const
		{
			return mpLastElement;
		}
		CONSTEXPR pointer rbegin()
		{
			return mpLastElement;
		}
		CONSTEXPR const_pointer rbegin() const
		{
			return mpLastElement;
		}
		CONSTEXPR pointer rend()
		{
			return mpFirstElement;
		}
		CONSTEXPR const_pointer rend() const
		{
			return mpFirstElement;
		}
		CONSTEXPR bool empty() const
		{
			return (mpFirstElement ? (mpFirstElement == mpLastElement) : true);
		}
		//=============================================================================
		CONSTEXPR void clear()
		{
			//resize( 0 );
			//erase( begin(), end() );
			if (!empty())
			{
				destroy();
			}
		}
		CONSTEXPR void reserve(size_type new_capacity_size, const_reference val)//= value_type() )
		{
			if (new_capacity_size > capacity())
			{
				if (new_capacity_size > buffer_t::SIZE::TSIZE || !is_buffered())
				{
					bool isfrombuffer = is_buffered();
					swap_type new_pool(size(), new_capacity_size, begin(), end(), val);
					swap(new_pool);
					if (isfrombuffer)
					{
						new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
						//new_pool.mpBeginOfPool = nullptr;
						//new_pool.mpFirstElement = nullptr;
						//new_pool.mpLastElement = nullptr;
						//new_pool.mpEndOfPool = nullptr;
					}
				}
				else
				{
					mpBeginOfPool = mBuffer.get();
					mpFirstElement = mpBeginOfPool;
					mpLastElement = mpFirstElement + new_capacity_size;
					mpEndOfPool = mpFirstElement + buffer_t::SIZE::TSIZE;
					for (size_type i = 0; i < new_capacity_size; ++i)
					{
						mBuffer[i] = val;
					}
				}
			}
		}
		CONSTEXPR void reserve(size_type new_capacity_size)
		{
			if (new_capacity_size > capacity())
			{
				bool isfrombuffer = is_buffered();
				if (new_capacity_size > buffer_t::SIZE::TSIZE || !isfrombuffer)
				{
					swap_type new_pool(size(), new_capacity_size, begin(), end());
					swap(new_pool);
					if (isfrombuffer)
					{
						new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
						//new_pool.mpBeginOfPool = nullptr;
						//new_pool.mpFirstElement = nullptr;
						//new_pool.mpLastElement = nullptr;
						//new_pool.mpEndOfPool = nullptr;
					}
				}
				else
				{
					mpBeginOfPool = mBuffer.get();
					mpFirstElement = mpBeginOfPool;
					mpLastElement = mpFirstElement + new_capacity_size;
					mpEndOfPool = mpFirstElement + buffer_t::SIZE::TSIZE;
				}
			}
		}
		// HINT: resize( 0, value ) deallocates the complete pool memory (size = 0 and capacity = 0).
		CONSTEXPR void resize(size_type new_element_size, const_reference val)//= value_type() )
		{
			if (new_element_size)
			{
				const size_type element_size = size();
				if (new_element_size != element_size)
				{
					if (new_element_size > capacity())
					{
						bool isfrombuffer = is_buffered();
						if (new_element_size > buffer_t::SIZE::TSIZE || !isfrombuffer)
						{
							swap_type new_pool(new_element_size,
								next_capacity_size(new_element_size),
								begin(), end(), val);
							swap(new_pool);
							if (isfrombuffer)
							{
								new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
								//new_pool.mpBeginOfPool = nullptr;
								//new_pool.mpFirstElement = nullptr;
								//new_pool.mpLastElement = nullptr;
								//new_pool.mpEndOfPool = nullptr;
							}
						}
						else
						{
							mpBeginOfPool = mBuffer.get();
							mpFirstElement = mpBeginOfPool;
							mpLastElement = mpFirstElement + new_element_size;
							mpEndOfPool = mpFirstElement + buffer_t::SIZE::TSIZE;
							for (size_type i = 0; i < new_element_size; ++i)
							{
								mBuffer[i] = val;
							}
						}
					}
					else // if ( new_element_size <= capacity() ) 
					{
						if (new_element_size < element_size)
						{
							destroy(element_size - new_element_size);
						}
						else // if ( new_element_size >= element_size )
						{
							constructFill(new_element_size - element_size, val);
						}
					}
				}
			}
			else
			{
				//this_type new_pool;
				//swap(new_pool);

				initialize(buffer_t::SIZE::TSIZE, true);
			}
		}
		// HINT: resize( 0 ) deallocates the complete pool memory (size = 0 and capacity = 0).
		CONSTEXPR void resize(size_type new_element_size)
		{
			if (new_element_size)
			{
				const size_type element_size = size();
				if (new_element_size != element_size)
				{
					if (new_element_size > capacity())
					{
						bool isfrombuffer = is_buffered();
						if (new_element_size > buffer_t::SIZE::TSIZE || !isfrombuffer)
						{
							swap_type new_pool(new_element_size,
								next_capacity_size(new_element_size + 1),
								begin(), end()); // , val );
							swap(new_pool);
							if (isfrombuffer)
							{
								new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
								//new_pool.mpBeginOfPool = nullptr;
								//new_pool.mpFirstElement = nullptr;
								//new_pool.mpLastElement = nullptr;
								//new_pool.mpEndOfPool = nullptr;
							}
						}
						else
						{
							mpBeginOfPool = mBuffer.get();
							mpFirstElement = mpBeginOfPool;
							mpLastElement = mpFirstElement + new_element_size;
							mpEndOfPool = mpFirstElement + buffer_t::SIZE::TSIZE;
						}
					}
					else // if ( new_element_size <= capacity() ) 
					{
						if (new_element_size < element_size)
						{
							destroy(element_size - new_element_size);
						}
						else // if ( new_element_size >= element_size )
						{
							constructDefault(new_element_size - element_size);
						}
					}
				}
			}
			else
			{
				//this_type new_pool;
				//swap(new_pool);

				initialize(buffer_t::SIZE::TSIZE, true);
			}
		}
		CONSTEXPR void push_back(const_reference val)
		{
			ValidateIfFull();
			constructFillLast(val); //constructFill( 1, val );
		}
		CONSTEXPR void push_back(value_type && val)
		{
			//emplace_back( CELL::TOOLS::MEMORY::TForward<value_type>(val) );
			ValidateIfFull();
			emplaceFillLast(CELL::TOOLS::MEMORY::TForward<value_type>(val));
		}
		template <typename... TArgumentTypes>
		CONSTEXPR void emplace_back(TArgumentTypes &&... args)
		{
			ValidateIfFull();
			emplaceFillLast(CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
		}
		template <typename... TArgumentTypes>
		CONSTEXPR void emplace(pointer position, TArgumentTypes &&... args)
		{
			if (is_from(position))
			{
				emplaceFill(position, CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
			}
		}
		CONSTEXPR void pop_back()
		{
			if (!empty())
			{
				destroyLast();
			}
		}
		CONSTEXPR void pop_front()
		{
			if (!empty())
			{
				destroyFirst();
			}
		}
		//=============================================================================
		// assign will destroy all existing elements and initialize the pool 
		// with the given range elements.
		template <typename InputIterator>
		CONSTEXPR this_type & assign(InputIterator _First, InputIterator _Last)
		{
			const size_type len = size_distance_pointer(_First, _Last);
			if (len > buffer_t::SIZE::TSIZE)
			{
				bool isfrombuffer = is_buffered();
				swap_type new_pool(_First, _Last);
				swap(new_pool);
				if (isfrombuffer)
				{
					new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
					//new_pool.mpBeginOfPool = nullptr;
					//new_pool.mpFirstElement = nullptr;
					//new_pool.mpLastElement = nullptr;
					//new_pool.mpEndOfPool = nullptr;
				}
			}
			else
			{
				initialize(buffer_t::SIZE::TSIZE, true);
				mpLastElement = mpFirstElement + len;
				copyIn<InputIterator>(_First, _Last, mpFirstElement);
			}
			return (*this);
		}
		CONSTEXPR this_type & assign(size_type len, const_reference value)
		{
			if (len > buffer_t::SIZE::TSIZE)
			{
				bool isfrombuffer = is_buffered();
				swap_type new_pool(len, value);
				swap(new_pool);
				if (isfrombuffer)
				{
					new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
					//new_pool.mpBeginOfPool = nullptr;
					//new_pool.mpFirstElement = nullptr;
					//new_pool.mpLastElement = nullptr;
					//new_pool.mpEndOfPool = nullptr;
				}
			}
			else
			{
				initialize(buffer_t::SIZE::TSIZE, true);
				mpLastElement = mpFirstElement + len;
				for (size_type i = 0; i < len; ++i)
				{
					mBuffer[i] = value;
				}
			}
			return (*this);
		}
#ifdef USE_INITIALIZER_LIST
		CONSTEXPR this_type & assign(std::initializer_list<TValueType> initList)
		{
			const size_type len = size_distance_pointer(initList.begin(), initList.end());
			if (len > buffer_t::SIZE::TSIZE)
			{
				bool isfrombuffer = is_buffered();
				swap_type new_pool(initList.begin(), initList.end());
				swap(new_pool);
				if (isfrombuffer)
				{
					new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
					//new_pool.mpBeginOfPool = nullptr;
					//new_pool.mpFirstElement = nullptr;
					//new_pool.mpLastElement = nullptr;
					//new_pool.mpEndOfPool = nullptr;
				}
			}
			else
			{
				initialize(buffer_t::SIZE::TSIZE, true);
				mpLastElement = mpFirstElement + len;
				//copyIn<std::initializer_list<TValueType>::iterator>(initList.begin(), initList.end(), mpFirstElement);
				copyIn(initList.begin(), initList.end(), mpFirstElement);
			}
			return (*this);
		}
#endif
		// insert will insert new elements at the end of the pool.
		template <typename InputIterator>
		CONSTEXPR void insert(InputIterator _First, InputIterator _Last)
		{
			insert(mpLastElement, _First, _Last);
		}
		// insert will insert new elements at the given pool position.
		// insert can also be used to move values from another pool to this pool.
		template <typename InputIterator>
		CONSTEXPR void insert(pointer pPosition, InputIterator _First, InputIterator _Last)
		{
			if (is_from(pPosition))
			{
				const size_type element_size = size();
				const size_type iter_element_size = size_distance_pointer(_First, _Last);
				const size_type pos_element_size = size_distance_pointer(mpFirstElement, pPosition);
				const size_type new_element_size = pos_element_size + iter_element_size;
				if (new_element_size > capacity())
				{
					bool isfrombuffer = is_buffered();
					swap_type new_pool(begin(), end(), _First, _Last);
					swap(new_pool);
					if (isfrombuffer)
					{
						new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
						//new_pool.mpBeginOfPool = nullptr;
						//new_pool.mpFirstElement = nullptr;
						//new_pool.mpLastElement = nullptr;
						//new_pool.mpEndOfPool = nullptr;
					}
				}
				else // if ( new_element_size <= capacity() ) 
				{
					if (new_element_size <= element_size)
					{
						copyIn<InputIterator>(_First, _Last, pPosition);
					}
					else
					{
						InputIterator _Mid = _First;
						const size_type pos_end_element_size = size_distance_pointer(pPosition, mpLastElement);
						if (pos_end_element_size)
						{
							//advance_pointer( _Mid, pos_element_size );
							advance_iterator(_Mid, pos_end_element_size);
							copyIn<InputIterator>(_First, _Mid, pPosition);
						}
						const size_type pos_new_end_element_size = new_element_size - element_size;
						constructCopy(pos_new_end_element_size, _Mid, _Last);
					}
				}
			}
		}
		// insert and replace are in my opinion the same therefore there will be no replace
		CONSTEXPR void insert(pointer iter, size_type len, const_reference value)
		{
			if (is_from(iter))
			{
				pointer pLastPosition = iter + len;
				if (pLastPosition >= mpLastElement)
				{
					resize(pLastPosition - mpFirstElement, value);
				}
				else
				{
					while (iter < pLastPosition)
					{
						(*iter) = value;
						++iter;
					}
				}
			}
		}
		// insert and replace are in my opinion the same therefore there will be no replace
		CONSTEXPR pointer insert(pointer iter, const_reference value)
		{
			if (is_from(iter))
			{
				(*iter) = value;
			}
			return iter;
		}
		// erase will destroy elements at the given pool position.
		// If you also want to deallocate the memory use resize.
		CONSTEXPR pointer erase(pointer iter)
		{
			if (is_from(iter))
			{
				iterator iter_pos = iter + 1;
				insert_pool(iter_pos, mpLastElement, iter);
				destroyLast(); //destroy( 1 );
			}
			return iter;
		}
		CONSTEXPR pointer erase(pointer _First, pointer _Last)
		{
			GLOBAL_ASSERT_RANGE_ERROR(_First, _Last);
			if (is_range_from(_First, _Last))
			{
				//const size_type dist = size_distance_pointer( _First, _Last );
				if (_Last < mpLastElement)
				{
					insert_pool(_Last, mpLastElement, _First);
				}
				destroy(size_distance_pointer(_First, _Last));
			}
			return _First;
		}
		// fill will assign a given value to existing pool elements.
		CONSTEXPR void fill(const_reference value = value_type())
		{
			fill(mpFirstElement, mpLastElement, value);
		}
		CONSTEXPR void fill(pointer _First, pointer _Last, const_reference value = value_type())
		{
			GLOBAL_ASSERT_RANGE_ERROR(_First, _Last);
			while (_First != _Last)
			{
				*_First = value;
				++_First;
			}
		}
		CONSTEXPR void swap(this_type & other) THROW_NEVER
		{
			if (this != &other)
			{
				swap_growing(other.mGrow);
				swap_allocator(other.mAllocator);

				bool fromBuffer = is_buffered();
				bool fromOtherBuffer = other.is_buffered();

				if (fromOtherBuffer && fromBuffer) // fast
				{
					const size_type size_count = size();
					const size_type other_size_count = other.size();

					const size_type capacity_count = capacity();
					const size_type other_capacity_count = other.capacity();

					buffer_t temp = mBuffer;
					mBuffer = other.mBuffer;
					other.mBuffer = temp;

					other.mpLastElement = other.mpFirstElement + size_count;
					other.mpEndOfPool = other.mpFirstElement + capacity_count;

					mpLastElement = mpFirstElement + other_size_count;
					mpEndOfPool = mpFirstElement + other_capacity_count;
				}
				else if (!fromOtherBuffer && fromBuffer) // slow
				{
					const size_type size_count = size();
					const size_type other_size_count = other.size();

					const size_type capacity_count = capacity();
					const size_type other_capacity_count = other.capacity();

					buffer_t temp = mBuffer;
					swapCopy(other_size_count, other_capacity_count, other.begin(), other.end());
					other.swapCopy(size_count, capacity_count, temp.get(), temp.get() + size_count);
				}
				else if (fromOtherBuffer && !fromBuffer) // slow
				{
					const size_type size_count = size();
					const size_type other_size_count = other.size();

					const size_type capacity_count = capacity();
					const size_type other_capacity_count = other.capacity();

					buffer_t temp = other.mBuffer;
					other.swapCopy(size_count, capacity_count, begin(), end());
					swapCopy(other_size_count, other_capacity_count, temp.get(), temp.get() + other_size_count);
				}
				else // as fast as possible
				{
					pointer	mSwapTemp;		// pointer to swap temp

					mSwapTemp = mpBeginOfPool;
					mpBeginOfPool = other.mpBeginOfPool;
					other.mpBeginOfPool = mSwapTemp;

					mSwapTemp = mpFirstElement;
					mpFirstElement = other.mpFirstElement;
					other.mpFirstElement = mSwapTemp;

					mSwapTemp = mpLastElement;
					mpLastElement = other.mpLastElement;
					other.mpLastElement = mSwapTemp;

					mSwapTemp = mpEndOfPool;
					mpEndOfPool = other.mpEndOfPool;
					other.mpEndOfPool = mSwapTemp;
				}
			}
		}
		CONSTEXPR void move(this_type && other) THROW_NEVER
		{
			if (this != &other)
			{
				release(true);

				mGrow = other.mGrow;
				mAllocator = other.mAllocator;

				bool fromOtherBuffer = other.is_buffered();

				if (fromOtherBuffer) // fast
				{
					const size_type other_size_count = other.size();
					const size_type other_capacity_count = other.pool_capacity();

					mBuffer = other.mBuffer;

					mpBeginOfPool = mBuffer.get();
					mpFirstElement = mpBeginOfPool;
					mpLastElement = mpFirstElement + other_size_count;
					mpEndOfPool = mpFirstElement + other_capacity_count;

					other.mpFirstElement = nullptr;
					other.mpLastElement = nullptr;
					other.mpEndOfPool = nullptr;
				}
				else // as fast as possible
				{
					mpBeginOfPool = other.mpBeginOfPool;
					mpFirstElement = other.mpFirstElement;
					mpLastElement = other.mpLastElement;
					mpEndOfPool = other.mpEndOfPool;

					other.mpBeginOfPool = nullptr;
					other.mpFirstElement = nullptr;
					other.mpLastElement = nullptr;
					other.mpEndOfPool = nullptr;
				}
			}
		}
		CONSTEXPR this_type & assign(const this_type & other)
		{
			if (this != &other)
			{
				bool fromOtherBuffer = other.is_buffered();

				if (fromOtherBuffer) // fast
				{
					release(true);

					const size_type other_size_count = other.size();
					const size_type other_capacity_count = other.capacity();

					mBuffer = other.mBuffer;

					mpBeginOfPool = mBuffer.get();
					mpFirstElement = mpBeginOfPool;
					mpLastElement = mpFirstElement + other_size_count;
					mpEndOfPool = mpFirstElement + other_capacity_count;
				}
				else // as fast as possible
				{
					initialize(other.size(), true);
					mpLastElement = mpFirstElement + other.size();
					copyIn<const_pointer>(other.begin(), other.end());
					//this_type new_pool(other);
					//swap(new_pool);
				}
			}
			return (*this);
		}
		//=========================== STL compliant interface end ===================
		// copy from input range
		template <class InputIterator>
		CONSTEXPR void copyIn(InputIterator _First, InputIterator _Last)
		{
			copyIn<InputIterator>(_First, _Last, mpFirstElement);
		}
		template <class InputIterator>
		CONSTEXPR void copyIn(InputIterator _First, InputIterator _Last, pointer _Dest)
		{
			GLOBAL_ASSERT_RANGE_ERROR(_First, _Last);
			while (_First != _Last)
			{
				*_Dest = *_First;
				++_First;
				++_Dest;
			}
		}
		CONSTEXPR void copyIn(const_pointer _First, const_pointer _Last)
		{
			copyIn(_First, _Last, mpFirstElement);
		}
		CONSTEXPR void copyIn(const_pointer _First, const_pointer _Last, pointer _Dest)
		{
			insert_pool(_First, _Last, _Dest);
		}
		CONSTEXPR void copyIn(pointer _First, pointer _Last)
		{
			copyIn(_First, _Last, mpFirstElement);
		}
		CONSTEXPR void copyIn(pointer _First, pointer _Last, pointer _Dest)
		{
			insert_pool(_First, _Last, _Dest);
		}
		// copy to output range which is from _Dest to _Dest + size()
		// So check if the destination range is initialized and large enough. 
		// Because this can't be done here.
		template <class OutputIterator>
		CONSTEXPR void copyOut(OutputIterator _Dest) const
		{
			copyOut<OutputIterator>(mpFirstElement, mpLastElement, _Dest);
		}
		template <class OutputIterator>
		CONSTEXPR void copyOut(pointer _First, pointer _Last, OutputIterator _Dest) const
		{
			GLOBAL_ASSERT_RANGE_ERROR(_First, _Last);
			while (_First != _Last)
			{
				*_Dest = *_First;
				++_First;
				++_Dest;
			}
		}
		CONSTEXPR void shrink_to_fit()
		{
			const size_type new_size = shrink_capacity_size();
			if (new_size != capacity())
			{	// worth shrinking, do it
				bool isfrombuffer = is_buffered();
				swap_type new_pool(begin(), end());
				swap(new_pool);
				if (isfrombuffer)
				{
					new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
					//new_pool.mpBeginOfPool = nullptr;
					//new_pool.mpFirstElement = nullptr;
					//new_pool.mpLastElement = nullptr;
					//new_pool.mpEndOfPool = nullptr;
				}
			}
		}
		//=============================================================================
#ifdef POOL_USE_EXTENDED_METHODS
		CONSTEXPR reference erase(size_type pos, size_type len)
		{
			GLOBAL_ASSERT_RANGE_ERROR(mpFirstElement, mpLastElement);
			pointer iter = begin() + pos;
			pointer iter_end = iter + len;
			GLOBAL_ASSERT_RANGE_ERROR(iter, iter_end);
			if (is_from(iter) && is_from(iter_end))
			{
				while (iter < iter_end)
				{
					erase(iter);
					++iter;
				}
			}
		}
		// insert and replace are in my opinion the same therefore there will be no replace
		CONSTEXPR void insert(size_type pos, size_type len, const_reference value)
		{
			GLOBAL_ASSERT_RANGE_ERROR(mpFirstElement, mpLastElement);
			pointer iter = begin() + pos;
			pointer iter_end = iter + len;
			GLOBAL_ASSERT_RANGE_ERROR(iter, iter_end);
			if (is_from(iter))
			{
				if (iter_end >= mpLastElement)
				{
					resize(iter_end - mpFirstElement, value);
				}
				else
				{
					while (iter < iter_end)
					{
						(*iter) = value;
						++iter;
					}
				}
			}
		}
		// method like substr
		template < typename TPoolType >
		CONSTEXPR TPoolType sub(size_type pos, size_type len) const
		{
			TPoolType new_pool;
			if (sub(new_pool, pos, len))
			{
				return new_pool;
			}
			else
			{
				return TPoolType();
			}
		}
		template < typename TPoolType >
		CONSTEXPR bool sub(TPoolType & out, size_type pos, size_type len) const
		{
			GLOBAL_ASSERT_RANGE_ERROR(mpFirstElement, mpLastElement);
			if (pos < 0)
			{
				return false;
			}
			if (len <= 0)
			{
				return false;
			}
			if (pos > size())
			{
				return false;
			}
			if (pos + len > size())
			{
				return false;
			}
			pointer iter = const_cast<pointer>(begin()) + pos;
			pointer iter_end = iter + len;
			if (!is_from(iter))
			{
				return false;
			}
			if (!is_from(iter_end))
			{
				return false;
			}
			GLOBAL_ASSERT_RANGE_ERROR(iter, iter_end);
			//out.reserve(len);
			while (iter != iter_end)
			{
				out.push_back((*iter));
				++iter;
			}
			return true;
		}
		template <typename InputIterator1, typename InputIterator2>
		CONSTEXPR void merge(InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2)
		{
			if (_Begin1 != _End1 && _Begin2 != _End2)
			{
				this_type newEP(_Begin1, _End1, _Begin2, _End2);
				this->swap(newEP);
			}
			else if (_Begin1 != _End1)
			{
				this_type newEP(_Begin1, _End1);
				this->swap(newEP);
			}
			else
			{
				this_type newEP(_Begin2, _End2);
				this->swap(newEP);
			}
		}
		template <typename InputIterator1, typename InputIterator2, typename InputIterator3>
		CONSTEXPR void merge(InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2,
			InputIterator3 _Begin3, InputIterator3 _End3)
		{
			if (_Begin1 != _End1 && _Begin2 != _End2 && _Begin3 != _End3)
			{
				this_type newEP(_Begin1, _End1, _Begin2, _End2, _Begin3, _End3);
				this->swap(newEP);
			}
			else if (_Begin1 != _End1 && _Begin2 != _End2)
			{
				this_type newEP(_Begin1, _End1, _Begin2, _End2);
				this->swap(newEP);
			}
			else if (_Begin2 != _End2 && _Begin3 != _End3)
			{
				this_type newEP(_Begin2, _End2, _Begin3, _End3);
				this->swap(newEP);
			}
			else if (_Begin1 != _End1 && _Begin3 != _End3)
			{
				this_type newEP(_Begin1, _End1, _Begin3, _End3);
				this->swap(newEP);
			}
			else if (_Begin1 != _End1)
			{
				this_type newEP(_Begin1, _End1);
				this->swap(newEP);
			}
			else if (_Begin2 != _End2)
			{
				this_type newEP(_Begin2, _End2);
				this->swap(newEP);
			}
			else
			{
				this_type newEP(_Begin3, _End3);
				this->swap(newEP);
			}
		}
		template <typename InputIterator>
		CONSTEXPR void merge(pointer _IterPos, InputIterator _Begin, InputIterator _End)
		{
			if (_Begin != _End)
			{
				this_type newEP(begin(), _IterPos, _Begin, _End, _IterPos, end());
				this->swap(newEP);
			}
		}
		// split this pool elements into two the given two pools using a 
		// split value. Where the pool types have to implement push_back.
		template < typename TPoolType1, typename TPoolType2 >
		CONSTEXPR void split(TPoolType1 & first, TPoolType2 & second, const_reference split_value) const
		{
			pointer iter = begin();
			GLOBAL_ASSERT_RANGE_ERROR(iter, mpLastElement);
			while (iter != mpLastElement)
			{
				if ((*iter) < split_value)
				{
					first.push_back((*iter));
				}
				else
				{
					second.push_back((*iter));
				}
				++iter;
			}
		}
		// split by a given split position. Which is more efficient because 
		// the elements will be assigned to the given pools. 
		// The given pool
		template < typename TPoolType1, typename TPoolType2 >
		CONSTEXPR void split(TPoolType1 & first, TPoolType2 & second, pointer split_position) const
		{
			first.assign(begin(), split_position);
			second.assign(split_position, end());
		}
		// reverse the order of the elements. 
		// This is complete optional maybe it would be more efficient 
		// to use insert and reverse_iterator instead. 
		CONSTEXPR void reverse()
		{
			reverse(mpFirstElement, mpLastElement);
		}
		CONSTEXPR void reverse(pointer _First, pointer _Last)
		{
			while (_First != --_Last)
			{
				swap_pointer(_First, _Last);
				++_First;
			}
		}
		CONSTEXPR void destroyElement(const size_type pos)
		{
			pointer ptr = mpFirstElement + pos;
			if (ptr != nullptr && mpFirstElement <= ptr && ptr < mpLastElement)
			{
				if (is_buffered())
				{
					mBuffer.destroy(ptr);
				}
				else
				{
					mAllocator.destroy(ptr);
				}
			}
		}
		CONSTEXPR void destroyElement(pointer ptr)
		{
			if (ptr != nullptr && mpFirstElement <= ptr && ptr < mpLastElement)
			{
				if (is_buffered())
				{
					mBuffer.destroy(ptr);
				}
				else
				{
					mAllocator.destroy(ptr);
				}
			}
		}
#endif // POOL_USE_EXTENDED_METHODS
		//=============================================================================
		CONSTEXPR bool equals(const this_type & rhs) const THROW_NEVER
		{
			return (begin() == rhs.begin() && end() == rhs.end());
		}
		CONSTEXPR bool less(const this_type & rhs) const THROW_NEVER
		{
			return (begin() < rhs.begin() && end() < rhs.end());
		}
		CONSTEXPR bool greater(const this_type & rhs) const THROW_NEVER
		{
			return (begin() > rhs.begin() && end() > rhs.end());
		}
		CONSTEXPR void setPointer(pointer beginOfPool, pointer endOfPool, pointer firstElement, pointer lastElement)
		{
			mpBeginOfPool = beginOfPool;
			mpEndOfPool = endOfPool;
			mpFirstElement = firstElement;
			mpLastElement = lastElement;
			//mpBeginOfPool = nullptr;
			//mpFirstElement = nullptr;
			//mpLastElement = nullptr;
			//mpEndOfPool = nullptr;
		}
		//=============================================================================
	protected:
		//=============================================================================
		CONSTEXPR void swap(swap_type & other) THROW_NEVER
		{
			pointer beginOfPool = mpBeginOfPool;
			mpBeginOfPool = other.begin_of_pool();
			//other.mpBeginOfPool = beginOfPool;

			pointer endOfPool = mpEndOfPool;
			mpEndOfPool = other.end_of_pool();
			//other.mpEndOfPool = endOfPool;

			pointer firstElement = mpFirstElement;
			mpFirstElement = other.begin();
			//other.mpFirstElement = firstElement;

			pointer lastElement = mpLastElement;
			mpLastElement = other.end();
			//other.mpLastElement = lastElement;

			other.setPointer(beginOfPool, endOfPool, firstElement, lastElement);

		}
		CONSTEXPR bool is_buffered() const
		{
			return (mBuffer.is_from_buffer(mpFirstElement));
		}
		CONSTEXPR bool ValidateIfFull()
		{
			//if (is_full())
			//{
			//	const size_type new_element_size = size();
			//	this_type new_pool(new_element_size,
			//		next_capacity_size(new_element_size + 1),
			//		begin(), end()); // , val );
			//	swap(new_pool);
			//}
			if (is_full())
			{
				const size_type len = size() + 1;
				bool isfrombuffer = is_buffered();
				if (len > buffer_t::SIZE::TSIZE || !isfrombuffer)
				{
					const size_type new_element_size = size();
					swap_type new_pool(new_element_size,
						next_capacity_size(new_element_size + 1),
						begin(), end()); // , val );
					swap(new_pool);
					if (isfrombuffer)
					{
						new_pool.setPointer(nullptr, nullptr, nullptr, nullptr);
					}
				}
				else
				{
					mpBeginOfPool = mBuffer.get();
					mpFirstElement = mpBeginOfPool;
					mpLastElement = mpFirstElement + len;
					mpEndOfPool = mpFirstElement + buffer_t::SIZE::TSIZE;
				}
				return true;
			}
			return false;
		}
		// release elements is equals to calling the destructor and 
		// deallocation of the whole object pool memory.
		CONSTEXPR void release(bool destroyPool) THROW_NEVER
		{
			if (nullptr != mpFirstElement) // is_releaseable( mpFirstElement )
			{
				if (destroyPool)
				{
					destroy();
				}
				if (is_buffered())
				{
					//mBuffer.deallocate(mpFirstElement, capacity());
					mBuffer.deallocate(mpBeginOfPool, pool_capacity());
				}
				else
				{
					//mAllocator.deallocate(mpFirstElement, capacity());
					mAllocator.deallocate(mpBeginOfPool, pool_capacity());
				}
				mpBeginOfPool = nullptr;
				mpFirstElement = nullptr;
				mpLastElement = nullptr;
				mpEndOfPool = nullptr;
			}
		}
		// the following initializer methods will be called by the different 
		// constructors. Each of them will match one concrete construction 
		// scenario. 
		//bool is_initializeable( const size_type & capacity_count ) const
		//{
		//	return ( ( 0 < capacity_count ) && ( capacity_count <= max_size() ) );
		//}
		CONSTEXPR bool initialize(const size_type & capacity_count, bool releaseOnInit)
		{
			// zero value case has to be separated from range check.
			// The reason is that the method clear, resize( 0 ) and 
			// the default constructor are calling initialize with 
			// a zero value for the new capacity count. 
			// This case can be handled by the methods itself or here. 
			// I decided to handle it here as seperated case, because 
			// this is exactly the place were it is critical to reject 
			// zero values to prevent that the allocation has to handle
			// such requests. 
			// The allocator would have to throw an bad_alloc exception 
			// if he has to handle this case.
			if (capacity_count <= 0)
			{
				return false;
			}
			// Range overflow is handled by returning false.
			// This will lead to a strict no throw policy.
			// Any range overflow won't be seen as system critical error. 
			// As a consequence one has to check if the pool is allocated 
			// by calling the methods empty() or size() after the construction 
			// of the pool.
			if (capacity_count > max_size())
			{
				return false;
			}
			pointer pElements = mBuffer.allocate(capacity_count);
			if (nullptr == pElements)
			{
				pElements = mAllocator.allocate(capacity_count);
			}
			if (nullptr != pElements)
			{
				if (releaseOnInit)
				{
					release(true);
				}
				mpBeginOfPool = pElements;
				mpFirstElement = pElements;
				mpLastElement = pElements;
				mpEndOfPool = pElements + capacity_count;
				return true;
			}
			else
			{
				return false;
			}
		}
		// initialize is equal to allocate memory by copying of existing elements
		template <typename InputIterator>
		CONSTEXPR void initializeCopy(InputIterator _Begin, InputIterator _End)
		{
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin, _End);
			const size_type size_count = size_distance_pointer(_Begin, _End);
			const size_type capacity_count = initialize_capacity_size(size_count);
			if (initialize(capacity_count, false))
			{
				constructCopy(size_count, _Begin, _End);
			}
		}
		template <typename InputIterator>
		CONSTEXPR void initializeCopy(const size_type size_count, const size_type capacity_count,
			InputIterator _Begin, InputIterator _End)
		{
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin, _End);
			if (initialize(capacity_count, false))
			{
				constructCopy(size_count, _Begin, _End);
			}
		}
		template <typename InputIterator>
		CONSTEXPR void swapCopy(const size_type size_count, const size_type capacity_count,
			InputIterator _Begin, InputIterator _End)
		{
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin, _End);
			if (initialize(capacity_count, true))
			{
				constructCopy(size_count, _Begin, _End);
			}
		}
		//template <typename InputIterator>
		template <typename InputIterator1, typename InputIterator2>
		CONSTEXPR void initializeMergeCopy(
			InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2)
		{
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin1, _End1);
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin2, _End2);
			const size_type size_count_1 = size_distance_pointer(_Begin1, _End1);
			const size_type size_count_2 = size_distance_pointer(_Begin2, _End2);
			const size_type capacity_count = initialize_capacity_size(size_count_1 + size_count_2);
			if (initialize(capacity_count, false))
			{
				constructCopy(size_count_1, _Begin1, _End1);
				constructCopy(size_count_2, _Begin2, _End2);
			}
		}
		template <typename InputIterator1, typename InputIterator2, typename InputIterator3>
		CONSTEXPR void initializeMergeCopy3(
			InputIterator1 _Begin1, InputIterator1 _End1,
			InputIterator2 _Begin2, InputIterator2 _End2,
			InputIterator3 _Begin3, InputIterator3 _End3)
		{
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin1, _End1);
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin2, _End2);
			const size_type size_count_1 = size_distance_pointer(_Begin1, _End1);
			const size_type size_count_2 = size_distance_pointer(_Begin2, _End2);
			const size_type size_count_3 = size_distance_pointer(_Begin3, _End3);
			const size_type capacity_count =
				initialize_capacity_size(size_count_1 + size_count_2 + size_count_3);
			if (initialize(capacity_count, false))
			{
				constructCopy(size_count_1, _Begin1, _End1);
				constructCopy(size_count_2, _Begin2, _End2);
				constructCopy(size_count_3, _Begin3, _End3);
			}
		}
		CONSTEXPR void initializeCopyFill(const size_type & size_count,
			const size_type & capacity_count, pointer _Begin, pointer _End,
			const_reference val)
		{
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin, _End);
			if (initialize(capacity_count, false))
			{
				constructCopyFill(size_count, _Begin, _End, val);
			}
		}
		// initialize is equal to allocate memory by filling with one element
		CONSTEXPR void initializeFill(const size_type & size_count, const_reference val)
		{
			initializeFill(size_count, initialize_capacity_size(size_count), val);
		}
		CONSTEXPR void initializeFill(const size_type & size_count,
			const size_type & capacity_count, const_reference val)
		{
			if (initialize(capacity_count, false))
			{
				constructFill(size_count, val);
			}
		}
		// initialize is equal to allocate memory by calling the default constructor
		CONSTEXPR void initializeDefault(const size_type & size_count)
		{
			initializeDefault(size_count, initialize_capacity_size(size_count));
		}
		CONSTEXPR void initializeDefault(const size_type & size_count, const size_type & capacity_count)
		{
			if (initialize(capacity_count, false))
			{
				constructDefault(size_count);
			}
		}
		//=============================================================================
		// construct by filling with one element
		CONSTEXPR void constructFillLast(const_reference val)
		{
			if (is_buffered())
			{
				mBuffer.construct(mpLastElement, val);
			}
			else
			{
				//mAllocator.construct(mpLastElement, val);
				std::allocator_traits<allocator_type>::construct(mAllocator, mpLastElement, val);
			}
			++mpLastElement;
		}
		template <typename... TArgumentTypes>
		CONSTEXPR void emplaceFillLast(TArgumentTypes &&... args)
		{
			if (is_buffered())
			{
				mBuffer.construct(mpLastElement, CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
			}
			else
			{
				//mAllocator.construct(mpLastElement, CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
				std::allocator_traits<allocator_type>::construct(mAllocator, mpLastElement, CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
			}
			++mpLastElement;
		}
		template <typename... TArgumentTypes>
		CONSTEXPR void emplaceFill(pointer positiob, TArgumentTypes &&... args)
		{
			if (is_buffered())
			{
				mBuffer.destroy(positiob);
				mBuffer.construct(positiob, CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
			}
			else
			{
				//mAllocator.destroy(positiob);
				std::allocator_traits<allocator_type>::destroy(mAllocator, positiob);
				//mAllocator.construct(positiob, CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
				std::allocator_traits<allocator_type>::construct(mAllocator, positiob, CELL::TOOLS::MEMORY::TForward<TArgumentTypes>(args)...);
			}
		}
		CONSTEXPR void constructFill(size_type count, const_reference val)
		{
			pointer _Last = mpLastElement + count;
			//GLOBAL_ASSERT_RANGE_ERROR(mpLastElement, _Last);
			bool fromBuffer = is_buffered();
			while (mpLastElement != _Last)
			{
				if (fromBuffer)
				{
					mBuffer.construct(mpLastElement, val);
				}
				else
				{
					//mAllocator.construct(mpLastElement, val);
					std::allocator_traits<allocator_type>::construct(mAllocator, mpLastElement, val);
				}
				++mpLastElement;
			}
		}
		// construct by filling with no element default constructor
		CONSTEXPR void constructDefault(size_type count)
		{
			pointer _Last = mpLastElement + count;

			//call the std allocator method construct for copy constructor NEW has default constructor
			bool fromBuffer = is_buffered();
			while (mpLastElement != _Last)
			{
				if (fromBuffer)
				{
					mBuffer.construct(mpLastElement);
				}
				else
				{
					//mAllocator.construct(mpLastElement);
					std::allocator_traits<allocator_type>::construct(mAllocator, mpLastElement);
				}
				++mpLastElement;
			}
		}
		//void construct( pointer ptr ) 
		//{ 
		//	GLOBAL_ASSERT( ptr && "ERROR NOT CONSTRUCTABLE: parameter has null value!" );
		//	::new ( static_cast< void * >( ptr ) ) value_type;
		//	//::new (ptr) value_type;
		//}
		// construct by copying with existing elements
		template <typename InputIterator>
		CONSTEXPR void constructCopy(size_type count, InputIterator _Begin, InputIterator _End)
		{
			pointer _Last = mpLastElement + count;
			//GLOBAL_ASSERT_RANGE_ERROR(mpLastElement, _Last);
			bool fromBuffer = is_buffered();
			while (mpLastElement != _Last)
			{
				if (fromBuffer)
				{
					mBuffer.construct(mpLastElement, *_Begin);
				}
				else
				{
					//mAllocator.construct(mpLastElement, *_Begin);
					std::allocator_traits<allocator_type>::construct(mAllocator, mpLastElement, *_Begin);
				}
				++mpLastElement;
				++_Begin;
			}
		}
		// construct by copying of existing elements and filling the rest with a 
		// default element.
		CONSTEXPR void constructCopyFill(size_type count, pointer _Begin, pointer _End,
			const_reference val)
		{
			pointer _Last = mpLastElement + count;
			//GLOBAL_ASSERT_RANGE_ERROR(mpLastElement, _Last);
			//GLOBAL_ASSERT_RANGE_ERROR(_Begin, _End);
			bool fromBuffer = is_buffered();
			while ((mpLastElement != _Last) && (_Begin != _End))
			{
				if (fromBuffer)
				{
					mBuffer.construct(mpLastElement, *_Begin);
				}
				else
				{
					//mAllocator.construct(mpLastElement, *_Begin);
					std::allocator_traits<allocator_type>::construct(mAllocator, mpLastElement, *_Begin);
				}
				++mpLastElement;
				++_Begin;
			}
			while (mpLastElement != _Last)
			{
				if (fromBuffer)
				{
					mBuffer.construct(mpLastElement, val);
				}
				else
				{
					//mAllocator.construct(mpLastElement, val);
					std::allocator_traits<allocator_type>::construct(mAllocator, mpLastElement, val);
				}
				++mpLastElement;
			}
		}
		// destroy elements is equals to calling the destructor.
		CONSTEXPR void destroy()
		{
			destroy(mpFirstElement, mpLastElement);
			mpFirstElement = mpBeginOfPool;
			mpLastElement = mpFirstElement;
		}
		CONSTEXPR void destroyLast()
		{
			--mpLastElement;
			if (is_buffered())
			{
				mBuffer.destroy(mpLastElement);
			}
			else
			{
				//mAllocator.destroy( mpLastElement );
				std::allocator_traits<allocator_type>::destroy(mAllocator, mpLastElement);
			}
		}
		CONSTEXPR void destroyFirst()
		{
			if (is_buffered())
			{
				mBuffer.destroy(mpFirstElement);
			}
			else
			{
				//mAllocator.destroy(mpFirstElement);
				std::allocator_traits<allocator_type>::destroy(mAllocator, mpFirstElement);
			}
			++mpFirstElement;
		}
		CONSTEXPR void destroy(size_type count)
		{
			pointer _First = mpLastElement - count;
			destroy(_First, mpLastElement);
			mpLastElement = _First;
		}
		CONSTEXPR void destroy(pointer _First, pointer _Last)
		{
			GLOBAL_ASSERT_RANGE_ERROR(_First, _Last);
			bool fromBuffer = is_buffered();
			while (_First != _Last)
			{
				if (fromBuffer)
				{
					mBuffer.destroy(_First);
				}
				else
				{
					//mAllocator.destroy(_First);
					std::allocator_traits<allocator_type>::destroy(mAllocator, _First);
				}
				++_First;
			}
		}
		//=============================================================================
		// insert new pool elements for internal usage:
		CONSTEXPR void insert_pool(pointer & src_from, pointer & src_to, pointer & dest)
		{
			GLOBAL_ASSERT_RANGE_ERROR(src_from, src_to);
			//std::copy( src_from, src_to, dest );
			const size_type len = size_distance_pointer(src_from, src_to);
			::memcpy(dest, src_from, len * sizeof(value_type));
		}
		CONSTEXPR void insert_pool(const_pointer & src_from, const_pointer & src_to, pointer & dest)
		{
			GLOBAL_ASSERT_RANGE_ERROR(src_from, src_to);
			//std::copy( src_from, src_to, dest );
			const size_type len = size_distance_pointer(src_from, src_to);
			::memcpy(dest, src_from, len * sizeof(value_type));
		}
		// advance pointer for external usage like insert list elements
		template <typename InputIterator>
		CONSTEXPR void advance_iterator(InputIterator & iter, difference_type offset) const
		{
			// Basically this is the algorithm for advancing an iterator:   
			while (offset)
			{
				++iter; 
				--offset;
			}
			// This won't work for all iterators like bidirectional iterator and so on.
			// This means that i will use the method std advance because it 
			// has lots of specializations for random, bidirectional, ... iterators.
			//std::advance( iter, offset );
		}
		// advance pointer for internal usage 
		// This is the same as advancing a random access iterator.
		CONSTEXPR void advance_iterator(pointer & iter, difference_type offset) const
		{
			iter += offset;
		}
		//void advance_pointer( const_pointer & iter, difference_type offset ) const 
		//{
		//	iter += offset;
		//}
		// distance pointer for external usage like insert list elements
		template <typename InputIterator>
		CONSTEXPR size_type size_distance_pointer(InputIterator lhs, InputIterator rhs) const
		{
			// Basically this is the algorithm for calculating the distance 
			// between arbitrary container iterators:
			size_type size_distance = 0;
			while (lhs != rhs)
			{
				++lhs;
				++size_distance;
			}
			return size_distance;
			// But i will use std advance because it has lots of specializations 
			// for random, bidirectional, ... iterators.
			//return std::distance( lhs, rhs );
		}
		// distance pointer for internal usage
		// This is the same as calculating the distance between 
		// random access iterators.
		CONSTEXPR size_type size_distance_pointer(pointer lhs, pointer rhs) const
		{
			return ((lhs < rhs) ? (rhs - lhs) : 0);
		}
		CONSTEXPR size_type size_distance_pointer(const_pointer lhs, const_pointer rhs) const
		{
			return ((lhs < rhs) ? (rhs - lhs) : 0);
		}
		// Calculate the capacity size at the time of the pool initialization.
		CONSTEXPR size_type initialize_capacity_size(const size_type & new_elements_size) const
		{
			const size_type new_grow = mGrow.initialize(new_elements_size);
			return ((new_grow > new_elements_size) ? new_grow : new_elements_size);
		}
		// Calculate the next capacity size that is at least as big  as the given element size.
		CONSTEXPR size_type next_capacity_size(const size_type & new_elements_size) const
		{
			const size_type next_grow = mGrow.next(pool_capacity());
			return ((next_grow > new_elements_size) ? next_grow : new_elements_size);
		}
		// Calculate the prev capacity size that is at least as big  as the given element size.
		CONSTEXPR size_type shrink_capacity_size() const
		{
			size_type new_grow = pool_capacity();
			const size_type sz = size();
			while (new_grow > sz)
			{
				new_grow = mGrow.prev(new_grow);
			}
			return next_capacity_size(new_grow);
		}
		// swap pointer: (Could use std swap i know keep it dont buy it.)
		CONSTEXPR void swap_pointer(pointer & lhs, pointer & rhs)
		{
			const pointer temp(lhs);
			lhs = rhs;
			rhs = temp;
		}
		//=============================================================================
	}; // TObjectPoolPOD
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR void swap(TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & lhs,
		TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & rhs)
	{
		lhs.swap(rhs);
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> &
	operator << (TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & os,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & pool)
	{
		os.insert(pool.begin(), pool.end());
		return os;
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> &
	operator >> (TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & is,
		TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & pool)
	{
		pool.insert(is.begin(), is.end());
		return is;
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR TObjectPoolPOD<TValueType, TAllocType, TGrowStratType>
	operator + (
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & left,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & right)
	{
		TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> new_pool(left.begin(), left.end(), right.begin(), right.end());
		return CELL::TOOLS::MEMORY::TForward<TObjectPoolPOD<TValueType, TAllocType, TGrowStratType>>(new_pool);
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR bool operator == (
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & left,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & right)
	{
		return (left.equals(right));
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR bool operator != (
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & left,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & right)
	{
		return (!left.equals(right));
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR bool operator < (
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & left,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & right)
	{
		return (left.less(right));
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR bool operator <= (
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & left,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & right)
	{
		return (!left.greater(right));
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR bool operator > (
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & left,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & right)
	{
		return (left.greater(right));
	}
	//=============================================================================
	template
	<
		typename TValueType,
		typename TAllocType,
		typename TGrowStratType
	>
	CONSTEXPR bool operator >= (
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & left,
		const TObjectPoolPOD<TValueType, TAllocType, TGrowStratType> & right)
	{
		return (!left.less(right));
	}
	//=============================================================================
	template
	<
		typename TValueType,
		size_t TBUFELEMS = 0,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	struct TPoolPOD
	{
		//typedef TObjectPoolPOD
		//<
		//	TValueType,
		//	TAllocType,
		//	OPOOLGROW::TPoolGStratPOD< typename TAllocType::size_type, TBUFELEMS >
		//> Type;
		using Type = OPOOLPOD::TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			OPOOLGROW::TPoolGStratPOD< typename TAllocType::size_type, TBUFELEMS >
		>;
	};
	//=============================================================================
	template
	<
		typename TValueType,
		size_t TBUFELEMS = 0,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	struct TPO2PoolPOD
	{
		using Type = OPOOLPOD::TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			OPOOLGROW::TPO2GStratPOD< typename TAllocType::size_type, TBUFELEMS >
		>;
	};
	//=============================================================================
	template
	<
		typename TValueType,
		size_t TBUFELEMS = 0,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	struct TArrayPoolPOD
	{
		using Type = OPOOLPOD::TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			OPOOLGROW::TArrayGStratPOD< typename TAllocType::size_type, TBUFELEMS >
		>;
	};
	//=============================================================================
	template
	<
		typename TValueType,
		size_t TBUFELEMS = 0,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	struct TVectorPoolPOD
	{
		using Type = OPOOLPOD::TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			OPOOLGROW::TVecGStratPOD< typename TAllocType::size_type, TBUFELEMS >
		>;
	};
	//=============================================================================
	template
	<
		typename TValueType,
		size_t TBUFELEMS = 0,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	struct TBTPoolPOD
	{
		using Type = OPOOLPOD::TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			OPOOLGROW::TBinTreeGStratPOD< typename TAllocType::size_type, TBUFELEMS >
		>;
	};
	//=============================================================================
	template
	<
		typename TValueType,
		size_t TBUFELEMS = 0,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	struct TSPoolPOD
	{
		using Type = OPOOLPOD::TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			OPOOLGROW::TStaticGStratPOD< typename TAllocType::size_type, TBUFELEMS >
		>;
	};
	//=============================================================================
	template
	<
		typename TValueType,
		size_t TBUFELEMS = 0,
		typename TAllocType = DEFAULT_OBJECT_ALLOCATOR< TValueType >
	>
	struct TPPoolPOD
	{
		using Type = OPOOLPOD::TObjectPoolPOD
		<
			TValueType,
			TAllocType,
			OPOOLGROW::TPrimeGStratPOD< typename TAllocType::size_type, TBUFELEMS >
		>;
	};
//=============================================================================
} // namespace OPOOLPOD
//=============================================================================
} // namespace CELL
//=============================================================================
#ifdef  _MSC_VER
	#pragma PRAGMA_WARNING_DEFAULT4201
	#pragma PRAGMA_WARNING_DEFAULT4996
	#pragma PRAGMA_WARNING_POP
	#pragma PRAGMA_PACK_POP
#endif 
//=============================================================================

#endif // __POD_OBJECT_POOL_H__
