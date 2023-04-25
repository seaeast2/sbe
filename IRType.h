#pragma once

namespace gclang
{

	// ============================================
	// IR �󿡼��� type �� ǥ���Ѵ�.
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

		// IR Ÿ�Ե�
		enum class EIRType : int
		{
			//eI8,  // 1 byte 
			//eI16,	// 2 byte
			eI32, // 4 byte
			//eI64,  // 8 byte
			eF32,	// 4 byte
			ePTR,	// pointer type. target �� ���� 32bit or 64bit 
			eNONE
		};

	protected:
		EIRTypeKind kind_{ EIRTypeKind::eIRType };
		EIRType etype_{ EIRType::eI32 };
		std::string name_;

		IRType* ptr_type_{nullptr}; // �������� ��� �������� ���� Ÿ��

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

	// IRArrayType ���� �ϱ�
	// [2 x i32] �������� ��� �ؾ� ��.
	// ex) int[2][3] => [2 x [3 x i32]]
	class IRArrayType : public IRType
	{
	private:
		int count_; // item ����
		bool is_string_{false}; // string type ����
	public:
		IRArrayType() = delete;
		// type : array�� original type, count : array �� ����
		IRArrayType(IRType* type, int count);
		virtual ~IRArrayType() = default;

		void SetCount(int count);
		int GetCount();

		void SetIsString(bool is_string);
		bool IsString();

		std::string PrintInitializer(); // Initializer ���
		virtual std::string toString();
	};

	// �Լ� Ÿ��
	// ex) 'i32 (i32, f32, ptr, i32)' : �Լ� Ÿ���� �̸� ����
	class IRFunctionType : public IRType
	{
	private:
		IRType* retty_{ nullptr };
		std::vector<IRType*> paramtys_; // array, class type �� ��� type

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


	// IRStructType �����ϱ�
	// toString() : %struct.name
	// toStringFull() : %struct.name = type { %struct.base, i32 }
	class IRStructType : public IRType
	{
	private:
		std::string org_name_;
		std::vector<IRType*> field_types_; // array, class type �� ��� type

	public:
		IRStructType() = delete;
		IRStructType(std::string name);
		virtual ~IRStructType() = default;

		std::string GetOrgName(); // ���� �̸� ��ȯ

		void AddFieldTy(IRType* field_type);
		std::vector<IRType*>& GetFieldTys();

		std::string toStringFull(); // ��ü ���Ǹ� ���
		virtual std::string toString();
	};

	struct IRTypeEqual
	{
		bool operator()(const IRType* ty1, const IRType* ty2) const {
			return ty1->GetName() == ty2->GetName();
		}
	};

}

