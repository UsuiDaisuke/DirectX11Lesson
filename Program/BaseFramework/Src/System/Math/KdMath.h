#pragma once

//3Dベクトル
class KdVec3 : public DirectX::XMFLOAT3
{
public:

	//デフォルトコンストラクタ
	KdVec3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	//座標指定付きコンストラクタ
	KdVec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	//XMVECTORから代入してきた時
	KdVec3(const DirectX::XMVECTOR& v)
	{
		//変換して代入
		DirectX::XMStoreFloat3(this, v);
		//下記と同意だが、上記のSIMD命令を使用した方が高速
		// x = v.m128_f32[0];
		// y = v.m128_f32[1];
		// z = v.m128_f32[2];
	}

	//XMVECTORへ変換
	operator DirectX::XMVECTOR() const
	{
		return DirectX::XMLoadFloat3(this);
	}

	//Math::Vector3と互換性を持つための関数
	operator Math::Vector3& ()
	{
		return *(Math::Vector3*)this;
	}

	//算術演算子 乗算(*)
	KdVec3& operator*= (float s)
	{
		*this = DirectX::XMVectorScale(*this, s);
		return *this;
	}

	//自分を正規化
	void Normalize()
	{
		*this = DirectX::XMVector3Normalize(*this);
	}

	//長さ
	float Length() const
	{
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}

	//長さの2乗(高速なので判定用に使用することが多い)
	float LengthSquared() const
	{
		return DirectX::XMVector3LengthSq(*this).m128_f32[0];
	}

};

//4x4の行列
class KdMatrix : public DirectX::XMFLOAT4X4
{
public:

	//デフォルトコンストラクタは単位行列
	KdMatrix()
	{
		*this = DirectX::XMMatrixIdentity();
	}

	//XMMATRIXから代入してきた
	KdMatrix(const DirectX::XMMATRIX& m)
	{
		DirectX::XMStoreFloat4x4(this, m);
	}

	//XMFLOAT4X4, Math::Matrixから代入してきた
	KdMatrix(const DirectX::XMFLOAT4X4& m)
	{
		memcpy_s(this, sizeof(float) * 16, &m, sizeof(XMFLOAT4X4));
	}

	//XMMATRIXへ変換
	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(this);
	}

	//Math::Matrixと互換性を持つための関数
	operator Math::Matrix& ()
	{
		return *(Math::Matrix*)this;
	}

	//代入演算子　乗算
	KdMatrix& operator*= (const KdMatrix& m)
	{
		*this = DirectX::XMMatrixMultiply(*this, m);
		return *this;
	}

	/* ==========作成========== */

	//移動行列作成
	void CreateTranslation(float x, float y, float z)
	{
		*this = DirectX::XMMatrixTranslation(x, y, z);
	}

	//X軸回転行列
	void CreateRotationX(float angle)
	{
		*this = DirectX::XMMatrixRotationX(angle);
	}

	//Y軸回転行列
	void CreateRotationY(float angle)
	{
		*this = DirectX::XMMatrixRotationY(angle);
	}

	//Z軸回転行列
	void CreateRotationZ(float angle)
	{
		*this = DirectX::XMMatrixRotationZ(angle);
	}

	/* ==========操作========== */

	//X軸回転
	void RotateX(float angle)
	{
		*this *= DirectX::XMMatrixRotationX(angle);
	}

	//Y軸回転
	void RotateY(float angle)
	{
		*this *= DirectX::XMMatrixRotationY(angle);
	}

	//Z軸回転
	void RotateZ(float angle)
	{
		*this *= DirectX::XMMatrixRotationZ(angle);
	}

	/* ==========プロパティ========== */

	//X軸取得
	KdVec3 GetAxisX() const
	{
		return{ _11, _12, _13 };
	}

	//X軸セット
	void SetAxisX(const KdVec3& v)
	{
		_11 = v.x;
		_12 = v.y;
		_13 = v.z;
	}

	//Y軸取得
	KdVec3 GetAxisY() const
	{
		return{ _21, _22, _23 };
	}

	//Y軸セット
	void SetAxisY(const KdVec3& v)
	{
		_21 = v.x;
		_22 = v.y;
		_23 = v.z;
	}

	//Z軸取得
	KdVec3 GetAxisZ() const
	{
		return{ _31, _32, _33 };
	}

	//Z軸セット
	void SetAxisZ(const KdVec3& v)
	{
		_31 = v.x;
		_32 = v.y;
		_33 = v.z;
	}

	//座標取得
	KdVec3 GetTranslation() const
	{
		return{ _41, _42, _43 };
	}

	//座標セット
	void SetTranslation(const KdVec3& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}

	void Move(const KdVec3& v)
	{
		_41 += v.x;
		_42 += v.y;
		_43 += v.z;
	}

	// 逆行列にする
	void Inverse()
	{
		*this = DirectX::XMMatrixInverse(nullptr, *this);
	}

	KdMatrix Invert()
	{
		return DirectX::XMMatrixInverse(nullptr, *this);
	}

private:
};

// KdMatrix同士の合成
inline KdMatrix operator* (const KdMatrix& m1, const KdMatrix& m2)
{
	using namespace DirectX;
	return XMMatrixMultiply(m1, m2);
}
