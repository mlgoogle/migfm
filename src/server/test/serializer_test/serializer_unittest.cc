// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "basic/basictypes.h"
#include "gtest/gtest.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "log/mig_log.h"
#include <limits>
#include <string>
class ValuesTest: public testing::Test {
};

void ValuesSerializeTest(base_logic::Value* value){
	 std::string json_str;
	 base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(0,&json_str);
	 engine->Serialize(*value);
	 MIG_INFO(USER_LEVEL,"%s\n\n",json_str.c_str());
}

TEST(ValuesSerializerTest, Basic){

	base_logic::Value* boolean_value = base_logic::Value::CreateBooleanValue(true);
	ValuesSerializeTest(boolean_value);

	base_logic::Value* null_value = base_logic::Value::CreateNullValue();
	ValuesSerializeTest(null_value);

	base_logic::Value* integer_value = base_logic::Value::CreateIntegerValue(10);
	ValuesSerializeTest(integer_value);

	base_logic::Value* big_integer_value = base_logic::Value::CreateBigIntegerValue(544294967295);
	ValuesSerializeTest(big_integer_value);

	base_logic::Value* real_value = base_logic::Value::CreateRealValue(1.02);
	ValuesSerializeTest(real_value);

	base_logic::Value* string_value = base_logic::Value::CreateStringValue("老K");
	ValuesSerializeTest(string_value);

	base_logic::Value* wstring_value = base_logic::Value::CreateStringValue(L"董翔");
	ValuesSerializeTest(wstring_value);
}

TEST(ValuesSerializerTest, List){
	base_logic::ListValue* mixed_list = new base_logic::ListValue();
	mixed_list->Set(0, base_logic::Value::CreateNullValue());
	mixed_list->Set(1, base_logic::Value::CreateBooleanValue(true));
	mixed_list->Set(2, base_logic::Value::CreateIntegerValue(42));
	mixed_list->Set(3, base_logic::Value::CreateBigIntegerValue(44294967295));
	mixed_list->Set(4, base_logic::Value::CreateRealValue(88.8));
	mixed_list->Set(5, base_logic::Value::CreateStringValue("kerry"));
	mixed_list->Set(6, base_logic::Value::CreateStringValue(L"flaght"));
	ValuesSerializeTest(mixed_list);
}

TEST(ValuesSerializerTest, Dictionary){
	base_logic::DictionaryValue* settings  = new base_logic::DictionaryValue();
	settings->Set(L"name",base_logic::Value::CreateStringValue("kerry"));
	settings->Set(L"age",base_logic::Value::CreateIntegerValue(42));
	settings->Set(L"sex",base_logic::Value::CreateBooleanValue(true));
	settings->Set(L"billing",base_logic::Value::CreateBigIntegerValue(44294967295));
	settings->Set(L"eye",base_logic::Value::CreateRealValue(5.01));
	settings->Set(L"nickname",base_logic::Value::CreateStringValue(L"dong"));
	settings->Set(L"like",base_logic::Value::CreateNullValue());
	ValuesSerializeTest(settings);
}

