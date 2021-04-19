#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <thread>
#include <tuple>
#include <deque>
#include <vector>
#include <functional>
#include <future>

namespace detail
{
template <class T>
struct IsReferenceWrapper { static const bool value = false; };
template <class U>
struct IsReferenceWrapper<std::reference_wrapper<U>> { static const bool value = true; };
template <class T, class Type, class Self,
	bool B1 = std::is_base_of<T, std::decay_t<Self>>::value,
	bool B2 = IsReferenceWrapper<std::decay_t<Self>>::value>
	struct InvokeMemFunc_impl
{
	template <class ...Args>
	static auto apply(Type T::* f, Self&& self, Args&& ...args)
		->decltype(((*std::forward<Self>(self)).*f)(std::forward<Args>(args)...))
	{
		return ((*std::forward<Self>(self)).*f)(std::forward<Args>(args)...);
	}
};
template <class T, class Type, class Self, bool B2>
struct InvokeMemFunc_impl<T, Type, Self, true, B2>
{
	template <class ...Args>
	static auto apply(Type T::* f, Self&& self, Args&& ...args)
		->decltype((std::forward<Self>(self).*f)(std::forward<Args>(args)...))
	{
		return (std::forward<Self>(self).*f)(std::forward<Args>(args)...);
	}
};
template <class T, class Type, class Self>
struct InvokeMemFunc_impl<T, Type, Self, false, true>
{
	template <class ...Args>
	static auto apply(Type T::* f, Self&& self, Args&& ...args)
		->decltype((self.get().*f)(std::forward<Args>(args)...))
	{
		return (self.get().*f)(std::forward<Args>(args)...);
	}
};

template <class T, class Type, class Self, class ...Args>
auto Invoke_impl(Type T::* f, Self&& self, Args&& ...args)
	->decltype(InvokeMemFunc_impl<T, Type, Self>::
			   apply(std::forward<decltype(f)>(f), std::forward<Self>(self), std::forward<Args>(args)...))
{
	return InvokeMemFunc_impl<T, Type, Self>::
		apply(std::forward<decltype(f)>(f), std::forward<Self>(self), std::forward<Args>(args)...);
}
template <class Func, class ...Args>
auto Invoke_impl(Func&& f, Args&& ...t)
	->decltype(std::forward<Func>(f)(std::forward<Args>(t)...))
{
	return std::forward<Func>(f)(std::forward<Args>(t)...);
}

template <class AlwaysVoid, class, class...>
struct InvokeResult { };
template <typename F, typename...Args>
struct InvokeResult<decltype(void(detail::Invoke_impl(std::declval<F>(), std::declval<Args>()...))), F, Args...>
{
	using Type = decltype(detail::Invoke_impl(std::declval<F>(), std::declval<Args>()...));
};

}
//std::invokeの代替。
//あちらと違いメンバ変数には非対応。
//メンバ変数を関数風に呼び出すとか変態的な使い方が必要になるとは思えん。
template <class F, class... ArgTypes>
struct InvokeResult : detail::InvokeResult<void, F, ArgTypes...> {};
template <class Func, class ...Args>
auto Invoke(Func&& f, Args&& ...args)
	->decltype(detail::Invoke_impl(std::forward<Func>(f), std::forward<Args>(args)...))
{
	return detail::Invoke_impl(std::forward<Func>(f), std::forward<Args>(args)...);
}

