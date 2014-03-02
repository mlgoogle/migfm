#include "xmlconstants.h"

using namespace base;

const std::string & XmlConstants::str_empty() {
  static const std::string str_empty_;
  return str_empty_;
}

const std::string & XmlConstants::ns_xml() {
  static const std::string ns_xml_("http://www.w3.org/XML/1998/namespace");
  return ns_xml_;
}

const std::string & XmlConstants::ns_xmlns() {
  static const std::string ns_xmlns_("http://www.w3.org/2000/xmlns/");
  return ns_xmlns_;
}

const std::string & XmlConstants::str_xmlns() {
  static const std::string str_xmlns_("xmlns");
  return str_xmlns_;
}

const std::string & XmlConstants::str_xml() {
  static const std::string str_xml_("xml");
  return str_xml_;
}

const std::string & XmlConstants::str_version() {
  static const std::string str_version_("version");
  return str_version_;
}

const std::string & XmlConstants::str_encoding() {
  static const std::string str_encoding_("encoding");
  return str_encoding_;
}
