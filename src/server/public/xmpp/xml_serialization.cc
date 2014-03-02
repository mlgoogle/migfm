#include "xml_serialization.h"
#include <sstream>
#include "log/mig_log.h"

namespace base {

bool XmlSerialization::XmlUserInfoSerialization(std::string& xml_result,const int32 usr_id,
	                   const std::string& username,const int32& sex,const std::string& extadd,
                           const std::string& street,const std::string& locality,
                           const std::string& regin,const int32& pcode,std::string& ctry,
                           const std::string& head,const std::string& birthday,
                           const std::string& nickname){

 /*<iq id='v1' to = 'flaght@gmail.com' type='result'>
    <vCard xmlns="¡¯vcard-temp¡¯">
     <id>10001</id>
     <sex>1</sex>
     <address>
        <extadd>suite 600</extadd>
        <street>1899 west lake street</street>
        <locality>hangzhou</locality>
        <region>west lake</region>
        <pcode>310000</pcode>
        <ctry>China</ctry>
     </address>
     <head>http://fm.miglab.com/10001.jpg</head>
     <birthday>1986-09-03</birthday>
     <nickname>kerry</nickname>
    </vCard>
</iq>*/
    MIG_INFO(USER_LEVEL,"usr_id[%d] sex[%d] extadd[%d] street[%s]",usr_id,sex,extadd.c_str());
    std::stringstream output;
    output<<"<iq id='v1' to='"<<username<<"' type='result'>"
          <<"<vCard xmlns='vcard-temp'>";
    
    output<<"<id>"<<usr_id<<"</id>";
    output<<"<sex>"<<sex<<"</sex>";

    //addres
    output<<"<address>";
    if(extadd.empty())
        output<<"<extadd/>";
    else
        output<<"<extadd>"<<extadd<<"</extadd>";
    
    
    if(street.empty())
        output<<"<street/>";
    else
        output<<"<street>"<<street<<"</street>";

    if(locality.empty())
        output<<"<locality/>";
    else
        output<<"<locality>"<<locality<<"</locality>";

    if(regin.empty())
        output<<"<regin/>";
    else
        output<<"<regin>"<<regin<<"</regin>";

    output<<"<pcode>"<<pcode<<"</pcode>";

    if(ctry.empty())
        output<<"<ctry/>";
    else
        output<<"<ctry>"<<ctry<<"</ctry>";

    output<<"</address>";
    //address end
    
    if(head.empty())
        output<<"<head/>";
    else
        output<<"<head>"<<head<<"</head>";

    
    if(birthday.empty())
        output<<"<birthday/>";
    else
        output<<"<birthday>"<<birthday<<"</birthday>";

    if(nickname.empty())
        output<<"<nickname/>";
    else
        output<<"<nickname>"<<nickname<<"</nickname>";
    output<<"</vCard></iq>";
    xml_result = output.str();
    return true;
}

}
