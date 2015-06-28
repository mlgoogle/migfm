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
	 std::string xml_str;
	 base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(1,&xml_str);
	 engine->Serialize(*value);
	 MIG_INFO(USER_LEVEL,"%s\n\n",xml_str.c_str());
}

TEST(ValuesSerializerTest, Dictionary){


	base_logic::DictionaryValue* strategy2 = new base_logic::DictionaryValue();
	base_logic::DictionaryValue* advert = new base_logic::DictionaryValue();
	advert->Set(L"-name",base_logic::Value::CreateStringValue("广告位"));
	advert->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	strategy2->Set(L"root_node",advert);

	base_logic::DictionaryValue* strategy1 = new base_logic::DictionaryValue();
	base_logic::DictionaryValue* recomm = new base_logic::DictionaryValue();
	recomm->Set(L"-name",base_logic::Value::CreateStringValue("市场推荐"));
	recomm->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	strategy1->Set(L"root_node",recomm);


	base_logic::DictionaryValue* strategy3 = new base_logic::DictionaryValue();
	base_logic::DictionaryValue* rank = new base_logic::DictionaryValue();
	rank->Set(L"-name",base_logic::Value::CreateStringValue("排行榜"));
	rank->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	strategy3->Set(L"root_node",rank);




	base_logic::DictionaryValue* strategy4 = new base_logic::DictionaryValue();
	base_logic::DictionaryValue* policy_mm_new = new base_logic::DictionaryValue();
	policy_mm_new->Set(L"-name",base_logic::Value::CreateStringValue("最新排名"));
	policy_mm_new->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	policy_mm_new->Set(L"#text",base_logic::Value::CreateBigIntegerValue(1004));
	strategy4->Set(L"policy",policy_mm_new);


	base_logic::DictionaryValue* strategy5 = new base_logic::DictionaryValue();
	base_logic::DictionaryValue* policy_mm_total = new base_logic::DictionaryValue();
	policy_mm_total->Set(L"-name",base_logic::Value::CreateStringValue("总排名"));
	policy_mm_total->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	policy_mm_total->Set(L"#text",base_logic::Value::CreateBigIntegerValue(1003));
	strategy5->Set(L"policy",policy_mm_total);



	base_logic::ListValue* application = new base_logic::ListValue();
	application->Append(strategy4);
	application->Append(strategy5);

	base_logic::DictionaryValue* strategy6 = new base_logic::DictionaryValue();
	strategy6->Set(L"-name",base_logic::Value::CreateStringValue("MM"));
	strategy6->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	strategy6->Set(L"application",application);


	/*strategy->Append(strategy2);
	strategy->Append(strategy1);
	strategy->Append(strategy3);
	strategy->Append(strategy4);
	strategy->Append(strategy5);*/

	ValuesSerializeTest(strategy6);

}
/*
TEST(ValuesSerializerTest, Dictionary){

	base_logic::DictionaryValue* settings  = new base_logic::DictionaryValue();

	base_logic::DictionaryValue* age = new base_logic::DictionaryValue();
	age->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	age->Set(L"-name",base_logic::Value::CreateIntegerValue(1));
	age->Set(L"#text",base_logic::Value::CreateIntegerValue(15));
	settings->Set(L"age",age);

	base_logic::DictionaryValue* name = new base_logic::DictionaryValue();
	name->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	name->Set(L"-nametest",base_logic::Value::CreateIntegerValue(1));
	name->Set(L"#text",base_logic::Value::CreateStringValue("kerry"));
	settings->Set(L"name",name);

	base_logic::DictionaryValue* sex = new base_logic::DictionaryValue();
	sex->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(2));
	sex->Set(L"-sextest",base_logic::Value::CreateIntegerValue(3));
	settings->Set(L"sex",sex);
	//sex->Set(L"age",age);

	//sex->Set(L"#text",base_logic::Value::CreateStringValue("boy"));
	//settings->Set(L"sex",sex);

	base_logic::DictionaryValue* billing = new base_logic::DictionaryValue();
	billing->Set(L"#xmltype",base_logic::Value::CreateIntegerValue(1));
	//billing->Set(L"#text",base_logic::Value::CreateIntegerValue(4));
	//billing->Set(L"sex",sex);
	//billing->Set(L"name",name);
	settings->Set(L"billing",billing);

	ValuesSerializeTest(settings);
}*/



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
