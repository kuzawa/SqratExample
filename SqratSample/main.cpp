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

class Human
{
public:
	void hello() {
		std::cout << "Hello.\n" << std::flush;
	}
};

void test1(Human* human, const char* script)
{
	SqratVM vm1;
	SqratVM vm2;

	Class<Human, NoConstructor<Human>> humanClass1(vm1.GetVM());
	humanClass1.Func("hello", &Human::hello);

	Class<Human, NoConstructor<Human>> humanClass2(vm2.GetVM());
	humanClass2.Func("hello", &Human::hello);

	vm1.GetRootTable().Bind("Human", humanClass1);
	vm2.GetRootTable().Bind("Human", humanClass2);

	vm1.DoString(script);
	Function f1 = vm1.GetRootTable().GetFunction("greeting");
	assert(!f1.IsNull());

	vm2.DoString(script);
	Function f2 = vm2.GetRootTable().GetFunction("greeting");
	assert(!f2.IsNull());

	f1(human);
	if ( Error::Instance().Occurred(vm1.GetVM()) ) {
		std::cout << Error::Instance().Message(vm1.GetVM()) << std::endl << std::flush;
		Error::Instance().Clear(vm1.GetVM());
	}

	f2(human); // <-- faild here if you are trun on line 96.
	if ( Error::Instance().Occurred(vm2.GetVM()) ) {
		std::cout << Error::Instance().Message(vm2.GetVM()) << std::endl << std::flush;
		Error::Instance().Clear(vm2.GetVM());
	}
}

void test2(Human* human)
{
	const char* script2 =
	"function f(h) {				"	// thread inherit roottable from parent vm.
#warning Please on/off this line.
#if 0
	"	h.hello();					"
#else
	"	print(\"print hello\");		"	// <- I cant't access Human instance.
#endif
	"}								"
	"g_human.hello();				"	// <- this is ok.
	"local t = newthread(f);		"
	"t.call(g_human);				"	// <- but fail hello in thread(on friend vm)
	"								";

	SqratVM vm;

	Class<Human, NoConstructor<Human>> humanClass(vm.GetVM());
	humanClass.Func("hello", &Human::hello);
	vm.GetRootTable().Bind("Human", humanClass);
	vm.GetRootTable().SetInstance("g_human", human);

	if ( vm.DoString(script2) != SqratVM::SQRAT_NO_ERROR ) {
		std::cout << vm.GetLastErrorMsg() << std::endl << std::flush;
	}
	std::cout << std::flush;
}

int main(int argc, const char * argv[])
{
	Human human;

	const char* script1 =
	"function greeting(human){	"
	"	human.hello();			"
#warning Please on/off this line.
#if 1
	"	suspend();				"
#endif
	"}							";

	std::cout << "======= test1 =======\n" << std::flush;
	// I can't use one C++ class instance simultaneously.
	test1(&human, script1);
	std::cout << "======= test1 end =======\n" << std::flush;

	// AND
	std::cout << "======= test2 =======\n" << std::flush;
	// I can't use C++ class instance in newthread'ed VM.
	test2(&human);
	std::cout << "======= test2 end=======\n" << std::flush;

    return 0;
}

