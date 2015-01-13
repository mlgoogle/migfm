// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>
#include "basic/basictypes.h"
#include "gtest/gtest.h"
#include "../../base/logic/base_values.h"
#include "../../public/basic/scoped_ptr.h"
#include "log/mig_log.h"
#include<typeinfo>
class ValuesTest: public testing::Test {
};


TEST(ValuesTest, Basic){
	bool bool_value = false;
	int int_value = 0;
	int64 bigint_value = 0;
	double double_value = 0.0;
	std::string string_value;
	std::wstring wstring_value;
	MIG_INFO(USER_LEVEL,"bool_value:%s\n\n",typeid(bool_value).name());
	MIG_INFO(USER_LEVEL,"int_value:%s\n\n",typeid(int_value).name());
	MIG_INFO(USER_LEVEL,"bigint_value:%s\n\n",typeid(bigint_value).name());
	MIG_INFO(USER_LEVEL,"double_value:%s\n\n",typeid(double_value).name());
	MIG_INFO(USER_LEVEL,"string_value:%s\n\n",typeid(string_value).name());
	MIG_INFO(USER_LEVEL,"wstring_value:%s\n\n",typeid(wstring_value).name());
}