//c++14以下にはremove_cvrefがないので、その代用品。
template <class T>
struct RemoveCVRef
{
	using Type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <class T>
using RemoveCVRefT = typename RemoveCVRef<T>::Type;

template <class ...T>
struct TypeList
{
	static constexpr std::size_t Size = sizeof...(T);
};
template <class T>
struct IsFunctor
{
	template <class U>
	static auto check(U&) -> decltype(&U::operator(), std::true_type());
	static auto check(...) -> decltype(std::false_type());
	typedef decltype(check(std::declval<T&>())) type;
	static constexpr bool value = type::value;
};

namespace detail
{

template <class Function, bool B = IsFunctor<Function>::value>
struct FunctionTraits_impl;
template <class Function>
struct FunctionTraits_impl<Function, true>
{
	using ArgTypes = typename FunctionTraits_impl<decltype(&Function::operator())>::ArgTypes;
	using RetType = typename FunctionTraits_impl<decltype(&Function::operator())>::RetType;
};
template <class R, class T, class ...Args>
struct FunctionTraits_impl<R(T::*)(Args...) const, false>
{
	using ArgTypes = TypeList<Args...>;
	using RetType = R;
};
template <class R, class T, class ...Args>
struct FunctionTraits_impl<R(T::*)(Args...), false>
{
	using ArgTypes = TypeList<Args...>;
	using RetType = R;
};
template <class R, class ...Args>
struct FunctionTraits_impl<R(*)(Args...), false>
{
	using ArgTypes = TypeList<Args...>;
	using RetType = R;
};
template <class R, class ...Args>
struct FunctionTraits_impl<R(Args...), false>
{
	using ArgTypes = TypeList<Args...>;
	using RetType = R;
};

}

//関数ポインタ、メンバ関数ポインタ、関数オブジェクト、いずれかを受け取り、
//その引数と戻り値の方を取得する。
template <class Function>
struct FunctionTraits
{
	using ArgTypes = typename detail::FunctionTraits_impl<Function>::ArgTypes;
	using RetType = typename detail::FunctionTraits_impl<Function>::RetType;
};

namespace detail
{
template <class Type>
std::add_rvalue_reference_t<Type> ForwardAsRValue(std::remove_reference_t<Type>& t)
{
	return static_cast<std::add_rvalue_reference_t<Type>>(t);
}
template <class Func, class Tuple, std::size_t ...Indices>
auto Apply(Func&& f, Tuple&& t, std::index_sequence<Indices...>)
	->decltype(Invoke(std::forward<Func>(f), ForwardAsRValue<std::tuple_element_t<Indices, std::remove_reference_t<Tuple>>>(std::get<Indices>(t))...))
{
	//C++17以降ならstd::invokeでいい。
	return Invoke(std::forward<Func>(f), ForwardAsRValue<std::tuple_element_t<Indices, std::remove_reference_t<Tuple>>>(std::get<Indices>(t))...);
}
template <class T>
struct UnwrapReference
{
	using Type = T;
};
template <class T>
struct UnwrapReference<std::reference_wrapper<T>>
{
	using Type = T & ;
};
template <class T>
using Unwrap_and_Decay = typename UnwrapReference<typename std::decay_t<T>>::Type;

template <class Task, class ...Args>
struct TaskBinder
{
	//std::bindを使うとrvalue referenceを扱うことが出来ない。
	//かといってラムダ式を使うとstd::functionのせいでnon-copyableなオブジェクトを扱えない。
	//仕方ないのでバインドクラスを自作する。
	//using Result = decltype(detail::Apply(std::declval<std::decay_t<Task>>(), std::declval<std::tuple<detail::Unwrap_and_Decay<Args>...>>(), std::make_index_sequence<sizeof...(Args)>()));
	using Result = typename FunctionTraits<RemoveCVRefT<Task>>::RetType;
	template <class ...Args_>
	TaskBinder(std::promise<Result>& p, Task f, Args_&& ...args)
		: mPromise(p), mTask(std::forward<Task>(f)), mTuple(std::make_tuple(std::forward<Args_>(args)...))
	{}
	template <class ...Args_>
	TaskBinder(std::promise<Result>&& p, Task f, Args_&& ...args)
		: mPromise(std::move(p)), mTask(std::forward<Task>(f)), mTuple(std::make_tuple(std::forward<Args_>(args)...))
	{}
	TaskBinder(const TaskBinder<Task, Args...>& b)
		: mPromise(), mTask(MakeTaskDummy()), mTuple(MakeTupleDummy())
	{
		//頭の悪いstd::functionはcopy constructibleでないとコンパイルエラーを出すため、
		//呼ばれると例外を送出するだけのコピーコンストラクタを用意しておく。
		//ムーブコンストラクタが定義されている限り、
		//こちらが実際に呼ばれることはない、はず。
	}
	TaskBinder(TaskBinder<Task, Args...>&& b) noexcept
		: mPromise(std::move(b.mPromise)), mTask(std::move(b.mTask)), mTuple(std::move(b.mTuple)) {}
	template <bool B = !std::is_void<Result>::value,
		std::enable_if_t<B, std::nullptr_t> = nullptr>
		void operator()()
	{
		mPromise.set_value(detail::Apply(mTask, mTuple, std::make_index_sequence<sizeof...(Args)>()));
	}
	template <class Dummy = void, bool B = std::is_void<Result>::value,
		std::enable_if_t<B, std::nullptr_t> = nullptr>
		void operator()()
	{
		detail::Apply(mTask, mTuple, std::make_index_sequence<sizeof...(Args)>());
	}
private:
	Task MakeTaskDummy() const { throw ("Copy constructor of Binder must not be called."); }
	std::tuple<detail::Unwrap_and_Decay<Args>...> MakeTupleDummy() const { throw ("Copy constructor of Binder must not be called."); }

