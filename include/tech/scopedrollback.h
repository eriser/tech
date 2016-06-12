#ifndef TECH_SCOPEDROLLBACK_H
#define TECH_SCOPEDROLLBACK_H


namespace Tech {


template<typename T>
class ScopedRollback {
public:
	explicit ScopedRollback(T& variable) :
		reference_(variable)
	{
		oldValue_ = reference_;
	}

	ScopedRollback(const ScopedRollback<T>&) = delete;
	ScopedRollback<T>& operator=(const ScopedRollback<T>&) = delete;

	~ScopedRollback()
	{
		reference_ = oldValue_;
	}

	void commit()
	{
		oldValue_ = reference_;
	}

private:
	T& reference_;
	T oldValue_;
};


} // namespace Tech


#endif // TECH_SCOPEDROLLBACK_H
