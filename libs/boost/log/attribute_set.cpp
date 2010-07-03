/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   attribute_set.cpp
 * \author Andrey Semashev
 * \date   19.04.2007
 *
 * \brief  This header is the Boost.Log library implementation, see the library documentation
 *         at http://www.boost.org/libs/log/doc/log.html.
 */

#include <deque>
#include <boost/assert.hpp>
#include <boost/intrusive/options.hpp>
#include <boost/intrusive/list.hpp>
#include <boost/intrusive/link_mode.hpp>
#include <boost/intrusive/derivation_value_traits.hpp>
#include <boost/log/attributes/attribute_set.hpp>

namespace boost {

namespace BOOST_LOG_NAMESPACE {

template< typename CharT >
inline basic_attribute_set< CharT >::node_base::node_base() :
    m_pPrev(NULL),
    m_pNext(NULL)
{
}

template< typename CharT >
inline basic_attribute_set< CharT >::node::node() :
    node_base(),
    m_Value()
{
}

template< typename CharT >
inline basic_attribute_set< CharT >::node::node(node const& that) :
    node_base(),
    m_Value(that.m_Value)
{
}

template< typename CharT >
inline basic_attribute_set< CharT >::node::node(key_type const& key, mapped_type const& data) :
    node_base(),
    m_Value(key, data)
{
}

template< typename CharT >
inline typename basic_attribute_set< CharT >::node&
basic_attribute_set< CharT >::node::operator= (node const& that)
{
    const_cast< key_type& >(m_Value.first) = that.m_Value.first;
    m_Value.second = that.m_Value.second;
    return *this;
}

//! Attribute set implementation
template< typename CharT >
struct basic_attribute_set< CharT >::implementation
{
public:
    //! Attribute name identifier type
    typedef typename key_type::id_type id_type;

    //! The container that stores elements
    typedef std::deque< node > node_storage;

    //! Node base class traits for the intrusive list
    struct node_traits
    {
        typedef node_base node;
        typedef node* node_ptr;
        typedef node const* const_node_ptr;
        static node* get_next(const node* n) { return n->m_pNext; }
        static void set_next(node* n, node* next) { n->m_pNext = next; }
        static node* get_previous(const node* n) { return n->m_pPrev; }
        static void set_previous(node* n, node* prev) { n->m_pPrev = prev; }
    };

    //! Contained node traits for the intrusive list
    typedef intrusive::derivation_value_traits<
        node,
        node_traits,
        intrusive::normal_link
    > value_traits;

    //! The container that allows to iterate through elements
    typedef intrusive::list<
        node,
        intrusive::value_traits< value_traits >,
        intrusive::constant_time_size< true >
    > node_list;