	std::decay_t<Task> mTask;
	std::tuple<detail::Unwrap_and_Decay<Args>...> mTuple;
	std::promise<Result> mPromise;
};

}

class ThreadPool
{
	enum Status
	{
		STANDBY, WORKING, JOINING,
	};

	struct ThTerminator {};

	struct WorkerThread
	{
		WorkerThread(ThreadPool& j) : mMngr(j) {}
		void operator()()
		{
			while (1)
			{
				try
				{
					auto job = mMngr.GetQueueFront();
					if (job) job();
				}
				catch (ThTerminator) { return; }
			}
		}
		ThreadPool& mMngr;
	};

public:

	ThreadPool(int numthreads)
		: mStatus(STANDBY)
	{
		Start(numthreads);
	}
	~ThreadPool()
	{
		Join();
	}

	//invokeコンセプトとおおよそ同じ規則に基づいて、
	//関数を処理の順番待ちに追加する。
	//参照型を渡すときはstd::reference_wrapperを、Moveしたい場合はstd::moveを使う。
	//与えた関数の戻り値は、AddTaskの戻り値であるstd::futureから取得できる。
	//ただし、参照型はそのまま参照型メンバとして、ポインタも同様に保持されるため、
	//関数呼び出しまでインスタンスが消滅しないよう注意しなければならない。
	//またmoveしたインスタンスは関数中でどう扱われるかに関係なくmove constructorによって破壊される。
	//このあたりの挙動に問題があれば、自身でラムダ式なりによって適切な処理を定義すること。
	template <class Task, class ...Args>
	auto AddTask(Task&& w, Args&& ...args) -> std::future<decltype(Invoke(w, std::forward<Args>(args)...))>
	{
		using Result = decltype(Invoke(w, std::forward<Args>(args)...));
		std::promise<Result> p;
		std::future<Result> f = p.get_future();
		{
			std::unique_lock<std::mutex> ul(mMtx);
			mQueue.emplace_back(detail::TaskBinder<Task, Args...>(std::move(p), std::forward<Task>(w), std::forward<Args>(args)...));
		}
		mLaunchCv.notify_all();
		return std::move(f);
	}

	bool HasQueue() const { return !mQueue.empty(); }
	bool IsJoining() const { return mStatus == JOINING; }

	void Start(int numthreads)
	{
		if (mStatus != STANDBY) return;
		std::unique_lock<std::mutex> ul(mMtx);
		mStatus = WORKING;
		mWorkerThreads.clear();
		mWorkerThreads.reserve(numthreads);
		for (int i = 0; i < numthreads; ++i)
			mWorkerThreads.emplace_back(std::thread(WorkerThread(*this)));
	}

	void Join()
	{
		{
			std::unique_lock<std::mutex> ul(mMtx);
			mStatus = JOINING;
		}
		mLaunchCv.notify_all();
		for (auto& w : mWorkerThreads) w.join();
		mWorkerThreads.clear();
	}

private:

	std::function<void()> GetQueueFront()
	{
		std::unique_lock<std::mutex> l(mMtx);
		while (!HasQueue())
		{
			if (IsJoining()) throw ThTerminator();
			mLaunchCv.wait(l);//処理開始通知が来るまで待機。
		}
		if (mQueue.empty()) return std::function<void()>();
		std::function<void()> f = std::move(*mQueue.begin());
		mQueue.pop_front();
		return std::move(f);
	}

	std::mutex mMtx;
	std::deque<std::function<void()>> mQueue;
	Status mStatus;

	std::condition_variable mLaunchCv;

	std::vector<std::thread> mWorkerThreads;

};

#endif