TEST(ValuesSerializerTest, Mixed){
	base_logic::DictionaryValue* settings  = new base_logic::DictionaryValue();
	settings->Set(L"name",base_logic::Value::CreateStringValue("kerry"));
	settings->Set(L"age",base_logic::Value::CreateIntegerValue(42));
	settings->Set(L"sex",base_logic::Value::CreateBooleanValue(true));
	settings->Set(L"billing",base_logic::Value::CreateBigIntegerValue(44294967295));
	settings->Set(L"eye",base_logic::Value::CreateRealValue(5.01));
	settings->Set(L"nickname",base_logic::Value::CreateStringValue(L"dong"));
	settings->Set(L"like",base_logic::Value::CreateNullValue());


	base_logic::ListValue* mixed_list = new base_logic::ListValue();
	mixed_list->Set(0, base_logic::Value::CreateNullValue());
	mixed_list->Set(1, base_logic::Value::CreateBooleanValue(true));
	mixed_list->Set(2, base_logic::Value::CreateIntegerValue(42));
	mixed_list->Set(3, base_logic::Value::CreateBigIntegerValue(44294967295));
	mixed_list->Set(4, base_logic::Value::CreateRealValue(88.8));
	mixed_list->Set(5, base_logic::Value::CreateStringValue("kerry"));
	mixed_list->Set(6, base_logic::Value::CreateStringValue(L"flaght"));
	mixed_list->Set(7,settings);
	//mixed_list->Set(8,mixed_list);

	base_logic::DictionaryValue* settings2  = new base_logic::DictionaryValue();
	settings2->Set("name",base_logic::Value::CreateStringValue("kerry2"));
	settings2->Set("age",base_logic::Value::CreateIntegerValue(42));
	settings2->Set("sex",base_logic::Value::CreateBooleanValue(true));
	settings2->Set("billing",base_logic::Value::CreateBigIntegerValue(44294967295));
	settings2->Set("eye",base_logic::Value::CreateRealValue(5.01));
	settings2->Set("nickname",base_logic::Value::CreateStringValue(L"dong2"));
	settings2->Set("like",base_logic::Value::CreateNullValue());

	base_logic::DictionaryValue* settings3  = new base_logic::DictionaryValue();
	settings3->Set("name",base_logic::Value::CreateStringValue("kerry3"));
	settings3->Set("age",base_logic::Value::CreateIntegerValue(42));
	settings3->Set("sex",base_logic::Value::CreateBooleanValue(true));
	settings3->Set("billing",base_logic::Value::CreateBigIntegerValue(44294967295));
	settings3->Set("eye",base_logic::Value::CreateRealValue(5.01));
	settings3->Set("nickname",base_logic::Value::CreateStringValue(L"董翔"));
	settings3->Set("like",base_logic::Value::CreateNullValue());


	base_logic::DictionaryValue* settings1  = new base_logic::DictionaryValue();
	settings1->Set("list",mixed_list);
	settings1->Set("dict2",settings2);
	settings1->Set("dict3",settings3);
	ValuesSerializeTest(settings1);
}

class RootTestValue :public base_logic::DictionaryValue{
public:
	RootTestValue(const std::string& status,const std::string& msg)
		:status_(base_logic::Value::CreateStringValue(status))
		,msg_(base_logic::Value::CreateStringValue(msg))
		,result_(new base_logic::DictionaryValue()){
		this->Set("status",status_);
		this->Set("msg",msg_);
		this->Set("result",result_);
	}

	virtual ~RootTestValue(){}
private:
	base_logic::Value*  status_;
	base_logic::Value*  msg_;
public:
	base_logic::DictionaryValue*  result_;
};

class ExpandTestValue : public RootTestValue{
public:
	ExpandTestValue(const std::string& uid,const std::string& username,
			const std::string& nickname,const std::string& gender,
			const int32 type,const std::string& birthday,const std::string& location,
			const std::string age,const std::string& head,const std::string& token,
			const std::string& new_msg_num,const std::string& status,const std::string& msg)
		:uid_(base_logic::Value::CreateStringValue(uid))
		,username_(base_logic::Value::CreateStringValue(username))
		,nickname_(base_logic::Value::CreateStringValue(nickname))
		,gender_(base_logic::Value::CreateStringValue(gender))
		,type_(base_logic::Value::CreateIntegerValue(type))
		,birthday_(base_logic::Value::CreateStringValue(birthday))
		,location_(base_logic::Value::CreateStringValue(location))
		,age_(base_logic::Value::CreateStringValue(age))
		,head_(base_logic::Value::CreateStringValue(head))
		,token_(base_logic::Value::CreateStringValue(token))
		,new_msg_num_(base_logic::Value::CreateStringValue(new_msg_num))
		,chat_list_(new base_logic::ListValue())
		,engine_(NULL)
		,RootTestValue(status,msg){

	}
	virtual ~ExpandTestValue(){}
public:
	//序列化
	bool Serializer(std::string* str){
		result_->Set("uid",uid_);
		result_->Set("username",username_);
		result_->Set("nickname",nickname_);
		result_->Set("gender",gender_);
		result_->Set("type",type_);
		result_->Set("birthday",birthday_);
		result_->Set("location",location_);
		result_->Set("age",age_);
		result_->Set("head",head_);
		result_->Set("token",token_);
		result_->Set("new_msg_num",new_msg_num_);
		result_->Set("hischat",chat_list_);

		engine_ = base_logic::ValueSerializer::Create(0,str);
		engine_->Serialize(*this);
	}

