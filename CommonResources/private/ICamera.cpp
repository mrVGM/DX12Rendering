#include "ICamera.h"

rendering::ICamera::ICamera(const BaseObjectMeta& meta) :
	BaseObject(meta)
{
}

rendering::ICamera::~ICamera()
{
}

DirectX::XMMATRIX rendering::cam_utils::MakePerspectiveProjectionMatrix(
	const DirectX::XMVECTOR& right,
	const DirectX::XMVECTOR& fwd,
	const DirectX::XMVECTOR& up,
	const DirectX::XMVECTOR& origin,
	float nearPlane,
	float farPlane,
	float fovDegrees,
	float aspect
)
{
	float fovRad = DirectX::XMConvertToRadians(fovDegrees);

	float h = tan(fovRad / 2);
	float w = aspect * h;

	DirectX::XMMATRIX translate(
		DirectX::XMVECTOR{ 1, 0, 0, -DirectX::XMVectorGetX(origin) },
		DirectX::XMVECTOR{ 0, 1, 0, -DirectX::XMVectorGetY(origin) },
		DirectX::XMVECTOR{ 0, 0, 1, -DirectX::XMVectorGetZ(origin) },
		DirectX::XMVECTOR{ 0, 0, 0, 1 }
	);

	DirectX::XMMATRIX view(
		DirectX::XMVECTOR{ DirectX::XMVectorGetX(right), DirectX::XMVectorGetY(right), DirectX::XMVectorGetZ(right), 0 },
		DirectX::XMVECTOR{ DirectX::XMVectorGetX(up), DirectX::XMVectorGetY(up), DirectX::XMVectorGetZ(up), 0 },
		DirectX::XMVECTOR{ DirectX::XMVectorGetX(fwd), DirectX::XMVectorGetY(fwd), DirectX::XMVectorGetZ(fwd), 0 },
		DirectX::XMVECTOR{ 0, 0, 0, 1 }
	);


	DirectX::XMMATRIX project(
		DirectX::XMVECTOR{ 1 / w, 0, 0, 0 },
		DirectX::XMVECTOR{ 0, 1 / h, 0, 0 },
		DirectX::XMVECTOR{ 0, 0, farPlane / (farPlane - nearPlane), -farPlane * nearPlane / (farPlane - nearPlane) },
		DirectX::XMVECTOR{ 0, 0, 1, 0 }
	);


	DirectX::XMMATRIX mvp = project * view * translate;
	return DirectX::XMMatrixTranspose(mvp);
}