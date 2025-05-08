#include "Memory/Ref.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

namespace Ryu::Memory::Tests
{
    // Simple class for testing reference counting
    class TestObject : public RefCounted
    {
    public:
        TestObject() : m_value(0) {}
        explicit TestObject(int value) : m_value(value) {}

        void SetValue(int value) { m_value = value; }
        int GetValue() const { return m_value; }

    private:
        int m_value;
    };

    // Derived class for testing inheritance
    class DerivedTestObject : public TestObject
    {
    public:
        DerivedTestObject() : TestObject(), m_extraValue(0) {}
        explicit DerivedTestObject(int value, int extraValue)
            : TestObject(value), m_extraValue(extraValue) {
        }

        int GetExtraValue() const { return m_extraValue; }
        void SetExtraValue(int value) { m_extraValue = value; }

    private:
        int m_extraValue;
    };

    TEST_CASE("Ref basic functionality")
    {
        SUBCASE("Default constructor creates null reference")
        {
            Ref<TestObject> ref;
            CHECK(ref == nullptr);
            CHECK_FALSE(ref);
            CHECK(ref.Get() == nullptr);
            CHECK(ref.GetRefCount() == 0);
        }

        SUBCASE("Constructor with nullptr creates null reference")
        {
            Ref<TestObject> ref(nullptr);
            CHECK(ref == nullptr);
            CHECK_FALSE(ref);
            CHECK(ref.Get() == nullptr);
            CHECK(ref.GetRefCount() == 0);
        }

        SUBCASE("Constructor with pointer creates valid reference")
        {
            TestObject* obj = new TestObject(42);
            Ref<TestObject> ref(obj);
            CHECK(ref != nullptr);
            CHECK(ref);
            CHECK(ref.Get() == obj);
            CHECK(ref.GetRefCount() == 1);
            CHECK(ref->GetValue() == 42);
        }

        SUBCASE("Copy constructor increments reference count")
        {
            Ref<TestObject> ref1(new TestObject(42));
            CHECK(ref1.GetRefCount() == 1);

            Ref<TestObject> ref2(ref1);
            CHECK(ref1.GetRefCount() == 2);
            CHECK(ref2.GetRefCount() == 2);
            CHECK(ref1.Get() == ref2.Get());
        }

        SUBCASE("Move constructor transfers ownership")
        {
            Ref<TestObject> ref1(new TestObject(42));
            TestObject* ptr = ref1.Get();

            Ref<TestObject> ref2(std::move(ref1));
            CHECK(ref2.Get() == ptr);
            CHECK(ref1.Get() == nullptr);
            CHECK(ref2.GetRefCount() == 1);
        }
    }

    TEST_CASE("Ref Create static method")
    {
        SUBCASE("Create default constructor")
        {
            auto ref = Ref<TestObject>::Create<TestObject>();
            CHECK(ref != nullptr);
            CHECK(ref.GetRefCount() == 1);
            CHECK(ref->GetValue() == 0);
        }

        SUBCASE("Create with arguments")
        {
            auto ref = Ref<TestObject>::Create<TestObject>(42);
            CHECK(ref != nullptr);
            CHECK(ref.GetRefCount() == 1);
            CHECK(ref->GetValue() == 42);
        }

        SUBCASE("Create derived class")
        {
            auto ref = Ref<TestObject>::Create<DerivedTestObject>(42, 84);
            CHECK(ref != nullptr);
            CHECK(ref.GetRefCount() == 1);
            CHECK(ref->GetValue() == 42);

            // Cast to derived class to access extra functionality
            auto derivedRef = RefCast<DerivedTestObject>(ref);
            CHECK(derivedRef != nullptr);
            CHECK(derivedRef->GetExtraValue() == 84);
        }
    }

