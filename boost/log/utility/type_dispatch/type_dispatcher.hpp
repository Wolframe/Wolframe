/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   type_dispatcher.hpp
 * \author Andrey Semashev
 * \date   15.04.2007
 *
 * The header contains definition of generic type dispatcher interfaces.
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_TYPE_DISPATCHER_HPP_INCLUDED_
#define BOOST_LOG_TYPE_DISPATCHER_HPP_INCLUDED_

#include <typeinfo>
#include <boost/static_assert.hpp>
#include <boost/log/detail/prologue.hpp>
#include <boost/log/detail/visible_type.hpp>
#include <boost/log/utility/explicit_operator_bool.hpp>

namespace boost {

namespace BOOST_LOG_NAMESPACE {

namespace aux {

    //! The base class for type visitors
    class type_visitor_base
    {
    protected:
        void* m_pReceiver;
        void* m_pTrampoline;

    public:
        explicit type_visitor_base(void* receiver = 0, void* tramp = 0) :
            m_pReceiver(receiver),
            m_pTrampoline(tramp)
        {
        }
        template< typename ValueT >
        explicit type_visitor_base(void* receiver, void (*tramp)(void*, ValueT const&)) :
            m_pReceiver(receiver)
        {
            typedef void (*trampoline_t)(void*, ValueT const&);
            BOOST_STATIC_ASSERT(sizeof(trampoline_t) == sizeof(void*));
            union
            {
                void* as_pvoid;
                trampoline_t as_trampoline;
            }
            caster;
            caster.as_trampoline = tramp;
            m_pTrampoline = caster.as_pvoid;
        }

        template< typename ReceiverT, typename ValueT >
        static void trampoline(void* receiver, ValueT const& value)
        {
            (*static_cast< ReceiverT* >(receiver))(value);
        }
    };

} // namespace aux

/*!
 * \brief An interface to the concrete type visitor
 *
 * This interface is used by type dispatchers to consume the dispatched value.
 */
template< typename T >
class type_visitor :
    private boost::log::aux::type_visitor_base
{
private:
    //! Base class type
    typedef boost::log::aux::type_visitor_base base_type;
    //! Type of the trampoline method
    typedef void (*trampoline_t)(void*, T const&);

public:
    //! The type, which the visitor is able to consume
    typedef T supported_type;

public:
#ifndef BOOST_LOG_DOXYGEN_PASS
    /*!
     * Default constructor. Creates an empty visitor.
     */
    type_visitor() : base_type()
    {
    }
    /*!
     * Initializing constructor. Creates a visitor that refers to the specified receiver.
     */
    explicit type_visitor(base_type const& base) : base_type(base)
    {
    }
#endif // BOOST_LOG_DOXYGEN_PASS

    /*!
     * The operator invokes the visitor-specific logic with the given value
     *
     * \param value The dispatched value
     */
    void operator() (T const& value) const
    {
        BOOST_STATIC_ASSERT(sizeof(trampoline_t) == sizeof(void*));
        union
        {
            void* as_pvoid;
            trampoline_t as_trampoline;
        }
        caster;
        caster.as_pvoid = this->m_pTrampoline;
        (caster.as_trampoline)(this->m_pReceiver, value);
    }

    /*!
     * The operator checks if the visitor is attached to a receiver
     */
    BOOST_LOG_EXPLICIT_OPERATOR_BOOL()

    /*!
     * The operator checks if the visitor is not attached to a receiver
     */
    bool operator! () const
    {
        return (this->m_pReceiver == 0);
    }
};

/*!
 * \brief A type dispatcher interface
 *
 * All type dispatchers support this interface. It is used to acquire the
 * visitor interface for the requested type.
 */
struct BOOST_LOG_NO_VTABLE type_dispatcher
{
public:
    /*!
     * Virtual destructor
     */
    virtual ~type_dispatcher() {}

    /*!
     * The method requests a type visitor for a value of type \c T
     *
     * \return The type-specific visitor or NULL, if the type is not supported
     */
    template< typename T >
    type_visitor< T > get_visitor()
    {
        return type_visitor< T >(this->get_visitor(
            typeid(boost::log::aux::visible_type< T >)));
    }

private:
#ifndef BOOST_LOG_DOXYGEN_PASS
    //! The get_visitor method implementation
    virtual boost::log::aux::type_visitor_base get_visitor(std::type_info const& type) = 0;
#endif // BOOST_LOG_DOXYGEN_PASS
};

} // namespace log

} // namespace boost

#endif // BOOST_LOG_TYPE_DISPATCHER_HPP_INCLUDED_
