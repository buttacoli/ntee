#ifndef INCLUDED_BUILDER_HPP
#define INCLUDED_BUILDER_HPP

namespace ntee {

class NTee;
class Settings;

class Builder {
public:
   static NTee* build( const Settings& s );
};

} //end namespace ntee

#endif    
