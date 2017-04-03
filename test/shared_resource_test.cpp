#include "../src/shared_resource.hpp"
#include <gtest/gtest.h>

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

TEST(SharedResourceTest, ArrowOperator) {
	SharedResource<std::string, true> that("ciao");
	auto res1 = that.sharedLock();
	auto res2 = that.sharedLock();
	std::string copy1{res1->c_str()};
	std::string copy2{res2->c_str()};
	ASSERT_EQ(copy1, copy2);
}

TEST(SharedResourceTest, ExclusiveLockArrowOperator) {
	SharedResource<std::string, true> that("ciao");
	auto res1 = that.sharedLock();
	std::string copy1{res1->c_str()};
	ASSERT_EQ(copy1, "ciao");
}

TEST(SharedResourceTest, DereferenceOperator) {
	SharedResource<std::string, true> that("ciao");
	auto res1 = that.sharedLock();
	auto res2 = that.sharedLock();
	std::string copy1 = *res1;
	std::string copy2 = *res2;
	ASSERT_EQ(copy1, copy2);
}

TEST(SharedResourceTest, ExclusiveLockDereferenceOperator) {
	SharedResource<std::string, true> that("ciao");
	auto res1 = that.exclusiveLock();
	std::string copy1 = *res1;
	ASSERT_EQ(copy1, "ciao");
}
