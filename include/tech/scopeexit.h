#ifndef TECH_SCOPEEXIT_H
#define TECH_SCOPEEXIT_H

#include <functional>


#define SCOPE_EXIT_CREATE_ID2(line)	scopeExitGuard_##line
#define SCOPE_EXIT_CREATE_ID(line)	SCOPE_EXIT_CREATE_ID2(line)

#define SCOPE_EXIT(f) \
	const auto& SCOPE_EXIT_CREATE_ID(__LINE__) = Tech::createScopeExit([&]{ f; })


namespace Tech {


template<typename F>
class ScopeExit {
public:
	ScopeExit(const F& f) :
		f_(f)
	{
	}

	~ScopeExit()
	{
		f_();
	}

private:
	F f_;
};


template<typename F>
ScopeExit<F> createScopeExit(F f)
{
	return ScopeExit<F>(f);
}


} // namespace Tech


#endif // TECH_SCOPEEXIT_H
