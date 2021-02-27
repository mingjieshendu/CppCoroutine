//#include <iostream>
//#include <string>
//#include <chrono>
//#include <thread>
//
//using namespace std::chrono;
//
//std::string fetchDataFromDB(std::string recvData) {
//	//ȷ������Ҫ5�����ִ�����
//	std::this_thread::sleep_for(seconds(5));
//
//	//���������ݿ����ӡ���ȡ���ݵ�����
//	return "DB_" + recvData;
//}
//
//std::string fetchDataFromFile(std::string recvData) {
//	//ȷ������Ҫ5�����ִ�����
//	std::this_thread::sleep_for(seconds(5));
//
//	//�����ȡ�ļ�����
//	return "File_" + recvData;
//}
//
//int main() {
//	//��ȡ��ʼʱ��
//	system_clock::time_point start = system_clock::now();
//
//	//�����ݿ��ȡ����
//	std::string dbData = fetchDataFromDB("Data");
//
//	//���ļ���ȡ����
//	std::string fileData = fetchDataFromFile("Data");
//
//	//��ȡ����ʱ��
//	auto end = system_clock::now();
//
//	auto diff = duration_cast<std::chrono::seconds>(end - start).count();
//	std::cout << "Total Time taken= " << diff << "Seconds" << std::endl;
//
//	//��װ����
//	std::string data = dbData + " :: " + fileData;
//
//	//�����װ������
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
	// ��Ĭ�ϲ��Ե��� x.foo(42, "Hello") ��
	// ����ͬʱ��ӡ "Hello 42" ���ӳ�ִ��
	auto a1 = std::async(&X::foo, &x, 42, "Hello");
	// �� deferred ���Ե��� x.bar("world!")
	// ���� a2.get() �� a2.wait() ʱ��ӡ "world!"
	auto a2 = std::async(std::launch::deferred, &X::bar, x, "world!");
	// �� async ���Ե��� X()(43) ��
	// ͬʱ��ӡ "43"
	auto a3 = std::async(std::launch::async, X(), 43);
	a2.wait();                     // ��ӡ "world!"
	std::cout << a3.get() << '\n'; // ��ӡ "53"

	system("pause");
} // �� a1 �ڴ˵�δ��ɣ��� a1 �����������ڴ˴�ӡ "Hello 42"
