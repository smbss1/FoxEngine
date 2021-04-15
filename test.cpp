
#include <iostream>
#include <string>
#include <chrono>
#include <future>
#include <thread>
#include "FoxEngine.hpp"

struct Transform
{
	int x;
};

struct Other
{
};

struct A
{
};

struct B
{
};

void test(int data)
{
	std::cout << "I received: " << data << std::endl;
}

int main(int argc, char**args)
{
	World oWorld;

	oWorld.system<Transform>().kind(fox::ecs::OnAdd)
		.each([](Entity e, Transform& t) {
			std::cout << "Add Transform" <<std::endl;
	});
	oWorld.system<Transform>().kind(fox::ecs::OnSet)
		.each([](Entity e, Transform& t) {
			std::cout << "Set Transform" <<std::endl;
	});
	oWorld.system<Transform>().kind(fox::ecs::OnRemove)
		.each([](Entity e, Transform& t) {
			std::cout << "Remove Transform" <<std::endl;
	});
	oWorld.system<B>().kind(fox::ecs::OnAdd)
		.each([](Entity e, B& t) {
			std::cout << "Add B" <<std::endl;
	});
	oWorld.system<B, Transform>().kind(fox::ecs::OnAdd)
		.each([](Entity e, B& t, Transform& te) {
			std::cout << "Add B & Transform" <<std::endl;
	});

	oWorld.system<Transform>().kind(fox::ecs::PostUpdate)
		.each([](Entity e, Transform& t) {
			std::cout << "----- PostUpdate ------" <<std::endl;
	});
	oWorld.system<Transform>()
		.each([](Entity e, Transform& t) {
			std::cout << "----- Update ------" <<std::endl;
	});
	oWorld.system<Other>()
		.each([](Entity e, Other& t) {
			std::cout << "----- Update Other ------" <<std::endl;
	});
	oWorld.system<Transform>().kind(fox::ecs::PreUpdate)
		.each([](Entity e, Transform& t) {
			std::cout << "----- PreUpdate ------" <<std::endl;
	});
	oWorld.system<Other>().kind(fox::ecs::PreUpdate)
		.each([](Entity e, Other& t) {
			std::cout << "----- PreUpdate Other ------" <<std::endl;
	});
	{
		auto e1 = oWorld.new_entity();
		e1.add<Other>()
		  .add<Transform>();
		auto opt = e1.get<Transform>();
		if (opt)
			std::cout << "Transform.x: " << opt->x << std::endl;
		e1.remove<Transform>();
		auto opt2 = e1.get<Transform>();
		if (!opt2)
			std::cout << "Transform Component remove in Entity: " << e1.get_id() << std::endl;
	}
	{
		auto e2 = oWorld.new_entity();
		e2.add<Other>();
		auto op4t = e2.get<Transform>();
		if (op4t)
			std::cout << op4t->x << std::endl;
	}
	{
		auto e3 = oWorld.new_entity();
		e3.add<A>();
		e3.add<B>();
		e3.add<Transform>();
		e3.set<Transform>({});
	}
	oWorld.progress();

	fox::EventNotifier<void(int)> event;
	auto idx = event.subscribe(test);
	event.notifiy(45);
	event.unsubscribe(idx);
	event.notifiy(87);
	event.subscribe(test);
	event.notifiy(87);



	fox::LockFreeQueue<int> queue;

	// only one thread can push and pop items from one end
	std::thread owner([&] () {
		for(int i=0; i < 1000000; ++i) {
			queue.push(i);
			// printf("Push: %d\n", i);
		}
		while(!queue.empty()) {
			fox::Option<int> item = queue.pop();
			// printf("Pop: %d\n", item.value());
			// std::cout << "Pop: " << item.value() << "\n";
		}
	});

	// multiple threads can steal items from the other end
	std::thread thief([&] () {
		while(!queue.empty()) {
			fox::Option<int> item = queue.steal();
			// printf("Steal: %d\n", item.value());
		}
	});

	owner.join();
	thief.join();

	fox::LockFreeQueue<int> q(2);
	auto t = std::thread([&] {
		while (q.empty());
		std::cout << q.pop().value_or(-1) << std::endl;
		while (q.empty());
		std::cout << q.pop().value_or(-1) << std::endl;
	});
	q.push(5);
	q.push(15);
	t.join();
    return 0;
}
