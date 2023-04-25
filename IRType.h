#pragma once

namespace gclang
{

	// ============================================
	// IR 상에서의 type 을 표현한다.
	class IRType
	{
	public:
		enum class EIRTypeKind : int
		{
			eIRType, // primitive types
			eIRArrayType,
			eIRFunctionType,
			eIRStructType,
			eNone
		};

		// IR 타입들
		enum class EIRType : int
		{
			//eI8,  // 1 byte 
			//eI16,	// 2 byte
			eI32, // 4 byte
			//eI64,  // 8 byte
			eF32,	// 4 byte
			ePTR,	// pointer type. target 에 따라 32bit or 64bit 
			eNONE
		};

	protected:
		EIRTypeKind kind_{ EIRTypeKind::eIRType };
		EIRType etype_{ EIRType::eI32 };
		std::string name_;

		IRType* ptr_type_{nullptr}; // 포인터일 경우 포인터의 원래 타입

	public:
		IRType() = default;
		IRType(EIRType etype);
		IRType(EIRType etype, IRType* ptr_type);
		virtual ~IRType() = default;

		void SetETypeKind(EIRTypeKind kind);
		EIRTypeKind GetETypeKind();

		void SetEType(EIRType etype);
		EIRType GetEType();

		void SetName(std::string name);
		std::string GetName();
		std::string GetName() const;

		void SetPtrType(IRType* ptr_type);
		IRType* GetPtrType();
		bool IsPtrType();

		virtual std::string toString();
	};

	// IRArrayType 개발 하기
	// [2 x i32] 형식으로 출력 해야 함.
	// ex) int[2][3] => [2 x [3 x i32]]
	class IRArrayType : public IRType
	{
	private:
		int count_; // item 개수
		bool is_string_{false}; // string type 여부
	public:
		IRArrayType() = delete;
		// type : array의 original type, count : array 의 개수
		IRArrayType(IRType* type, int count);
		virtual ~IRArrayType() = default;

		void SetCount(int count);
		int GetCount();

		void SetIsString(bool is_string);
		bool IsString();

		std::string PrintInitializer(); // Initializer 출력
		virtual std::string toString();
	};

	// 함수 타입
	// ex) 'i32 (i32, f32, ptr, i32)' : 함수 타입의 이름 형식
	class IRFunctionType : public IRType
	{
	private:
		IRType* retty_{ nullptr };
		std::vector<IRType*> paramtys_; // array, class type 일 경우 type

	public:
		IRFunctionType() = delete;
		IRFunctionType(IRType* retty);
		virtual ~IRFunctionType() = default;

		void SetReturnTy(IRType* retty);
		IRType* GetReturnTy();

		void AddParam(IRType* param);
		std::vector<IRType*>& GetParams();

		void UpdateName();

		virtual std::string toString();
	};


	// IRStructType 개발하기
	// toString() : %struct.name
	// toStringFull() : %struct.name = type { %struct.base, i32 }
	class IRStructType : public IRType
	{
	private:
		std::string org_name_;
		std::vector<IRType*> field_types_; // array, class type 일 경우 type

	public:
		IRStructType() = delete;
		IRStructType(std::string name);
		virtual ~IRStructType() = default;

		std::string GetOrgName(); // 원래 이름 반환

		void AddFieldTy(IRType* field_type);
		std::vector<IRType*>& GetFieldTys();

		std::string toStringFull(); // 전체 정의를 출력
		virtual std::string toString();
	};

	struct IRTypeEqual
	{
		bool operator()(const IRType* ty1, const IRType* ty2) const {
			return ty1->GetName() == ty2->GetName();
		}
	};

}

