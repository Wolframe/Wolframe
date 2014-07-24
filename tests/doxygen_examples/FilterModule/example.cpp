#include "appdevel/moduleFrameMacros.hpp"
#include "appdevel/filterModuleMacros.hpp"
#include "filter/filter.hpp"

class MyInputFilter
	:public _Wolframe::langbind::InputFilter
{
public:
	MyInputFilter()
		:_Wolframe::langbind::InputFilter("myfilter")
	{
		// ... initialize your input filter here
	}

	virtual _Wolframe::langbind::InputFilter* copy() const
	{
		// ... create a copy of this input filter here
	}

	virtual _Wolframe::langbind::InputFilter* initcopy() const
	{
		// ... create a copy of this in first initialization state here
	}

	virtual void putInput( const void* ptr, std::size_t size, bool end)
	{
		// ... feed the filter with the next input chunk here
	}

	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		// ... get the unprocessed input here
	}

	virtual bool getNext( typename _Wolframe::langbind::InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		// ... get the next element produced by this input filter here
		// ... return false on error or end of buffer (state)
		// ... return true, if we returned a valid element in type,element,elementsize
	}

	virtual bool setFlags( _Wolframe::langbind::FilterBase::Flags f)
	{
		// ... check if you can fulfill the requirements imposed by the flags and set them if yes
		// ... return false, if the requirements imposed by the flags cannot be met
	}

	virtual bool checkSetFlags( _Wolframe::langbind::FilterBase::Flags f)const
	{
		// ... check, if the requirements imposed by the flags can be met
	}

	virtual const _Wolframe::types::DocMetaData* getMetaData()
	{
		// ... get the document meta data if possible
		// ... return false on error, or if another input chunk is needed
	}
};

class MyOutputFilter
	:public _Wolframe::langbind::OutputFilter
{
public:
	MyOutputFilter()
		:_Wolframe::langbind::OutputFilter("myfilter")
	{
		// ... initialize your input filter here
	}

	virtual _Wolframe::langbind::OutputFilter* copy() const
	{
		// ... create and return a copy of this output filter here
	}

	virtual bool print( typename _Wolframe::langbind::OutputFilter::ElementType type, const void* element, std::size_t elementsize)
	{
		// ... do the output of one element here
		// ... use the method write( const void*, std::size_t) for writing to sink
	}

	virtual bool close()
	{
		// ... do check if the final close has been called (it does not have to) and do the final actions of the output filter
		// ... return false, if this method has to be called again because not all output could be flushed (failed method write( const void*, std::size_t))
	}
};

class MyFilter
	:public _Wolframe::langbind::Filter
{
public:
	MyFilter()
		:_Wolframe::langbind::Filter( new MyInputFilter(), new MyOutputFilter()){}
};

class MyFilterType
	:public _Wolframe::langbind::FilterType
{
public:
	MyFilterType()
		:_Wolframe::langbind::FilterType("myfilter"){}

	virtual _Wolframe::langbind::Filter* create( const std::vector<_Wolframe::langbind::FilterArgument>&) const
	{
		return new MyFilter();
	}
};

WF_MODULE_BEGIN( "MyFilter", "my content filter")
 WF_FILTER_TYPE( "myfilter", MyFilterType)
WF_MODULE_END
