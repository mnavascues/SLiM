//
//  eidos_intrusive_ptr.h
//  Eidos
//
//  Created by Ben Haller on 9/25/15.
//  Copyright (c) 2015 Messer Lab, http://messerlab.org/software/. All rights reserved.
//


//	Eidos_intrusive_ptr is adapted from Boost's intrusive_ptr.hpp, version 1.59.0.
//	That distribution was commented as follows:
//
//  Copyright (c) 2001, 2002 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. See
// accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt


// Apropos of that, the Boost Software License Version 1.0 is as follows:

/*
	Boost Software License - Version 1.0 - August 17th, 2003

	Permission is hereby granted, free of charge, to any person or organization
	obtaining a copy of the software and accompanying documentation covered by
	this license (the "Software") to use, reproduce, display, distribute,
	execute, and transmit the Software, and to prepare derivative works of the
	Software, and to permit third-parties to whom the Software is furnished to
	do so, all subject to the following:

	The copyright notices in the Software and this entire statement, including
	the above license grant, this restriction and the following disclaimer,
	must be included in all copies of the Software, in whole or in part, and
	all derivative works of the Software, unless such copies or derivative
	works are solely in the form of machine-executable object code generated by
	a source language processor.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
	SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
	FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	DEALINGS IN THE SOFTWARE.
*/

// This source file has been derived from Boost's intrusive_ptr.hpp and
// intrusive_ref_counter.hpp in accordance with this license.


#ifndef SLiM_eidos_intrusive_ptr_h
#define SLiM_eidos_intrusive_ptr_h

#include <ostream>


//
//  Eidos_intrusive_ptr
//
//  A smart pointer that uses intrusive reference counting.
//
//  Relies on unqualified calls to
//  
//      void Eidos_intrusive_ptr_add_ref(T * p);
//      void Eidos_intrusive_ptr_release(T * p);
//
//          (p != 0)
//
//  The object is responsible for destroying itself.
//

template<class T> class Eidos_intrusive_ptr
{
private:
	
	typedef Eidos_intrusive_ptr this_type;
	
public:
	
	typedef T element_type;
	
	Eidos_intrusive_ptr() : px( 0 )
	{
	}
	
	explicit Eidos_intrusive_ptr( T * p, bool add_ref = true ): px( p )
	{
		if( px != 0 && add_ref ) Eidos_intrusive_ptr_add_ref( px );
	}
	
	template<class U>
	Eidos_intrusive_ptr( Eidos_intrusive_ptr<U> const & rhs ) : px( rhs.get() )
	{
		if( px != 0 ) Eidos_intrusive_ptr_add_ref( px );
	}
	
	Eidos_intrusive_ptr(Eidos_intrusive_ptr const & rhs): px( rhs.px )
	{
		if( px != 0 ) Eidos_intrusive_ptr_add_ref( px );
	}
	
	~Eidos_intrusive_ptr()
	{
		if( px != 0 ) Eidos_intrusive_ptr_release( px );
	}
	/*
	 // I think I don't need/want this...
	template<class U> Eidos_intrusive_ptr & operator=(Eidos_intrusive_ptr<U> const & rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}
	*/
	// Move support
	
	Eidos_intrusive_ptr(Eidos_intrusive_ptr && rhs) : px( rhs.px )
	{
		rhs.px = 0;
	}
	
	Eidos_intrusive_ptr & operator=(Eidos_intrusive_ptr && rhs)
	{
		this_type( static_cast< Eidos_intrusive_ptr && >( rhs ) ).swap(*this);
		return *this;
	}
	
	Eidos_intrusive_ptr & operator=(Eidos_intrusive_ptr const & rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}
	/*
	 // I think I don't need/want this...
	Eidos_intrusive_ptr & operator=(T * rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}
	*/
	void reset()
	{
		this_type().swap( *this );
	}
	
	void reset( T * rhs )
	{
		this_type( rhs ).swap( *this );
	}
	
	void reset( T * rhs, bool add_ref )
	{
		this_type( rhs, add_ref ).swap( *this );
	}
	
	T * get() const
	{
		return px;
	}
	
	T * detach()
	{
		T * ret = px;
		px = 0;
		return ret;
	}
	
	T & operator*() const
	{
		return *px;
	}
	
	T * operator->() const
	{
		return px;
	}
	
	// implicit conversion to "bool", from boost/smart_ptr/detail/operator_bool.hpp
	
	explicit operator bool () const
	{
		return px != 0;
	}
	
	bool operator! () const
	{
		return px == 0;
	}
	
	void swap(Eidos_intrusive_ptr & rhs)
	{
		T * tmp = px;
		px = rhs.px;
		rhs.px = tmp;
	}
	
private:
	
	T * px;
};

template<class T, class U> inline bool operator==(Eidos_intrusive_ptr<T> const & a, Eidos_intrusive_ptr<U> const & b)
{
	return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(Eidos_intrusive_ptr<T> const & a, Eidos_intrusive_ptr<U> const & b)
{
	return a.get() != b.get();
}

template<class T, class U> inline bool operator==(Eidos_intrusive_ptr<T> const & a, U * b)
{
	return a.get() == b;
}

template<class T, class U> inline bool operator!=(Eidos_intrusive_ptr<T> const & a, U * b)
{
	return a.get() != b;
}

template<class T, class U> inline bool operator==(T * a, Eidos_intrusive_ptr<U> const & b)
{
	return a == b.get();
}

template<class T, class U> inline bool operator!=(T * a, Eidos_intrusive_ptr<U> const & b)
{
	return a != b.get();
}

template<class T> inline bool operator<(Eidos_intrusive_ptr<T> const & a, Eidos_intrusive_ptr<T> const & b)
{
	return std::less<T *>()(a.get(), b.get());
}

template<class T> void swap(Eidos_intrusive_ptr<T> & lhs, Eidos_intrusive_ptr<T> & rhs)
{
	lhs.swap(rhs);
}

// mem_fn support

template<class T> T * get_pointer(Eidos_intrusive_ptr<T> const & p)
{
	return p.get();
}

template<class T, class U> Eidos_intrusive_ptr<T> static_pointer_cast(Eidos_intrusive_ptr<U> const & p)
{
	return static_cast<Eidos_intrusive_ptr<T>>(static_cast<T *>(p.get()));
}

template<class T, class U> Eidos_intrusive_ptr<T> const_pointer_cast(Eidos_intrusive_ptr<U> const & p)
{
	return static_cast<Eidos_intrusive_ptr<T>>(const_cast<T *>(p.get()));
}

template<class T, class U> Eidos_intrusive_ptr<T> dynamic_pointer_cast(Eidos_intrusive_ptr<U> const & p)
{
	return static_cast<Eidos_intrusive_ptr<T>>(dynamic_cast<T *>(p.get()));
}

// operator<<

template<class Y> std::ostream & operator<< (std::ostream & os, Eidos_intrusive_ptr<Y> const & p)
{
	os << p.get();
	return os;
}


#endif


























