    //! Cleanup function object used to erase elements from the container
    struct node_disposer
    {
        typedef void result_type;
        void operator() (node* p) const
        {
            p->m_pPrev = p->m_pNext = NULL;
            const_cast< key_type& >(p->m_Value.first) = key_type();
            p->m_Value.second.reset();
        }
    };

public:
    //! The base identifier value for the storage container
    id_type m_BaseID;
    //! Node storage
    node_storage m_Storage;
    //! List of nodes
    node_list m_Nodes;

public:
    implementation() : m_BaseID(0) {}

private:
    implementation(implementation const&);
    implementation& operator= (implementation const&);
};

//! Default constructor
template< typename CharT >
basic_attribute_set< CharT >::basic_attribute_set() :
    m_pImpl(new implementation())
{
}

//! Copy constructor
template< typename CharT >
basic_attribute_set< CharT >::basic_attribute_set(basic_attribute_set const& that) :
    m_pImpl(new implementation())
{
    implementation* const p = that.m_pImpl;
    if (!p->m_Nodes.empty())
    {
        // Reserve necessary space in the storage
        m_pImpl->m_BaseID = p->m_Nodes.front().m_Value.first.id();
        m_pImpl->m_Storage.insert(
            m_pImpl->m_Storage.end(),
            p->m_Nodes.back().m_Value.first.id() - m_pImpl->m_BaseID + 1,
            node());

        // Copy elements
        typename implementation::node_list::const_iterator
            it = p->m_Nodes.begin(),
            _end = p->m_Nodes.end();
        for (; it != _end; ++it)
        {
            node& n = m_pImpl->m_Storage[it->m_Value.first.id() - m_pImpl->m_BaseID];
            const_cast< key_type& >(n.m_Value.first) = it->m_Value.first;
            n.m_Value.second = it->m_Value.second;
            m_pImpl->m_Nodes.push_back(n);
        }
    }
}

//! Destructor
template< typename CharT >
basic_attribute_set< CharT >::~basic_attribute_set()
{
    delete m_pImpl;
}

//! Assignment
template< typename CharT >
basic_attribute_set< CharT >& basic_attribute_set< CharT >::operator= (basic_attribute_set that)
{
    this->swap(that);
    return *this;
}

//  Iterator generators
template< typename CharT >
typename basic_attribute_set< CharT >::iterator basic_attribute_set< CharT >::begin()
{
    return iterator(m_pImpl->m_Nodes.begin().pointed_node());
}
template< typename CharT >
typename basic_attribute_set< CharT >::iterator basic_attribute_set< CharT >::end()
{
    return iterator(m_pImpl->m_Nodes.end().pointed_node());
}
template< typename CharT >
typename basic_attribute_set< CharT >::const_iterator basic_attribute_set< CharT >::begin() const
{
    return const_iterator(m_pImpl->m_Nodes.begin().pointed_node());
}
template< typename CharT >
typename basic_attribute_set< CharT >::const_iterator basic_attribute_set< CharT >::end() const
{
    return const_iterator(m_pImpl->m_Nodes.end().pointed_node());
}

//! The method returns number of elements in the container
template< typename CharT >
typename basic_attribute_set< CharT >::size_type basic_attribute_set< CharT >::size() const
{
    return m_pImpl->m_Nodes.size();
}

//! Insertion method
template< typename CharT >
std::pair< typename basic_attribute_set< CharT >::iterator, bool >
basic_attribute_set< CharT >::insert(key_type key, mapped_type const& data)
{
    BOOST_ASSERT(!!key);

    // Verify that the key fits into the storage and grow it, if needed
    node* p = NULL;
    if (key.id() < m_pImpl->m_BaseID)
    {
        m_pImpl->m_Storage.insert(
            m_pImpl->m_Storage.begin(),
            m_pImpl->m_BaseID - key.id(),
            node());
        m_pImpl->m_BaseID = key.id();
        p = &m_pImpl->m_Storage.front();
    }
    else if (key.id() - m_pImpl->m_BaseID >= m_pImpl->m_Storage.size())
    {
        m_pImpl->m_Storage.insert(
            m_pImpl->m_Storage.end(),
            key.id() - m_pImpl->m_BaseID - m_pImpl->m_Storage.size() + 1,
            node());
        p = &m_pImpl->m_Storage.back();
    }
    else
    {
        // The node is already within the storage boundaries
        p = &m_pImpl->m_Storage[key.id() - m_pImpl->m_BaseID];
    }

    if (!p->m_Value.second)
    {
        // The key is not used in the storage
        const_cast< key_type& >(p->m_Value.first) = key;
        p->m_Value.second = data;
        m_pImpl->m_Nodes.push_back(*p);
        return std::make_pair(iterator(p), true);
    }
    else
        return std::make_pair(iterator(p), false);
}

//! The method erases all attributes with the specified name
template< typename CharT >
typename basic_attribute_set< CharT >::size_type
basic_attribute_set< CharT >::erase(key_type key)
{
    iterator it = this->find(key);
    if (it != end())
    {
        this->erase(it);
        return 1;
    }
    else
        return 0;
}

//! The method erases the specified attribute
template< typename CharT >
void basic_attribute_set< CharT >::erase(iterator it)
{
    m_pImpl->m_Nodes.erase_and_dispose(
        m_pImpl->m_Nodes.iterator_to(*static_cast< node* >(it.base())),
        typename implementation::node_disposer());
}
//! The method erases all attributes within the specified range
template< typename CharT >
void basic_attribute_set< CharT >::erase(iterator _begin, iterator _end)
{
    while (_begin != _end)
    {
        m_pImpl->m_Nodes.erase_and_dispose(
            m_pImpl->m_Nodes.iterator_to(*static_cast< node* >((_begin++).base())),
            typename implementation::node_disposer());
    }
}

//! The method clears the container
template< typename CharT >
void basic_attribute_set< CharT >::clear()
{
    m_pImpl->m_Nodes.clear();
    m_pImpl->m_Storage.clear();
    m_pImpl->m_BaseID = 0;
}

//! Internal lookup implementation
template< typename CharT >
typename basic_attribute_set< CharT >::iterator
basic_attribute_set< CharT >::find(key_type key)
{
    if (key.id() >= m_pImpl->m_BaseID)
    {
        typedef typename implementation::node_storage::size_type _size_type;
        _size_type index = key.id() - m_pImpl->m_BaseID;
        if (index < m_pImpl->m_Storage.size())
        {
            node& n = m_pImpl->m_Storage[index];
            if (!!n.m_Value.second)
            {
                return iterator(&n);
            }
        }
    }

    return iterator(m_pImpl->m_Nodes.end().pointed_node());
}

#ifdef BOOST_LOG_USE_CHAR
template class basic_attribute_set< char >;
#endif
#ifdef BOOST_LOG_USE_WCHAR_T
template class basic_attribute_set< wchar_t >;
#endif

} // namespace log

} // namespace boost
