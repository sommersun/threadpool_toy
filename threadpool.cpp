#include"threadpool.h"
#include<iostream>
void hello() {
	std::cout << "hello from thread #" << std::this_thread::get_id() << "!\n";
}
int main(int argc,char**argv) {
	ThreadPool pool;
	for (int i = 0; i < 20;i++) {
		pool.commit(hello);
	}
	std::this_thread::sleep_for(std::chrono::seconds(3));
	return 0;
}
