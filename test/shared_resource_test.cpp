#include <boost/thread/shared_mutex.hpp>
#include <gtest/gtest.h>
#include <memory>

template <typename _ResourceType, bool _ABORT_ON_TIMEOUT> class SharedResource {
public:
	using ResourceType = _ResourceType;
	static constexpr int ABORT_ON_TIMEOUT = _ABORT_ON_TIMEOUT;

	class ExclusiveLock {
	public:
		ExclusiveLock(boost::shared_mutex* mutex)
				: mutex_{mutex} {
			if (ABORT_ON_TIMEOUT) {
				bool ok = mutex_->try_lock_for(boost::chrono::seconds{1});
				if (!ok) {
					::abort();
				}
			} else {
				mutex_->lock();
			}
		}
		~ExclusiveLock() {
			mutex_->unlock();
		}

	private:
		boost::shared_mutex* mutex_;
	};

	template <typename... Args>
	SharedResource(Args&&... args)
			: resource_{new ResourceType(std::forward<Args>(args)...)} {
	}

	ExclusiveLock exclusiveLock() {
		return ExclusiveLock{&mutex_};
	}

	SharedResource() = default;
	SharedResource& operator=(SharedResource&&) = default;

private:
	std::unique_ptr<ResourceType> resource_;
	boost::shared_mutex mutex_;
};

TEST(SharedResourceTest, InitAsInt) {
	SharedResource<int, false> that;
}

TEST(SharedResourceTest, InitAsString) {
	SharedResource<std::string, false> that;
}

TEST(SharedResourceTest, InitWithArgs) {
	SharedResource<std::string, false> that("ciao");
}

TEST(SharedResourceTest, ExclusiveLockAbortIfCannotGetLock) {
	SharedResource<std::string, true> that("ciao");
	auto res = that.exclusiveLock();
	ASSERT_DEATH(that.exclusiveLock(), ".*");
}

TEST(SharedResourceTest, ExclusiveLockReleaseWhenOutOfScope) {
	SharedResource<std::string, true> that("ciao");
	{ auto res = that.exclusiveLock(); }
	{ auto res = that.exclusiveLock(); }
}
