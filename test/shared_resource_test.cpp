#include <boost/thread/shared_mutex.hpp>
#include <gtest/gtest.h>
#include <memory>

template <typename T> class SharedResource {
public:
	using ResourceType = T;

	class ExclusiveLock {
		ExclusiveLock(boost::shared_mutex* mutex)
				: mutex_{mutex} {
			mutex_->lock();
		}
		~ExclusiveLock() {
			mutex_->unlock();
		}
		boost::shared_mutex* mutex_;
	};

	template <typename... Args>
	SharedResource(Args&&... args)
			: resource_{new T(std::forward<Args>(args)...)} {
	}

	ExclusiveLock exclusiveLock() {
		return ExclusiveLock{&mutex_};
	}

	SharedResource() = default;
	SharedResource& operator=(SharedResource&&) = default;

private:
	std::unique_ptr<T> resource_;
	boost::shared_mutex mutex_;
};

TEST(SharedResourceTest, InitAsInt) {
	SharedResource<int> that;
}

TEST(SharedResourceTest, InitAsString) {
	SharedResource<std::string> that;
}

TEST(SharedResourceTest, InitWithArgs) {
	SharedResource<std::string> that("ciao");
}
