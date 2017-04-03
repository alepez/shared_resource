#ifndef SHARED_RESOURCE_HPP_JLFTPMCE
#define SHARED_RESOURCE_HPP_JLFTPMCE

#include <boost/thread/shared_mutex.hpp>
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

		ResourceType* operator->() {
			return parent_->resource_.get();
		}

		ResourceType& operator*() {
			return *parent_->resource_;
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

		ResourceType* operator->() {
			return parent_->resource_.get();
		}

		ResourceType& operator*() {
			return *parent_->resource_;
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



#endif /* end of include guard: SHARED_RESOURCE_HPP_JLFTPMCE */