    TEST_CASE("Ref assignment operators")
    {
        SUBCASE("Copy assignment operator")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));

            CHECK(ref1->GetValue() == 42);
            CHECK(ref2->GetValue() == 84);

            ref2 = ref1;
            CHECK(ref1.GetRefCount() == 2); // Copy exists
            CHECK(ref2.GetRefCount() == 2); // Shares the same count
            CHECK(ref2->GetValue() == 42);
        }

        SUBCASE("Move assignment operator")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));
            TestObject* ptr = ref1.Get();

            ref2 = std::move(ref1);
            CHECK(ref1.Get() == nullptr);
            CHECK(ref2.Get() == ptr);
            CHECK(ref2.GetRefCount() == 1); // Still has count of 1
            CHECK(ref2->GetValue() == 42);
        }

        SUBCASE("nullptr assignment")
        {
            Ref<TestObject> ref(new TestObject(42));
            CHECK(ref != nullptr);

            ref = nullptr;
            CHECK(ref == nullptr);
            CHECK_FALSE(ref);
        }
    }

    TEST_CASE("Ref polymorphic behavior")
    {
        SUBCASE("Assigning derived to base")
        {
            Ref<DerivedTestObject> derived(new DerivedTestObject(42, 84));
            Ref<TestObject> base;

            base = derived;
            CHECK(base.GetRefCount() == 2);
            CHECK(derived.GetRefCount() == 2); // Same reference count
            CHECK(base->GetValue() == 42);
        }

        SUBCASE("Constructing base from derived")
        {
            Ref<DerivedTestObject> derived(new DerivedTestObject(42, 84));
            Ref<TestObject> base(derived);

            CHECK(base.GetRefCount() == 2);
            CHECK(derived.GetRefCount() == 2); // Same reference count
            CHECK(base->GetValue() == 42);
        }

        SUBCASE("RefCast from base to derived")
        {
            Ref<TestObject> base(new DerivedTestObject(42, 84));
            Ref<DerivedTestObject> derived = RefCast<DerivedTestObject>(base);

            CHECK(derived != nullptr);
            CHECK(derived.GetRefCount() == 2);
            CHECK(base.GetRefCount() == 2); // Same reference count
            CHECK(derived->GetValue() == 42);
            CHECK(derived->GetExtraValue() == 84);
        }

        SUBCASE("RefCast with invalid cast returns null")
        {
            Ref<TestObject> base(new TestObject(42));  // Not a derived object
            Ref<DerivedTestObject> derived = RefCast<DerivedTestObject>(base);

            CHECK(derived == nullptr);
            CHECK(base.GetRefCount() == 1);
        }
    }

    TEST_CASE("Ref manipulation methods")
    {
        SUBCASE("Release gives up ownership")
        {
            Ref<TestObject> ref(new TestObject(42));
            CHECK(ref.GetRefCount() == 1);

            TestObject* ptr = ref.Release();
            CHECK(ref == nullptr);
            CHECK(ptr->GetValue() == 42);
            CHECK(ptr->GetRefCount() == 1);

            // Clean up manually since we released it
            ptr->Release();
        }

        SUBCASE("Reset to nullptr")
        {
            Ref<TestObject> ref(new TestObject(42));
            CHECK(ref != nullptr);

            ref.Reset();
            CHECK(ref == nullptr);
        }

        SUBCASE("Reset to new pointer")
        {
            Ref<TestObject> ref(new TestObject(42));
            [[maybe_unused]] TestObject* oldPtr = ref.Get();
            TestObject* newPtr = new TestObject(84);

            ref.Reset(newPtr);
            CHECK(ref.Get() == newPtr);
            CHECK(ref->GetValue() == 84);
        }

        SUBCASE("Swap exchanges ownership")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));

            TestObject* ptr1 = ref1.Get();
            TestObject* ptr2 = ref2.Get();

            ref1.Swap(ref2);

            CHECK(ref1.Get() == ptr2);
            CHECK(ref2.Get() == ptr1);
            CHECK(ref1->GetValue() == 84);
            CHECK(ref2->GetValue() == 42);
        }
    }

    TEST_CASE("Ref operators")
    {
        SUBCASE("Arrow and dereference operators")
        {
            Ref<TestObject> ref(new TestObject(42));

            CHECK(ref->GetValue() == 42);
            CHECK((*ref).GetValue() == 42);

            ref->SetValue(84);
            CHECK((*ref).GetValue() == 84);
        }

        SUBCASE("Equality operators")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(ref1);
            Ref<TestObject> ref3(new TestObject(42));

            CHECK(ref1 == ref2);
            CHECK(ref1 != ref3);
            CHECK(ref2 != ref3);
        }

        SUBCASE("Equality with nullptr")
        {
            Ref<TestObject> ref1;
            Ref<TestObject> ref2(new TestObject(42));

            CHECK(ref1 == nullptr);
            CHECK(nullptr == ref1);
            CHECK(ref2 != nullptr);
            CHECK(nullptr != ref2);
        }
    }

    TEST_CASE("Ref destruction")
    {
        SUBCASE("Object is destroyed when last reference is gone")
        {
            static bool objectDestroyed1 = false;

            // Use a special test class that tracks destruction
            class DestructorTestObject : public RefCounted
            {
            public:
                DestructorTestObject() { objectDestroyed1 = false; }
                ~DestructorTestObject() override { objectDestroyed1 = true; }
            };

            objectDestroyed1 = false;
            {
                // Create the Ref directly instead of manually managing the object
                Ref<DestructorTestObject> ref(new DestructorTestObject());
                CHECK_FALSE(objectDestroyed1);
            }  // ref goes out of scope here, which should trigger destruction

            // Verify the object was destroyed when last reference was released
            CHECK(objectDestroyed1);
        }

        SUBCASE("Object is not destroyed while references exist")
        {
            static bool objectDestroyed2 = false;

            // Use a special test class that tracks destruction
            class DestructorTestObject : public RefCounted
            {
            public:
                DestructorTestObject() { objectDestroyed2 = false; }
                ~DestructorTestObject() override { objectDestroyed2 = true; }
            };

            objectDestroyed2 = false;
            {
                // Create a scope with multiple references to the same object
                Ref<DestructorTestObject> ref1(new DestructorTestObject());
                {
                    Ref<DestructorTestObject> ref2(ref1);
                    CHECK_FALSE(objectDestroyed2);
                }  // ref2 goes out of scope here
                CHECK_FALSE(objectDestroyed2);
            }  // ref1 goes out of scope here, should trigger destruction

            // Verify the object was destroyed when all references were gone
            CHECK(objectDestroyed2);
        }
    }

    TEST_CASE("MakeRef helper functions")
    {
        SUBCASE("MakeRef from pointer")
        {
            TestObject* ptr = new TestObject(42);
            auto ref = MakeRef(ptr);

            CHECK(ref.Get() == ptr);
            CHECK(ref.GetRefCount() == 1);
        }

        SUBCASE("MakeRef from existing reference")
        {
            auto ref1 = Ref<TestObject>::Create<TestObject>(42);
            auto ref2 = MakeRef(ref1);

            CHECK(ref1.Get() == ref2.Get());
            CHECK(ref1.GetRefCount() == 2);
        }
    }

    TEST_CASE("WeakRef basic functionality")
    {
        SUBCASE("Default constructor creates null reference")
        {
            WeakRef<TestObject> weak;
            CHECK_FALSE(weak.IsValid());
            CHECK(weak.IsExpired());
            CHECK(weak.GetWeakCount() == 0);
            CHECK(weak.GetRefCount() == 0);

            auto locked = weak.Lock();
            CHECK(locked == nullptr);
        }

        SUBCASE("Constructor from Ref creates valid weak reference")
        {
            Ref<TestObject> ref(new TestObject(42));
            WeakRef<TestObject> weak(ref);

            CHECK(weak.IsValid());
            CHECK_FALSE(weak.IsExpired());
            CHECK(weak.GetWeakCount() == 1);
            CHECK(weak.GetRefCount() == 1);

            auto locked = weak.Lock();
            CHECK(locked != nullptr);
            CHECK(locked.Get() == ref.Get());
            CHECK(locked->GetValue() == 42);
        }

        SUBCASE("Copy constructor creates new weak reference")
        {
            Ref<TestObject> ref(new TestObject(42));
            WeakRef<TestObject> weak1(ref);
            WeakRef<TestObject> weak2(weak1);

            CHECK(weak1.IsValid());
            CHECK(weak2.IsValid());
            CHECK(weak1.GetWeakCount() == 2);
            CHECK(weak2.GetWeakCount() == 2);

            auto locked1 = weak1.Lock();
            auto locked2 = weak2.Lock();
            CHECK(locked1.Get() == locked2.Get());
        }

        SUBCASE("Move constructor transfers weak reference")
        {
            Ref<TestObject> ref(new TestObject(42));
            WeakRef<TestObject> weak1(ref);
            WeakRef<TestObject> weak2(std::move(weak1));

            CHECK(weak2.IsValid());
            CHECK_FALSE(weak1.IsValid());  // Moved from
            CHECK(weak2.GetWeakCount() == 1);

            auto locked = weak2.Lock();
            CHECK(locked.Get() == ref.Get());
        }
    }

    TEST_CASE("WeakRef with expired references")
    {
        SUBCASE("Weak reference expires when strong references are gone")
        {
            WeakRef<TestObject> weak;

            {
                // Create a scope for the strong reference
                Ref<TestObject> ref(new TestObject(42));
                weak = ref;
                CHECK(weak.IsValid());
                CHECK(weak.GetRefCount() == 1);
            } // ref goes out of scope here, which should trigger Release()

            // Now the weak reference should be expired
            CHECK_FALSE(weak.IsValid());
            CHECK(weak.IsExpired());
            CHECK(weak.GetRefCount() == 0);

            auto locked = weak.Lock();
            CHECK(locked == nullptr);
        }

        SUBCASE("Control block is cleaned up properly")
        {
            WeakRef<TestObject> weak;

            {
                // Create the Ref directly to properly manage the object
                Ref<TestObject> ref(new TestObject(42));
                weak = ref;
                CHECK(weak.IsValid());
            } // ref goes out of scope, object should be destroyed but control block remains

            // Now the object should be destroyed but weak ref still valid
            CHECK(weak.IsExpired());
            CHECK(weak.GetWeakCount() == 1);  // Control block still exists

            // Reset the weak reference which should clean up the control block
            weak.Reset();
            CHECK(weak.GetWeakCount() == 0);
        }
    }


    TEST_CASE("WeakRef assignment operators")
    {
        SUBCASE("Copy assignment from WeakRef")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));

            WeakRef<TestObject> weak1(ref1);
            WeakRef<TestObject> weak2(ref2);

            weak2 = weak1;

            CHECK(weak1.IsValid());
            CHECK(weak2.IsValid());
            CHECK(weak1.GetWeakCount() == 2);
            CHECK(weak2.GetWeakCount() == 2);

            auto locked1 = weak1.Lock();
            auto locked2 = weak2.Lock();
            CHECK(locked1.Get() == locked2.Get());
            CHECK(locked1->GetValue() == 42);
        }

        SUBCASE("Move assignment from WeakRef")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));

            WeakRef<TestObject> weak1(ref1);
            WeakRef<TestObject> weak2(ref2);

            weak2 = std::move(weak1);

            CHECK_FALSE(weak1.IsValid());  // Moved from
            CHECK(weak2.IsValid());
            CHECK(weak2.GetWeakCount() == 1);

            auto locked = weak2.Lock();
            CHECK(locked.Get() == ref1.Get());
            CHECK(locked->GetValue() == 42);
        }

        SUBCASE("Assignment from Ref")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));

            WeakRef<TestObject> weak(ref1);
            CHECK(weak.Lock()->GetValue() == 42);

            weak = ref2;
            CHECK(weak.Lock()->GetValue() == 84);
        }

        SUBCASE("Assignment to nullptr")
        {
            Ref<TestObject> ref(new TestObject(42));
            WeakRef<TestObject> weak(ref);

            CHECK(weak.IsValid());

            weak = nullptr;
            CHECK_FALSE(weak.IsValid());
            CHECK(weak.GetWeakCount() == 0);
        }
    }

    TEST_CASE("WeakRef polymorphic behavior")
    {
        SUBCASE("WeakRef from derived to base")
        {
            Ref<DerivedTestObject> derived(new DerivedTestObject(42, 84));
            WeakRef<DerivedTestObject> weakDerived(derived);
            WeakRef<TestObject> weakBase(derived);

            CHECK(weakDerived.IsValid());
            CHECK(weakBase.IsValid());

            auto lockedDerived = weakDerived.Lock();
            auto lockedBase = weakBase.Lock();

            CHECK(lockedDerived.Get() == derived.Get());
            CHECK(lockedBase.Get() == derived.Get());
        }

        SUBCASE("WeakCast from base to derived")
        {
            Ref<TestObject> base(new DerivedTestObject(42, 84));
            WeakRef<TestObject> weakBase(base);

            auto weakDerived = WeakCast<DerivedTestObject>(weakBase);
            CHECK(weakDerived.IsValid());

            auto locked = weakDerived.Lock();
            CHECK(locked != nullptr);
            CHECK(locked->GetValue() == 42);
            CHECK(locked->GetExtraValue() == 84);
        }

        SUBCASE("WeakCast with invalid cast")
        {
            Ref<TestObject> base(new TestObject(42));  // Not a derived object
            WeakRef<TestObject> weakBase(base);

            auto weakDerived = WeakCast<DerivedTestObject>(weakBase);
            CHECK_FALSE(weakDerived.IsValid());

            auto locked = weakDerived.Lock();
            CHECK(locked == nullptr);
        }
    }

    TEST_CASE("WeakRef methods")
    {
        SUBCASE("Reset clears weak reference")
        {
            Ref<TestObject> ref(new TestObject(42));
            WeakRef<TestObject> weak(ref);

            CHECK(weak.IsValid());

            weak.Reset();
            CHECK_FALSE(weak.IsValid());
            CHECK(weak.GetWeakCount() == 0);
        }

        SUBCASE("Swap exchanges weak references")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));

            WeakRef<TestObject> weak1(ref1);
            WeakRef<TestObject> weak2(ref2);

            weak1.Swap(weak2);

            CHECK(weak1.Lock()->GetValue() == 84);
            CHECK(weak2.Lock()->GetValue() == 42);
        }
    }

    TEST_CASE("WeakRef equality operators")
    {
        SUBCASE("Equality with valid references")
        {
            Ref<TestObject> ref1(new TestObject(42));
            Ref<TestObject> ref2(new TestObject(84));

            WeakRef<TestObject> weak1a(ref1);
            WeakRef<TestObject> weak1b(ref1);
            WeakRef<TestObject> weak2(ref2);

            CHECK(weak1a == weak1b);
            CHECK(weak1a != weak2);
        }

        SUBCASE("Equality with expired references")
        {
            WeakRef<TestObject> weak1;
            WeakRef<TestObject> weak2;

            {
                Ref<TestObject> ref1(new TestObject(42));
                Ref<TestObject> ref2(new TestObject(42));  // Different object, same value

                weak1 = ref1;
                weak2 = ref2;
            }  // refs go out of scope, weaks expire

            CHECK(weak1 != weak2);  // Different expired objects
        }
    }

    TEST_CASE("MakeWeak helper function")
    {
        SUBCASE("MakeWeak from Ref")
        {
            Ref<TestObject> ref(new TestObject(42));
            auto weak = MakeWeak(ref);

            CHECK(weak.IsValid());
            CHECK(weak.GetWeakCount() == 1);
            CHECK(weak.Lock().Get() == ref.Get());
        }
    }


}
