//PF:HACK: This is a temporary module to make thinks link on Windows. 
//!!! PLEASE DO NOT REMOVE BEFORE HAVING A SOLUTION FOR WINDOWS
#include "processor/procProvider.hpp"
#include "procProviderImpl.hpp"

using namespace _Wolframe;
using namespace _Wolframe::proc;

db::Database* ProcessorProvider::transactionDatabase() const
{
	return m_impl->transactionDatabase();
}




