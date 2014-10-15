//
//  main.cpp
//  SqratSample
//
//  Created by kuzawa on 2014/10/14.
//  Copyright (c) 2014年 kuzawa. All rights reserved.
//

#include <iostream>

#include "sqrat.h"
#include "sqratVM.h"

using namespace Sqrat;

static void print(HSQUIRRELVM vm, const SQChar* str, ...)
{
	SQChar buff[512];
	va_list arglist;
	va_start(arglist, str);
	vsnprintf(buff, sizeof(buff), str, arglist);
	va_end(arglist);
	printf("%s", buff);
}

class Human
{
public:
	void hello() { std::cout << "Hello.\n"; }
};

void test1(Human* human)
{
	const char* script =
		"function greeting(human){	"
		"	human.hello();			"
		"}							";

	SqratVM vm1;
	SqratVM vm2;

	Class<Human> humanClass1(vm1.GetVM());
	humanClass1.Func("hello", &Human::hello);

	Class<Human> humanClass2(vm2.GetVM());
	humanClass2.Func("hello", &Human::hello);

//	vm1.GetRootTable().Bind("Human", humanClass1);
//	vm2.GetRootTable().Bind("Human", humanClass2);
	RootTable(vm1.GetVM()).Bind("Human", humanClass1);
	RootTable(vm2.GetVM()).Bind("Human", humanClass2);

	vm1.DoString(script);
	Function f1 = vm1.GetRootTable().GetFunction("greeting");
	assert(!f1.IsNull());

	vm2.DoString(script);
	Function f2 = vm2.GetRootTable().GetFunction("greeting");
	assert(!f2.IsNull());

	// this is OK.
	f1(&human);
	f2(&human);
}

void test2(Human* human)
{
	const char* script =
	"function greeting(human){	"
	"	human.hello();			"
	"	suspend();				"	// just add 1 line.
	"}							";

	SqratVM vm1;
	SqratVM vm2;

	Class<Human> humanClass1(vm1.GetVM());
	humanClass1.Func("hello", &Human::hello);

	Class<Human> humanClass2(vm2.GetVM());
	humanClass2.Func("hello", &Human::hello);

	vm1.GetRootTable().Bind("Human", humanClass1);
	vm2.GetRootTable().Bind("Human", humanClass2);

	vm1.DoString(script);
	Function f1 = vm1.GetRootTable().GetFunction("greeting");

	vm2.DoString(script);
	Function f2 = vm2.GetRootTable().GetFunction("greeting");

	// this is OK.
	f1(&human);
	f2(&human);

	sq_resume(vm1.GetVM(), SQFalse, SQTrue);
	sq_resume(vm2.GetVM(), SQFalse, SQTrue);
}

int main(int argc, const char * argv[])
{
	Human human;

	test1(&human);	// this is OK.

	test2(&human);	// this is fail.

    return 0;
}

