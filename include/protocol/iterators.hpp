#ifndef _Wolframe_PROTOCOL_ITERATORS_HPP_INCLUDED
#define _Wolframe_PROTOCOL_ITERATORS_HPP_INCLUDED
#include <iterator>

namespace _Wolframe {
namespace protocol {

namespace array
{
//template for random access iterators for arrays as used by the protocol
template <class array_type_, typename size_type_, typename value_type_, typename access_type_, typename deref_type_>
class iterator_t
{
public:
   //STL traits and typedefs
   typedef array_type_ array_type;
   typedef access_type_ access_type;
   typedef deref_type_ deref_type;
   typedef std::random_access_iterator_tag iterator_category;
   typedef size_type_ size_type;
   typedef value_type_ value_type;
   typedef size_type difference_type;
   typedef size_type distance_type;
   typedef value_type* pointer;
   typedef value_type& reference;
   
   //constructors
   iterator_t()                                             :m_this(0),m_idx(0){}
   iterator_t( array_type* t)                               :m_this(t),m_idx(0){}
   iterator_t( const iterator_t& o)                         :m_this(o.m_this),m_idx(o.m_idx){}
   
   iterator_t& operator=( const iterator_t& o)              {m_this = o.m_this; m_idx = o.m_idx; return *this;}
   access_type operator*()                                  {return (*m_this)[ m_idx];}
   access_type operator*() const                            {return (*m_this)[ m_idx];}
   deref_type operator->()                                  {return &m_this->at( m_idx);}
   deref_type operator->() const                            {return &m_this->at( m_idx);}
   iterator_t& operator++()                                 {++m_idx; return *this;}
   iterator_t operator++(int)                               {iterator_t rt=*this; ++m_idx; return rt;}
   iterator_t& operator--()                                 {--m_idx; return *this;}
   iterator_t operator--(int)                               {iterator_t rt=*this; --m_idx; return rt;}
   iterator_t operator+( difference_type d)                 {iterator_t rt=*this; rt.m_idx+=d; return rt;}
   iterator_t operator+( difference_type d) const           {iterator_t rt=*this; rt.m_idx+=d; return rt;}
   iterator_t operator-( difference_type d)                 {iterator_t rt=*this; rt.m_idx-=d; return rt;}
   iterator_t operator-( difference_type d) const           {iterator_t rt=*this; rt.m_idx-=d; return rt;}
   iterator_t& operator+=( difference_type d)               {m_idx+=d; return *this;}
   iterator_t& operator-=( difference_type d)               {m_idx-=d; return *this;}
   difference_type operator-( const iterator_t& b) const    {return m_idx-b.m_idx;}
   bool operator == (const iterator_t& o) const             {return (o.m_this == m_this && o.m_idx == m_idx);}
   bool operator != (const iterator_t& o) const             {return (o.m_this != m_this || o.m_idx != m_idx);}
   bool operator > (const iterator_t& o) const              {return (o.m_this == m_this && m_idx > o.m_idx);}
   bool operator < (const iterator_t& o) const              {return (o.m_this == m_this && m_idx < o.m_idx);}
   bool operator >= (const iterator_t& o) const             {return (o.m_this == m_this && m_idx >= o.m_idx);}
   bool operator <= (const iterator_t& o) const             {return (o.m_this == m_this && m_idx <= o.m_idx);}
   const value_type& operator[]( size_type& i) const        {return (*m_this)[m_idx+i];}
   value_type& operator[]( size_type& i)                    {return (*m_this)[m_idx+i];}
   deref_type ptr() const                                   {return (deref_type)(m_this->ptr()) + m_idx;}
   deref_type ptr()                                         {return (deref_type)(m_this->ptr()) + m_idx;}

private:
   array_type* m_this;     //< the array is accessed over the structure this, to catch ABR/ABW for sure
   size_type m_idx;
};
}//namespace array

}}//namespace
#endif
