#include <boost/thread/shared_mutex.hpp>
#include <gtest/gtest.h>
#include <memory>

template <typename _ResourceType, bool _ABORT_ON_TIMEOUT> class SharedResource {
public:
	using ResourceType = _ResourceType;
	static constexpr int ABORT_ON_TIMEOUT = _ABORT_ON_TIMEOUT;

	class ExclusiveLock {
	public:
		ExclusiveLock(SharedResource<_ResourceType, _ABORT_ON_TIMEOUT>* parent)
				: parent_{parent} {
			if (ABORT_ON_TIMEOUT) {
				bool ok = parent_->mutex_.try_lock_for(boost::chrono::milliseconds{100});
				if (!ok) {
					::abort();
				}
			} else {
				parent_->mutex_.lock();
			}
		}
		~ExclusiveLock() {
			parent_->mutex_.unlock();
		}

	private:
		SharedResource<_ResourceType, _ABORT_ON_TIMEOUT>* parent_;
	};

	class SharedLock {
	public:
		SharedLock(SharedResource<_ResourceType, _ABORT_ON_TIMEOUT>* parent)
				: parent_{parent} {
			if (ABORT_ON_TIMEOUT) {
				bool ok = parent_->mutex_.try_lock_shared_for(boost::chrono::seconds{1});
				if (!ok) {
					::abort();
				}
			} else {
				parent_->mutex_.lock_shared();
			}
		}
		~SharedLock() {
			parent_->mutex_.unlock_shared();
		}

	private:
		SharedResource<_ResourceType, _ABORT_ON_TIMEOUT>* parent_;
	};

	template <typename... Args>
	SharedResource(Args&&... args)
			: resource_{new ResourceType(std::forward<Args>(args)...)} {
	}

	SharedLock sharedLock() {
		return SharedLock{this};
	}

	ExclusiveLock exclusiveLock() {
		return ExclusiveLock{this};
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

TEST(SharedResourceTest, SharedLockReleaseWhenOutOfScope) {
	SharedResource<std::string, true> that("ciao");
	{ auto res = that.sharedLock(); }
	{ auto res = that.exclusiveLock(); }
}

TEST(SharedResourceTest, SharedLockAbortIfCannotGetLock) {
	SharedResource<std::string, true> that("ciao");
	auto res = that.exclusiveLock();
	ASSERT_DEATH(that.sharedLock(), ".*");
}

TEST(SharedResourceTest, SharedLockMultipleIsOk) {
	SharedResource<std::string, true> that("ciao");
	auto res0 = that.sharedLock();
	auto res1 = that.sharedLock();
	auto res2 = that.sharedLock();
	auto res3 = that.sharedLock();
}
