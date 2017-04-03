#include <gtest/gtest.h>
#include <memory>

template <typename T> class SharedResource {
public:
	using ResourceType = T;

	template <typename... Args>
	SharedResource(Args&&... args)
			: resource_{new T(std::forward<Args>(args)...)} {
	}

private:
	std::unique_ptr<T> resource_;
};

TEST(SharedResourceTest, Init) {
	auto that = SharedResource<std::string>();
}

TEST(SharedResourceTest, InitWithArgs) {
	auto that = SharedResource<std::string>("ciao");
}
