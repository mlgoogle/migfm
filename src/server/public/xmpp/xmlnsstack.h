#ifndef _xmlnsstack_h_
#define _xmlnsstack_h_

#include <string>
#include "basic/scoped_ptr.h"
#include "basic/stl_decl.h"
#include "qname.h"

namespace base {

class XmlnsStack {
public:
  XmlnsStack();
  ~XmlnsStack();

  void AddXmlns(const std::string & prefix, const std::string & ns);
  void RemoveXmlns();
  void PushFrame();
  void PopFrame();
  void Reset();

  const std::string * NsForPrefix(const std::string & prefix);
  bool PrefixMatchesNs(const std::string & prefix, const std::string & ns);
  std::pair<std::string, bool> PrefixForNs(const std::string & ns, bool isAttr);
  std::pair<std::string, bool> AddNewPrefix(const std::string & ns, bool isAttr);
  std::string FormatQName(const QName & name, bool isAttr);

private:

  scoped_ptr<std::vector<std::string, std::allocator<std::string> > > pxmlnsStack_;
  scoped_ptr<std::vector<size_t, std::allocator<size_t> > > pxmlnsDepthStack_;
};
}

#endif
