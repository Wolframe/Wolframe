#include <boost/thread.hpp>


void
someWork ()
{
  double a;
  for (int i = 0; i < 1000; i++)
    {
      a += log ((double) i);
    }
}

int
main ()
{
  for (int i = 0; i < 10; ++i)
    {
      boost::thread_group pool;
      for (int j = 0; j < 100; ++j)
	{
	  pool.create_thread (someWork);
	}
      pool.join_all ();
    }
}
