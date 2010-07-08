#ifndef INCLUDED_NTEE_HPP
#define INCLUDED_NTEE_HPP

#include <list>
#include <boost/shared_ptr.hpp>

namespace ntee {

//! An interface class which abstracts the behavior of recording data.
class Recorder {
public:
   virtual ~Recorder() {}
   virtual void record() = 0;
};



class NTee {
public:
   virtual ~NTee();
   virtual int start();
   
   void addRecorder( const boost::shared_ptr<Recorder>& );

private:
   std::list<boost::shared_ptr<Recorder> > recorders_;
};

} // end namespace ntee


#endif
