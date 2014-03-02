#include "basic/stl_decl.h"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "xmlelement.h"
#include "xmlprinter.h"
#include "xmlnsstack.h"
#include "xmlconstants.h"

namespace base {

class XmlPrinterImpl {
public:
  XmlPrinterImpl(std::ostream * pout,
    const std::string * const xmlns, int xmlnsCount);
  void PrintElement(const XmlElement * element);
  void PrintQuotedValue(const std::string & text);
  void PrintBodyText(const std::string & text);

private:
  std::ostream *pout_;
  XmlnsStack xmlnsStack_;
};

void
XmlPrinter::PrintXml(std::ostream * pout, const XmlElement * element) {
  PrintXml(pout, element, NULL, 0);
}

void
XmlPrinter::PrintXml(std::ostream * pout, const XmlElement * element,
    const std::string * const xmlns, int xmlnsCount) {
  XmlPrinterImpl printer(pout, xmlns, xmlnsCount);
  printer.PrintElement(element);
}

XmlPrinterImpl::XmlPrinterImpl(std::ostream * pout,
    const std::string * const xmlns, int xmlnsCount) :
  pout_(pout),
  xmlnsStack_() {
  int i;
  for (i = 0; i < xmlnsCount; i += 2) {
    xmlnsStack_.AddXmlns(xmlns[i], xmlns[i + 1]);
  }
}

void
XmlPrinterImpl::PrintElement(const XmlElement * element) {
  xmlnsStack_.PushFrame();

  // first go through attrs of pel to add xmlns definitions
  const XmlAttr * pattr;
  for (pattr = element->FirstAttr(); pattr; pattr = pattr->NextAttr()) {
    if (pattr->Name() == QN_XMLNS)
      xmlnsStack_.AddXmlns(STR_EMPTY, pattr->Value());
    else if (pattr->Name().Namespace() == NS_XMLNS)
      xmlnsStack_.AddXmlns(pattr->Name().LocalPart(),
        pattr->Value());
  }

  // then go through qnames to make sure needed xmlns definitons are added
  std::vector<std::string> newXmlns;
  std::pair<std::string, bool> prefix;
  prefix = xmlnsStack_.AddNewPrefix(element->Name().Namespace(), false);
  if (prefix.second) {
    newXmlns.push_back(prefix.first);
    newXmlns.push_back(element->Name().Namespace());
  }

  for (pattr = element->FirstAttr(); pattr; pattr = pattr->NextAttr()) {
    prefix = xmlnsStack_.AddNewPrefix(pattr->Name().Namespace(), true);
    if (prefix.second) {
      newXmlns.push_back(prefix.first);
      newXmlns.push_back(pattr->Name().Namespace());
    }
  }

  // print the element name
  *pout_ << '<' << xmlnsStack_.FormatQName(element->Name(), false);

  // and the attributes
  for (pattr = element->FirstAttr(); pattr; pattr = pattr->NextAttr()) {
    *pout_ << ' ' << xmlnsStack_.FormatQName(pattr->Name(), true) << "=\"";
    PrintQuotedValue(pattr->Value());
    *pout_ << '"';
  }

  // and the extra xmlns declarations
  std::vector<std::string>::iterator i(newXmlns.begin());
  while (i < newXmlns.end()) {
    if (*i == STR_EMPTY)
      *pout_ << " xmlns=\"" << *(i + 1) << '"';
    else
      *pout_ << " xmlns:" << *i << "=\"" << *(i + 1) << '"';
    i += 2;
  }

  // now the children
  const XmlChild * pchild = element->FirstChild();

  if (pchild == NULL)
    *pout_ << "/>";
  else {
    *pout_ << '>';
    while (pchild) {
      if (pchild->IsText())
        PrintBodyText(pchild->AsText()->Text());
      else
        PrintElement(pchild->AsElement());
      pchild = pchild->NextChild();
    }
    *pout_ << "</" << xmlnsStack_.FormatQName(element->Name(), false) << '>';
  }

  xmlnsStack_.PopFrame();
}

void
XmlPrinterImpl::PrintQuotedValue(const std::string & text) {
  size_t safe = 0;
  for (;;) {
    size_t unsafe = text.find_first_of("<>&\"", safe);
    if (unsafe == std::string::npos)
      unsafe = text.length();
    *pout_ << text.substr(safe, unsafe - safe);
    if (unsafe == text.length())
      return;
    switch (text[unsafe]) {
      case '<': *pout_ << "&lt;"; break;
      case '>': *pout_ << "&gt;"; break;
      case '&': *pout_ << "&amp;"; break;
      case '"': *pout_ << "&quot;"; break;
    }
    safe = unsafe + 1;
    if (safe == text.length())
      return;
  }
}

void
XmlPrinterImpl::PrintBodyText(const std::string & text) {
  size_t safe = 0;
  for (;;) {
    size_t unsafe = text.find_first_of("<>&", safe);
    if (unsafe == std::string::npos)
      unsafe = text.length();
    *pout_ << text.substr(safe, unsafe - safe);
    if (unsafe == text.length())
      return;
    switch (text[unsafe]) {
      case '<': *pout_ << "&lt;"; break;
      case '>': *pout_ << "&gt;"; break;
      case '&': *pout_ << "&amp;"; break;
    }
    safe = unsafe + 1;
    if (safe == text.length())
      return;
  }
}


}
