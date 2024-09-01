#pragma once



class TypeInfo;

/// @brief 맨 처음 타입의 정보를 초기화 하는데 사용한다.
template <typename T> requires requires{typename T::SuperType; }
struct TypeInfoInitializer
{
	TypeInfoInitializer(const char* name)
	{
		this->name = name;
		id = typeid(T).hash_code();

		if constexpr (is_same_v<void, typename T::SuperType> == false)
		{
			superInfo = T::SuperType::StaticInfo();
		}
	}

	const char* name;
	uint64 id = typeid(T).hash_code();
	TypeInfo* superInfo = nullptr;
};


class TypeInfo
{
public:
	TypeInfo*	super;	///< 부모의 타입
	uint64		id;		///< typeid
	const char* name;	///< type이름

	template <typename T>
	explicit TypeInfo(const TypeInfoInitializer<T>& info)
	{
		super = info.superInfo;
		id = info.id;
		name = info.name;
	}
};

#define REGISTER_INHERITANCE(typeName)\
public: \
	using SuperType = ThisType;\
	using ThisType = typeName;\
	static TypeInfo* StaticInfo()\
	{\
		static TypeInfo info = TypeInfo(TypeInfoInitializer<ThisType>(#typeName));\
		return &info;\
	}\
	virtual const TypeInfo* GetTypeInfo() { return  _typeInfo; }\
	private:\
	inline static TypeInfo* _typeInfo = StaticInfo();