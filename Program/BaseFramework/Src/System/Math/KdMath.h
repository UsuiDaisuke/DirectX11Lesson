#pragma once

//3Dベクトル
class KdVec3 : public DirectX::XMFLOAT3
{
public:

	// 指定行列でVectorを変換する
	KdVec3& TransformCoord(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformCoord(*this, m);
		return *this;
	}

	// 指定(回転)行列でVectorを変換する
	KdVec3& TransformNormal(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformNormal(*this, m);
		return *this;
	}

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

	// 内積
	static float Dot(const KdVec3& v1, const KdVec3& v2)
	{
		return DirectX::XMVector3Dot(v1, v2).m128_f32[0];
	}

	// 外積
	static KdVec3 Cross(const KdVec3& v1, const KdVec3& v2)
	{
		return DirectX::XMVector3Cross(v1, v2);
	}

	// 徐々に特定の方向を向く
	inline void Complement(const KdVec3& vTo, float rot)
	{
		// ※※※※※回転軸作成（この軸で回転する）※※※※※
		KdVec3 vRotAxis = KdVec3::Cross(*this, vTo);

		// 0ベクトルなら回転しない
		if (vRotAxis.LengthSquared() != 0)
		{
			// 自分のZ方向ベクトルと自身からターゲットへ向かうベクトルの内積
			float d = KdVec3::Dot(*this, vTo);

			// 誤差で-1～1以外になる可能性大なので、クランプする
			if (d > 1.0f)d = 1.0f;
			else if (d < -1.0f)d = -1.0f;

			// 自分の前方向ベクトルと自身からターゲットへ向かうベクトル間の角度(radian)
			float radian = acos(d);

			// 角度制限 １フレームにつき最大で指定角度以上回転しない
			if (radian > rot * KdToRadians)
			{
				radian = rot * KdToRadians;
			}

			KdVec3 axis = (vTo - *this);

			*this = *this + (axis * radian);
		}
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

	//回転行列作成
	void CreateRotationAxis(KdVec3 vec, float angle)
	{
		*this = DirectX::XMMatrixRotationAxis(vec, angle);
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

	//拡縮行列作成
	void CreateScalling(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x, y, z);
	}

	//透視影行列の作成
	KdMatrix& CreateProjectionPerspectiveFov(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		return *this;
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

	void Scale(float x, float y, float z)
	{
		*this *= DirectX::XMMatrixScaling(x, y, z);
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

	KdVec3 GetAngles()
	{
		KdMatrix mat = *this;

		// 各軸を取得
		KdVec3 axisX = mat.GetAxisX();
		KdVec3 axisY = mat.GetAxisY();
		KdVec3 axisZ = mat.GetAxisZ();

		// 各軸を正規化
		axisX.Normalize();
		axisY.Normalize();
		axisZ.Normalize();

		// マトリックスを正規化
		mat.SetAxisX(axisX);
		mat.SetAxisY(axisY);
		mat.SetAxisZ(axisZ);

		KdVec3 angles;
		angles.x = atan2f(mat.m[1][2], mat.m[2][2]);
		angles.y = atan2f(-mat.m[0][2], sqrt(mat.m[1][2] * mat.m[1][2] + mat.m[2][2] * mat.m[2][2]));
		angles.z = atan2f(mat.m[0][1], mat.m[0][0]);

		return angles;
	}

	KdVec3 GetScale()
	{
		KdMatrix mat = *this;

		KdVec3 sca;

		sca.x = sqrt(pow(mat.m[0][0], 2.0) + pow(mat.m[0][1], 2.0) + pow(mat.m[0][2], 2.0));
		sca.y = sqrt(pow(mat.m[1][0], 2.0) + pow(mat.m[1][1], 2.0) + pow(mat.m[1][2], 2.0));
		sca.z = sqrt(pow(mat.m[2][0], 2.0) + pow(mat.m[2][1], 2.0) + pow(mat.m[2][2], 2.0));

		return sca;
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

	//正面を向かせる
	inline void SetBillboard(const KdMatrix& mat)
	{
		//拡大率の保持
		float scaleX = this->GetAxisX().Length();
		float scaleY = this->GetAxisY().Length();
		float scaleZ = this->GetAxisZ().Length();

		// ビルボード処理
		KdMatrix drawMat;
		drawMat.CreateScalling(scaleX, scaleY, scaleZ);

		KdMatrix targetMat = mat;
		drawMat *= targetMat.Invert();

		drawMat.SetTranslation(this->GetTranslation());

		*this = drawMat;
	}

private:
};

// KdMatrix同士の合成
inline KdMatrix operator* (const KdMatrix& m1, const KdMatrix& m2)
{
	using namespace DirectX;
	return XMMatrixMultiply(m1, m2);
}
