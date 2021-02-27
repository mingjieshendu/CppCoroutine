//#include <iostream>
//#include <string>
//#include <chrono>
//#include <thread>
//
//using namespace std::chrono;
//
//std::string fetchDataFromDB(std::string recvData) {
//	//确保函数要5秒才能执行完成
//	std::this_thread::sleep_for(seconds(5));
//
//	//处理创建数据库连接、获取数据等事情
//	return "DB_" + recvData;
//}
//
//std::string fetchDataFromFile(std::string recvData) {
//	//确保函数要5秒才能执行完成
//	std::this_thread::sleep_for(seconds(5));
//
//	//处理获取文件数据
//	return "File_" + recvData;
//}
//
//int main() {
//	//获取开始时间
//	system_clock::time_point start = system_clock::now();
//
//	//从数据库获取数据
//	std::string dbData = fetchDataFromDB("Data");
//
//	//从文件获取数据
//	std::string fileData = fetchDataFromFile("Data");
//
//	//获取结束时间
//	auto end = system_clock::now();
//
//	auto diff = duration_cast<std::chrono::seconds>(end - start).count();
//	std::cout << "Total Time taken= " << diff << "Seconds" << std::endl;
//
//	//组装数据
//	std::string data = dbData + " :: " + fileData;
//
//	//输出组装的数据
//	std::cout << "Data = " << data << std::endl;
//
//	system("pause");
//	return 0;
//}

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <future>
#include <string>
#include <mutex>

std::mutex m;
struct X {
	void foo(int i, const std::string& str) {
		std::lock_guard<std::mutex> lk(m);
		std::cout << str << ' ' << i << '\n';
	}
	void bar(const std::string& str) {
		std::lock_guard<std::mutex> lk(m);
		std::cout << str << '\n';
	}
	int operator()(int i) {
		std::lock_guard<std::mutex> lk(m);
		std::cout << i << '\n';
		return i + 10;
	}
};

template <typename RandomIt>int parallel_sum(RandomIt beg, RandomIt end) {
	auto len = end - beg;
	if (len < 1000)
		return std::accumulate(beg, end, 0);

	RandomIt mid = beg + len / 2;
	auto handle = std::async(std::launch::async,
		parallel_sum<RandomIt>, mid, end);
	int sum = parallel_sum(beg, mid);
	return sum + handle.get();
}

int main() {
	std::vector<int> v(10000, 1);
	std::cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';

	X x;
	// 以默认策略调用 x.foo(42, "Hello") ：
	// 可能同时打印 "Hello 42" 或延迟执行
	auto a1 = std::async(&X::foo, &x, 42, "Hello");
	// 以 deferred 策略调用 x.bar("world!")
	// 调用 a2.get() 或 a2.wait() 时打印 "world!"
	auto a2 = std::async(std::launch::deferred, &X::bar, x, "world!");
	// 以 async 策略调用 X()(43) ：
	// 同时打印 "43"
	auto a3 = std::async(std::launch::async, X(), 43);
	a2.wait();                     // 打印 "world!"
	std::cout << a3.get() << '\n'; // 打印 "53"

	system("pause");
} // 若 a1 在此点未完成，则 a1 的析构函数在此打印 "Hello 42"
