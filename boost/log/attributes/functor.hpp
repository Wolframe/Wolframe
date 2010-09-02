/*
 *          Copyright Andrey Semashev 2007 - 2010.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */
/*!
 * \file   functor.hpp
 * \author Andrey Semashev
 * \date   24.06.2007
 *
 * The header contains implementation of an attribute that calls a third-party function on value acquirement.
 */

#if (defined(_MSC_VER) && _MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef BOOST_LOG_ATTRIBUTES_FUNCTOR_HPP_INCLUDED_
#define BOOST_LOG_ATTRIBUTES_FUNCTOR_HPP_INCLUDED_

#include <boost/static_assert.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/log/detail/prologue.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/basic_attribute_value.hpp>

namespace boost {

namespace BOOST_LOG_NAMESPACE {

namespace attributes {

/*!
 * \brief A class of an attribute that acquires its value from a third-party functor
 *
 * The attribute calls a stored nullary function object to acquire each value.
 * The result type of the function object is the attribute value type.
 *
 * It is not recommended to use this class directly. Use \c make_functor_attr convenience functions
 * to construct the attribute instead.
 */
template< typename R >
class functor :
    public attribute
{
    //  The result type of the function object must not be void
    BOOST_STATIC_ASSERT(!is_void< R >::value);

public:
    //! The attribute value type
    typedef R value_type;

protected:
    //! Base class for factory implementation
    class BOOST_LOG_NO_VTABLE BOOST_LOG_VISIBLE impl :
        public attribute::impl
    {
    };

    //! Factory implementation
    template< typename T >
    class impl_template :
        public impl
    {
    private:
        //! Functor that returns attribute values
        /*!
         * \note The constness signifies that the function object should avoid
         *       modifying its state since it's not protected against concurrent calls.
         */
        const T m_Functor;

    public:
        /*!
         * Constructor with the stored delegate imitialization
         */
        explicit impl_template(T const& fun) : m_Functor(fun) {}

        attribute_value get_value()
        {
            typedef basic_attribute_value< value_type > attr_value;
            return attribute_value(new attr_value(m_Functor()));
        }
    };

public:
    /*!
     * Initializing constructor
     */
    template< typename T >
    explicit functor(T const& fun) : attribute(new impl_template< T >(fun))
    {
    }
    /*!
     * Constructor for casting support
     */
    explicit functor(cast_source const& source) :
        attribute(source.as< impl >())
    {
    }
};

#ifndef BOOST_NO_RESULT_OF

/*!
 * The function constructs functor attribute instance with the provided functional object.
 *
 * \param fun Nullary functional object that returns an actual stored value for an attribute value.
 * \return Pointer to the attribute instance
 */
template< typename T >
inline functor<
    typename remove_cv<
        typename remove_reference<
            typename result_of< T() >::type
        >::type
    >::type
> make_functor_attr(T const& fun)
{
    typedef typename remove_cv<
        typename remove_reference<
            typename result_of< T() >::type
        >::type
    >::type result_type;

    typedef functor< result_type > functor_t;
    return functor_t(fun);
}

#endif // BOOST_NO_RESULT_OF

#ifndef BOOST_LOG_DOXYGEN_PASS

/*!
 * The function constructs functor attribute instance with the provided functional object.
 * Use this version if your compiler fails to determine the result type of the functional object.
 *
 * \param fun Nullary functional object that returns an actual stored value for an attribute value.
 * \return Pointer to the attribute instance
 */
template< typename R, typename T >
inline functor<
    typename remove_cv<
        typename remove_reference< R >::type
    >::type
> make_functor_attr(T const& fun)
{
    typedef typename remove_cv<
        typename remove_reference< R >::type
    >::type result_type;

    typedef functor< result_type > functor_t;
    return functor_t(fun);
}

#endif // BOOST_LOG_DOXYGEN_PASS

} // namespace attributes

} // namespace log

} // namespace boost

#endif // BOOST_LOG_ATTRIBUTES_FUNCTOR_HPP_INCLUDED_
