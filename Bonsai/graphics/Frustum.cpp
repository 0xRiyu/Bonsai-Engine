#include "Frustum.h"
namespace bonsai
{
	namespace graphics
	{

		Frustum::Frustum()
		{
		}

		void Frustum::CleanUp()
		{
		}

		void Frustum::ConstructFrustum(float screenDepth, XMMATRIX projectionMat, const XMMATRIX& viewMatrix,
			const XMMATRIX& world)
		{
			XMStoreFloat4x4(&view, viewMatrix);
			XMFLOAT4X4 projectionMatrix;
			XMStoreFloat4x4(&projectionMatrix, projectionMat);

			float zMinimum, r;
			XMFLOAT4X4 matrix;

			//Calculate the minimum z distance in the frustum
			zMinimum = -projectionMatrix._43 / projectionMatrix._33;
			r = screenDepth / (screenDepth - zMinimum);
			projectionMatrix._33 = r;
			projectionMatrix._43 = -r * zMinimum;

			// Create the frustum matrix from the view matrix and updated projection matrix.
			XMStoreFloat4x4(&matrix, XMMatrixMultiply(XMMatrixMultiply(viewMatrix, world), XMLoadFloat4x4(&projectionMatrix)));

			// Calculate near plane of frustum.
			m_Planes[0].x = matrix._14 + matrix._13;
			m_Planes[0].y = matrix._24 + matrix._23;
			m_Planes[0].z = matrix._34 + matrix._33;
			m_Planes[0].w = matrix._44 + matrix._43;
			XMStoreFloat4(&m_PlanesNorm[0], XMPlaneNormalize(XMLoadFloat4(&m_Planes[0])));

			// Calculate far plane of frustum.
			m_Planes[1].x = matrix._14 - matrix._13;
			m_Planes[1].y = matrix._24 - matrix._23;
			m_Planes[1].z = matrix._34 - matrix._33;
			m_Planes[1].w = matrix._44 - matrix._43;
			XMStoreFloat4(&m_PlanesNorm[1], XMPlaneNormalize(XMLoadFloat4(&m_Planes[1])));

			// Calculate left plane of frustum.
			m_Planes[2].x = matrix._14 + matrix._11;
			m_Planes[2].y = matrix._24 + matrix._21;
			m_Planes[2].z = matrix._34 + matrix._31;
			m_Planes[2].w = matrix._44 + matrix._41;
			XMStoreFloat4(&m_PlanesNorm[2], XMPlaneNormalize(XMLoadFloat4(&m_Planes[2])));

			// Calculate right plane of frustum.
			m_Planes[3].x = matrix._14 - matrix._11;
			m_Planes[3].y = matrix._24 - matrix._21;
			m_Planes[3].z = matrix._34 - matrix._31;
			m_Planes[3].w = matrix._44 - matrix._41;
			XMStoreFloat4(&m_PlanesNorm[3], XMPlaneNormalize(XMLoadFloat4(&m_Planes[3])));

			// Calculate top plane of frustum.
			m_Planes[4].x = matrix._14 - matrix._12;
			m_Planes[4].y = matrix._24 - matrix._22;
			m_Planes[4].z = matrix._34 - matrix._32;
			m_Planes[4].w = matrix._44 - matrix._42;
			XMStoreFloat4(&m_PlanesNorm[4], XMPlaneNormalize(XMLoadFloat4(&m_Planes[4])));

			// Calculate bottom plane of frustum.
			m_Planes[5].x = matrix._14 + matrix._12;
			m_Planes[5].y = matrix._24 + matrix._22;
			m_Planes[5].z = matrix._34 + matrix._32;
			m_Planes[5].w = matrix._44 + matrix._42;
			XMStoreFloat4(&m_PlanesNorm[5], XMPlaneNormalize(XMLoadFloat4(&m_Planes[5])));

		}

		bool Frustum::CheckPoint(const XMFLOAT3& point) const
		{
			XMVECTOR p = XMLoadFloat3(&point);
			for (short i = 0; i < 6; i++)
			{
				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlanesNorm[i]), p)) < 0.0f)
				{
					return false;
				}
			}

			return true;
		}

		bool Frustum::CheckSphere(const XMFLOAT3& center, float radius) const
		{
			int i;
			XMVECTOR c = XMLoadFloat3(&center);
			for (i = 0; i < 6; i++)
			{
				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlanesNorm[i]), c)) < -radius)
				{
					return false;
				}
			}

			return true;
		}

		bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
		{


			// Check if any one point of the cube is in the view frustum.
			for (int i = 0; i<6; i++)
			{
				
				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter - radius))))) >= 0.0f)
				{
					continue;
				}

				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter - radius))))) >= 0.0f)
				{
					continue;
				}

				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter - radius))))) >= 0.0f)
				{
					continue;
				}

				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter - radius))))) >= 0.0f)
				{
					continue;
				}

				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter - radius), (zCenter + radius))))) >= 0.0f)
				{
					continue;
				}

				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter - radius), (zCenter + radius))))) >= 0.0f)
				{
					continue;
				}

				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter - radius), (yCenter + radius), (zCenter + radius))))) >= 0.0f)
				{
					continue;
				}

				if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Planes[i]), XMLoadFloat3(&XMFLOAT3((xCenter + radius), (yCenter + radius), (zCenter + radius))))) >= 0.0f)
				{
					continue;
				}

				return false;
			}

			return true;
		}

		int Frustum::CheckBox(const AABB& box) const
		{
			int iTotalIn = 0;
			for (int p = 0; p < 6; ++p)
			{

				int iInCount = 8;
				int iPtIn = 1;

				for (int i = 0; i < 8; ++i)
				{
					if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlanesNorm[p]), XMLoadFloat3(&box.corner(i)))) < 0.0f)
					{
						iPtIn = 0;
						--iInCount;
					}
				}
				if (iInCount == 0)
					return(false);
				iTotalIn += iPtIn;
			}
			if (iTotalIn == 6) {
				return(BOX_FRUSTUM_INSIDE);
			}
			return(BOX_FRUSTUM_INTERSECTS);
		}


		const XMFLOAT4& Frustum::getLeftPlane() const { return m_PlanesNorm[2]; }
		const XMFLOAT4& Frustum::getRightPlane() const { return m_PlanesNorm[3]; }
		const XMFLOAT4& Frustum::getTopPlane() const { return m_PlanesNorm[4]; }
		const XMFLOAT4& Frustum::getBottomPlane() const { return m_PlanesNorm[5]; }
		const XMFLOAT4& Frustum::getFarPlane() const { return m_PlanesNorm[1]; }
		const XMFLOAT4& Frustum::getNearPlane() const { return m_PlanesNorm[0]; }
		XMFLOAT3 Frustum::getCameraPos() const
		{
			return XMFLOAT3(-view._41, -view._42, -view._43);
		}
	}
}