	void SetChatHisMessage(const int64 fid,const int64 tid,const int64 mid,const std::string& msg,
			const std::string& time){
		base_logic::DictionaryValue * hischat = new base_logic::DictionaryValue();
		hischat->Set("fid",base_logic::Value::CreateBigIntegerValue(fid));
		hischat->Set("tid",base_logic::Value::CreateBigIntegerValue(tid));
		hischat->Set("msg",base_logic::Value::CreateStringValue(msg));
		hischat->Set("mid",base_logic::Value::CreateBigIntegerValue(mid));
		hischat->Set("time",base_logic::Value::CreateStringValue(time));
		chat_list_->Append(hischat);
	}

private:
	base_logic::Value* uid_;
	base_logic::Value* username_;
	base_logic::Value* nickname_;
	base_logic::Value* gender_;
	base_logic::Value* type_;
	base_logic::Value* birthday_;
	base_logic::Value* location_;
	base_logic::Value* age_;
	base_logic::Value* head_;
	base_logic::Value* token_;
	base_logic::Value* new_msg_num_;
	//聊天记录
	base_logic::ListValue* chat_list_;
private:
	base_logic::ValueSerializer* engine_;
};

TEST(ValuesSerializerTest, ExpandTestValue){
	std::string uid = "10108";
	std::string username = "flaght";
	std::string nickname = "老K";
	std::string gender = "1";
	int32 type = 0;
	std::string birthday = "1986-09-03";
	std::string location = "浙江杭州";
	std::string age = "24";
	std::string head = "http://pic.aadad.com/1.jpg";
	std::string token = "DSASDASDASDAS21312312";
	std::string new_msg_num = "1";
	std::string str;
	std::string status = "0";
	std::string msg = "错误未知";
	ExpandTestValue * value = new ExpandTestValue(uid,username,nickname,gender,type,birthday,
			location,age,head,token,new_msg_num,status,msg);

	int64 fid = 10008;
	int64 tid = 11411;
	int64 mgsid = 123123;
	std::string time = "2014-09-03";
	std::string chatmsg = "大家好";
	value->SetChatHisMessage(fid,tid,mgsid,chatmsg,time);
	value->SetChatHisMessage(fid,tid,mgsid,chatmsg,time);
	value->SetChatHisMessage(fid,tid,mgsid,chatmsg,time);
	value->SetChatHisMessage(fid,tid,mgsid,chatmsg,time);
	value->SetChatHisMessage(fid,tid,mgsid,chatmsg,time);
	bool r = value->Serializer(&str);
	MIG_INFO(USER_LEVEL,"%s\n\n",str.c_str());
}
// This is a Value object that allows us to tell if it's been
// properly deleted by modifying the value of external flag on destruction.
/*class DeletionTestValue : public Value {
 public:
  explicit DeletionTestValue(bool* deletion_flag) : Value(TYPE_NULL) {
    Init(deletion_flag);  // Separate function so that we can use ASSERT_*
  }

  void Init(bool* deletion_flag) {
    ASSERT_TRUE(deletion_flag);
    deletion_flag_ = deletion_flag;
    *deletion_flag_ = false;
  }

  ~DeletionTestValue() {
    *deletion_flag_ = true;
  }

 private:
  bool* deletion_flag_;
};

TEST(ValuesTest, ListDeletion) {
  bool deletion_flag = true;

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
  }
  EXPECT_TRUE(deletion_flag);

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    list.Clear();
    EXPECT_TRUE(deletion_flag);
  }

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(list.Set(0, Value::CreateNullValue()));
    EXPECT_TRUE(deletion_flag);
  }
}

TEST(ValuesTest, ListRemoval) {
  bool deletion_flag = true;
  Value* removed_item = NULL;

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_EQ(1U, list.GetSize());
    EXPECT_FALSE(list.Remove(std::numeric_limits<size_t>::max(),
                             &removed_item));
    EXPECT_FALSE(list.Remove(1, &removed_item));
    EXPECT_TRUE(list.Remove(0, &removed_item));
    ASSERT_TRUE(removed_item);
    EXPECT_EQ(0U, list.GetSize());
  }
  EXPECT_FALSE(deletion_flag);
  delete removed_item;
  removed_item = NULL;
  EXPECT_TRUE(deletion_flag);

  {
    ListValue list;
    list.Append(new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(list.Remove(0, NULL));
    EXPECT_TRUE(deletion_flag);
    EXPECT_EQ(0U, list.GetSize());
  }

  {
    ListValue list;
    DeletionTestValue* value = new DeletionTestValue(&deletion_flag);
    list.Append(value);
    EXPECT_FALSE(deletion_flag);
    EXPECT_EQ(0, list.Remove(*value));
    EXPECT_TRUE(deletion_flag);
    EXPECT_EQ(0U, list.GetSize());
  }
}

TEST(ValuesTest, DictionaryDeletion) {
  std::wstring key = L"test";
  bool deletion_flag = true;

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
  }
  EXPECT_TRUE(deletion_flag);

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    dict.Clear();
    EXPECT_TRUE(deletion_flag);
  }

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    dict.Set(key, Value::CreateNullValue());
    EXPECT_TRUE(deletion_flag);
  }
}

TEST(ValuesTest, DictionaryRemoval) {
  std::wstring key = L"test";
  bool deletion_flag = true;
  Value* removed_item = NULL;

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(dict.HasKey(key));
    EXPECT_FALSE(dict.Remove(L"absent key", &removed_item));
    EXPECT_TRUE(dict.Remove(key, &removed_item));
    EXPECT_FALSE(dict.HasKey(key));
    ASSERT_TRUE(removed_item);
  }
  EXPECT_FALSE(deletion_flag);
  delete removed_item;
  removed_item = NULL;
  EXPECT_TRUE(deletion_flag);

  {
    DictionaryValue dict;
    dict.Set(key, new DeletionTestValue(&deletion_flag));
    EXPECT_FALSE(deletion_flag);
    EXPECT_TRUE(dict.HasKey(key));
    EXPECT_TRUE(dict.Remove(key, NULL));
    EXPECT_TRUE(deletion_flag);
    EXPECT_FALSE(dict.HasKey(key));
  }
}

TEST(ValuesTest, DictionaryWithoutPathExpansion) {
  DictionaryValue dict;
  dict.Set(L"this.is.expanded", Value::CreateNullValue());
  dict.SetWithoutPathExpansion(L"this.isnt.expanded", Value::CreateNullValue());

  EXPECT_FALSE(dict.HasKey(L"this.is.expanded"));
  EXPECT_TRUE(dict.HasKey(L"this"));
  Value* value1;
  EXPECT_TRUE(dict.Get(L"this", &value1));
  DictionaryValue* value2;
  ASSERT_TRUE(dict.GetDictionaryWithoutPathExpansion(L"this", &value2));
  EXPECT_EQ(value1, value2);
  EXPECT_EQ(1U, value2->size());

  EXPECT_TRUE(dict.HasKey(L"this.isnt.expanded"));
  Value* value3;
  EXPECT_FALSE(dict.Get(L"this.isnt.expanded", &value3));
  Value* value4;
  ASSERT_TRUE(dict.GetWithoutPathExpansion(L"this.isnt.expanded", &value4));
  EXPECT_EQ(Value::TYPE_NULL, value4->GetType());
}

TEST(ValuesTest, DeepCopy) {
  DictionaryValue original_dict;
  Value* original_null = Value::CreateNullValue();
  original_dict.Set(L"null", original_null);
  Value* original_bool = Value::CreateBooleanValue(true);
  original_dict.Set(L"bool", original_bool);
  Value* original_int = Value::CreateIntegerValue(42);
  original_dict.Set(L"int", original_int);
  Value* original_real = Value::CreateRealValue(3.14);
  original_dict.Set(L"real", original_real);
  Value* original_string = Value::CreateStringValue("hello");
  original_dict.Set(L"string", original_string);
  Value* original_wstring = Value::CreateStringValue(L"peek-a-boo");
  original_dict.Set(L"wstring", original_wstring);
  Value* original_utf16 =
      Value::CreateStringValueFromUTF16(ASCIIToUTF16("hello16"));
  original_dict.Set(L"utf16", original_utf16);

  char* original_buffer = new char[42];
  memset(original_buffer, '!', 42);
  BinaryValue* original_binary = Value::CreateBinaryValue(original_buffer, 42);
  original_dict.Set(L"binary", original_binary);

  ListValue* original_list = new ListValue();
  Value* original_list_element_0 = Value::CreateIntegerValue(0);
  original_list->Append(original_list_element_0);
  Value* original_list_element_1 = Value::CreateIntegerValue(1);
  original_list->Append(original_list_element_1);
  original_dict.Set(L"list", original_list);

  scoped_ptr<DictionaryValue> copy_dict(
      static_cast<DictionaryValue*>(original_dict.DeepCopy()));
  ASSERT_TRUE(copy_dict.get());
  ASSERT_NE(copy_dict.get(), &original_dict);

  Value* copy_null = NULL;
  ASSERT_TRUE(copy_dict->Get(L"null", &copy_null));
  ASSERT_TRUE(copy_null);
  ASSERT_NE(copy_null, original_null);
  ASSERT_TRUE(copy_null->IsType(Value::TYPE_NULL));

  Value* copy_bool = NULL;
  ASSERT_TRUE(copy_dict->Get(L"bool", &copy_bool));
  ASSERT_TRUE(copy_bool);
  ASSERT_NE(copy_bool, original_bool);
  ASSERT_TRUE(copy_bool->IsType(Value::TYPE_BOOLEAN));
  bool copy_bool_value = false;
  ASSERT_TRUE(copy_bool->GetAsBoolean(&copy_bool_value));
  ASSERT_TRUE(copy_bool_value);

  Value* copy_int = NULL;
  ASSERT_TRUE(copy_dict->Get(L"int", &copy_int));
  ASSERT_TRUE(copy_int);
  ASSERT_NE(copy_int, original_int);
  ASSERT_TRUE(copy_int->IsType(Value::TYPE_INTEGER));
  int copy_int_value = 0;
  ASSERT_TRUE(copy_int->GetAsInteger(&copy_int_value));
  ASSERT_EQ(42, copy_int_value);

  Value* copy_real = NULL;
  ASSERT_TRUE(copy_dict->Get(L"real", &copy_real));
  ASSERT_TRUE(copy_real);
  ASSERT_NE(copy_real, original_real);
  ASSERT_TRUE(copy_real->IsType(Value::TYPE_REAL));
  double copy_real_value = 0;
  ASSERT_TRUE(copy_real->GetAsReal(&copy_real_value));
  ASSERT_EQ(3.14, copy_real_value);

  Value* copy_string = NULL;
  ASSERT_TRUE(copy_dict->Get(L"string", &copy_string));
  ASSERT_TRUE(copy_string);
  ASSERT_NE(copy_string, original_string);
  ASSERT_TRUE(copy_string->IsType(Value::TYPE_STRING));
  std::string copy_string_value;
  std::wstring copy_wstring_value;
  string16 copy_utf16_value;
  ASSERT_TRUE(copy_string->GetAsString(&copy_string_value));
  ASSERT_TRUE(copy_string->GetAsString(&copy_wstring_value));
  ASSERT_TRUE(copy_string->GetAsUTF16(&copy_utf16_value));
  ASSERT_EQ(std::string("hello"), copy_string_value);
  ASSERT_EQ(std::wstring(L"hello"), copy_wstring_value);
  ASSERT_EQ(ASCIIToUTF16("hello"), copy_utf16_value);

  Value* copy_wstring = NULL;
  ASSERT_TRUE(copy_dict->Get(L"wstring", &copy_wstring));
  ASSERT_TRUE(copy_wstring);
  ASSERT_NE(copy_wstring, original_wstring);
  ASSERT_TRUE(copy_wstring->IsType(Value::TYPE_STRING));
  ASSERT_TRUE(copy_wstring->GetAsString(&copy_string_value));
  ASSERT_TRUE(copy_wstring->GetAsString(&copy_wstring_value));
  ASSERT_TRUE(copy_wstring->GetAsUTF16(&copy_utf16_value));
  ASSERT_EQ(std::string("peek-a-boo"), copy_string_value);
  ASSERT_EQ(std::wstring(L"peek-a-boo"), copy_wstring_value);
  ASSERT_EQ(ASCIIToUTF16("peek-a-boo"), copy_utf16_value);

  Value* copy_utf16 = NULL;
  ASSERT_TRUE(copy_dict->Get(L"utf16", &copy_utf16));
  ASSERT_TRUE(copy_utf16);
  ASSERT_NE(copy_utf16, original_utf16);
  ASSERT_TRUE(copy_utf16->IsType(Value::TYPE_STRING));
  ASSERT_TRUE(copy_utf16->GetAsString(&copy_string_value));
  ASSERT_TRUE(copy_utf16->GetAsString(&copy_wstring_value));
  ASSERT_TRUE(copy_utf16->GetAsUTF16(&copy_utf16_value));
  ASSERT_EQ(std::string("hello16"), copy_string_value);
  ASSERT_EQ(std::wstring(L"hello16"), copy_wstring_value);
  ASSERT_EQ(ASCIIToUTF16("hello16"), copy_utf16_value);

  Value* copy_binary = NULL;
  ASSERT_TRUE(copy_dict->Get(L"binary", &copy_binary));
  ASSERT_TRUE(copy_binary);
  ASSERT_NE(copy_binary, original_binary);
  ASSERT_TRUE(copy_binary->IsType(Value::TYPE_BINARY));
  ASSERT_NE(original_binary->GetBuffer(),
    static_cast<BinaryValue*>(copy_binary)->GetBuffer());
  ASSERT_EQ(original_binary->GetSize(),
    static_cast<BinaryValue*>(copy_binary)->GetSize());
  ASSERT_EQ(0, memcmp(original_binary->GetBuffer(),
               static_cast<BinaryValue*>(copy_binary)->GetBuffer(),
               original_binary->GetSize()));

  Value* copy_value = NULL;
  ASSERT_TRUE(copy_dict->Get(L"list", &copy_value));
  ASSERT_TRUE(copy_value);
  ASSERT_NE(copy_value, original_list);
  ASSERT_TRUE(copy_value->IsType(Value::TYPE_LIST));
  ListValue* copy_list = static_cast<ListValue*>(copy_value);
  ASSERT_EQ(2U, copy_list->GetSize());

  Value* copy_list_element_0;
  ASSERT_TRUE(copy_list->Get(0, &copy_list_element_0));
  ASSERT_TRUE(copy_list_element_0);
  ASSERT_NE(copy_list_element_0, original_list_element_0);
  int copy_list_element_0_value;
  ASSERT_TRUE(copy_list_element_0->GetAsInteger(&copy_list_element_0_value));
  ASSERT_EQ(0, copy_list_element_0_value);

  Value* copy_list_element_1;
  ASSERT_TRUE(copy_list->Get(1, &copy_list_element_1));
  ASSERT_TRUE(copy_list_element_1);
  ASSERT_NE(copy_list_element_1, original_list_element_1);
  int copy_list_element_1_value;
  ASSERT_TRUE(copy_list_element_1->GetAsInteger(&copy_list_element_1_value));
  ASSERT_EQ(1, copy_list_element_1_value);
}

TEST(ValuesTest, Equals) {
  Value* null1 = Value::CreateNullValue();
  Value* null2 = Value::CreateNullValue();
  EXPECT_NE(null1, null2);
  EXPECT_TRUE(null1->Equals(null2));

  Value* boolean = Value::CreateBooleanValue(false);
  EXPECT_FALSE(null1->Equals(boolean));
  delete null1;
  delete null2;
  delete boolean;

  DictionaryValue dv;
  dv.SetBoolean(L"a", false);
  dv.SetInteger(L"b", 2);
  dv.SetReal(L"c", 2.5);
  dv.SetString(L"d1", "string");
  dv.SetString(L"d2", L"string");
  dv.Set(L"e", Value::CreateNullValue());

  DictionaryValue* copy = static_cast<DictionaryValue*>(dv.DeepCopy());
  EXPECT_TRUE(dv.Equals(copy));

  ListValue* list = new ListValue;
  list->Append(Value::CreateNullValue());
  list->Append(new DictionaryValue);
  dv.Set(L"f", list);

  EXPECT_FALSE(dv.Equals(copy));
  copy->Set(L"f", list->DeepCopy());
  EXPECT_TRUE(dv.Equals(copy));

  list->Append(Value::CreateBooleanValue(true));
  EXPECT_FALSE(dv.Equals(copy));
  delete copy;
}

TEST(ValuesTest, RemoveEmptyChildren) {
  scoped_ptr<DictionaryValue> root(new DictionaryValue);
  // Remove empty lists and dictionaries.
  root->Set(L"empty_dict", new DictionaryValue);
  root->Set(L"empty_list", new ListValue);
  root->SetWithoutPathExpansion(L"a.b.c.d.e", new DictionaryValue);
  root.reset(root->DeepCopyWithoutEmptyChildren());
  EXPECT_TRUE(root->empty());

  // Make sure we don't prune too much.
  root->SetBoolean(L"bool", true);
  root->Set(L"empty_dict", new DictionaryValue);
  root->SetString(L"empty_string", "");
  root.reset(root->DeepCopyWithoutEmptyChildren());
  EXPECT_EQ(2U, root->size());

  // Should do nothing.
  root.reset(root->DeepCopyWithoutEmptyChildren());
  EXPECT_EQ(2U, root->size());

  // Nested test cases.  These should all reduce back to the bool and string
  // set above.
  {
    root->Set(L"a.b.c.d.e", new DictionaryValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }
  {
    DictionaryValue* inner = new DictionaryValue;
    root->Set(L"dict_with_emtpy_children", inner);
    inner->Set(L"empty_dict", new DictionaryValue);
    inner->Set(L"empty_list", new ListValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }
  {
    ListValue* inner = new ListValue;
    root->Set(L"list_with_empty_children", inner);
    inner->Append(new DictionaryValue);
    inner->Append(new ListValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }

  // Nested with siblings.
  {
    ListValue* inner = new ListValue;
    root->Set(L"list_with_empty_children", inner);
    inner->Append(new DictionaryValue);
    inner->Append(new ListValue);
    DictionaryValue* inner2 = new DictionaryValue;
    root->Set(L"dict_with_empty_children", inner2);
    inner2->Set(L"empty_dict", new DictionaryValue);
    inner2->Set(L"empty_list", new ListValue);
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(2U, root->size());
  }

  // Make sure nested values don't get pruned.
  {
    ListValue* inner = new ListValue;
    root->Set(L"list_with_empty_children", inner);
    ListValue* inner2 = new ListValue;
    inner->Append(new DictionaryValue);
    inner->Append(inner2);
    inner2->Append(Value::CreateStringValue("hello"));
    root.reset(root->DeepCopyWithoutEmptyChildren());
    EXPECT_EQ(3U, root->size());
    EXPECT_TRUE(root->GetList(L"list_with_empty_children", &inner));
    EXPECT_EQ(1U, inner->GetSize());  // Dictionary was pruned.
    EXPECT_TRUE(inner->GetList(0, &inner2));
    EXPECT_EQ(1U, inner2->GetSize());
  }
}

TEST(ValuesTest, MergeDictionary) {
  scoped_ptr<DictionaryValue> base(new DictionaryValue);
  base->SetString(L"base_key", "base_key_value_base");
  base->SetString(L"collide_key", "collide_key_value_base");
  DictionaryValue* base_sub_dict = new DictionaryValue;
  base_sub_dict->SetString(L"sub_base_key", "sub_base_key_value_base");
  base_sub_dict->SetString(L"sub_collide_key", "sub_collide_key_value_base");
  base->Set(L"sub_dict_key", base_sub_dict);

  scoped_ptr<DictionaryValue> merge(new DictionaryValue);
  merge->SetString(L"merge_key", "merge_key_value_merge");
  merge->SetString(L"collide_key", "collide_key_value_merge");
  DictionaryValue* merge_sub_dict = new DictionaryValue;
  merge_sub_dict->SetString(L"sub_merge_key", "sub_merge_key_value_merge");
  merge_sub_dict->SetString(L"sub_collide_key", "sub_collide_key_value_merge");
  merge->Set(L"sub_dict_key", merge_sub_dict);

  base->MergeDictionary(merge.get());

  EXPECT_EQ(4U, base->size());
  std::string base_key_value;
  EXPECT_TRUE(base->GetString(L"base_key", &base_key_value));
  EXPECT_EQ("base_key_value_base", base_key_value); // Base value preserved.
  std::string collide_key_value;
  EXPECT_TRUE(base->GetString(L"collide_key", &collide_key_value));
  EXPECT_EQ("collide_key_value_merge", collide_key_value); // Replaced.
  std::string merge_key_value;
  EXPECT_TRUE(base->GetString(L"merge_key", &merge_key_value));
  EXPECT_EQ("merge_key_value_merge", merge_key_value); // Merged in.

  DictionaryValue* res_sub_dict;
  EXPECT_TRUE(base->GetDictionary(L"sub_dict_key", &res_sub_dict));
  EXPECT_EQ(3U, res_sub_dict->size());
  std::string sub_base_key_value;
  EXPECT_TRUE(res_sub_dict->GetString(L"sub_base_key", &sub_base_key_value));
  EXPECT_EQ("sub_base_key_value_base", sub_base_key_value); // Preserved.
  std::string sub_collide_key_value;
  EXPECT_TRUE(res_sub_dict->GetString(L"sub_collide_key",
                                      &sub_collide_key_value));
  EXPECT_EQ("sub_collide_key_value_merge", sub_collide_key_value); // Replaced.
  std::string sub_merge_key_value;
  EXPECT_TRUE(res_sub_dict->GetString(L"sub_merge_key", &sub_merge_key_value));
  EXPECT_EQ("sub_merge_key_value_merge", sub_merge_key_value); // Merged in.
}
*/
